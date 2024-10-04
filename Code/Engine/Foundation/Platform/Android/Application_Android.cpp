#include <Foundation/FoundationPCH.h>

#if PL_ENABLED(PL_PLATFORM_ANDROID)

#  include <Foundation/Application/Application.h>
#  include <Foundation/Application/Implementation/Android/Application_android.h>
#  include <Foundation/Logging/Log.h>
#  include <android/log.h>
#  include <android_native_app_glue.h>

static void plAndroidHandleCmd(struct android_app* pApp, int32_t cmd)
{
  plAndroidApplication* pAndroidApp = static_cast<plAndroidApplication*>(pApp->userData);
  pAndroidApp->HandleCmd(cmd);
}

static int32_t plAndroidHandleInput(struct android_app* pApp, AInputEvent* pEvent)
{
  plAndroidApplication* pAndroidApp = static_cast<plAndroidApplication*>(pApp->userData);
  return pAndroidApp->HandleInput(pEvent);
}

plAndroidApplication::plAndroidApplication(struct android_app* pApp, plApplication* pPlApp)
  : m_pApp(pApp)
  , m_pPlApp(pPlApp)
{
  pApp->userData = this;
  pApp->onAppCmd = plAndroidHandleCmd;
  pApp->onInputEvent = plAndroidHandleInput;
  // #TODO: acquire sensors, set app->onAppCmd, set app->onInputEvent
}

plAndroidApplication::~plAndroidApplication() {}

void plAndroidApplication::AndroidRun()
{
  bool bRun = true;
  while (true)
  {
    struct android_poll_source* pSource = nullptr;
    int iIdent = 0;
    int iEvents = 0;
    while ((iIdent = ALooper_pollAll(0, nullptr, &iEvents, (void**)&pSource)) >= 0)
    {
      if (pSource != nullptr)
        pSource->process(m_pApp, pSource);

      HandleIdent(iIdent);
    }

    // APP_CMD_INIT_WINDOW has not triggered yet. Engine is not yet started.
    if (!m_bStarted)
      continue;

    if (bRun && m_pPlApp->Run() != plApplication::Execution::Continue)
    {
      bRun = false;
      ANativeActivity_finish(m_pApp->activity);
    }
    if (m_pApp->destroyRequested)
    {
      break;
    }
  }
}

void plAndroidApplication::HandleCmd(int32_t cmd)
{
  switch (cmd)
  {
    case APP_CMD_INIT_WINDOW:
      if (m_pApp->window != nullptr)
      {
        PL_VERIFY(plRun_Startup(m_pPlApp).Succeeded(), "Failed to startup engine");
        m_bStarted = true;

        int width = ANativeWindow_getWidth(m_pApp->window);
        int height = ANativeWindow_getHeight(m_pApp->window);
        plLog::Info("Init Window: {}x{}", width, height);
      }
      break;
    case APP_CMD_TERM_WINDOW:
      m_pPlApp->RequestQuit();
      break;
    default:
      break;
  }
  plAndroidUtils::s_AppCommandEvent.Broadcast(cmd);
}

int32_t plAndroidApplication::HandleInput(AInputEvent* pEvent)
{
  plAndroidInputEvent event;
  event.m_pEvent = pEvent;
  event.m_bHandled = false;

  plAndroidUtils::s_InputEvent.Broadcast(event);
  return event.m_bHandled ? 1 : 0;
}

void plAndroidApplication::HandleIdent(plInt32 iIdent)
{
  // #TODO:
}

PL_FOUNDATION_DLL void plAndroidRun(struct android_app* pApp, plApplication* pPlApp)
{
  plAndroidApplication androidApp(pApp, pPlApp);

  // This call will loop until APP_CMD_INIT_WINDOW is emitted which triggers plRun_Startup
  androidApp.AndroidRun();

  plRun_Shutdown(pPlApp);

  const int iReturnCode = pPlApp->GetReturnCode();
  if (iReturnCode != 0)
  {
    const char* szReturnCode = pPlApp->TranslateReturnCode();
    if (szReturnCode != nullptr && szReturnCode[0] != '\0')
      __android_log_print(ANDROID_LOG_ERROR, "plEngine", "Return Code: '%s'", szReturnCode);
  }
}

#endif
