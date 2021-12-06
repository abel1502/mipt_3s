#include "chemistry.h"

#include <ACL/math/cmath.h>
#include "../manager.h"
#include "../molecule.h"


bool ChemComp::updatePair(ChemComp &other, bool colliding, bool inGracePeriod) {
    return colliding && !inGracePeriod && maybeInteract(other);
}

bool ChemComp::maybeInteract(ChemComp &other) {
    if (abel::randDouble(1.) > INTERACTION_CHANCE)
        return false;

    interact(other);
    return true;
}

void ChemComp::interact(ChemComp &other) {
    if (potency == 1 || other.potency == 1) {
        // Destroy these, create an amalgam with potency + other.potency

        potency += other.potency;  // This interaction is our responsibility
        energy += other.energy;

        double oldPhysEnergy = getObj().getComp<PhysComp>().getEnergy() +
                         other.getObj().getComp<PhysComp>().getEnergy();

        // The rest is delegated to the molecule
        Molecule &survivor = getObj().onChemAbsorb(other.getObj(), potency);
        assert(&survivor == object);  // Should have been ensured by onChemAbsorb, by reconnecting this to them

        energy += oldPhysEnergy - survivor.getComp<PhysComp>().getEnergy();

        return;
    }

    // Explode
    PhysComp &physMe = getObj().getComp<PhysComp>(),
             &physIt = other.getObj().getComp<PhysComp>();

    // DBG("Was (%u) %lg, (%lg, %lg)",       potency, physMe.getEnergy() +       getEnergy(), physMe.getImpulse().x(), physMe.getImpulse().y());
    // DBG("Was (%u) %lg, (%lg, %lg)", other.potency, physIt.getEnergy() + other.getEnergy(), physIt.getImpulse().x(), physIt.getImpulse().y());

    Vector2d pos = physMe.getCOMWith(physIt);

          explode(pos);
    other.explode(pos);

    //PAUSE();
}

void ChemComp::explode(const Vector2d pos) {
    getObj().markDead();
    unsigned nClones = potency;
    double impulse = std::sqrt(2 * getObj().getComp<PhysComp>().getMass() * energy) / nClones;

    MoleculeManager &manager = getObj().getManager();

    /*DBG("Pre explosion:\n"
        "  chem energy  = %lg\n"
        "  chem potency = %u",
        energy,
        potency);
    getObj().getComp<PhysComp>().dump();*/

    //Molecule &mol = manager.addMolecule(pos, 1.d, Molecule::P_BALL);
    Molecule &mol = manager.copyMolecule(getObj());
    mol.scaleTrue(1. / nClones);
    mol.getComp<PhysComp>().getPos() = pos;
    //mol.getComp<PhysComp>().getRadius() = 1.d;
    mol.renderBecomeCircle();
    //mol.getComp<RenderComp>().scaleTo(2.d);
    mol.getComp<ChemComp>().potency = 1;
    mol.getComp<ChemComp>().energy = 0;

    manager.explodeClones(mol, nClones, impulse);

    energy = 0;
}

ChemComp *ChemComp::copy() const {
    return new ChemComp(object, potency);
}

void ChemComp::scaleTrue(double coeff) {
    // I guess we can't really determine the correct new potency here
    // potency = (unsigned)((double)potency * std::sqrt(coeff));
    energy *= coeff;
}

