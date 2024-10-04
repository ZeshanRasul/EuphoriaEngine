#pragma once
#if PL_DISABLED(PL_PLATFORM_WEB)
#  error "This header should only be included on Web"
#endif
// #include <cstdio>
// #include <malloc.h>
// #include <pthread.h>
// #include <stdarg.h>
// #include <sys/time.h>
// #include <unistd.h>
//
//// unset common macros
// #ifdef min
// #  undef min
// #endif
// #ifdef max
// #  undef max
// #endif
#ifndef NULL
#  define NULL 0
#endif
#undef PL_PLATFORM_LITTLE_ENDIAN
#define PL_PLATFORM_LITTLE_ENDIAN PL_ON