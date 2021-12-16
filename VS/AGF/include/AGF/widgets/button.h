#ifndef AGF_WIDGETS_BUTTON_H
#define AGF_WIDGETS_BUTTON_H

#include <AGF/widget.h>
#include <AGF/widgets/static_group.h>
#include <AGF/widgets/label.h>
#include <AGF/helpers/mouse_tracker.h>
#include <AGF/helpers/animation.h>


namespace abel::gui::widgets {


class Button : public StaticGroup<Label> {
public:
    using Base = StaticGroup<Label>;
    EVENT_HANDLER_USING(Base);


    Signal<bool ()> sigClick{};


    Button(Widget *parent_, const Rect<double> &region_, const char *text_);

    EVENT_HANDLER_OVERRIDE(MouseClick);
    EVENT_HANDLER_OVERRIDE(MouseMove);
    EVENT_HANDLER_OVERRIDE(MouseScroll);
    EVENT_HANDLER_OVERRIDE(Render);
    EVENT_HANDLER_OVERRIDE(VisibilityUpdate);

    inline const Label &getLabel() const;
    inline       Label &getLabel()      ;

protected:
    MouseTracker mt{this};
    Animation anim{};

    SGRP_DECLARE_BINDING_T(label, Label);

    EventStatus renderCustomized(const RenderEvent &event, Style::Element elem, bool ignoreLabel = false);

};


inline const Label &Button::getLabel() const {
    return label();
}

inline Label &Button::getLabel() {
    return label();
}


}


#endif // AGF_WIDGETS_BUTTON_H
