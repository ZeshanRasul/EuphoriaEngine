#pragma once

#define PL_INCLUDING_BASICS_H

// Very basic Preprocessor defines
#include <Foundation/Basics/PreprocessorUtils.h>

// Set all feature #defines to PL_OFF
#include <Foundation/Basics/AllDefinesOff.h>

// General detection of the OS and hardware
#include <Foundation/Basics/Platform/DetectArchitecture.h>
#include <Foundation/Basics/Platform/DetectPlatform.h>

// Options by the user to override the build
#include <Foundation/UserConfig.h>

// Configure the DLL Import/Export Define
#if PL_ENABLED(PL_COMPILE_ENGINE_AS_DLL)
#  ifdef BUILDSYSTEM_BUILDING_FOUNDATION_LIB
#    define PL_FOUNDATION_DLL PL_DECL_EXPORT
#    define PL_FOUNDATION_DLL_FRIEND PL_DECL_EXPORT_FRIEND
#  else
#    define PL_FOUNDATION_DLL PL_DECL_IMPORT
#    define PL_FOUNDATION_DLL_FRIEND PL_DECL_IMPORT_FRIEND
#  endif
#else
#  define PL_FOUNDATION_DLL
#  define PL_FOUNDATION_DLL_FRIEND
#endif

#include <Foundation/FoundationInternal.h>

// include the different headers for the supported platforms
#if PL_ENABLED(PL_PLATFORM_WINDOWS)
#  include <Foundation/Basics/Platform/Win/Platform_win.h>
#elif PL_ENABLED(PL_PLATFORM_OSX)
#  include <Foundation/Basics/Platform/OSX/Platform_OSX.h>
#elif PL_ENABLED(PL_PLATFORM_LINUX) || PL_ENABLED(PL_PLATFORM_ANDROID)
#  include <Foundation/Basics/Platform/Linux/Platform_Linux.h>
#elif PL_ENABLED(PL_PLATFORM_WEB)
#  include <Foundation/Basics/Platform/Web/Platform_Web.h>
#else
#  error "Undefined platform!"
#endif

// include headers for the supported compilers
#include <Foundation/Basics/Compiler/Clang.h>
#include <Foundation/Basics/Compiler/GCC.h>
#include <Foundation/Basics/Compiler/MSVC.h>

// Here all the different features that each platform supports are declared.
#include <Foundation/Basics/Platform/PlatformFeatures.h>

// Include this last, it will ensure the previous includes have setup everything correctly
#include <Foundation/Basics/Platform/CheckDefinitions.h>

// Include common definitions and macros (e.g. static_assert)
#include <Foundation/Basics/Platform/Common.h>

// Include magic preprocessor macros
#include <Foundation/Basics/Platform/BlackMagic.h>

// Now declare all fundamental types
#include <Foundation/Types/Types.h>

// Type trait utilities
#include <Foundation/Types/TypeTraits.h>

// Assert macros should always be available
#include <Foundation/Basics/Assert.h>

// String formatting is needed by the asserts
#include <Foundation/Strings/FormatString.h>

#include <Foundation/Time/Time.h>

class PL_FOUNDATION_DLL plFoundation
{
public:
  static plAllocator* s_pDefaultAllocator;
  static plAllocator* s_pAlignedAllocator;

  /// \brief The default allocator can be used for any kind of allocation if no alignment is required
  PL_ALWAYS_INLINE static plAllocator* GetDefaultAllocator()
  {
    if (s_bIsInitialized)
      return s_pDefaultAllocator;
    else // the default allocator is not yet set so we return the static allocator instead.
      return GetStaticsAllocator();
  }

  /// \brief The aligned allocator should be used for all allocations which need alignment
  PL_ALWAYS_INLINE static plAllocator* GetAlignedAllocator()
  {
    PL_ASSERT_ALWAYS(s_pAlignedAllocator != nullptr,
      "plFoundation must have been initialized before this function can be called."
      "This error can occur when you have a global variable or a static member variable that (indirectly) requires an allocator."
      "Check out the documentation for 'plStaticsAllocatorWrapper' for more information about this issue.");
    return s_pAlignedAllocator;
  }

  /// \brief Returns the allocator that is used by global data and static members before the default allocator is created.
  static plAllocator* GetStaticsAllocator();

private:
  friend class plStartup;
  friend struct plStaticsAllocatorWrapper;

  static void Initialize();
  static bool s_bIsInitialized;
};

#undef PL_INCLUDING_BASICS_H
