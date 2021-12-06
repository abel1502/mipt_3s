#ifndef MANAGER_H
#define MANAGER_H

#include "borders.h"
#include "molecule.h"
#include <ACL/gui/coords.h>
#include <ACL/math/vector.h>
#include <ACL/vector.h>
#include <ACL/general.h>
#include <AGF/llgui_pre.h>


using abel::gui::Texture;
using abel::gui::Coords;

class MoleculeManager {
public:
    DECLARE_ERROR(error, abel::error);

    //static constexpr double RENDER_INTERVAL = 0.05;
    static constexpr double RENDER_INTERVAL = 0;


    MoleculeManager(Texture &target_);

    // Has an explicit specialization to move molecules,
    // but for copying, use copyMolecule instead.
    template <typename ... Ts>
    Molecule &addMolecule(Ts &&... args) {
        if (newMolecules.appendEmplace(this, std::forward<Ts>(args)...)) {
            updateBackrefs();
        }

        return newMolecules[-1];
    }

    Molecule &addRandomMolecule();
    Molecule &addRandomMolecule(Molecule::Preset base);

    Molecule &copyMolecule(const Molecule &original);

    void explodeClones(Molecule &mol, unsigned n, double impulse);

    void tick(double deltaT);

    void render();

    constexpr const Texture &getTarget() const { return target; }
    constexpr       Texture &getTarget()       { return target; }
    constexpr const Coords  &getCoords() const { return coords; }
    constexpr       Coords  &getCoords()       { return coords; }
    constexpr const abel::vector<Border>   &getBorders  () const { return borders;   }
    constexpr       abel::vector<Border>   &getBorders  ()       { return borders;   }
    constexpr const abel::vector<Molecule> &getMolecules() const { return molecules; }
    constexpr       abel::vector<Molecule> &getMolecules()       { return molecules; }


protected:
    static constexpr double HALF_VIRT_WIDTH = 40. * 2;
    static constexpr unsigned BORDERS_CNT = 4;

    static constexpr abel::math::Vector2d BORDER_DIRS[BORDERS_CNT] = {
        {-1, 0}, {0, -1}, {1, 0}, {0, 1}
    };


    double renderTimer;
    Texture &target;
    Coords coords;

    abel::vector<Border>   borders;
    abel::vector<Molecule> molecules;
    abel::vector<Molecule> newMolecules;


    inline bool needsRender(double deltaT = 0) {
        renderTimer -= deltaT;

        return abel::sgnDbl(renderTimer) <= 0;
    }

    inline void haveRendered() {
        renderTimer = RENDER_INTERVAL;
    }

    unsigned refToIdx(const Molecule &molecule);

    void updateBackrefs();

    inline void deleteMolecule(Molecule &molecule) {
        deleteMolecule(refToIdx(molecule));
    }

    void deleteMolecule(unsigned idx);

    void processFlags();

};


#endif // MANAGER_H
