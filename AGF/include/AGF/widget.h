#ifndef AGF_WIDGET_H
#define AGF_WIDGET_H


#include <ACL/general.h>
#include <ACL/signal.h>
#include <AGF/events.h>
#include <AGF/widget_pos.h>
#include <AGF/llgui_pre.h>
#include <ACL/unique_ptr.h>

/*
#define EVENT_HANDLER_NAME(EVENT_NAME) \
    handler_on##EVENT_NAME
*/

#define EVENT_HANDLER_USING(BASE_CLS) \
    using BASE_CLS::dispatchEvent;

#define EVENT_HANDLER_METHOD(EVENT_NAME) \
    EventStatus dispatchEvent(const EVENT_CLS_NAME(EVENT_NAME) &event)

#define EVENT_HANDLER_METHOD_SCOPED(CLS, EVENT_NAME) \
    Widget::EventStatus CLS::dispatchEvent(const EVENT_CLS_NAME(EVENT_NAME) &event)

#define EVENT_HANDLER_DECL(EVENT_NAME) \
    virtual EVENT_HANDLER_METHOD(EVENT_NAME);

#define EVENT_HANDLER_OVERRIDE(EVENT_NAME) \
    virtual EVENT_HANDLER_METHOD(EVENT_NAME) override;

#define EVENT_HANDLER_IMPL(CLS, EVENT_NAME) \
    EVENT_HANDLER_METHOD_SCOPED(CLS, EVENT_NAME)

#define EVENT_HANDLER_CALL_BASE(BASE_CLS, EVENT_OBJ) \
    BASE_CLS::dispatchEvent(EVENT_OBJ)

#define EVENT_HANDLER_CALL_INST(INSTANCE, EVENT_OBJ) \
    (INSTANCE)->dispatchEvent(EVENT_OBJ)


namespace abel::gui {


class Widget {
public:
    #pragma pack(push,1)
    struct EventStatus {
        enum Level : unsigned char {
            CONT = 0,
            NODE = 1,
            SIBL = 2,
            TREE = 3,
        };

        Level stopLvl : 2;
        bool  handled : 1;

        constexpr EventStatus() :
            stopLvl{0}, handled{false} {}

        constexpr EventStatus(Level stopLvl_) :
            stopLvl{stopLvl_}, handled{true} {}

        static constexpr EventStatus skip()            { return EventStatus{     }; }
        static constexpr EventStatus done()            { return EventStatus{CONT }; }
        static constexpr EventStatus stop(Level level) { return EventStatus{level}; }

        constexpr bool shouldHandle(Level level) { return level > stopLvl; }

        constexpr EventStatus &update(bool handled_ = false) {
            handled |= handled_;
            if (stopLvl <= SIBL)
                stopLvl = CONT;
            return *this;
        }

    };
    #pragma pack(pop)

    static_assert(sizeof(EventStatus) <= 4);


    Widget() = delete;

    virtual ~Widget() = default;

    #define EVENTS_DSL_ITEM_(NAME) EVENT_HANDLER_DECL(NAME)
    #include <AGF/events.dsl.h>

    constexpr const Rect<double> &getRegion() const { return region; }
    constexpr       Rect<double> &getRegion()       { return region; }

protected:
    Widget *parent;
    Rect<double> region;
    bool visible = true;


    Widget(Widget *parent_, const Rect<double> &region_);

};


}


#endif // AGF_WIDGET_H
