#ifndef AGF_LLGUI_H
#define AGF_LLGUI_H


#define AGF_BASE_FRAMEWORK_TXLIB          1
#define AGF_BASE_FRAMEWORK_GDIPLUS_TXLIB  2
// ... other options in the future ...


#ifndef AGF_BASE_FRAMEWORK
#define AGF_BASE_FRAMEWORK AGF_BASE_FRAMEWORK_GDIPLUS_TXLIB
#endif


#define AGF_LLGUI_IMPL_GUARD

#if AGF_BASE_FRAMEWORK == AGF_BASE_FRAMEWORK_TXLIB
#include <AGF/impl/llgui_txlib.h>
#elif AGF_BASE_FRAMEWORK == AGF_BASE_FRAMEWORK_GDIPLUS_TXLIB
#include <AGF/impl/llgui_gdiplus_txlib.h>
#else
#error "Unknown base gui framework"
#endif

#undef AGF_LLGUI_IMPL_GUARD

#endif // AGF_LLGUI_H
