#ifndef AGF_WIDGETS_LABEL_H
#define AGF_WIDGETS_LABEL_H

#include <AGF/widget.h>


namespace abel::gui::widgets {


class Label : public Widget {
public:
    EVENT_HANDLER_USING(Widget)

    Label(Widget *parent_, const Rect<double> &region_, const char *text_ = "", double fontSize_ = 0);

    constexpr const char *getText() const noexcept {
        return text;
    }

    constexpr double getFontSize() const noexcept {
        return fontSize;
    }

    void updateText(const char *text_);

    void updateFontSize(double fontSize_);

    EVENT_HANDLER_OVERRIDE(Render)

protected:
    double fontSize;
    const char *text;

    void bakeFont();

};


}


#endif // AGF_WIDGETS_LABEL_H

