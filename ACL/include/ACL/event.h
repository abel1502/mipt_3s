#ifndef ACL_EVENT_H
#define ACL_EVENT_H

#include <ACL/general.h>
#include <ACL/vector.h>


namespace abel {


template <typename T>
class Event;


template <typename ... As>
class Event<void (As...)> {
public:
    using callback_t = void (As...);


    void invoke(As ... args) {
        beingInvoked = true;

        for (callback_t *cb : callbacks) {
            (*cb)(args...);
        }

        beingInvoked = false;

        processUnsubQueue();
    }

    #define ACL_EVENT_TPL_GUARD
    #include <ACL/event.tpl.h>
    #undef ACL_EVENT_TPL_GUARD

};


template <typename ... As>
class Event<bool (As...)> {
public:
    using callback_t = bool (As...);


    void invoke(As ... args) {
        beingInvoked = true;

        for (callback_t *cb : callbacks) {
            if ((*cb)(args...)) {
                queueUnsubscribe(cb);
            }
        }

        beingInvoked = false;

        processUnsubQueue();
    }

    #define ACL_EVENT_TPL_GUARD
    #include <ACL/event.tpl.h>
    #undef ACL_EVENT_TPL_GUARD

};


}


#endif // ACL_EVENT_H
