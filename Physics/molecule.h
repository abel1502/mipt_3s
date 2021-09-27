#ifndef MOLECULE_H
#define MOLECULE_H

#include <ACL/general.h>
#include <ACL/math/vector.h>

#include "components/physics.h"
#include "components/render.h"
#include "components/chemistry.h"


using abel::math::Vector2d;


class Molecule {
public:
    enum Preset {
        P_BALL,
        P_SQUARE,
    };


    Molecule();

    Molecule(const Vector2d &pos, Preset preset);

    void update(double deltaT);

    /// Not defined for the generic case, since it only features present components
    template <typename T>
    T &getComp();

protected:
    Vector2d pos;
    PhysComp *phys;
    RenderComp *render;
    ChemComp *chem;

};


template <> inline PhysComp   &Molecule::getComp<PhysComp>  () { return *phys;   }
template <> inline RenderComp &Molecule::getComp<RenderComp>() { return *render; }
template <> inline ChemComp   &Molecule::getComp<ChemComp>  () { return *chem;   }


#endif // MOLECULE_H
