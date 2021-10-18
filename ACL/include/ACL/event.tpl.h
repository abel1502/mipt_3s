#ifndef ACL_EVENT_TPL_GUARD
#pragma GCC error "DON'T use this file in your own projects!!! Use event.h instead"
#endif


public:
    Event() :
        callbacks{}, beingInvoked{false} {}

    void subscribe(callback_t *cb) {
        // I could demand this, but it's probably fine as it is
        // REQUIRE(!beingInvoked);

        callbacks.append(cb);
    }

    inline Event &operator+=(callback_t *cb) {
        subscribe(cb);
    }

    void unsubscribe(callback_t *cb) {
        if (beingInvoked) {
            queueUnsubscribe(cb);
            return;
        }

        unsigned i = findCallback(cb);
        if (i == BAD_IDX)
            return;

        std::swap(callbacks[i], callbacks[-1]);

        callbacks.pop();
    }

    void unsubscribeAll(callback_t *cb) {
        if (beingInvoked) {
            queueUnsubscribeAll(cb);
            return;
        }

        for (unsigned i = 0; i < callbacks.getSize(); /* intentional lack of increment */) {
            if (callbacks[i] != cb) {
                ++i;
                continue;
            }

            std::swap(callbacks[i], callbacks[-1]);
            callbacks.pop();
        }
    }

    inline Event &operator-=(callback_t *cb) {
        unsubscribe(cb);
    }

    void queueUnsubscribe(callback_t *cb) {
        unsubQueue.appendEmplace(cb, false);
    }

    void queueUnsubscribeAll(callback_t *cb) {
        unsubQueue.appendEmplace(cb, true);
    }

    void processUnsubQueue() {
        for (const UnsubRequest &req : unsubQueue) {
            if (req.all) {
                unsubscribeAll(req.cb);
            } else {
                unsubscribe(req.cb);
            }
        }

        unsubQueue.clear();
    }

    void clear() {
        callbacks.clear();
    }

    bool hasCallback(callback_t *cb) const {
        return findCallback(cb) != BAD_IDX;
    }

protected:
    static constexpr unsigned BAD_IDX = -1u;
    struct UnsubRequest {
        callback_t *cb;
        bool all;
    };

    // I initially wanted to make this a set, but the more I think about it,
    // the more it seems like a vector is the way to go. It will at least
    // allow subscribing a callback more than once. Although I think I should
    vector<callback_t *> callbacks;
    vector<UnsubRequest> unsubQueue;
    mutable bool beingInvoked;


    // Returns callback index, or BAD_IDX on error
    unsigned findCallback(callback_t *cb) {
        for (unsigned i = 0; i < callbacks.getSize(); ++i) {
            if (callbacks[i] == cb) {
                return i;
            }
        }

        return BAD_IDX;
    }
