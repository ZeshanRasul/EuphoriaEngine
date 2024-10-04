#pragma once

#if PL_DISABLED(PL_PLATFORM_OSX)
#  error "This header should only be included on OSX"
#endif

#include <cstdio>
#include <pthread.h>
#include <sys/malloc.h>
#include <sys/time.h>

// unset common macros
#undef min
#undef max

#undef PL_PLATFORM_LITTLE_ENDIAN
#define PL_PLATFORM_LITTLE_ENDIAN PL_ON
