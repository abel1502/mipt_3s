#ifndef COMPONENTS_COMPONENT_H
#define COMPONENTS_COMPONENT_H

#include <cassert>


class Molecule;

/// Warning: not suitable for actual use, since this is NOT a polymorphic base class.
/// Only provides some common methods.
/// Preferable this way, since some components may not need to be polymorphic, so they could be sped up this way
class Component {
public:
    Component(Molecule *object_) noexcept;

    constexpr const Molecule &getObj() const noexcept { assert(object); return *object; }
    constexpr       Molecule &getObj()       noexcept { assert(object); return *object; }

    void updateObj(Molecule *new_object) noexcept;

    /// ================================================================================
    /// After this point functions aren't virtual, nor implemented - just interface guidelines
    /// ================================================================================
    Component *copy() const;

    void scaleTrue(double coeff);

protected:
    Molecule *object;

};


#endif // COMPONENTS_COMPONENT_H
