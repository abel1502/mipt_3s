#ifndef AGF_MOUSE_H
#define AGF_MOUSE_H

#include <ACL/general.h>
#include <windef.h>
#include <windowsx.h>
#include <winuser.h>


namespace abel::gui {


enum class MouseBtn : unsigned char {
    Left,
    Middle,
    Right,
};


enum class MouseClickType : unsigned char {
    Down,
    Up,
};


struct MouseAttrs {
    bool btnL  : 1;
    bool btnM  : 1;
    bool btnR  : 1;

    bool ctrl  : 1;
    bool shift : 1;


    constexpr MouseAttrs(WPARAM wParam) noexcept :
        btnL{!!(wParam & MK_LBUTTON)}, btnM{!!(wParam & MK_MBUTTON)}, btnR{!!(wParam & MK_RBUTTON)},
        ctrl{!!(wParam & MK_CONTROL)}, shift{!!(wParam & MK_SHIFT)} {}
};


}


#endif // AGF_MOUSE_H
