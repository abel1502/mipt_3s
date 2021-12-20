#ifndef PHOTOSHOOP_PLUGINS_VER
#define PHOTOSHOOP_PLUGINS_VER 2
#endif


#ifndef JUST_DEFINE_PLUGINS_VER
#ifndef PHOTOSHOOP_PLUGINS_IMPL_GUARD
#define PHOTOSHOOP_PLUGINS_IMPL_GUARD

#if PHOTOSHOOP_PLUGINS_VER == 1
#include "impl/loader_v1.h"
#elif PHOTOSHOOP_PLUGINS_VER == 2
#include "impl/loader_v2.h"
#else
#error "Unknown plugins version"
#endif

#undef PHOTOSHOOP_PLUGINS_IMPL_GUARD
#endif
#else
#undef JUST_DEFINE_PLUGINS_VER
#endif