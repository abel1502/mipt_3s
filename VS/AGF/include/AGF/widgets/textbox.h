#ifndef AGF_WIDGETS_TEXTBOX_H
#define AGF_WIDGETS_TEXTBOX_H

#include <AGF/widget.h>
#include <AGF/helpers/mouse_tracker.h>
#include <AGF/helpers/animation.h>
#include <string>


namespace abel::gui::widgets {


class TextBox : public Widget {
public:
    using Base = Widget;
    EVENT_HANDLER_USING(Base);


    class Caret {
    public:
        Caret(TextBox &textBox_);

        constexpr Caret &operator++() {
            shift(1);

            return *this;
        }

        constexpr Caret &operator--() {
            shift(-1);

            return *this;
        }

        constexpr unsigned getPos() const {
            return pos;
        }

        void shift(int direction);

    protected:
        TextBox &textBox;
        unsigned pos = 0;

    };


    Signal<bool (const char *text)> sigSubmit{};


    TextBox(Widget *parent_, const Rect<double> &region_, const char *defaultText = "");

    EVENT_HANDLER_OVERRIDE(MouseClick);
    EVENT_HANDLER_OVERRIDE(MouseMove);
    EVENT_HANDLER_OVERRIDE(MouseScroll);
    EVENT_HANDLER_OVERRIDE(Render);
    EVENT_HANDLER_OVERRIDE(FocusUpdate);
    EVENT_HANDLER_OVERRIDE(KeyboardInput);
    EVENT_HANDLER_OVERRIDE(KeyPress);

    inline const char *getText() const;

    inline unsigned getLength() const;

    void setText(const char *text_);

    bool isActive() const;

protected:
    MouseTracker mt{this};
    std::string text{};
    Caret caret{*this};
    Vector2d textOffset{};


    Rect<double> getCharRect(unsigned idx) const;

    Rect<double> getTextRect() const;

    void shiftToShowCaret();

    void addChar(char chr);

    void remChar();

    void maybeChangeActive(bool active);

};


inline const char *TextBox::getText() const {
    return text.data();
}

inline unsigned TextBox::getLength() const {
    return (unsigned)text.size();
}


}


#endif // AGF_WIDGETS_TEXTBOX_H
