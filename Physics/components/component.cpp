#include "component.h"

#include "../molecule.h"


Component::Component(Molecule *object_) noexcept :
    object{object_} {

    // Can't assert this anymore, since random molecule creation involves having this at nullptr and then updating it
    // assert(object_);
}

void Component::updateObj(Molecule *new_object) noexcept {
    object = new_object;
}
