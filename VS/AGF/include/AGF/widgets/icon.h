#ifndef AGF_WIDGETS_ICON_H
#define AGF_WIDGETS_ICON_H

#include <AGF/widget.h>


namespace abel::gui::widgets {


class Icon : public Widget {
public:
    using Base = Widget;
    EVENT_HANDLER_USING(Base);

    Icon(Widget *parent_, const Rect<double> &region_, const Texture &source);


    EVENT_HANDLER_OVERRIDE(Render);

protected:
    unique_ptr<Texture> texture = nullptr;

};


}


#endif // AGF_WIDGETS_ICON_H
