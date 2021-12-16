#include <AGF/llgui.h>
#include <AGF/helpers/animation.h>
#include <AGF/application.h>


namespace abel::gui {


Animation::Animation(unique_ptr<Texture> &&tex0_, unique_ptr<Texture> &&tex1_, double duration_,
                     Style::ElementState state0_, Style::ElementState state1_) :
    tex0{std::move(tex0_)},
    tex1{std::move(tex1_)},
    state0{state0_},
    state1{state1_},
    duration{duration_} {}


void Animation::render(Texture &target, const Rect<double> &at) {
    assert(state >= 0);

    double curTime = Application::getInstance().getTime();

    if (curTime - startTime > (OVERDUE_COEFF + 1) * duration) {
        // We assume it is too late to pretend we're on time
        state = duration;
    }

    // Intentionally in this order, because with duration == 0 we want to render the final texture
    if (cmpDbl(state, duration) >= 0) {
        target.embed(at, *tex1);

        return;
    }

    if (cmpDbl(state, 0) == 0) {
        target.embed(at, *tex0);

        tick();

        return;
    }

    float part = (float)(state / duration);

    target.embedTransformed(at, *tex0, {1, 1, 1, 1/* - part*/});
    target.embedTransformed(at, *tex1, {1, 1, 1,       part  });

    tick();
}


void Animation::tick() {
    double curTime = Application::getInstance().getTime();

    state = curTime - startTime;
}

void Animation::start() {
    state = 0;
    startTime = Application::getInstance().getTime();
}


}