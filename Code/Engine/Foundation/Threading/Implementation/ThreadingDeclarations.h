#pragma once

#if PL_ENABLED(PL_PLATFORM_WINDOWS)
#  include <Foundation/Threading/Implementation/Win/ThreadingDeclarations_win.h>
#elif PL_ENABLED(PL_PLATFORM_OSX) || PL_ENABLED(PL_PLATFORM_LINUX) || PL_ENABLED(PL_PLATFORM_ANDROID) || PL_ENABLED(PL_PLATFORM_WEB)
#  include <Foundation/Threading/Implementation/Posix/ThreadingDeclarations_posix.h>
#else
#  error "Unknown Platform."
#endif
