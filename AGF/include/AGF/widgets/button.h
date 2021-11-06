#ifndef AGF_WIDGETS_BUTTON_H
#define AGF_WIDGETS_BUTTON_H

#include <AGF/widget.h>
#include <AGF/widgets/static_group.h>
#include <AGF/widgets/rectangle.h>
#include <AGF/widgets/label.h>


namespace abel::gui::widgets {


class SimpleButton : public StaticGroup<Rectangle, Label> {
public:
    using Base = StaticGroup<Rectangle, Label>;
    EVENT_HANDLER_USING(Base)

    static constexpr Color COL_DEFAULT{0.9f};
    static constexpr Color COL_PRESSED{0.6f};


    Signal<bool ()> sigClick{};


    SimpleButton(Widget *parent_, const Rect<double> &region_, const char *text_);

    EVENT_HANDLER_OVERRIDE(MouseClick)

protected:
    // TODO: Wrappers for body and label?

    void onMouseDown(const EVENT_CLS_NAME(MouseClick) &event);
    void onMouseUp  (const EVENT_CLS_NAME(MouseClick) &event);

};


}


#endif // AGF_WIDGETS_BUTTON_H
