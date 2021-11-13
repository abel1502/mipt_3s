#ifndef AGF_LLGUI_WINTHEME_H
#define AGF_LLGUI_WINTHEME_H

#ifndef AGF_LLGUI_IMPL_GUARD
#error "This file should not be included manually"
#endif

#include <vsstyle.h>
#include <vssym32.h>


namespace abel::gui {


class WinTheme {
public:
    constexpr WinTheme() {}
    WinTheme(const wchar_t *name);
    ~WinTheme();

    WinTheme(WinTheme &&other) noexcept;
    WinTheme &operator=(WinTheme &&other) noexcept;

    WinTheme(const WinTheme &other) = delete;
    WinTheme &operator=(const WinTheme &other) = delete;

    constexpr HTHEME getHandle() const noexcept {
        return handle;
    }

protected:
    const wchar_t *name = nullptr;
    HTHEME handle = NULL;

};


}


#endif
