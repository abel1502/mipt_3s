#ifndef AGF_WIDGETS_BUTTON_H
#define AGF_WIDGETS_BUTTON_H

#include <AGF/widget.h>
#include <AGF/widgets/rectangle.h>


namespace abel::gui::widgets {


class SimpleButton : public Widget {
public:
    EVENT_HANDLER_USING(Widget)

    static constexpr Color COL_DEFAULT{0.9f};
    static constexpr Color COL_PRESSED{0.6f};


    Signal<bool ()> sigClick{};


    SimpleButton(Widget *parent_, const Rect<double> &region_, const char *text_);

    #define EVENTS_DSL_ITEM_(NAME) \
        EVENT_HANDLER_OVERRIDE(NAME);

    #include <AGF/events.dsl.h>

    ~SimpleButton();

protected:
    unique_ptr<Rectangle> body{nullptr};

private:  // TODO: maybe keep in protected instead
    template <typename T>
    EventStatus _processEvent(const T &event) {
        EventStatus status = EVENT_HANDLER_CALL_BASE(Widget, event);

        if (!status.shouldHandle(status.NODE))
            return status.update();

        assert(body);
        status = EVENT_HANDLER_CALL_INST(body, event);

        // TODO: Lots of encapsulation!
        if (status == E_STOP_GLOBAL)
            return status;

        return status.update();
    }

};


}


#endif // AGF_WIDGETS_BUTTON_H
