#pragma once


#ifndef PHOTOSHOOP_PLUGINS_VER
#define PHOTOSHOOP_PLUGINS_VER 1
#endif


#define PHOTOSHOOP_PLUGINS_IMPL_GUARD

#if PHOTOSHOOP_PLUGINS_VER == 1
#include "impl/loader_v1.h"
#elif PHOTOSHOOP_PLUGINS_VER == 2
#include "impl/loader_v2.h"
#else
#error "Unknown plugins version"
#endif

#undef PHOTOSHOOP_PLUGINS_IMPL_GUARD