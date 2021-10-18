#ifndef COMPONENTS_PHYSICS_H
#define COMPONENTS_PHYSICS_H

#include <ACL/general.h>
#include <ACL/math/vector.h>
#include <ACL/gui/color.h>

#include "component.h"
#include "../borders.h"


using abel::math::Vector2d;


class PhysComp : public Component {
public:
    static constexpr bool ACCOUNT_FOR_SKIPPED_MOVEMENT = true;

    DECLARE_ERROR(error, abel::error);


    inline PhysComp(Molecule *object_, const Vector2d &pos_, double mass_, double radius_) :
        PhysComp(object_, pos_, Vector2d::ZERO, mass_, radius_) {}

    PhysComp(Molecule *object_, const Vector2d &pos_, const Vector2d &impulse_,
             double mass_, double radius_);

    bool testForCollision(const PhysComp &other) const;

    virtual void respondToCollision(PhysComp &other);

    bool testForBorderCollision(const Border &border) const;

    virtual void respondToBorderCollision(Border &border);

    virtual void update(double deltaT, bool inGracePeriod = false);

    // Returns whether they collide
    virtual bool updatePair(PhysComp &other, bool inGracePeriod = false);

    virtual PhysComp *copy() const;

    virtual ~PhysComp() = default;

    virtual Molecule &absorb(PhysComp &other, bool reflectable = true);

    virtual abel::gui::Color getTypeColor() const noexcept;

    virtual void scaleTrue(double coeff);

    virtual void dump() const noexcept;

    Vector2d getCOMWith(const PhysComp &other) const;

    // A lot of getters-setters
    constexpr const Vector2d &getPos()     const { return pos;     }
    constexpr       Vector2d &getPos()           { return pos;     }
    constexpr const Vector2d &getImpulse() const { return impulse; }
    constexpr       Vector2d &getImpulse()       { return impulse; }
    constexpr         double  getMass()    const { return mass;    }
    constexpr         double &getMass()          { return mass;    }
    constexpr         double  getRadius()  const { return radius;  }
    constexpr         double &getRadius()        { return radius;  }

    constexpr         double  getEnergy()  const { return impulse.magnitude() / mass / 2; }

protected:
    Vector2d pos;
    Vector2d impulse;
    double mass;
    double radius;

    void dump(const char *clsName) const noexcept;

};


class GravityPhysComp : public PhysComp {
public:
    static constexpr double GRAVITY_COEFF = 9.8d;
    static constexpr Vector2d GRAVITY_DIR{0.d, 1.d};


    inline GravityPhysComp(Molecule *object_, const Vector2d &pos_, double mass_, double radius_) :
        PhysComp(object_, pos_, mass_, radius_) {}

    inline GravityPhysComp(Molecule *object_, const Vector2d &pos_, const Vector2d &impulse_,
                           double mass_, double radius_) :
        PhysComp(object_, pos_, impulse_, mass_, radius_) {}

    virtual void update(double deltaT, bool inGracePeriod = false) override;

    virtual GravityPhysComp *copy() const override;

    virtual ~GravityPhysComp() override = default;

    virtual abel::gui::Color getTypeColor() const noexcept override;

    virtual void dump() const noexcept override;

};


class MagneticPhysComp : public PhysComp {
public:
    static constexpr double MAGNETISM_COEFF = 40000.d;  // The real one was absurdly high


    inline MagneticPhysComp(Molecule *object_, const Vector2d &pos_, double mass_,
                            double radius_, double charge_) :
        PhysComp(object_, pos_, mass_, radius_), charge{charge_}, magneticForce{0} {}

    inline MagneticPhysComp(Molecule *object_, const Vector2d &pos_, const Vector2d &impulse_,
                           double mass_, double radius_, double charge_) :
        PhysComp(object_, pos_, impulse_, mass_, radius_), charge{charge_}, magneticForce{0} {}

    virtual void respondToCollision(PhysComp &other) override;

    virtual void update(double deltaT, bool inGracePeriod = false) override;

    // Returns whether they collide
    virtual bool updatePair(PhysComp &other, bool inGracePeriod = false) override;

    virtual MagneticPhysComp *copy() const override;

    virtual ~MagneticPhysComp() override = default;

    virtual Molecule &absorb(PhysComp &other, bool reflectable = true) override;

    virtual abel::gui::Color getTypeColor() const noexcept override;

    virtual void scaleTrue(double coeff) override;

    virtual void dump() const noexcept override;

    constexpr double &getCharge()       { return charge; }
    constexpr double  getCharge() const { return charge; }

protected:
    double charge;
    Vector2d magneticForce;

};


#endif // COMPONENTS_PHYSICS_H
