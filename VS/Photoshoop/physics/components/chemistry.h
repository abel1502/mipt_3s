#ifndef COMPONENTS_CHEMISTRY_H
#define COMPONENTS_CHEMISTRY_H

#include <ACL/general.h>
#include <ACL/math/vector.h>

#include "component.h"


using abel::math::Vector2d;


class ChemComp : public Component {
public:
    static constexpr double INTERACTION_CHANCE = 0.4;

    DECLARE_ERROR(error, abel::error);


    inline ChemComp(Molecule *object_, unsigned potency_ = 1) noexcept :
        Component(object_), potency{potency_}, energy{0} {}

    // Returns true if a reaction took place
    bool updatePair(ChemComp &other, bool colliding, bool inGracePeriod = false);

    // True if chemically interacted
    bool maybeInteract(ChemComp &other);

    void interact(ChemComp &other);

    ChemComp *copy() const;

    constexpr unsigned getPotency() const { return potency; }

    constexpr double getEnergy() const { return energy; }

    void scaleTrue(double coeff);

protected:
    unsigned potency;
    double energy;

    void explode(const Vector2d pos);

};


#endif // COMPONENTS_CHEMISTRY_H


