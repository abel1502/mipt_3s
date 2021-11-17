#ifndef AGF_WIDGETS_BUTTON_H
#define AGF_WIDGETS_BUTTON_H

#include <AGF/widget.h>
#include <AGF/widgets/static_group.h>
#include <AGF/widgets/rectangle.h>
#include <AGF/widgets/label.h>


namespace abel::gui::widgets {


class Button : public StaticGroup<Label> {
public:
    using Base = StaticGroup<Label>;
    EVENT_HANDLER_USING(Base)


    Signal<bool ()> sigClick{};


    Button(Widget *parent_, const Rect<double> &region_, const char *text_);

    EVENT_HANDLER_OVERRIDE(MouseClick)
    EVENT_HANDLER_OVERRIDE(MouseMove)
    EVENT_HANDLER_OVERRIDE(Render)

protected:
    bool isDown = false;
    bool isHovered = false;

    SGRP_DECLARE_BINDING_T(label, Label)

    void onMouseDown(const MouseClickEvent &event, bool hit);
    void onMouseUp  (const MouseClickEvent &event, bool hit);

    EventStatus renderCustomized(const RenderEvent &event, Style::Element elem, bool ignoreLabel);

};


}


#endif // AGF_WIDGETS_BUTTON_H
