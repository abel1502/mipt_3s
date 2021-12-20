#pragma once


#include <cstdint>
#include <cctype>
#include <cstring>


namespace plugin {

namespace Keyboard {

enum class Key {
    none = -1,

    a           ,
    b           ,
    c           ,
    d           ,
    e           ,
    f           ,
    g           ,
    h           ,
    i           ,
    j           ,
    k           ,
    l           ,
    m           ,
    n           ,
    o           ,
    p           ,
    q           ,
    r           ,
    s           ,
    t           ,
    u           ,
    v           ,
    w           ,
    x           ,
    y           ,
    z           ,
    num0        ,
    num1        ,
    num2        ,
    num3        ,
    num4        ,
    num5        ,
    num6        ,
    num7        ,
    num8        ,
    num9        ,
    escape      ,
    lctrl       ,
    lshift      ,
    lalt        ,
    lsuper      ,
    rctrl       ,
    rshift      ,
    ralt        ,
    rsuper      ,
    menu        ,
    lbracket    ,
    rbracket    ,
    semicolon   ,
    comma       ,
    period      ,
    quote       ,
    slash       ,
    backslash   ,
    tilde       ,
    equal       ,
    hyphen      ,
    space       ,
    enter       ,
    backspace   ,
    tab         ,
    pageup      ,
    pagedown    ,
    end         ,
    home        ,
    insert      ,
    del         ,
    add         ,
    subtract    ,
    multiply    ,
    divide      ,
    left        ,
    right       ,
    up          ,
    down        ,
    numpad0     ,
    numpad1     ,
    numpad2     ,
    numpad3     ,
    numpad4     ,
    numpad5     ,
    numpad6     ,
    numpad7     ,
    numpad8     ,
    numpad9     ,
    f1          ,
    f2          ,
    f3          ,
    f4          ,
    f5          ,
    f6          ,
    f7          ,
    f8          ,
    f9          ,
    f10         ,
    f11         ,
    f12         ,
    f13         ,
    f14         ,
    f15         ,
    pause       ,
};

}

struct SpecKey {
    enum KeyIndex {
        lctrl,
        rctrl,
        lshift,
        rshift,
        lalt,
        ralt,
        lsuper,
        rsuper
    };

    char content = 0;

    SpecKey(char content) : content(content) {}

    SpecKey(Keyboard::Key key_1 = Keyboard::Key::none,
            Keyboard::Key key_2 = Keyboard::Key::none,
            Keyboard::Key key_3 = Keyboard::Key::none,
            Keyboard::Key key_4 = Keyboard::Key::none,
            Keyboard::Key key_5 = Keyboard::Key::none,
            Keyboard::Key key_6 = Keyboard::Key::none,
            Keyboard::Key key_7 = Keyboard::Key::none,
            Keyboard::Key key_8 = Keyboard::Key::none)
    {
        add(key_1);
        add(key_2);
        add(key_3);
        add(key_4);
        add(key_5);
        add(key_6);
        add(key_7);
        add(key_8);
    }

    void add(Keyboard::Key key) {
        switch (key) {
            case Keyboard::Key::lctrl:
                content |= (1 << KeyIndex::lctrl);
                break;

            case Keyboard::Key::rctrl:
                content |= (1 << KeyIndex::rctrl);
                break;

            case Keyboard::Key::lshift:
                content |= (1 << KeyIndex::lshift);
                break;

            case Keyboard::Key::rshift:
                content |= (1 << KeyIndex::rshift);
                break;

            case Keyboard::Key::lalt:
                content |= (1 << KeyIndex::lalt);
                break;

            case Keyboard::Key::ralt:
                content |= (1 << KeyIndex::ralt);
                break;

            case Keyboard::Key::lsuper:
                content |= (1 << KeyIndex::lsuper);
                break;

            case Keyboard::Key::rsuper:
                content |= (1 << KeyIndex::rsuper);
                break;

            default:
                break;
        }
    }

    bool operator==(const SpecKey &other) const {
        return content == other.content;
    }

    bool operator!=(const SpecKey &other) const {
        return content != other.content;
    }
};


struct Hotkey {
    Keyboard::Key key;
    SpecKey spec_key;

    Hotkey(Keyboard::Key key) :
    key(key),
    spec_key(0)
    {}

    Hotkey(Keyboard::Key key, SpecKey spec_key) :
    key(key),
    spec_key(spec_key)
    {}

    static const Hotkey none;

    bool operator==(const Hotkey &other) const {
        return key == other.key && spec_key == other.spec_key;
    }

    bool operator!=(const Hotkey &other) const {
        return key != other.key || spec_key != other.spec_key;
    }
};

namespace Event {

struct TextEnter {
    uint32_t keycode;

    TextEnter(uint32_t unicode) : keycode(unicode) {}

    bool is_ascii() const {
        return ! (keycode & 0xFF00);
    }

    char ascii() const {
        if(!is_ascii()) return 0;

        return (char) keycode;
    }

    uint32_t unicode() const {
        return keycode;
    }

    inline bool is_symbolic() const {
        char c = ascii();
        return isspace(c) || isalpha(c) || isdigit(c) || ispunct(c);
    }
};

struct Key {
    Keyboard::Key code;

    explicit Key(Keyboard::Key code) : code(code) {}
};

struct KeyDown : public Key {
    KeyDown(Keyboard::Key code) : Key(code) {}
};

struct KeyUp : public Key {
    KeyUp(Keyboard::Key code) : Key(code) {}
};

}
}