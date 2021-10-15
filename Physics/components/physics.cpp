#include "physics.h"

#include "../molecule.h"

#include <typeinfo>
#include <cassert>
#include <ACL/math/cmath.h>
#include <numeric>


PhysComp::PhysComp(Molecule *object_, const Vector2d &pos_, const Vector2d &impulse_,
                   double mass_, double radius_) :
    Component(object_), pos{pos_}, impulse{impulse_}, mass{mass_}, radius{radius_} {}

bool PhysComp::testForCollision(const PhysComp &other) const {
    double goalDist = radius + other.radius;

    return (getPos() - other.getPos()).magnitude() <= goalDist * goalDist;
}

void PhysComp::respondToCollision(PhysComp &other) {
    assert(testForCollision(other));

    Vector2d axis = other.getPos() - getPos();

    if (axis.isZero()) {
        DBG("Precise central collision - ignoring");
        return;
    }

    // double oldEnergy = impulse.magnitude() / mass + other.impulse.magnitude() / other.mass;

    Vector2d deltaImpulseProj = (impulse.projected(axis) * other.mass -
                           other.impulse.projected(axis) *       mass ) * 2 / (mass + other.mass);

          impulse -= deltaImpulseProj;
    other.impulse += deltaImpulseProj;

    double clipping = radius + other.radius - (ACCOUNT_FOR_SKIPPED_MOVEMENT ? 2.d : 1.d) * axis.length();
    axis.normalize();  // Not gonna need the length anymore, presumably

    // We now shift the balls as if they had collided in time,
    // assuming the shift to be inversely proportional to their masses
    if (clipping > 0.d) {
        double coeff = mass / (mass + other.mass);

              getPos() -= axis * clipping * (1 - coeff);
        other.getPos() += axis * clipping * coeff;
    }

    // DBG("%lg -> %lg", oldEnergy, impulse.magnitude() / mass + other.impulse.magnitude() / other.mass);
}

bool PhysComp::testForBorderCollision(const Border &border) const {
    return border.getDist(pos) <= radius;
}

void PhysComp::respondToBorderCollision(Border &border) {
    impulse.mirrorAlong(border.getDirIn());

    Vector2d refPoint = border.getPoint() + border.getDirIn() * radius;

    if constexpr (ACCOUNT_FOR_SKIPPED_MOVEMENT) {
        // Not -ed because rvalue
        pos = refPoint + (pos - refPoint).mirrorAlong(border.getDirIn());
    } else {
        // Not -ed because rvalue
        pos = refPoint + (pos - refPoint).project(border.getLine());
    }
}

void PhysComp::update(double deltaT, bool /*inGracePeriod*/) {
    assert(!abel::isZero(mass));
    pos += impulse / mass * deltaT;
}

bool PhysComp::updatePair(PhysComp &other, bool inGracePeriod) {
    if (inGracePeriod)
        return false;

    if (testForCollision(other)) {
        respondToCollision(other);
        return true;
    }

    return false;
}

PhysComp *PhysComp::copy() const {
    return new PhysComp(object, pos, impulse, mass, radius);
}

Molecule &PhysComp::absorb(PhysComp &other, bool reflectable) {
    // We give subclasses a chance to extend absorption their way
    if (reflectable && typeid(other) != typeid(PhysComp))
        return other.absorb(*this, false);

    mass    += other.mass;
    impulse  = impulse + other.impulse;
    radius   = std::hypot(radius, other.radius);
    pos      = (pos + other.pos) / 2;

    other.getObj().markDead();

    return *object;
}

abel::gui::Color PhysComp::getTypeColor() const noexcept {
    return abel::gui::Color::ORANGE;
}

void PhysComp::scaleTrue(double coeff) {
    // Coeff is a characteristic of mass, and some values are affected
    // by the corresponding linear proportion
    double linCoeff = std::sqrt(coeff);

    // pos stays the same
    impulse *= coeff;
    radius  *= linCoeff;
    mass    *= coeff;
}

void PhysComp::dump() const noexcept {
    dump("PhysComp");
}

