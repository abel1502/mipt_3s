#ifndef AGF_WIDGETS_LAYOUT_H
#define AGF_WIDGETS_LAYOUT_H

#include <ACL/general.h>
#include <AGF/widget.h>
#include <AGF/widgets/group.h>


namespace abel::gui::widgets {


enum LayoutDirection {
    LAD_HORIZONTAL,
    LAD_VERTICAL,
};


template <typename Item>
class LayoutOf : public GroupOf<Item> {
public:
    using Base = GroupOf<Item>;
    EVENT_HANDLER_USING(Base);
    using Base::child_type;


    LayoutOf(Widget *parent_, const Rect<double> &region_, LayoutDirection dir_ = LAD_VERTICAL, double padding_ = 5) :
        Base(parent_, region_), dir{dir_}, padding{padding_} {}

    virtual child_type &addChild(child_type *child) override {
        assert(child);

        // child->dispatchEvent(MoveEvent(nextPos));
        child->staticShift(nextPos);

        incPos(child->getRegion().getDiag());

        return Base::addChild(child);
    }

    constexpr LayoutDirection getDir() const {
        return dir;
    }

    constexpr double getPadding() const {
        return padding;
    }

    constexpr double getCurSize() const {
        switch (dir) {
        case LAD_VERTICAL:
            return nextPos.y();

        case LAD_HORIZONTAL:
            return nextPos.x();

        NODEFAULT
        }
    }

protected:
    LayoutDirection dir;
    double padding = 5;
    Vector2d nextPos{padding, padding};


    void incPos(const Vector2d &childSize) {
        switch (dir) {
        case LAD_VERTICAL:
            nextPos.y() += childSize.y() + padding;
            break;

        case LAD_HORIZONTAL:
            nextPos.x() += childSize.x() + padding;
            break;

        NODEFAULT
        }
    }

};


extern template
class LayoutOf<Widget>;

using Layout = LayoutOf<Widget>;


}


#endif // AGF_WIDGETS_LAYOUT_H
