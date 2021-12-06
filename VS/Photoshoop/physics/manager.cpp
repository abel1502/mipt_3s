#include <AGF/llgui.h>

#include "manager.h"
#include "components/physics.h"
#include "../app.h"
#include <cassert>
#include <cstdint>


MoleculeManager::MoleculeManager(Texture &target_) :
    renderTimer{0}, target{target_},
    coords{target_.getRect(), HALF_VIRT_WIDTH},
    borders{}, molecules{}, newMolecules{} {

    Vector2d borderCorner{coords.getVirtualPlane().getDiag() / 2.};
    borderCorner -= Vector2d{HALF_VIRT_WIDTH * 0.05};

    for (unsigned i = 0; i < 4; ++i) {
        borders.appendEmplace(borderCorner, BORDER_DIRS[i]);
        borderCorner.values[i % 2] *= -1;
    }
}

// Still needed, because the move-ctor doesn't take MoleculeManager *
template <>
Molecule &MoleculeManager::addMolecule(Molecule &&molecule) {
    assert(&molecule.getManager() == this);

    if (newMolecules.append(std::forward<Molecule>(molecule))) {
        updateBackrefs();
    }

    return newMolecules[-1];
}

Molecule &MoleculeManager::addRandomMolecule() {
    Molecule::Preset base = Molecule::P_BALL;

    if (abel::randDouble() < 0.5)
        base = Molecule::P_SQUARE;

    return addRandomMolecule(base);
}

Molecule &MoleculeManager::addRandomMolecule(Molecule::Preset base) {
    Vector2d pos{abel::randDouble(-coords.getVirtualPlane().w(), coords.getVirtualPlane().w()),
                 abel::randDouble(-coords.getVirtualPlane().h(), coords.getVirtualPlane().h())};
    pos /= 2;  // Because we've used double constraints
    // pos *= 0.9;  // Just to make sure it's not inside borders

    Vector2d impulse = Vector2d::fromPhiDegreesR(abel::randDouble(360), abel::randDouble(
                            (coords.getVirtualPlane().w() + coords.getVirtualPlane().w()) * 0.3));

    double size = 0;
    switch (base) {
    case Molecule::P_BALL: {
        size = 1.;
    } break;

    case Molecule::P_SQUARE: {
        size = abel::randDouble(2., 6.);
    } break;

    NODEFAULT
    }

    static constexpr double MASS_COEFF = 1.;
    double mass = size * size * MASS_COEFF;
    assert(!abel::isZero(mass));

    PhysComp *phys = nullptr;
    {
        static constexpr double GENERIC_CHANCE = 0.5,
                                GRAVITY_CHANCE = 0.25,
                               MAGNETIC_CHANCE = 0.25;

        double rndVal = abel::randDouble(GENERIC_CHANCE + GRAVITY_CHANCE + MAGNETIC_CHANCE);

        if (rndVal < GENERIC_CHANCE) {
            phys = new PhysComp(nullptr, pos, impulse, mass, size);
        } else if ((rndVal -= GENERIC_CHANCE) < GRAVITY_CHANCE) {
            phys = new GravityPhysComp(nullptr, pos, impulse, mass, size);
        } else {
            static constexpr double CHARGE_COEFF = 0.4;
            phys = new MagneticPhysComp(nullptr, pos, impulse, mass, size, abel::randDouble(-mass * CHARGE_COEFF, mass * CHARGE_COEFF));
        }
    }

    RenderComp *rend = nullptr;
    switch (base) {
    case Molecule::P_BALL: {
        rend = new CircleRenderComp(nullptr, size);
    } break;

    case Molecule::P_SQUARE: {
        rend = new SquareRenderComp(nullptr, size * 2);
    } break;

    NODEFAULT
    }

    unsigned potency = 1;
    if (base == Molecule::P_SQUARE) {
        static constexpr unsigned MAX_POTENCY = 10;
        potency += 1 + (unsigned)abel::randLL(MAX_POTENCY - 2);
    }
    ChemComp *chem = new ChemComp(nullptr, potency);

    Molecule &result = addMolecule(phys, rend, chem);

    // We don't delete them, since now the molecule owns them!
    // delete phys;
    // delete rend;
    // delete chem;

    return result;
}

Molecule &MoleculeManager::copyMolecule(const Molecule &original) {
    return addMolecule(std::move(Molecule::ManagerProxy::copy(original)));
}

