#include <Core/System/Implementation/android/InputDevice_android.h>

#include <Core/Input/InputManager.h>
#include <Foundation/Basics/Platform/Android/AndroidUtils.h>
#include <android/log.h>
#include <android_native_app_glue.h>

// clang-format off
PL_BEGIN_DYNAMIC_REFLECTED_TYPE(plStandardInputDevice, 1, plRTTINoAllocator)
PL_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

// Comment in to get verbose output on android input
// #  define DEBUG_ANDROID_INPUT

#ifdef DEBUG_ANDROID_INPUT
#  define DEBUG_LOG(...) plLog::Debug(__VA_ARGS__)
#else
#  define DEBUG_LOG(...)
#endif

plStandardInputDevice::plStandardInputDevice(plUInt32 uiWindowNumber)
{
  plAndroidUtils::s_InputEvent.AddEventHandler(plMakeDelegate(&plStandardInputDevice::AndroidInputEventHandler, this));
  plAndroidUtils::s_AppCommandEvent.AddEventHandler(plMakeDelegate(&plStandardInputDevice::AndroidAppCommandEventHandler, this));
}

plStandardInputDevice::~plStandardInputDevice()
{
  plAndroidUtils::s_AppCommandEvent.RemoveEventHandler(plMakeDelegate(&plStandardInputDevice::AndroidAppCommandEventHandler, this));
  plAndroidUtils::s_InputEvent.RemoveEventHandler(plMakeDelegate(&plStandardInputDevice::AndroidInputEventHandler, this));
}

void plStandardInputDevice::SetShowMouseCursor(bool bShow) {}

bool plStandardInputDevice::GetShowMouseCursor() const
{
  return false;
}

void plStandardInputDevice::SetClipMouseCursor(plMouseCursorClipMode::Enum mode) {}

plMouseCursorClipMode::Enum plStandardInputDevice::GetClipMouseCursor() const
{
  return plMouseCursorClipMode::Default;
}

void plStandardInputDevice::InitializeDevice()
{
  plHybridArray<plScreenInfo, 2> screens;
  if (plScreen::EnumerateScreens(screens).Succeeded())
  {
    m_iResolutionX = screens[0].m_iResolutionX;
    m_iResolutionY = screens[0].m_iResolutionY;
  }
}

