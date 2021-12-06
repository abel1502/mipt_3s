#pragma once

#include <ACL/general.h>
#include <AGF/widget.h>
#include <ACL/unique_ptr.h>
#include "../app.h"
#include "manager.h"
#include "molecule.h"


using abel::gui::Rect;
using abel::gui::Color;
using abel::math::Vector2d;


class MoleculesWidget : public abel::gui::Widget {
public:
    using Base = Widget;
    EVENT_HANDLER_USING(Base);

    MoleculesWidget(Widget *parent_, const Rect<double> &region_);

    EVENT_HANDLER_OVERRIDE(abel::gui::Render);

protected:
    abel::unique_ptr<abel::gui::Texture> texture{new Texture(region.getDiag())};
    MoleculeManager manager{*texture};

};
