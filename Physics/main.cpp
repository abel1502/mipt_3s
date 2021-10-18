#include <AGF/llgui.h>
#include <ACL/general.h>
#include <typeinfo>
#include "manager.h"
#include "molecule.h"


using abel::gui::Vector2i;

int main() {
    abel::verbosity = 2;
    abel::srand(444);

    MAIN_TRY(
        abel::gui::Window window{};
        abel::gui::Texture texture{400, 600};

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

        static constexpr unsigned MOLECULES_CNT = 200;

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

        double deltaT = 0.0d;

        // 5 == txFramesToAverage, but the constant doesn't exist.
        for (unsigned i = 0; i < 5; ++i) {
            txGetFPS();
        }

        DBG("Starting");

        while (!txGetAsyncKeyState(VK_ESCAPE)) {
            deltaT = std::fmin(1.d / txGetFPS(), 1.d);

            manager.tick(deltaT);
            DBG("Molecular tick completed");

            window.renderAt(Vector2i{400, 0}, texture);
            window.update();
            DBG("Render completed");

            DBG("Tick in %lg", deltaT);

            txSleep(1);
        }

        return 0;
    )
}
