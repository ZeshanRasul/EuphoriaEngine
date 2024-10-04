#pragma once
#include <Foundation/Communication/GlobalEvent.h>
#include <Foundation/Platform/Web/EmscriptenUtils.h>
/// \file
/// \brief Same as PL_APPLICATION_ENTRY_POINT but should be used for applications that shall always show a console window.
#define PL_CONSOLEAPP_ENTRY_POINT PL_APPLICATION_ENTRY_POINT
/// \brief This macro allows for easy creation of application entry points (since they can't be placed in DLLs)
///
/// Just use the macro in a cpp file of your application and supply your app class (must be derived from plApplication).
/// The additional (optional) parameters are passed to the constructor of your app class.
#define PL_APPLICATION_ENTRY_POINT(AppClass, ...)                                                                                \
  alignas(PL_ALIGNMENT_OF(AppClass)) static char appBuffer[sizeof(AppClass)]; /* Not on the stack to cope with smaller stacks */ \
                                                                                                                                 \
  PL_APPLICATION_ENTRY_POINT_CODE_INJECTION                                                                                      \
                                                                                                                                 \
  AppClass* g_pApp = nullptr;                                                                                                    \
  plInt32 g_iAppState = 0;                                                                                                       \
                                                                                                                                 \
  void plWebRun()                                                                                                                \
  {                                                                                                                              \
    switch (g_iAppState)                                                                                                         \
    {                                                                                                                            \
      case 0:                                                                                                                    \
      {                                                                                                                          \
        bool bPreInitDone = true;                                                                                                \
        PL_BROADCAST_EVENT(WebApp_PreInit, &bPreInitDone);                                                                       \
        if (bPreInitDone)                                                                                                        \
          g_iAppState = 1;                                                                                                       \
        break;                                                                                                                   \
      }                                                                                                                          \
      case 1:                                                                                                                    \
        plRun_Startup(g_pApp).AssertSuccess();                                                                                   \
        g_iAppState = 2;                                                                                                         \
        break;                                                                                                                   \
      case 2:                                                                                                                    \
        g_pApp->Run();                                                                                                           \
        break;                                                                                                                   \
    }                                                                                                                            \
  }                                                                                                                              \
                                                                                                                                 \
  int main(int argc, const char** argv)                                                                                          \
  {                                                                                                                              \
    g_pApp = new (appBuffer) AppClass(__VA_ARGS__);                                                                              \
    g_pApp->SetCommandLineArguments((plUInt32)argc, argv);                                                                       \
    plEmscriptenUtils::SetMainLoopFunction(plWebRun);                                                                            \
    /* There's no real 'shutdown' in web. Just ignore this. */                                                                   \
    return 0;                                                                                                                    \
  }