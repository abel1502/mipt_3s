#include <AGF/llgui.h>
#include <AGF/widgets/window.h>


namespace abel::gui::widgets {


namespace _impl {

Header::Header(Window *parent_, const Rect<double> &region_, const char *title_) :
    Base(parent_, region_,
         new Label(this, region_.padded(LABEL_LPAD, LABEL_RPAD, LABEL_TPAD, LABEL_BPAD),
                   title_, 14, DT_LEFT | DT_VCENTER | DT_END_ELLIPSIS),
         new Button(this, Rect<double>::se(region_.x1() - BTN_CLOSE_LDX, region_.y0() + BTN_TPAD,
                                           region_.x1() - BTN_CLOSE_RDX, region_.y1()), "x"),
         new Button(this, Rect<double>::se(region_.x1() - BTN_MINIMIZE_LDX, region_.y0() + BTN_TPAD,
                                           region_.x1() - BTN_MINIMIZE_RDX, region_.y1()), "_")
         ) {}

Borders::Borders(Window *parent_, const Rect<double> &region_) :
    Base(parent_, region_,
         new Rectangle{this, region_, Color::LBLUE}
         ) {}

}  // namespace _impl


}


template
class abel::unique_ptr<abel::gui::widgets::Window>;
