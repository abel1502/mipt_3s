#ifndef AGF_WIDGETS_BUTTON_H
#define AGF_WIDGETS_BUTTON_H

#include <AGF/widget.h>
#include <AGF/widgets/static_group.h>
#include <AGF/widgets/rectangle.h>
#include <AGF/widgets/label.h>


namespace abel::gui::widgets {


class Button : public StaticGroup<Label, Rectangle> {
public:
    using Base = StaticGroup<Label, Rectangle>;
    EVENT_HANDLER_USING(Base)

    static constexpr Color COL_DEFAULT{0.9f};
    static constexpr Color COL_PRESSED{0.6f};


    Signal<bool ()> sigClick{};


    Button(Widget *parent_, const Rect<double> &region_, const char *text_);

    EVENT_HANDLER_OVERRIDE(MouseClick)

protected:
    bool isDown = false;

    SGRP_DECLARE_BINDING_T(body,  Rectangle)
    SGRP_DECLARE_BINDING_T(label, Label)

    void onMouseDown(const MouseClickEvent &event, bool hit);
    void onMouseUp  (const MouseClickEvent &event, bool hit);

};


}


#endif // AGF_WIDGETS_BUTTON_H
