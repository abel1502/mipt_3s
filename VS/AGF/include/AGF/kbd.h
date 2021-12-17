#ifndef AGF_KBD_H
#define AGF_KBD_H

#include <ACL/general.h>
#include <AGF/impl/windows.h>
#include <windowsx.h>


namespace abel::gui {


enum class Key : unsigned short {
    Lbutton = VK_LBUTTON,
    Rbutton = VK_RBUTTON,
    Cancel = VK_CANCEL,
    Mbutton = VK_MBUTTON,
    Xbutton1 = VK_XBUTTON1,
    Xbutton2 = VK_XBUTTON2,

    Back = VK_BACK,
    Tab = VK_TAB,
    Clear = VK_CLEAR,
    Return = VK_RETURN,
    Shift = VK_SHIFT,
    Control = VK_CONTROL,
    Menu = VK_MENU,
    Pause = VK_PAUSE,
    Capital = VK_CAPITAL,
    Escape = VK_ESCAPE,
    Space = VK_SPACE,
    Prior = VK_PRIOR,
    Next = VK_NEXT,
    End = VK_END,
    Home = VK_HOME,

    Left = VK_LEFT,
    Up = VK_UP,
    Right = VK_RIGHT,
    Down = VK_DOWN,

    Select = VK_SELECT,
    Print = VK_PRINT,
    Execute = VK_EXECUTE,
    Snapshot = VK_SNAPSHOT,
    Insert = VK_INSERT,
    Delete = VK_DELETE,
    Help = VK_HELP,

    N0 = '0',
    N1 = '1',
    N2 = '2',
    N3 = '3',
    N4 = '4',
    N5 = '5',
    N6 = '6',
    N7 = '7',
    N8 = '8',
    N9 = '9',

    A = 'A',
    B = 'B',
    C = 'C',
    D = 'D',
    E = 'E',
    F = 'F',
    G = 'G',
    H = 'H',
    I = 'I',
    J = 'J',
    K = 'K',
    L = 'L',
    M = 'M',
    N = 'N',
    O = 'O',
    P = 'P',
    Q = 'Q',
    R = 'R',
    S = 'S',
    T = 'T',
    U = 'U',
    V = 'V',
    W = 'W',
    X = 'X',
    Y = 'Y',
    Z = 'Z',

    LWin = VK_LWIN,
    RWin = VK_RWIN,
    Win = LWin,
    Apps = VK_APPS,
    Sleep = VK_SLEEP,

    Numpad0 = VK_NUMPAD0,
    Numpad1 = VK_NUMPAD1,
    Numpad2 = VK_NUMPAD2,
    Numpad3 = VK_NUMPAD3,
    Numpad4 = VK_NUMPAD4,
    Numpad5 = VK_NUMPAD5,
    Numpad6 = VK_NUMPAD6,
    Numpad7 = VK_NUMPAD7,
    Numpad8 = VK_NUMPAD8,
    Numpad9 = VK_NUMPAD9,

    Multiply = VK_MULTIPLY,
    Add = VK_ADD,
    Separator = VK_SEPARATOR,
    Subtract = VK_SUBTRACT,
    Decimal = VK_DECIMAL,
    Divide = VK_DIVIDE,

    F1 = VK_F1,
    F2 = VK_F2,
    F3 = VK_F3,
    F4 = VK_F4,
    F5 = VK_F5,
    F6 = VK_F6,
    F7 = VK_F7,
    F8 = VK_F8,
    F9 = VK_F9,
    F10 = VK_F10,
    F11 = VK_F11,
    F12 = VK_F12,
    F13 = VK_F13,
    F14 = VK_F14,
    F15 = VK_F15,
    F16 = VK_F16,
    F17 = VK_F17,
    F18 = VK_F18,
    F19 = VK_F19,
    F20 = VK_F20,
    F21 = VK_F21,
    F22 = VK_F22,
    F23 = VK_F23,
    F24 = VK_F24,

    Numlock = VK_NUMLOCK,
    Scroll = VK_SCROLL,

    LShift = VK_LSHIFT,
    RShift = VK_RSHIFT,
    Shift = LShift,
    LCtrl = VK_LCONTROL,
    RCtrl = VK_RCONTROL,
    Ctrl = LCtrl,
    LAlt = VK_LMENU,
    RAlt = VK_RMENU,
    Alt = LAlt,
};


enum class KeyPressType : unsigned char {
    Down,
    Up,
};


constexpr wchar_t getKeyCharacter(WPARAM wParam) {
    // TODO: Unicode magic?
    return (wchar_t)wParam;
}

constexpr Key keyCodeFromVk(unsigned vk) {
    return Key{vk};
}

constexpr unsigned getKeyRepeatCount(LPARAM lParam) {
    return (unsigned)LOWORD(lParam);
}


}


#endif // AGF_KBD_H
