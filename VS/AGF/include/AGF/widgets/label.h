#ifndef AGF_WIDGETS_LABEL_H
#define AGF_WIDGETS_LABEL_H

#include <AGF/widget.h>


namespace abel::gui::widgets {


class Label : public Widget {
public:
    EVENT_HANDLER_USING(Widget);

    Label(Widget *parent_, const Rect<double> &region_, const char *text_ = "", double fontSize_ = 0, unsigned format_ = -1u);

    constexpr const char *getText() const noexcept { return text; }
    void setText(const char *text_);

    constexpr double getFontSize() const noexcept { return fontSize; }
    void setFontSize(double fontSize_);

    constexpr unsigned getFormat() const noexcept { return format; }
    constexpr unsigned hasFormat() const noexcept { return format != -1u; }
    void resetFormat();
    void setFormat(unsigned format_);


    EVENT_HANDLER_OVERRIDE(Render);

protected:
    double fontSize;
    double deducedFontSize;
    const char *text;
    unsigned format;

    void updated();

};


}


#endif // AGF_WIDGETS_LABEL_H