void MoleculeManager::explodeClones(Molecule &mol_, unsigned n, double impulse) {
    REQUIRE(n >= 1);
    REQUIRE(&mol_.getManager() == this);

    PhysComp &oldPhys = mol_.getComp<PhysComp>();

    if (n == 1) {
        double oldImpulse = oldPhys.getImpulse().length();
        oldPhys.getImpulse() *= 1 + impulse / oldImpulse;
        return;
    }

    static constexpr double RADIUS_COEFF = 1.4;  // was 3.5 without grace period
    static constexpr double GRACE_PERIOD = 0.1;

    // Because we need mol to persist over reallocations of our buffers.
    // mol_ will get garbage-collected, while mol will be destroyed automatically
    Molecule mol = Molecule::ManagerProxy::copy(mol_);
    mol_.markDead();

    mol.addGracePeriod(GRACE_PERIOD);

    double radius = oldPhys.getRadius() * RADIUS_COEFF;

    const double angle = 360. / n;
    Vector2d dir{1, 0};
    dir.rotateDegrees(abel::randDouble(360.));

    mol.markDead();  // Just in case

    double totalEnergy = 0;
    Vector2d totalImpulse{};

    for (unsigned i = 0; i < n; ++i) {
        Molecule &clone = copyMolecule(mol);

        PhysComp &phys = clone.getComp<PhysComp>();
        phys.getPos()     += dir * radius;
        phys.getImpulse() += dir * impulse;

        dir.rotateDegrees(angle);

        totalEnergy  += phys.getEnergy();
        totalImpulse += phys.getImpulse();
    }

    // DBG("Is %lg, (%lg, %lg)", totalEnergy, totalImpulse.x(), totalImpulse.y());
}

void MoleculeManager::tick(double deltaT) {
    if (needsRender(deltaT)) {
        // TODO: Maybe not
        MyApp::getInstance().demandRedraw();
    }

    bool moleculesReallocated = false;
    while (newMolecules.getSize()) {
        Molecule &mol = newMolecules[-1];

        if (mol.isDead()) {
            newMolecules.pop();
            continue;
        }

        moleculesReallocated |= !!molecules.append(std::move(newMolecules.popVal()));
    }
    if (moleculesReallocated) {
        updateBackrefs();
    }

    // double totalCharge = 0;

    for (Molecule &mol : molecules) {
        if (mol.isDead())
            continue;

        mol.update(deltaT);

        // if (typeid(mol.getComp<PhysComp>()) == typeid(MagneticPhysComp)) {
        //     totalCharge += dynamic_cast<MagneticPhysComp &>(mol.getComp<PhysComp>()).getCharge();
        // }
    }

    // DBG(">> Total charge: %lg", totalCharge);

    unsigned moleculesCnt = molecules.getSize();

    for (unsigned i = 0; i < moleculesCnt; ++i) {
        Molecule &molA = molecules[i];
        if (molA.isDead())
            continue;

        for (unsigned j = i + 1; j < moleculesCnt; ++j) {
            Molecule &molB = molecules[j];
            if (molB.isDead())
                continue;

            molA.updatePair(molB);

            assert(molecules.getSize() == moleculesCnt);

            if (molA.isDead())
                break;
        }
    }

    processFlags();
}

void MoleculeManager::render() {
    if (!needsRender()) {
        return;
    }

    target.setFillColor(Color::WHITE);
    target.clear();

    for (const Border &border : borders) {
        border.render(target, coords);
    }

    for (Molecule &mol : molecules) {
        mol.render(target, coords);
    }

    haveRendered();
}

unsigned MoleculeManager::refToIdx(const Molecule &molecule) {
    ptrdiff_t idx = &molecule - molecules.getBuf();

    if (idx >= (ptrdiff_t)molecules.getSize()) {
        // TODO: Maybe test for newMolecules as well?
        throw error("Molecule not in manager's array");
    }

    return (unsigned)idx;
}

void MoleculeManager::updateBackrefs() {
    for (Molecule &molecule : newMolecules) {
        Molecule::ManagerProxy::updateComponentBackrefs(molecule);
    }

    for (Molecule &molecule : molecules) {
        Molecule::ManagerProxy::updateComponentBackrefs(molecule);
    }
}

void MoleculeManager::deleteMolecule(unsigned idx) {
    assert(idx < molecules.getSize());

    std::swap(molecules[idx], molecules[-1]);

    Molecule::ManagerProxy::updateComponentBackrefs(molecules[idx]);

    molecules.pop();
}

void MoleculeManager::processFlags() {
    for (unsigned i = 0; i < molecules.getSize(); /* no increment here intentionally */) {
        if (molecules[i].isDead()) {
            deleteMolecule(i);
            continue;
        }

        i++;
    }
}
