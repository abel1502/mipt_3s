#include "molecule.h"
#include "manager.h"
#include <typeinfo>


Molecule Molecule::ManagerProxy::copy(const Molecule &self) {
    return Molecule(self);
}

void Molecule::ManagerProxy::updateComponentBackrefs(Molecule &self) {
    #define UPDATE_BACKREF_(COMP_TYPE) \
        self.getComp<COMP_TYPE>().updateObj(&self);

    UPDATE_BACKREF_(  PhysComp)
    UPDATE_BACKREF_(RenderComp)
    UPDATE_BACKREF_(  ChemComp)

    #undef UPDATE_BACKREF_
}

Molecule::Molecule(MoleculeManager *manager_, const Vector2d &pos, double size, Preset preset) :
    manager{manager_}, phys{nullptr}, rend{nullptr}, chem{nullptr}, flags{false}, gracePeriod{0} {

    assert(abel::sgnDbl(size) > 0);

    static constexpr double MASS_COEFF = 1.d;

    switch (preset) {
    case P_BALL: {
        phys = new         PhysComp(this, pos, size * size * MASS_COEFF, size);
        rend = new CircleRenderComp(this, size);
        chem = new         ChemComp(this, 1);
    } break;

    case P_SQUARE: {
        unsigned potency = 2;
        if (size > 2.d) {
            potency += (int)(size - 2.d);
        }

        phys = new         PhysComp(this, pos, size * size * MASS_COEFF, size);
        rend = new SquareRenderComp(this, 2 * size);
        chem = new         ChemComp(this, potency);
    } break;

    NODEFAULT
    }
}

Molecule::Molecule(MoleculeManager *manager_, const PhysComp &phys_, const RenderComp &rend_, const ChemComp &chem_) :
    Molecule(manager_, phys_.copy(), rend_.copy(), chem_.copy()) {

    validate();
}

Molecule::Molecule(MoleculeManager *manager_, PhysComp *phys_, RenderComp *rend_, ChemComp *chem_) :
    manager{manager_}, phys{phys_}, rend{rend_}, chem{chem_}, flags{false}, gracePeriod{0} {

    validate();
    phys->updateObj(this);
    rend->updateObj(this);
    chem->updateObj(this);
}

Molecule::Molecule(Molecule &&other) :
    manager{other.manager}, phys{other.phys}, rend{other.rend}, chem{other.chem}, flags{other.flags}, gracePeriod{other.gracePeriod} {

    other.flags.dead = false;

    other.phys = nullptr;
    other.rend = nullptr;
    other.chem = nullptr;

    phys->updateObj(this);
    rend->updateObj(this);
    chem->updateObj(this);
}

Molecule &Molecule::operator=(Molecule &&other) {
    std::swap(manager, other.manager);
    std::swap(phys,  other.phys);
    std::swap(rend,  other.rend);
    std::swap(chem,  other.chem);
    std::swap(flags, other.flags);
    std::swap(gracePeriod, other.gracePeriod);

    if (phys)  phys->updateObj(this);
    if (rend)  rend->updateObj(this);
    if (chem)  chem->updateObj(this);

    return *this;
}

Molecule::~Molecule() noexcept {
    destroy();
}

void Molecule::update(double deltaT) {
    validate();

    passGracePeriod(deltaT);

    phys->update(deltaT, inGracePeriod());

    // Border collisions are handled irregarding the grace state
    for (Border &border : getManager().getBorders()) {
        if (!phys->testForBorderCollision(border))
            continue;

        phys->respondToBorderCollision(border);
    }

    // Other components don't currently have an update() method
}

void Molecule::updatePair(Molecule &other) {
    bool igp = inGracePeriod() || other.inGracePeriod();

    bool colliding = getComp<PhysComp>().updatePair(other.getComp<PhysComp>(), igp);

    if (isDead() || other.isDead())
        return;

    getComp<ChemComp>().updatePair(other.getComp<ChemComp>(), colliding, igp);
}

void Molecule::render(Texture &target, const Coords &coords) {
    getComp<RenderComp>().render(target, coords, getComp<PhysComp>().getTypeColor());
}

void Molecule::onBorderCollision(const Border &) {
    // For now we do nothing here
}

Molecule &Molecule::onChemAbsorb(Molecule &other, unsigned potency) {
    validate();
    other.validate();

    Molecule &survivor = phys->absorb(*other.phys);

    if (&survivor == this) {
        other.markDead();
    } else if (&survivor == &other) {
        markDead();
    } else {
        DBG("Shouldn't be possible");
        REQUIRE(false);
    }

    survivor.getComp<RenderComp>().scaleTo(survivor.getComp<PhysComp>().getRadius() * 2);

    if (potency > 1) {
        survivor.rend = survivor.rend->becomeSquare();
    } else {
        survivor.rend = survivor.rend->becomeCircle();
    }

    std::swap(survivor.chem, chem);  // So that out chem holds the correct

    survivor.phys->updateObj(&survivor);
    survivor.rend->updateObj(&survivor);
    survivor.chem->updateObj(&survivor);

    return survivor;
}

void Molecule::renderBecomeCircle() {
    rend = rend->becomeCircle();
}

void Molecule::renderBecomeSquare() {
    rend = rend->becomeSquare();
}

void Molecule::scaleTrue(double coeff) {
    validate();

    phys->scaleTrue(coeff);
    rend->scaleTrue(coeff);
    chem->scaleTrue(coeff);
}

void Molecule::dump() const noexcept {
    DBG("Molecule (%p) {\n"
        "  manager = %p\n"
        "  phys    = %p (%s)\n"
        "  rend    = %p (%s)\n"
        "  chem    = %p (%s)\n"
        "  flags   = dead:%d\n"
        "  gracePeriod = %lg\n"
        "}",
        this,
        manager,
        phys, typeid(phys).name(),
        rend, typeid(rend).name(),
        chem, typeid(chem).name(),
        flags.dead,
        gracePeriod);
}

void Molecule::destroy() noexcept {
    // The manager field remains

    if (phys)
        delete phys;
    if (rend)
        delete rend;
    if (chem)
        delete chem;

    phys = nullptr;
    rend = nullptr;
    chem = nullptr;

    flags.dead = false;
    gracePeriod = 0;
}

Molecule::Molecule(const Molecule &other) :
    manager{other.manager}, phys{nullptr}, rend{nullptr}, chem{nullptr}, flags{false}, gracePeriod{other.gracePeriod} {

    other.validate();

    phys = other.phys->copy();
    rend = other.rend->copy();
    chem = other.chem->copy();
}

Molecule &Molecule::operator=(const Molecule &other) {
    destroy();

    other.validate();

    flags.dead = false;
    gracePeriod = other.gracePeriod;

    manager = other.manager;
    phys = other.phys->copy();
    rend = other.rend->copy();
    chem = other.chem->copy();

    return *this;
}
