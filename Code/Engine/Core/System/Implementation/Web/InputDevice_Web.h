 #pragma once

#if PL_ENABLED(PL_PLATFORM_WEB)

#  include <Core/Input/DeviceTypes/MouseKeyboard.h>
#  include <Foundation/Containers/ArrayMap.h>

class PL_CORE_DLL plStandardInputDevice : public plInputDeviceMouseKeyboard
{
  PL_ADD_DYNAMIC_REFLECTION(plStandardInputDevice, plInputDeviceMouseKeyboard);

public:
  plStandardInputDevice();
  ~plStandardInputDevice();

  virtual void SetShowMouseCursor(bool bShow) override;
  virtual bool GetShowMouseCursor() const override;
  virtual void SetClipMouseCursor(plMouseCursorClipMode::Enum mode) override;
  virtual plMouseCursorClipMode::Enum GetClipMouseCursor() const override;

public:
  // Web callbacks to set the state
  static void onWebChar(const std::string& text);
  static void onWebKey(const std::string& scancode, bool bDown);
  static void onWebMouseClick(plInt32 iButton, bool bDown);
  static void onWebMouseMove(double x, double y);
  static void onWebMouseLeave();
  static void onWebMouseWheel(double y);

private:
  virtual void InitializeDevice() override;
  virtual void RegisterInputSlots() override;
  virtual void ResetInputSlotValues() override;

  static plArrayMap<plUInt32, plStringView> s_WebKeyNameToInputSlot;

  plVec2d m_LastPos = plVec2d(plMath::MaxValue<double>());
};

#endif
