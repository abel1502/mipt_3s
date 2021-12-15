#ifndef AGF_WIDGETS_ICON_H
#define AGF_WIDGETS_ICON_H

#include <AGF/widget.h>


namespace abel::gui::widgets {


class Icon : public Widget {
public:
    using Base = Widget;
    EVENT_HANDLER_USING(Base);

    Icon(Widget *parent_, const Rect<double> &region_);

    inline Icon(Widget *parent_, const Rect<double> &region_, const Texture &source) :
        Icon(parent_, region_) {

        setTexture(source);
    }


    EVENT_HANDLER_OVERRIDE(Render);

    void setTexture(const Texture &source);

    void resetTexture();

    inline bool hasTexture() const;

protected:
    unique_ptr<Texture> texture = nullptr;

};


inline bool Icon::hasTexture() const {
    return texture;
}


}


#endif // AGF_WIDGETS_ICON_H
