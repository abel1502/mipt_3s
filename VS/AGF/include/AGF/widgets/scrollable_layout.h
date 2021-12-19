#ifndef AGF_WIDGETS_SCROLLABLE_LAYOUT_H
#define AGF_WIDGETS_SCROLLABLE_LAYOUT_H

#include <AGF/widget.h>
#include <AGF/widgets/static_group.h>
#include <AGF/widgets/layout.h>
#include <AGF/widgets/scrollbar.h>
#include <AGF/helpers/widget_ref.h>
#include <AGF/helpers/mouse_tracker.h>
#include <AGF/application.h>
#include <ACL/math/cmath.h>


namespace abel::gui::widgets {


namespace _impl {

//

}  // namespace _impl


template <typename Item>
class ScrollableLayoutOf : public StaticGroup<LayoutOf<Item>,
                                              ScrollbarV> {
public:
    using Base = StaticGroup<LayoutOf<Item>,
                             ScrollbarV>;
    EVENT_HANDLER_USING(Base);
    using Widget::EventStatus;


    using child_type = typename Base::Types::template type<0>::child_type;


    ScrollableLayoutOf(Widget *parent_, const Rect<double> &region_, double padding = 5) :
        Base(parent_, region_,
             nullptr,
             nullptr) {

        this->setClip(true);

        constexpr double SB_WIDTH = 20;

        layout(new LayoutOf<Item>(this, this->region.padded(0, SB_WIDTH, 0, 0),
                                  LAD_VERTICAL, padding));

        Rect<double> sbRegion = this->region;
        sbRegion.x0(sbRegion.x1() - SB_WIDTH);

        scrollbar(new ScrollbarV(this, sbRegion, 0, 1, 0));

        scrollbar().sigChanged += [this](ScrollbarV &sb) {
            setShift(sb.getValue());

            return false;
        };

        onSizeChanged();
    }

    template <typename T = child_type, typename ... As>
    T &createChild(const Rect<double> &relRegion, As &&... args) {
        decltype(auto) result = layout().createChild<T, As...>(relRegion, std::forward<As>(args)...);

        onSizeChanged();

        return result;
    }

    child_type &addChild(child_type *child) {
        decltype(auto) result = layout().addChild(child);

        onSizeChanged();

        return result;
    }

    void setShift(double shift_) {
        shift_ = math::clamp(shift_, 0., 1.);

        double contentsSize  = layout().getCurSize();
        double containerSize = layout().getRegion().h();

        if (cmpDbl(contentsSize, containerSize) > 0) {
            layout().moveAllChildren(
                Vector2d{0, (shift - shift_) * (contentsSize - containerSize)}
            );
        }

        shift = shift_;

        Application::getInstance().demandRedraw();
    }

    EVENT_HANDLER_OVERRIDE(MouseScroll) {
        EventStatus status = Base::dispatchEvent(event);

        if (!status.shouldHandle(status.NODE)) {
            return status;
        }

        double containerSize = layout().getRegion().h();

        scrollbar().setValue(scrollbar().getValue() - (double)event.delta / 120. / 5.);

        return status;
    }

protected:
    MouseTracker mt{this};
    // [0-1], not pixels
    double shift = 0;


    using typename Base::Types;

    SGRP_DECLARE_BINDING_I(layout, 0);
    SGRP_DECLARE_BINDING_T(scrollbar, ScrollbarV);

    void onSizeChanged() {
        double contentsSize  = layout().getCurSize();
        double containerSize = layout().getRegion().h();

        if (cmpDbl(contentsSize, containerSize) <= 0) {
            scrollbar().setHidden(true);
            return;
        }

        scrollbar().setHidden(false);
        scrollbar().setThumbScale(math::clamp(containerSize / contentsSize, 0., 1.));
    }

};


using ScrollableLayout = ScrollableLayoutOf<Widget>;


extern template
class ScrollableLayoutOf<Widget>;


}


#endif // AGF_WIDGETS_SCROLLABLE_LAYOUT_H