void plStandardInputDevice::RegisterInputSlots()
{
  RegisterInputSlot(plInputSlot_TouchPoint0, "Touchpoint 0", plInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(plInputSlot_TouchPoint0_PositionX, "Touchpoint 0 Position X", plInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(plInputSlot_TouchPoint0_PositionY, "Touchpoint 0 Position Y", plInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(plInputSlot_TouchPoint1, "Touchpoint 1", plInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(plInputSlot_TouchPoint1_PositionX, "Touchpoint 1 Position X", plInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(plInputSlot_TouchPoint1_PositionY, "Touchpoint 1 Position Y", plInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(plInputSlot_TouchPoint2, "Touchpoint 2", plInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(plInputSlot_TouchPoint2_PositionX, "Touchpoint 2 Position X", plInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(plInputSlot_TouchPoint2_PositionY, "Touchpoint 2 Position Y", plInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(plInputSlot_TouchPoint3, "Touchpoint 3", plInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(plInputSlot_TouchPoint3_PositionX, "Touchpoint 3 Position X", plInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(plInputSlot_TouchPoint3_PositionY, "Touchpoint 3 Position Y", plInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(plInputSlot_TouchPoint4, "Touchpoint 4", plInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(plInputSlot_TouchPoint4_PositionX, "Touchpoint 4 Position X", plInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(plInputSlot_TouchPoint4_PositionY, "Touchpoint 4 Position Y", plInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(plInputSlot_TouchPoint5, "Touchpoint 5", plInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(plInputSlot_TouchPoint5_PositionX, "Touchpoint 5 Position X", plInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(plInputSlot_TouchPoint5_PositionY, "Touchpoint 5 Position Y", plInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(plInputSlot_TouchPoint6, "Touchpoint 6", plInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(plInputSlot_TouchPoint6_PositionX, "Touchpoint 6 Position X", plInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(plInputSlot_TouchPoint6_PositionY, "Touchpoint 6 Position Y", plInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(plInputSlot_TouchPoint7, "Touchpoint 7", plInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(plInputSlot_TouchPoint7_PositionX, "Touchpoint 7 Position X", plInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(plInputSlot_TouchPoint7_PositionY, "Touchpoint 7 Position Y", plInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(plInputSlot_TouchPoint8, "Touchpoint 8", plInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(plInputSlot_TouchPoint8_PositionX, "Touchpoint 8 Position X", plInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(plInputSlot_TouchPoint8_PositionY, "Touchpoint 8 Position Y", plInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(plInputSlot_TouchPoint9, "Touchpoint 9", plInputSlotFlags::IsTouchPoint);
  RegisterInputSlot(plInputSlot_TouchPoint9_PositionX, "Touchpoint 9 Position X", plInputSlotFlags::IsTouchPosition);
  RegisterInputSlot(plInputSlot_TouchPoint9_PositionY, "Touchpoint 9 Position Y", plInputSlotFlags::IsTouchPosition);

  RegisterInputSlot(plInputSlot_MouseWheelUp, "Mousewheel Up", plInputSlotFlags::IsMouseWheel);
  RegisterInputSlot(plInputSlot_MouseWheelDown, "Mousewheel Down", plInputSlotFlags::IsMouseWheel);
}

void plStandardInputDevice::ResetInputSlotValues()
{
  m_InputSlotValues[plInputSlot_MouseWheelUp] = 0;
  m_InputSlotValues[plInputSlot_MouseWheelDown] = 0;
  for (int id = 0; id < 10; ++id)
  {
    // We can't reset the position inside AndroidHandleInput as we want the position to be valid when lifting a finger. Thus, we clear the position here after the update has been performed.
    if (m_InputSlotValues[plInputManager::GetInputSlotTouchPoint(id)] == 0)
    {
      m_InputSlotValues[plInputManager::GetInputSlotTouchPointPositionX(id)] = 0;
      m_InputSlotValues[plInputManager::GetInputSlotTouchPointPositionY(id)] = 0;
    }
  }
}

void plStandardInputDevice::AndroidInputEventHandler(plAndroidInputEvent& event)
{
  event.m_bHandled = AndroidHandleInput(event.m_pEvent);
  SUPER::UpdateInputSlotValues();
}

void plStandardInputDevice::AndroidAppCommandEventHandler(plInt32 iCmd)
{
  if (iCmd == APP_CMD_WINDOW_RESIZED)
  {
    plHybridArray<plScreenInfo, 2> screens;
    if (plScreen::EnumerateScreens(screens).Succeeded())
    {
      m_iResolutionX = screens[0].m_iResolutionX;
      m_iResolutionY = screens[0].m_iResolutionY;
    }
  }
}

bool plStandardInputDevice::AndroidHandleInput(AInputEvent* pEvent)
{
  // #TODO_ANDROID Only touchscreen input is implemented right now.
  const plInt32 iEventType = AInputEvent_getType(pEvent);
  const plInt32 iEventSource = AInputEvent_getSource(pEvent);
  const plUInt32 uiAction = (plUInt32)AMotionEvent_getAction(pEvent);
  const plInt32 iKeyCode = AKeyEvent_getKeyCode(pEvent);
  const plInt32 iButtonState = AMotionEvent_getButtonState(pEvent);
  PL_IGNORE_UNUSED(iKeyCode);
  PL_IGNORE_UNUSED(iButtonState);
  DEBUG_LOG("Android INPUT: iEventType: {}, iEventSource: {}, uiAction: {}, iKeyCode: {}, iButtonState: {}", iEventType,
    iEventSource, uiAction, iKeyCode, iButtonState);

  if (m_iResolutionX == 0 || m_iResolutionY == 0)
    return false;

  // I.e. fingers have touched the touchscreen.
  if (iEventType == AINPUT_EVENT_TYPE_MOTION && (iEventSource & AINPUT_SOURCE_TOUCHSCREEN) != 0)
  {
    // Update pointer positions
    const plUInt64 uiPointerCount = AMotionEvent_getPointerCount(pEvent);
    for (plUInt32 uiPointerIndex = 0; uiPointerIndex < uiPointerCount; uiPointerIndex++)
    {
      const float fPixelX = AMotionEvent_getX(pEvent, uiPointerIndex);
      const float fPixelY = AMotionEvent_getY(pEvent, uiPointerIndex);
      const plInt32 id = AMotionEvent_getPointerId(pEvent, uiPointerIndex);
      if (id < 10)
      {
        m_InputSlotValues[plInputManager::GetInputSlotTouchPointPositionX(id)] = static_cast<float>(fPixelX / static_cast<float>(m_iResolutionX));
        m_InputSlotValues[plInputManager::GetInputSlotTouchPointPositionY(id)] = static_cast<float>(fPixelY / static_cast<float>(m_iResolutionY));
        DEBUG_LOG("Finger MOVE: {} = {} x {}", id, m_InputSlotValues[plInputManager::GetInputSlotTouchPointPositionX(id)], m_InputSlotValues[plInputManager::GetInputSlotTouchPointPositionY(id)]);
      }
    }

    // Update pointer state
    const plUInt32 uiActionEvent = uiAction & AMOTION_EVENT_ACTION_MASK;
    const plUInt32 uiActionPointerIndex = (uiAction & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;

    const plInt32 id = AMotionEvent_getPointerId(pEvent, uiActionPointerIndex);
    // We only support up to 10 touch points at the same time.
    if (id >= 10)
      return false;

    {
      // Not sure if the action finger is always present in the upper loop of uiPointerCount, so we update it here for good measure.
      const float fPixelX = AMotionEvent_getX(pEvent, uiActionPointerIndex);
      const float fPixelY = AMotionEvent_getY(pEvent, uiActionPointerIndex);
      m_InputSlotValues[plInputManager::GetInputSlotTouchPointPositionX(id)] = static_cast<float>(fPixelX / static_cast<float>(m_iResolutionX));
      m_InputSlotValues[plInputManager::GetInputSlotTouchPointPositionY(id)] = static_cast<float>(fPixelY / static_cast<float>(m_iResolutionY));
      DEBUG_LOG("Finger MOVE: {} = {} x {}", id, m_InputSlotValues[plInputManager::GetInputSlotTouchPointPositionX(id)], m_InputSlotValues[plInputManager::GetInputSlotTouchPointPositionY(id)]);
    }

    switch (uiActionEvent)
    {
      case AMOTION_EVENT_ACTION_DOWN:
      case AMOTION_EVENT_ACTION_POINTER_DOWN:
        m_InputSlotValues[plInputManager::GetInputSlotTouchPoint(id)] = 1;
        DEBUG_LOG("Finger DOWN: {}", id);
        return true;
      case AMOTION_EVENT_ACTION_MOVE:
        // Finger moved (we always update that at the top).
        return true;
      case AMOTION_EVENT_ACTION_UP:
      case AMOTION_EVENT_ACTION_POINTER_UP:
      case AMOTION_EVENT_ACTION_CANCEL:
      case AMOTION_EVENT_ACTION_OUTSIDE:
        m_InputSlotValues[plInputManager::GetInputSlotTouchPoint(id)] = 0;
        DEBUG_LOG("Finger UP: {}", id);
        return true;
      case AMOTION_EVENT_ACTION_SCROLL:
      {
        float fRotated = AMotionEvent_getAxisValue(pEvent, AMOTION_EVENT_AXIS_VSCROLL, 0);
        if (fRotated > 0)
          m_InputSlotValues[plInputSlot_MouseWheelUp] = fRotated;
        else
          m_InputSlotValues[plInputSlot_MouseWheelDown] = fRotated;
        return true;
      }
      case AMOTION_EVENT_ACTION_HOVER_ENTER:
      case AMOTION_EVENT_ACTION_HOVER_MOVE:
      case AMOTION_EVENT_ACTION_HOVER_EXIT:
        return false;
      default:
        DEBUG_LOG("Unknown AMOTION_EVENT_ACTION: {}", uiActionEvent);
        return false;
    }
  }
  return false;
}