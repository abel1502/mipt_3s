#ifndef AGF_LLGUI_H
#define AGF_LLGUI_H


#define AGF_BASE_FRAMEWORK_TXLIB 1
// ... other options in the future ...


#ifndef AGF_BASE_FRAMEWORK
#define AGF_BASE_FRAMEWORK AGF_BASE_FRAMEWORK_TXLIB
#endif


#define AGF_LLGUI_IMPL_GUARD

#if AGF_BASE_FRAMEWORK == AGF_BASE_FRAMEWORK_TXLIB
#include <AGF/impl/llgui_txlib.h>
#else
#pragma GCC error "Unknown base gui framework"
#endif

#undef AGF_LLGUI_IMPL_GUARD

#endif // AGF_LLGUI_H
