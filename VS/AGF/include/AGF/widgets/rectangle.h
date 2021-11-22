#ifndef AGF_WIDGETS_RECTANGLE_H
#define AGF_WIDGETS_RECTANGLE_H

#include <AGF/widget.h>


namespace abel::gui::widgets {


class Rectangle : public Widget {
public:
    EVENT_HANDLER_USING(Widget);

    Rectangle(Widget *parent_, const Rect<double> &region_, const Color &color_);

    // For now
    Rectangle(const Rectangle &other) = delete;
    Rectangle &operator=(const Rectangle &other) = delete;


    EVENT_HANDLER_OVERRIDE(Render);

    void recolor(const Color &color_);

protected:
    Color color;
    unique_ptr<Texture> texture;

    void bakeTexture();

};


}


#endif // AGF_WIDGETS_RECTANGLE_H

