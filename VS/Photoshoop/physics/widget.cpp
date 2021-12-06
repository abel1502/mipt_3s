#include <AGF/llgui.h>
#include "widget.h"


MoleculesWidget::MoleculesWidget(Widget *parent_, const Rect<double> &region_) :
    Widget(parent_, region_) {

    /*Molecule *cur = nullptr;
    cur = &manager.addMolecule(Vector2d{1, 1}, 1.d, Molecule::P_BALL);
    //cur->getComp<PhysComp>().getImpulse() = Vector2d{10, 1};
    cur->getComp<PhysComp>().getImpulse() = Vector2d{20, 1};

    //cur = &manager.addMolecule(Vector2d{-8, 1}, 2.d, Molecule::P_SQUARE);
    //cur->getComp<PhysComp>().getImpulse() = Vector2d{-50, -50};
    cur = &manager.addMolecule(Vector2d{21, 3}, 2.d, Molecule::P_SQUARE);
    cur->getComp<PhysComp>().getImpulse() = Vector2d{-5, 7};

    //manager.addMolecule(Vector2d{10, 2}, 2.d, Molecule::P_BALL).getComp<PhysComp>().getImpulse() = Vector2d{0, 0};*/

    static constexpr unsigned MOLECULES_CNT = 20;

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

    MyApp::getInstance().sigTick += [this](double deltaT) {
        manager.tick(deltaT);

        return false;
    };
}


EVENT_HANDLER_IMPL(MoleculesWidget, abel::gui::Render) {
    EventStatus status = Base::dispatchEvent(event);

    if (!status.shouldHandle(status.NODE)) {
        return status;
    }

    manager.render();
    event.target.embed(region, *texture);

    return status;
}
