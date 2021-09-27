#ifndef COMPONENTS_COMPONENT_H
#define COMPONENTS_COMPONENT_H


class Molecule;

class Component {
public:
    Component(Molecule &object_) noexcept;

    /// Not virtual, no implementation - just an interface guideline
    Component *copy();

protected:
    Molecule &object;

};


#endif // COMPONENTS_COMPONENT_H
