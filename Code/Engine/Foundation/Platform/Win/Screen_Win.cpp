#include <Foundation/FoundationPCH.h>

#if PL_ENABLED(PL_PLATFORM_WINDOWS_DESKTOP)

#  include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#  include <Foundation/System/Screen.h>

BOOL CALLBACK plMonitorEnumProc(HMONITOR pMonitor, HDC pHdcMonitor, LPRECT pLprcMonitor, LPARAM data)
{
  PL_IGNORE_UNUSED(pHdcMonitor);
  PL_IGNORE_UNUSED(pLprcMonitor);

  plHybridArray<plScreenInfo, 2>* pScreens = (plHybridArray<plScreenInfo, 2>*)data;

  MONITORINFOEXW info;
  info.cbSize = sizeof(info);

  if (!GetMonitorInfoW(pMonitor, &info))
    return TRUE;

  // In Windows screen coordinates are from top/left to bottom/right
  // ie. 0,0 is left/top , resx/resy is right/bottom

  auto& mon = pScreens->ExpandAndGetRef();
  mon.m_iOffsetX = info.rcMonitor.left;
  mon.m_iOffsetY = info.rcMonitor.top;
  mon.m_iResolutionX = info.rcMonitor.right - info.rcMonitor.left;
  mon.m_iResolutionY = info.rcMonitor.bottom - info.rcMonitor.top;
  mon.m_sDisplayName = info.szDevice;
  mon.m_bIsPrimary = (info.dwFlags & MONITORINFOF_PRIMARY) != 0;

  DISPLAY_DEVICEW ddev;
  ddev.cb = sizeof(ddev);

  if (EnumDisplayDevicesW(info.szDevice, 0, &ddev, 1) != FALSE)
  {
    mon.m_sDisplayName = ddev.DeviceString;
  }

  return TRUE;
}

plResult plScreen::EnumerateScreens(plDynamicArray<plScreenInfo>& out_screens)
{
  out_screens.Clear();
  if (EnumDisplayMonitors(nullptr, nullptr, plMonitorEnumProc, (LPARAM)&out_screens) == FALSE)
    return PL_FAILURE;

  if (out_screens.IsEmpty())
    return PL_FAILURE;

  return PL_SUCCESS;
}

#endif