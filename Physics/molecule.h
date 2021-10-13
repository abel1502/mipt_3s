#ifndef MOLECULE_H
#define MOLECULE_H

#include <ACL/general.h>
#include <ACL/math/vector.h>
#include <cassert>
#include <type_traits>

#include "components/physics.h"
#include "components/render.h"
#include "components/chemistry.h"
#include "borders.h"


using abel::math::Vector2d;


class MoleculeManager;
class Texture;
class Coords;

class Molecule {
public:
    // Sadly, a proxy like this is needed, since MoleculeManager's methods cannot be forward-declared here
    class ManagerProxy {
    protected:
        friend class MoleculeManager;

        static Molecule copy(const Molecule &self);

        static void updateComponentBackrefs(Molecule &self);
    };

    static constexpr bool VALIDATE_PTRS = true;

    enum Preset {
        P_BALL,
        P_SQUARE,
    };

    struct Flags {
        bool inactive : 1;
        bool dead     : 1;
    };


    Molecule(MoleculeManager *manager_, const Vector2d &pos = Vector2d{0, 0}, double size = 1.d, Preset preset = P_BALL);

    /// This will copy the components
    Molecule(MoleculeManager *manager_, const PhysComp &phys_, const RenderComp &rend_, const ChemComp &chem_);

    /// This will take over the components.
    /// They MUST be in dynamic memory, allocated with new
    Molecule(MoleculeManager *manager_, PhysComp *phys_, RenderComp *rend_, ChemComp *chem_);

    Molecule(Molecule &&other);
    Molecule &operator=(Molecule &&other);

    ~Molecule() noexcept;

    void update(double deltaT);

    void updatePair(Molecule &other);

    void render(Texture &target, const Coords &coords);

    // The macro just acts a glorified comment
    #define SOURCE_COMP_(NAME)

    SOURCE_COMP_(PhysComp)
    void onBorderCollision(const Border &border);

    SOURCE_COMP_(ChemComp)
    Molecule &onChemAbsorb(Molecule &other, unsigned potency);

    #undef SOURCE_COMP_

    void renderBecomeCircle();

    void renderBecomeSquare();

    /// Scales every aspect of the molecule as if it became `coeff` times larger in mass,
    /// while retaining all other properties of the material, shape & etc
    void scaleTrue(double coeff);

    /// Not defined for the generic case, since it only features present components
    template <typename T>
    T &getComp();

    /// Same
    template <typename T>
    constexpr const T &getComp() const;

    constexpr const Vector2d &getPos() const { validate(); return phys->getPos(); }
    constexpr       Vector2d &getPos()       { validate(); return phys->getPos(); }

    constexpr void markDead    (bool dead     = true) noexcept { flags.dead     = dead;     }
    constexpr void markInactive(bool inactive = true) noexcept { flags.inactive = inactive; }

    constexpr bool isDead()     const { return flags.dead;     }
    constexpr bool isInactive() const { return flags.inactive; }

    constexpr const MoleculeManager &getManager() const { validate(); return *manager; }
    constexpr       MoleculeManager &getManager()       { validate(); return *manager; }

    void dump() const noexcept;

protected:
    MoleculeManager *manager;

    /// Cannot be nullptr unless the molecule is malformed
    // (although the destructor should obviously be able to
    //  handle even the malformed molecules)
    PhysComp   *phys;
    RenderComp *rend;
    ChemComp   *chem;

    Flags flags;

    void destroy() noexcept;

    inline constexpr void validate() const {
        if constexpr (VALIDATE_PTRS) {
            assert(manager);
            assert(phys);
            assert(rend);
            assert(chem);
        }
    }

    friend class ManagerProxy;

    // Only to be called by the manager, since these methods don't add the molecule to the list
    Molecule(const Molecule &other);
    Molecule &operator=(const Molecule &other);

    // Not to be used at all, hopefully, but needed for std::swap, because otherwise it invokes the move-ctor on garbage
    Molecule() :
        manager{nullptr}, phys{nullptr}, rend{nullptr}, chem{nullptr}, flags{} {}

};


template <> constexpr       PhysComp   &Molecule::getComp<PhysComp>  ()       { validate(); return *phys; }
template <> constexpr const PhysComp   &Molecule::getComp<PhysComp>  () const { validate(); return *phys; }
template <> constexpr       RenderComp &Molecule::getComp<RenderComp>()       { validate(); return *rend; }
template <> constexpr const RenderComp &Molecule::getComp<RenderComp>() const { validate(); return *rend; }
template <> constexpr       ChemComp   &Molecule::getComp<ChemComp>  ()       { validate(); return *chem; }
template <> constexpr const ChemComp   &Molecule::getComp<ChemComp>  () const { validate(); return *chem; }


#endif // MOLECULE_H
