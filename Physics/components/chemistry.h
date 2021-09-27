#ifndef COMPONENTS_CHEMISTRY_H
#define COMPONENTS_CHEMISTRY_H

#include <ACL/general.h>

#include "component.h"


class ChemComp : public Component {
public:
    DECLARE_ERROR(error, abel::error);


    ChemComp(Molecule &object_, unsigned charge_ = 1);

    void maybeInteract(ChemComp &other);

    void interact(ChemComp &other);

protected:
    unsigned charge;

};


#endif // COMPONENTS_CHEMISTRY_H


