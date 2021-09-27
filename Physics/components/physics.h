#ifndef COMPONENTS_PHYSICS_H
#define COMPONENTS_PHYSICS_H

#include <ACL/general.h>
#include <ACL/math/vector.h>

#include "component.h"


using abel::math::Vector2d;


class PhysComp : public Component {
public:
    DECLARE_ERROR(error, abel::error);


    PhysComp(Molecule &object_, double mass_, double radius_);

    bool testForCollision(const PhysComp &other) const;

    virtual void respondToCollision(PhysComp &other);  // TODO: Somehow make collision response called only once per pair

    bool testForBorderCollision() const;

    virtual void respondToBorderCollision();

    virtual void update(double deltaT);

    virtual PhysComp *copy();

    virtual ~PhysComp();

protected:
    double mass;
    double radius;
    Vector2d impulse;

};


class GravityPhysComp : public PhysComp {
public:
    GravityPhysComp(Molecule &object_, double mass_, double radius_);

    virtual void update(double deltaT) override;

    virtual GravityPhysComp *copy() override;
};


class MagneticPhysComp : public PhysComp {
public:
    MagneticPhysComp(Molecule &object_, double mass_, double radius_, double charge_);

    // TODO: In perspective, dispatch table. For now, an if
    virtual void respondToCollision(PhysComp &other) override;

    virtual void update(double deltaT) override;

    virtual MagneticPhysComp *copy() override;
};


#endif // COMPONENTS_PHYSICS_H
