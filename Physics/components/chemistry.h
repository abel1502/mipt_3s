#ifndef COMPONENTS_CHEMISTRY_H
#define COMPONENTS_CHEMISTRY_H

#include <ACL/general.h>

#include "component.h"


class ChemComp : public Component {
public:
    static constexpr double INTERACTION_CHANCE = 0.4;

    DECLARE_ERROR(error, abel::error);


    inline ChemComp(Molecule *object_, unsigned potency_ = 1) noexcept :
        Component(object_), potency{potency_}, energy{0} {}

    // Returns true if a reaction took place
    bool updatePair(ChemComp &other, bool colliding);

    // True if chemically interacted
    bool maybeInteract(ChemComp &other);

    void interact(ChemComp &other);

    ChemComp *copy() const;

    unsigned getPotency() const { return potency; }

protected:
    unsigned potency;
    double energy;

};


#endif // COMPONENTS_CHEMISTRY_H


