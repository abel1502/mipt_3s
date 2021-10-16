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


    Event() :
        callbacks{} {}

    void fire(As ... args) const {
        for (const callback_t *cb : callbacks) {
            (*cb)(args...);
        }
    }

    void subscribe(const callback_t *cb) {
        if (hasCallback(cb))
            return;

        callbacks.append(cb);
    }

    void unsubscribe(const callback_t *cb) {
        unsigned i = findCallback(cb);
        if (i == BAD_IDX)
            return;

        std::swap(callbacks[i], callbacks[-1]);

        callbacks.pop();
    }

    void clear() {
        callbacks.clear();
    }

    bool hasCallback(const callback_t *cb) const {
        for (auto curCb : callbacks) {
            if (curCb == cb) {
                return true;
            }
        }

        return false;
    }

protected:
    static constexpr unsigned BAD_IDX = -1u;


    vector<const callback_t *> callbacks;  // TODO: set


    // Returns callback index, or BAD_IDX on error
    unsigned findCallback(const callback_t *cb) {
        for (unsigned i = 0; i < callbacks.getSize(); ++i) {
            if (callbacks[i] == cb) {
                return i;
            }
        }

        return BAD_IDX;
    }

};


}


#endif // ACL_EVENT_H