void PhysComp::dump(const char *clsName) const noexcept {
    Vector2d velocity{std::numeric_limits<Vector2d::TYPE>::infinity()};

    if (!abel::isZero(mass)) {
        velocity = impulse / mass;
    }

    DBG("%s (%p):\n"
        "  pos             = (%lg, %lg)\n"
        "  impulse         = (%lg, %lg)\n"
        "  impulse abs     = %lg\n"
        "  velocity        = (%lg, %lg)\n"
        "  velocity abs    = %lg\n"
        "  mass            = %lg\n"
        "  radius          = %lg\n"
        "  kinetic energy  = %lg",
        clsName, this,
        pos.x(), pos.y(),
        impulse.x(), impulse.y(),
        impulse.length(),
        velocity.x(), velocity.y(),
        velocity.length(),
        mass,
        radius,
        getEnergy());
}

Vector2d PhysComp::getCOMWith(const PhysComp &other) const {
    return (mass * pos + other.mass * other.pos) / (mass + other.mass);
}

void GravityPhysComp::update(double deltaT, bool inGracePeriod) {
    PhysComp::update(deltaT, inGracePeriod);

    impulse += GRAVITY_DIR * GRAVITY_COEFF * mass * deltaT;
}

GravityPhysComp *GravityPhysComp::copy() const {
    return new GravityPhysComp(object, pos, impulse, mass, radius);
}

abel::gui::Color GravityPhysComp::getTypeColor() const noexcept {
    return abel::gui::Color::GREEN;
}

void GravityPhysComp::dump() const noexcept {
    PhysComp::dump("GravityPhysComp");
}

// TODO: In perspective, dispatch table. For now, an if
void MagneticPhysComp::respondToCollision(PhysComp &other) {
    PhysComp::respondToCollision(other);

    if (typeid(other) != typeid(MagneticPhysComp))
        return;

    MagneticPhysComp &otherM = dynamic_cast<MagneticPhysComp &>(other);

    double totalCharge = charge + otherM.charge;
    double coeff = mass / (mass + otherM.mass);

           charge = totalCharge * coeff;
    otherM.charge = totalCharge * (1 - coeff);
}

void MagneticPhysComp::update(double deltaT, bool inGracePeriod) {
    PhysComp::update(deltaT, inGracePeriod);

    impulse += magneticForce * deltaT;
    magneticForce = Vector2d{0};
}

bool MagneticPhysComp::updatePair(PhysComp &other, bool inGracePeriod) {
    bool result = PhysComp::updatePair(other, inGracePeriod);

    if (inGracePeriod)
        return result;

    if (typeid(other) != typeid(MagneticPhysComp))
        return result;

    MagneticPhysComp &otherM = dynamic_cast<MagneticPhysComp &>(other);

    Vector2d dist = otherM.pos - pos;

    if (dist.isZero())
        return result;  // There's nothing to do in this case, I assume

    Vector2d force = -MAGNETISM_COEFF * (charge * otherM.charge / dist.magnitude()) * dist.normalized();
    //DBG("Force: %lg", force.length());

           magneticForce += force;
    otherM.magneticForce -= force;

    return result;
}

MagneticPhysComp *MagneticPhysComp::copy() const {
    return new MagneticPhysComp(object, pos, impulse, mass, radius, charge);
}

// We ignore reflectability, since we want this class to take priority
// and cannot have combinations
Molecule &MagneticPhysComp::absorb(PhysComp &other, bool) {
    Molecule &result = PhysComp::absorb(other, false);
    assert(&result == object);  // Because we forbade switching sides

    if (typeid(other) != typeid(MagneticPhysComp))
        return result;

    const MagneticPhysComp &otherM = dynamic_cast<const MagneticPhysComp &>(other);

    charge += otherM.charge;

    return result;
}

abel::gui::Color MagneticPhysComp::getTypeColor() const noexcept {
    static constexpr double MAX_CHARGE = 0.05d;

    Color base{};
    if (abel::sgnDbl(charge) > 0) {
        base = abel::gui::Color::RED;
    } else {
        base = abel::gui::Color::BLUE ;
    }

    return base * std::abs(charge) / MAX_CHARGE;
}

void MagneticPhysComp::scaleTrue(double coeff) {
    PhysComp::scaleTrue(coeff);

    charge *= coeff;
}

void MagneticPhysComp::dump() const noexcept {
    PhysComp::dump("MagneticPhysComp");

    DBG("...\n"
        "  charge          = %lg\n"
        "  cached force    = (%lg, %lg)\n"
        "  c. force (abs)  = %lg",
        charge,
        magneticForce.x(), magneticForce.y(),
        magneticForce.length());
}

