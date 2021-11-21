#include <AGF/llgui.h>


#if AGF_BASE_FRAMEWORK == AGF_BASE_FRAMEWORK_TXLIB || \
    AGF_BASE_FRAMEWORK == AGF_BASE_FRAMEWORK_GDIPLUS_TXLIB

#include <utility>
#include <cassert>
#include <cstdio>


namespace abel::gui {


#pragma region WinTheme
WinTheme::WinTheme(const wchar_t *name) :
    name{name}, handle{OpenThemeData(NULL, name)} {

    if (!handle) {
        throw llgui_error("OpenThemeData failed");
    }
}

WinTheme::~WinTheme() {
    CloseThemeData(handle);

    name = nullptr;
    handle = NULL;
}

WinTheme::WinTheme(WinTheme &&other) noexcept :
    handle{other.handle}, name{other.name} {
    
    other.handle = NULL;
    other.name = nullptr;
}

WinTheme &WinTheme::operator=(WinTheme &&other) noexcept {
    std::swap(handle, other.handle);
    std::swap(name,   other.name);

    return *this;
}
#pragma endregion WinTheme


}


#endif
