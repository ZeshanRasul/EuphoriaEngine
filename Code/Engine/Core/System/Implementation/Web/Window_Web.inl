#include <Core/CorePCH.h>

#include <Core/System/Window.h>
#include <Foundation/Basics.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/System/SystemInformation.h>

plResult plWindow::Initialize()
{
  PL_LOG_BLOCK("plWindow::Initialize", m_CreationDescription.m_Title.GetData());

  if (m_bInitialized)
  {
    Destroy().AssertSuccess();
  }

  m_hWindowHandle = nullptr; // TODO WebGPU: can we hold a reference to the canvas ? (should we?)
  m_pInputDevice = PL_DEFAULT_NEW(plStandardInputDevice);
  m_bInitialized = true;

  return PL_SUCCESS;
}

plResult plWindow::Destroy()
{
  if (!m_bInitialized)
    return PL_SUCCESS;

  PL_LOG_BLOCK("plWindow::Destroy");

  m_bInitialized = false;
  m_pInputDevice.Clear();

  plLog::Success("Window destroyed.");
  return PL_SUCCESS;
}

plResult plWindow::Resize(const plSizeU32& newWindowSize)
{
  m_CreationDescription.m_Resolution.width = newWindowSize.width;
  m_CreationDescription.m_Resolution.height = newWindowSize.height;
  return PL_SUCCESS;
}

void plWindow::ProcessWindowMessages()
{
}

void plWindow::OnResize(const plSizeU32& newWindowSize)
{
  plLog::Info("Window resized to ({0}, {1})", newWindowSize.width, newWindowSize.height);
}

plWindowHandle plWindow::GetNativeWindowHandle() const
{
  return m_hWindowHandle;
}
