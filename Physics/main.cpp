#include "gui/gui.h"
#include <ACL/general.h>
#include <typeinfo>
#include "manager.h"
#include "molecule.h"


/// Has to be employed because of gui frameworks' implications
MAIN_DECL {
    abel::verbosity = 2;
    abel::srand(444);

    MAIN_TRY(
        Window window{};
        Texture texture{window};

        MoleculeManager manager{texture};

        /*Molecule *cur = nullptr;
        cur = &manager.addMolecule(Vector2d{1, 1}, 1.d, Molecule::P_BALL);
        //cur->getComp<PhysComp>().getImpulse() = Vector2d{10, 1};
        cur->getComp<PhysComp>().getImpulse() = Vector2d{20, 1};

        //cur = &manager.addMolecule(Vector2d{-8, 1}, 2.d, Molecule::P_SQUARE);
        //cur->getComp<PhysComp>().getImpulse() = Vector2d{-50, -50};
        cur = &manager.addMolecule(Vector2d{21, 3}, 2.d, Molecule::P_SQUARE);
        cur->getComp<PhysComp>().getImpulse() = Vector2d{-5, 7};

        //manager.addMolecule(Vector2d{10, 2}, 2.d, Molecule::P_BALL).getComp<PhysComp>().getImpulse() = Vector2d{0, 0};*/

        static constexpr unsigned MOLECULES_CNT = 25;

        for (unsigned i = 0; i < MOLECULES_CNT; ++i) {
            //manager.addRandomMolecule();
            manager.addRandomMolecule(Molecule::P_BALL);

            /*bool good = false;

            while (!good) {
                Molecule &mol = manager.addRandomMolecule(Molecule::P_BALL);

                good = typeid(mol.getComp<PhysComp>()) == typeid(MagneticPhysComp);

                if (!good) {
                    mol.markDead();
                }
            }*/
        }

        /*manager.addMolecule(Vector2d{0, 0}, 1.d, Molecule::P_BALL);
        manager.addMolecule(Vector2d{0, 0.9}, 1.d, Molecule::P_BALL);*/

        MAIN_EVENT_LOOP(
            manager.tick(DELTA_T);
            DBG("Tick completed");

            window.render(texture);
            window.update();

            DBG("Tick in %lg", DELTA_T);
        , 1)

        return 0;
    )
}
