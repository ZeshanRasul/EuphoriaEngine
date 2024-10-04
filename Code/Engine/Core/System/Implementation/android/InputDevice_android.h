#pragma once

#include <Core/Input/DeviceTypes/MouseKeyboard.h>

struct plAndroidInputEvent;
struct AInputEvent;

/// \brief Android standard input device.
class PL_CORE_DLL plStandardInputDevice : public plInputDeviceMouseKeyboard
{
  PL_ADD_DYNAMIC_REFLECTION(plStandardInputDevice, plInputDeviceMouseKeyboard);

public:
  plStandardInputDevice(plUInt32 uiWindowNumber);
  ~plStandardInputDevice();

  virtual void SetShowMouseCursor(bool bShow) override;
  virtual bool GetShowMouseCursor() const override;
  virtual void SetClipMouseCursor(plMouseCursorClipMode::Enum mode) override;
  virtual plMouseCursorClipMode::Enum GetClipMouseCursor() const override;

private:
  virtual void InitializeDevice() override;
  virtual void RegisterInputSlots() override;
  virtual void ResetInputSlotValues() override;

private:
  void AndroidInputEventHandler(plAndroidInputEvent& event);
  void AndroidAppCommandEventHandler(plInt32 iCmd);
  bool AndroidHandleInput(AInputEvent* pEvent);

private:
  plInt32 m_iResolutionX = 0;
  plInt32 m_iResolutionY = 0;
};