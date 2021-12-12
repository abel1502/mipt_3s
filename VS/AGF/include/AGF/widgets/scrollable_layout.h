#ifndef AGF_WIDGETS_SCROLLABLE_LAYOUT_H
#define AGF_WIDGETS_SCROLLABLE_LAYOUT_H

#include <AGF/widget.h>
#include <AGF/widgets/static_group.h>
#include <AGF/widgets/layout.h>
#include <AGF/widgets/scrollbar.h>
#include <AGF/helpers/widget_ref.h>
#include <ACL/math/cmath.h>


namespace abel::gui::widgets {


namespace _impl {

//

}  // namespace _impl


template <typename Item>
class ScrollableLayoutOf : public StaticGroup<LayoutOf<Item>,
                                              ScrollbarH,
                                              ScrollbarV> {
public:
    using Base = StaticGroup<LayoutOf<Item>,
                             ScrollbarH,
                             ScrollbarV>;
    EVENT_HANDLER_USING(Base);


    using child_type = typename Base::Types::template type<0>::child_type;


    ScrollableLayoutOf(Widget *parent_, const Rect<double> &region_,
                     LayoutDirection dir = LAD_VERTICAL, double padding = 5) :
        Base(parent_, region_,
             nullptr,
             nullptr,
             nullptr) {
        constexpr double SB_WIDTH = 20;

        layout(new LayoutOf<Item>(this, this->region.padded(0, SB_WIDTH, 0, SB_WIDTH), dir, padding));

        Rect<double> sbHRegion = this->region;
        sbHRegion.y0(sbHRegion.y1() - SB_WIDTH);
        sbHRegion.x1(sbHRegion.x1() - SB_WIDTH);

        Rect<double> sbVRegion = this->region;
        sbVRegion.x0(sbVRegion.x1() - SB_WIDTH);
        sbVRegion.y1(sbVRegion.y1() - SB_WIDTH);

        scrollbarH(new ScrollbarH(this, sbHRegion));
        scrollbarV(new ScrollbarV(this, sbVRegion));

        // TODO: Hook signals

        scrollbarH().setHidden(true);
        // scrollbarV().setHidden(true);
    }

    child_type &addChild(child_type *child) {
        return layout().addChild(child);
    }

protected:
    using typename Base::Types;

    SGRP_DECLARE_BINDING_I(layout, 0);
    SGRP_DECLARE_BINDING_T(scrollbarH, ScrollbarH);
    SGRP_DECLARE_BINDING_T(scrollbarV, ScrollbarV);

    void updateScrollbarsVisibility();

};


using ScrollableLayout = ScrollableLayoutOf<Widget>;


extern template
class ScrollableLayoutOf<Widget>;


}


#endif // AGF_WIDGETS_SCROLLABLE_LAYOUT_H
