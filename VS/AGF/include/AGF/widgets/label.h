#ifndef AGF_WIDGETS_LABEL_H
#define AGF_WIDGETS_LABEL_H

#include <AGF/widget.h>


namespace abel::gui::widgets {


class Label : public Widget {
public:
    EVENT_HANDLER_USING(Widget);

    Label(Widget *parent_, const Rect<double> &region_, const char *text_ = "");

    constexpr const char *getText() const noexcept { return text; }
    void setText(const char *text_);

    EVENT_HANDLER_OVERRIDE(Render);

protected:
    const char *text;
    unsigned format;

    void updated();

};


}


#endif // AGF_WIDGETS_LABEL_H

