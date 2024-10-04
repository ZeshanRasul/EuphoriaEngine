#include <Core/CorePCH.h>

#include <Core/System/Window.h>
#include <Foundation/Basics.h>
#include <Foundation/Basics/Platform/Android/AndroidUtils.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Types/UniquePtr.h>
#include <android_native_app_glue.h>

struct ANativeWindow;

namespace
{
  ANativeWindow* s_androidWindow = nullptr;
  plEventSubscriptionID s_androidCommandID = 0;
} // namespace

plResult plWindow::Initialize()
{
  PL_LOG_BLOCK("plWindow::Initialize", m_CreationDescription.m_Title.GetData());
  if (m_bInitialized)
  {
    Destroy().AssertSuccess();
  }

  if (m_CreationDescription.m_WindowMode == plWindowMode::WindowResizable)
  {
    s_androidCommandID = plAndroidUtils::s_AppCommandEvent.AddEventHandler([this](plInt32 iCmd)
      {
        if (iCmd == APP_CMD_WINDOW_RESIZED)
        {
          plHybridArray<plScreenInfo, 2> screens;
          if (plScreen::EnumerateScreens(screens).Succeeded())
          {
            m_CreationDescription.m_Resolution.width = screens[0].m_iResolutionX;
            m_CreationDescription.m_Resolution.height = screens[0].m_iResolutionY;
            this->OnResize(plSizeU32(screens[0].m_iResolutionX, screens[0].m_iResolutionY));
          }
        } });
  }
  
  // Checking and adjustments to creation desc.
  if (m_CreationDescription.AdjustWindowSizeAndPosition().Failed())
    plLog::Warning("Failed to adjust window size and position settings.");

  PL_ASSERT_RELEASE(m_CreationDescription.m_Resolution.HasNonZeroArea(), "The client area size can't be zero sized!");
  PL_ASSERT_RELEASE(s_androidWindow == nullptr, "Window already exists. Only one Android window is supported at any time!");

  s_androidWindow = plAndroidUtils::GetAndroidApp()->window;
  m_hWindowHandle = s_androidWindow;
  m_pInputDevice = PL_DEFAULT_NEW(plStandardInputDevice, 0);
  m_bInitialized = true;

  return PL_SUCCESS;
}

plResult plWindow::Destroy()
{
  if (!m_bInitialized)
    return PL_SUCCESS;

  PL_LOG_BLOCK("plWindow::Destroy");

  s_androidWindow = nullptr;

  if (s_androidCommandID != 0)
  {
    plAndroidUtils::s_AppCommandEvent.RemoveEventHandler(s_androidCommandID);
  }

  plLog::Success("Window destroyed.");

  return PL_SUCCESS;
}

plResult plWindow::Resize(const plSizeU32& newWindowSize)
{
  // No need to resize on Android, swapchain can take any size at any time.
  m_CreationDescription.m_Resolution.width = newWindowSize.width;
  m_CreationDescription.m_Resolution.height = newWindowSize.height;
  return PL_SUCCESS;
}

void plWindow::ProcessWindowMessages()
{
  PL_ASSERT_RELEASE(s_androidWindow != nullptr, "No uwp window data available.");
}

void plWindow::OnResize(const plSizeU32& newWindowSize)
{
  plLog::Info("Window resized to ({0}, {1})", newWindowSize.width, newWindowSize.height);
}

plWindowHandle plWindow::GetNativeWindowHandle() const
{
  return m_hWindowHandle;
}