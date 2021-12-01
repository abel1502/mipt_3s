#ifndef AGF_HELPERS_ANIMATION_H
#define AGF_HELPERS_ANIMATION_H

#include <AGF/llgui_pre.h>
#include <AGF/style.h>
#include <ACL/general.h>
#include <ACL/unique_ptr.h>


namespace abel::gui {


// TODO: Optimize for styles speicifically?
class Animation {
public:
    static constexpr Style::ElementState BAD_STATE = (Style::ElementState)-1;


    Animation() = default;

    Animation(unique_ptr<Texture> &&tex0_, unique_ptr<Texture> &&tex1_, double duration_,
              Style::ElementState state0_ = BAD_STATE,
              Style::ElementState state1_ = BAD_STATE);

    // Implicitly ticks as well
    void render(Texture &target, const Rect<double> &at);

    // Updates the timer according to Application's one
    void tick();

    void start();

    constexpr void stop() {
        state = duration;
    }

    inline void refresh() {
        start();
    }

    constexpr bool isDone() const {
        return state >= duration;
    }

    constexpr bool isGoing() const {
        return !isDone();
    }

    inline void clear() {
        tex0 = nullptr;
        tex1 = nullptr;
        state = 0;
        startTime = 0;
        duration = 0;
    }

    inline void maybeStop(Style::ElementState curState) {
        if (!hasStateInfo()) {
            return;
        }

        if (isGoing() &&
            (curState != getState<1>() ||
             getState<0>() == getState<1>())) {

            stop();
        }
    }

    constexpr bool hasStateInfo() const {
        return state0 != BAD_STATE && state1 != BAD_STATE;
    }

    template <unsigned I>
    constexpr Style::ElementState getState() const {
        assert(hasStateInfo());

        if constexpr (I == 0) {
            return state0;
        } else if constexpr (I == 1) {
            return state1;
        } else {
            static_assert(false, "Index should be 0 or 1");
        }
    }

protected:
    unique_ptr<Texture> tex0 = nullptr,
                        tex1 = nullptr;
    Style::ElementState state0 = BAD_STATE;
    Style::ElementState state1 = BAD_STATE;
    double duration;
    double state = 0;
    double startTime = 0;

};


}


#endif // AGF_HELPERS_ANIMATION_H
