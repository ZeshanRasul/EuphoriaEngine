#pragma once

#ifdef BUILDSYSTEM_BUILDING_FOUNDATION_LIB
#  if BUILDSYSTEM_COMPILE_ENGINE_AS_DLL && PL_DISABLED(PL_COMPILE_ENGINE_AS_DLL)
#    error "The Buildsystem is configured to build the Engine as a shared library, but PL_COMPILE_ENGINE_AS_DLL is not defined in UserConfig.h"
#  endif
#  if !BUILDSYSTEM_COMPILE_ENGINE_AS_DLL && PL_ENABLED(PL_COMPILE_ENGINE_AS_DLL)
#    error "The Buildsystem is configured to build the Engine as a static library, but PL_COMPILE_ENGINE_AS_DLL is defined in UserConfig.h"
#  endif
#endif

#ifndef NULL
#  error "NULL is not defined."
#endif

#ifndef PL_FORCE_INLINE
#  error "PL_FORCE_INLINE is not defined."
#endif

#ifndef PL_ALWAYS_INLINE
#  error "PL_ALWAYS_INLINE is not defined."
#endif

#ifndef PL_ALIGNMENT_OF
#  error "PL_ALIGNMENT_OF is not defined."
#endif

#if PL_IS_NOT_EXCLUSIVE(PL_PLATFORM_32BIT, PL_PLATFORM_64BIT)
#  error "Platform is not defined as 32 Bit or 64 Bit"
#endif

#ifndef PL_DEBUG_BREAK
#  error "PL_DEBUG_BREAK is not defined."
#endif

#ifndef PL_SOURCE_FUNCTION
#  error "PL_SOURCE_FUNCTION is not defined."
#endif

#ifndef PL_SOURCE_FILE
#  error "PL_SOURCE_FILE is not defined."
#endif

#ifndef PL_SOURCE_LINE
#  error "PL_SOURCE_LINE is not defined."
#endif

#if PL_IS_NOT_EXCLUSIVE(PL_PLATFORM_LITTLE_ENDIAN, PL_PLATFORM_BIG_ENDIAN)
#  error "Endianess is not correctly defined."
#endif

#ifndef PL_MATH_CHECK_FOR_NAN
#  error "PL_MATH_CHECK_FOR_NAN is not defined."
#endif

#if PL_IS_NOT_EXCLUSIVE3(PL_PLATFORM_ARCH_X86, PL_PLATFORM_ARCH_ARM, PL_PLATFORM_ARCH_WEB)
#  error "Platform architecture is not correctly defined."
#endif

#if !defined(PL_SIMD_IMPLEMENTATION) || (PL_SIMD_IMPLEMENTATION == 0)
#  error "PL_SIMD_IMPLEMENTATION is not correctly defined."
#endif
