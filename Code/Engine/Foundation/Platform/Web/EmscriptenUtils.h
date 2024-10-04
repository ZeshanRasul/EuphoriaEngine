#pragma once

#if PL_ENABLED(PL_PLATFORM_WEB)

namespace plEmscriptenUtils
{
  using plEmscriptenMainLoopFunc = void (*)();

  /// \brief Calls emscripten_set_main_loop under the hood to replace the main loop callback.
  void SetMainLoopFunction(plEmscriptenMainLoopFunc func);

} // namespace plEmscriptenUtils

#endif
