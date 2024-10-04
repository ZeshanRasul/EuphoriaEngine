#include <Foundation/FoundationPCH.h>

#if PL_ENABLED(PL_PLATFORM_WEB)

#  include <Foundation/Platform/Web/EmscriptenUtils.h>

#  include <emscripten/emscripten.h>

void plEmscriptenUtils::SetMainLoopFunction(plEmscriptenMainLoopFunc func)
{
  emscripten_set_main_loop(func, 0, true);
}

#endif
