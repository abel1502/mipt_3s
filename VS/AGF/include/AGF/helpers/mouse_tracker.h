#ifndef AGF_HELPERS_MOUSE_TRACKER_H
#define AGF_HELPERS_MOUSE_TRACKER_H

#include <ACL/general.h>
#include <AGF/widget.h>
#include <AGF/mouse.h>
#include <ACL/signal.h>


namespace abel::gui {


class MouseClickTracker {
public:
    Signal<bool (const MouseClickEvent  &event)> sigDown  {};
    Signal<bool (const MouseClickEvent  &event)> sigUp    {};
    Signal<bool (const MouseClickEvent  &event)> sigClick {};
    Signal<bool (const MouseScrollEvent &event)> sigScroll{};
    // TODO: Implement
    // Signal<bool (const MouseClickEvent &event)> sigDoubleClick{};


    inline MouseClickTracker(Widget *widget_) :
        widget{widget_} {

        assert(widget);
    }

    Widget::EventStatus processEvent(const MouseClickEvent &event,
                                     Widget::EventStatus baseStatus);

    Widget::EventStatus processEvent(const MouseScrollEvent &event,
                                     Widget::EventStatus baseStatus);

    constexpr bool isDown(MouseBtn btn) const {
        assert((unsigned)btn < MOUSE_BTN_CNT);

        return isDown_[(unsigned)btn];
    }

    inline Style::ElementState getElemState() const {
        lastState = Style::ELS_NORMAL;

        if (isDown(MouseBtn::Left)) {
            lastState = Style::ELS_PRESSED;
        }

        return lastState;
    }

    inline Style::ElementState getLastElemState() const {
        return lastState;
    }

    bool isMouseCaptured() const;

    void captureMouse();

    void releaseMouse();

    constexpr bool isScrollable() const {
        return scrollable;
    }

    constexpr void setScrollable(bool value = true) {
        scrollable = value;
    }

protected:
    static constexpr unsigned MOUSE_BTN_CNT = 3;
    static_assert((unsigned)MouseBtn::Left   < MOUSE_BTN_CNT &&
                  (unsigned)MouseBtn::Middle < MOUSE_BTN_CNT &&
                  (unsigned)MouseBtn::Right  < MOUSE_BTN_CNT);


    Widget *widget;
    bool isDown_[MOUSE_BTN_CNT] = {};
    mutable Style::ElementState lastState = Style::ELS_NORMAL;
    bool scrollable = false;


    constexpr void isDown(MouseBtn btn, bool value) {
        assert((unsigned)btn < MOUSE_BTN_CNT);

        isDown_[(unsigned)btn] = value;
    }

};


class MouseTracker : public MouseClickTracker {
public:
    Signal<bool (bool state)>                                 sigHoverStateChange{};
    Signal<void (MouseBtn btn, MouseAttrs attrs, bool state)> sigDragStateChange {};
    Signal<bool (MouseBtn btn, const MouseMoveEvent &event)>  sigDrag            {};


    inline MouseTracker(Widget *widget_) :
        MouseClickTracker(widget_) {

        sigScroll += [this](const MouseScrollEvent &event) {
            updateHovered();

            return false;
        };
    }

    using MouseClickTracker::processEvent;

    Widget::EventStatus processEvent(const MouseClickEvent &event,
                                     Widget::EventStatus baseStatus);

    Widget::EventStatus processEvent(const MouseMoveEvent &event,
                                     Widget::EventStatus baseStatus);

    constexpr bool isHovered() const {
        return isHovered_;
    }

    constexpr bool isDragged(MouseBtn btn) const {
        return isDown(btn);
    }

    void updateHovered() const;

    inline Style::ElementState getElemState() const {
        lastState = Style::ELS_NORMAL;

        updateHovered();

        if (isDown(MouseBtn::Left)) {
            lastState = Style::ELS_PRESSED;
        } else if (isHovered()) {
            lastState = Style::ELS_HOVERED;
        }

        return lastState;
    }

protected:
    mutable bool isHovered_ = false;
    bool lastMoveType = false;  // <=> from

    constexpr void isHovered(bool value) {
        isHovered_ = value;
    }

};


}


#endif // AGF_HELPERS_MOUSE_TRACKER_H
