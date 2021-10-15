#include "chemistry.h"

#include "../manager.h"
#include "../molecule.h"
#include <ACL/math/cmath.h>


bool ChemComp::updatePair(ChemComp &other, bool colliding) {
    return colliding && maybeInteract(other);
}

bool ChemComp::maybeInteract(ChemComp &other) {
    if (abel::randDouble(1.d) > INTERACTION_CHANCE)
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
          getObj().markDead();
    other.getObj().markDead();

    unsigned nClones = potency + other.potency;

    PhysComp &   myPhys =       getObj().getComp<PhysComp>();
    PhysComp &otherPhys = other.getObj().getComp<PhysComp>();

    MoleculeManager &manager = getObj().getManager();
    Vector2d pos = (myPhys   .getPos() * myPhys   .getMass() +
                    otherPhys.getPos() * otherPhys.getMass()) /
                   (myPhys.getMass() + otherPhys.getMass());

    //Molecule &mol = manager.addMolecule(pos, 1.d, Molecule::P_BALL);
    Molecule &mol = manager.copyMolecule(getObj());
    mol.scaleTrue(1.d / nClones);
    mol.getComp<PhysComp>().getPos() = pos;
    //mol.getComp<PhysComp>().getRadius() = 1.d;
    mol.renderBecomeCircle();
    //mol.getComp<RenderComp>().scaleTo(2.d);
    //mol.getComp<ChemComp>().potency = 1;
    mol.getComp<ChemComp>().energy = 0;

    manager.explodeClones(mol, nClones, energy);

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

