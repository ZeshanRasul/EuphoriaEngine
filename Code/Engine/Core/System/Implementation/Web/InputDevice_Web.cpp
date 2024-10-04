#if PL_ENABLED(PL_PLATFORM_WEB)

#  include <Core/System/Implementation/Web/InputDevice_Web.h>

// clang-format off
PL_BEGIN_DYNAMIC_REFLECTED_TYPE(plStandardInputDevice, 1, plRTTINoAllocator)
PL_END_DYNAMIC_REFLECTED_TYPE;
// clang-format on

static plStandardInputDevice* s_pInputDevice = nullptr;
plArrayMap<plUInt32, plStringView> plStandardInputDevice::s_WebKeyNameToInputSlot;

plStandardInputDevice::plStandardInputDevice()
{
  PL_ASSERT_DEV(s_pInputDevice == nullptr, "Web only allows for one plStandardInputDevice.");
  s_pInputDevice = this;
}

plStandardInputDevice::~plStandardInputDevice()
{
  s_pInputDevice = nullptr;
}

void plStandardInputDevice::SetShowMouseCursor(bool bShow)
{
  if (!bShow)
  {
    plLog::Warning("plStandardInputDevice::SetShowMouseCursor: not available on this platform.");
  }
}

bool plStandardInputDevice::GetShowMouseCursor() const
{
  return true;
}

void plStandardInputDevice::SetClipMouseCursor(plMouseCursorClipMode::Enum mode)
{
  plLog::Warning("plStandardInputDevice::SetClipMouseCursor: not available on this platform.");
}

plMouseCursorClipMode::Enum plStandardInputDevice::GetClipMouseCursor() const
{
  return plMouseCursorClipMode::Default;
}

void plStandardInputDevice::InitializeDevice() {}

void plStandardInputDevice::RegisterInputSlots()
{
  RegisterInputSlot(plInputSlot_KeyLeft, "Left", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyRight, "Right", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyUp, "Up", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyDown, "Down", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_KeyEscape, "Escape", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeySpace, "Space", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyBackspace, "Backspace", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyReturn, "Return", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyTab, "Tab", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_KeyLeftShift, "Left Shift", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyRightShift, "Right Shift", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_KeyLeftCtrl, "Left Ctrl", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyRightCtrl, "Right Ctrl", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_KeyLeftAlt, "Left Alt", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyRightAlt, "Right Alt", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_KeyLeftWin, "Left Win", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyRightWin, "Right Win", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_KeyBracketOpen, "[", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyBracketClose, "]", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_KeySemicolon, ";", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyApostrophe, "'", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeySlash, "/", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyEquals, "=", plInputSlotFlags::IsButton);
  // TODO RegisterInputSlot(plInputSlot_KeyTilde, "~", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyHyphen, "-", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyComma, ",", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyPeriod, ".", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyBackslash, "\\", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyPipe, "|", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_Key1, "1", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_Key2, "2", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_Key3, "3", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_Key4, "4", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_Key5, "5", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_Key6, "6", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_Key7, "7", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_Key8, "8", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_Key9, "9", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_Key0, "0", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_KeyNumpad1, "Numpad 1", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpad2, "Numpad 2", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpad3, "Numpad 3", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpad4, "Numpad 4", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpad5, "Numpad 5", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpad6, "Numpad 6", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpad7, "Numpad 7", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpad8, "Numpad 8", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpad9, "Numpad 9", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpad0, "Numpad 0", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_KeyA, "A", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyB, "B", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyC, "C", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyD, "D", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyE, "E", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyF, "F", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyG, "G", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyH, "H", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyI, "I", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyJ, "J", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyK, "K", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyL, "L", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyM, "M", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyN, "N", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyO, "O", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyP, "P", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyQ, "Q", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyR, "R", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyS, "S", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyT, "T", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyU, "U", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyV, "V", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyW, "W", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyX, "X", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyY, "Y", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyZ, "Z", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_KeyF1, "F1", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyF2, "F2", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyF3, "F3", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyF4, "F4", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyF5, "F5", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyF6, "F6", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyF7, "F7", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyF8, "F8", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyF9, "F9", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyF10, "F10", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyF11, "F11", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyF12, "F12", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_KeyHome, "Home", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyEnd, "End", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyDelete, "Delete", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyInsert, "Insert", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyPageUp, "Page Up", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyPageDown, "Page Down", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_KeyNumLock, "Numlock", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpadPlus, "Numpad +", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpadMinus, "Numpad -", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpadStar, "Numpad *", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpadSlash, "Numpad /", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpadPeriod, "Numpad .", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNumpadEnter, "Enter", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_KeyCapsLock, "Capslock", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyPrint, "Print", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyScroll, "Scroll", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyPause, "Pause", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_KeyApps, "Application", plInputSlotFlags::IsButton);

  /* TODO
  RegisterInputSlot(plInputSlot_KeyPrevTrack, "Previous Track", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyNextTrack, "Next Track", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyPlayPause, "Play / Pause", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyStop, "Stop", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyVolumeUp, "Volume Up", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyVolumeDown, "Volume Down", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_KeyMute, "Mute", plInputSlotFlags::IsButton);
  */

  RegisterInputSlot(plInputSlot_MousePositionX, "Mouse Position X", plInputSlotFlags::IsMouseAxisPosition);
  RegisterInputSlot(plInputSlot_MousePositionY, "Mouse Position Y", plInputSlotFlags::IsMouseAxisPosition);

  RegisterInputSlot(plInputSlot_MouseMoveNegX, "Mouse Move Left", plInputSlotFlags::IsMouseAxisMove);
  RegisterInputSlot(plInputSlot_MouseMovePosX, "Mouse Move Right", plInputSlotFlags::IsMouseAxisMove);
  RegisterInputSlot(plInputSlot_MouseMoveNegY, "Mouse Move Down", plInputSlotFlags::IsMouseAxisMove);
  RegisterInputSlot(plInputSlot_MouseMovePosY, "Mouse Move Up", plInputSlotFlags::IsMouseAxisMove);

  RegisterInputSlot(plInputSlot_MouseButton0, "Mousebutton 0", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_MouseButton1, "Mousebutton 1", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_MouseButton2, "Mousebutton 2", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_MouseButton3, "Mousebutton 3", plInputSlotFlags::IsButton);
  RegisterInputSlot(plInputSlot_MouseButton4, "Mousebutton 4", plInputSlotFlags::IsButton);

  RegisterInputSlot(plInputSlot_MouseWheelUp, "Mousewheel Up", plInputSlotFlags::IsMouseWheel);
  RegisterInputSlot(plInputSlot_MouseWheelDown, "Mousewheel Down", plInputSlotFlags::IsMouseWheel);

  auto SetupKey = [&](plStringView sName, plStringView sKey)
  {
    const plUInt32 uiHash = plHashingUtils::xxHash32String(sName);
    s_WebKeyNameToInputSlot.Insert(uiHash, sKey);
  };

  // see https://developer.mozilla.org/en-US/docs/Web/API/UI_Events/Keyboard_event_code_values

  SetupKey("KeyA", plInputSlot_KeyA);
  SetupKey("KeyB", plInputSlot_KeyB);
  SetupKey("KeyC", plInputSlot_KeyC);
  SetupKey("KeyD", plInputSlot_KeyD);
  SetupKey("KeyE", plInputSlot_KeyE);
  SetupKey("KeyF", plInputSlot_KeyF);
  SetupKey("KeyG", plInputSlot_KeyG);
  SetupKey("KeyH", plInputSlot_KeyH);
  SetupKey("KeyI", plInputSlot_KeyI);
  SetupKey("KeyJ", plInputSlot_KeyJ);
  SetupKey("KeyK", plInputSlot_KeyK);
  SetupKey("KeyL", plInputSlot_KeyL);
  SetupKey("KeyM", plInputSlot_KeyM);
  SetupKey("KeyN", plInputSlot_KeyN);
  SetupKey("KeyO", plInputSlot_KeyO);
  SetupKey("KeyP", plInputSlot_KeyP);
  SetupKey("KeyQ", plInputSlot_KeyQ);
  SetupKey("KeyR", plInputSlot_KeyR);
  SetupKey("KeyS", plInputSlot_KeyS);
  SetupKey("KeyT", plInputSlot_KeyT);
  SetupKey("KeyU", plInputSlot_KeyU);
  SetupKey("KeyV", plInputSlot_KeyV);
  SetupKey("KeyW", plInputSlot_KeyW);
  SetupKey("KeyX", plInputSlot_KeyX);
  SetupKey("KeyY", plInputSlot_KeyY);
  SetupKey("KeyZ", plInputSlot_KeyZ);

  SetupKey("Quote", plInputSlot_KeyApostrophe);
  SetupKey("Backslash", plInputSlot_KeyBackslash);
  SetupKey("Backspace", plInputSlot_KeyBackspace);
  SetupKey("CapsLock", plInputSlot_KeyCapsLock);
  SetupKey("Comma", plInputSlot_KeyComma);
  SetupKey("Delete", plInputSlot_KeyDelete);
  SetupKey("ArrowDown", plInputSlot_KeyDown);

  SetupKey("End", plInputSlot_KeyEnd);
  SetupKey("Enter", plInputSlot_KeyReturn);

  SetupKey("Equal", plInputSlot_KeyEquals);

  SetupKey("Escape", plInputSlot_KeyEscape);

  SetupKey("F1", plInputSlot_KeyF1);
  SetupKey("F2", plInputSlot_KeyF2);
  SetupKey("F3", plInputSlot_KeyF3);
  SetupKey("F4", plInputSlot_KeyF4);
  SetupKey("F5", plInputSlot_KeyF5);
  SetupKey("F6", plInputSlot_KeyF6);
  SetupKey("F7", plInputSlot_KeyF7);
  SetupKey("F8", plInputSlot_KeyF8);
  SetupKey("F9", plInputSlot_KeyF9);
  SetupKey("F10", plInputSlot_KeyF10);
  SetupKey("F11", plInputSlot_KeyF11);
  SetupKey("F12", plInputSlot_KeyF12);

  SetupKey("Backquote", plInputSlot_KeyTilde);
  SetupKey("Home", plInputSlot_KeyHome);
  SetupKey("Insert", plInputSlot_KeyInsert);

  SetupKey("Numpad0", plInputSlot_KeyNumpad0);
  SetupKey("Numpad1", plInputSlot_KeyNumpad1);
  SetupKey("Numpad2", plInputSlot_KeyNumpad2);
  SetupKey("Numpad3", plInputSlot_KeyNumpad3);
  SetupKey("Numpad4", plInputSlot_KeyNumpad4);
  SetupKey("Numpad5", plInputSlot_KeyNumpad5);
  SetupKey("Numpad6", plInputSlot_KeyNumpad6);
  SetupKey("Numpad7", plInputSlot_KeyNumpad7);
  SetupKey("Numpad8", plInputSlot_KeyNumpad8);
  SetupKey("Numpad9", plInputSlot_KeyNumpad9);

  SetupKey("NumpadAdd", plInputSlot_KeyNumpadPlus);
  SetupKey("NumpadDecimal", plInputSlot_KeyNumpadPeriod);
  SetupKey("NumpadComma", plInputSlot_KeyNumpadPeriod); // intentionally duplicate mapping
  SetupKey("NumpadDivide", plInputSlot_KeyNumpadSlash);
  SetupKey("NumpadEnter", plInputSlot_KeyNumpadEnter);
  SetupKey("NumpadMultiply", plInputSlot_KeyNumpadStar);
  SetupKey("NumpadSubtract", plInputSlot_KeyNumpadMinus);

  SetupKey("ArrowLeft", plInputSlot_KeyLeft);
  SetupKey("AltLeft", plInputSlot_KeyLeftAlt);
  SetupKey("BracketLeft", plInputSlot_KeyBracketOpen);
  SetupKey("ControlLeft", plInputSlot_KeyLeftCtrl);
  SetupKey("ShiftLeft", plInputSlot_KeyLeftShift);
  SetupKey("MetaLeft", plInputSlot_KeyLeftWin);

  SetupKey("ContextMenu", plInputSlot_KeyApps);
  // SetupKey("Minus", plInputSlot_KeyMinus);

  SetupKey("Digit0", plInputSlot_Key0);
  SetupKey("Digit1", plInputSlot_Key1);
  SetupKey("Digit2", plInputSlot_Key2);
  SetupKey("Digit3", plInputSlot_Key3);
  SetupKey("Digit4", plInputSlot_Key4);
  SetupKey("Digit5", plInputSlot_Key5);
  SetupKey("Digit6", plInputSlot_Key6);
  SetupKey("Digit7", plInputSlot_Key7);
  SetupKey("Digit8", plInputSlot_Key8);
  SetupKey("Digit9", plInputSlot_Key9);

  SetupKey("NumLock", plInputSlot_KeyNumLock);
  SetupKey("PageDown", plInputSlot_KeyPageDown);
  SetupKey("PageUp", plInputSlot_KeyPageUp);
  SetupKey("Pause", plInputSlot_KeyPause);
  SetupKey("Period", plInputSlot_KeyPeriod);
  SetupKey("PrintScreen", plInputSlot_KeyPrint);

  SetupKey("ArrowRight", plInputSlot_KeyRight);
  SetupKey("AltRight", plInputSlot_KeyRightAlt);
  SetupKey("BracketRight", plInputSlot_KeyBracketClose);
  SetupKey("ControlRight", plInputSlot_KeyRightCtrl);
  SetupKey("ShiftRight", plInputSlot_KeyRightShift);
  SetupKey("MetaRight", plInputSlot_KeyRightWin);

  SetupKey("ScrollLock", plInputSlot_KeyScroll);
  SetupKey("Semicolon", plInputSlot_KeySemicolon);
  SetupKey("Slash", plInputSlot_KeySlash);
  SetupKey("Space", plInputSlot_KeySpace);

  SetupKey("Tab", plInputSlot_KeyTab);
  SetupKey("ArrowUp", plInputSlot_KeyUp);
  SetupKey("IntlBackslash", plInputSlot_KeyPipe);

  s_WebKeyNameToInputSlot.Sort();
}

void plStandardInputDevice::ResetInputSlotValues()
{
  m_InputSlotValues[plInputSlot_MouseWheelUp] = 0;
  m_InputSlotValues[plInputSlot_MouseWheelDown] = 0;
  m_InputSlotValues[plInputSlot_MouseMoveNegX] = 0;
  m_InputSlotValues[plInputSlot_MouseMovePosX] = 0;
  m_InputSlotValues[plInputSlot_MouseMoveNegY] = 0;
  m_InputSlotValues[plInputSlot_MouseMovePosY] = 0;
}

void plStandardInputDevice::onWebChar(const std::string& text)
{
  if (s_pInputDevice == nullptr)
    return;

  plStringBuilder tmp = text.c_str();
  if (tmp.IsEmpty())
    return;

  s_pInputDevice->m_uiLastCharacter = tmp.GetView().GetIteratorFront().GetCharacter();

  // plLog::Info("Char '{}'", text.c_str());
}

void plStandardInputDevice::onWebKey(const std::string& name, bool bDown)
{
  if (s_pInputDevice == nullptr)
    return;

  const plUInt32 hash = plHashingUtils::xxHash32String(name.c_str());
  const plUInt32 idx = s_WebKeyNameToInputSlot.Find(hash);

  if (idx == plInvalidIndex)
    return;

  plStringView sInputSlotName = s_WebKeyNameToInputSlot.GetValue(idx);
  s_pInputDevice->m_InputSlotValues[sInputSlotName] = bDown ? 1.0f : 0.0f;

  plLog::Info("Key '{}' - {}", sInputSlotName, bDown);
}

void plStandardInputDevice::onWebMouseClick(plInt32 iButton, bool bDown)
{
  if (s_pInputDevice == nullptr)
    return;

  const char* inputSlot = nullptr;
  switch (iButton)
  {
    case 0:
      inputSlot = plInputSlot_MouseButton0;
      break;
    case 1:
      inputSlot = plInputSlot_MouseButton2; // button '2' is the right button and button '1' is middle
      break;
    case 2:
      inputSlot = plInputSlot_MouseButton1;
      break;
    case 3:
      inputSlot = plInputSlot_MouseButton3;
      break;
    case 4:
      inputSlot = plInputSlot_MouseButton4;
      break;
  }

  if (inputSlot)
  {
    s_pInputDevice->m_InputSlotValues[inputSlot] = bDown ? 1.0f : 0.0f;

    plLog::Info("Click '{}' - {}", inputSlot, bDown);
  }
}

void plStandardInputDevice::onWebMouseMove(double x, double y)
{
  if (s_pInputDevice == nullptr)
    return;

  s_pInputDevice->m_InputSlotValues[plInputSlot_MousePositionX] = static_cast<float>(x);
  s_pInputDevice->m_InputSlotValues[plInputSlot_MousePositionY] = static_cast<float>(y);

  if (s_pInputDevice->m_LastPos.x != plMath::MaxValue<double>())
  {
    const plVec2d diff = plVec2d(x, y) - s_pInputDevice->m_LastPos;

    s_pInputDevice->m_InputSlotValues[plInputSlot_MouseMoveNegX] += ((diff.x < 0) ? (float)-diff.x : 0.0f) * s_pInputDevice->GetMouseSpeed().x;
    s_pInputDevice->m_InputSlotValues[plInputSlot_MouseMovePosX] += ((diff.x > 0) ? (float)diff.x : 0.0f) * s_pInputDevice->GetMouseSpeed().x;
    s_pInputDevice->m_InputSlotValues[plInputSlot_MouseMoveNegY] += ((diff.y < 0) ? (float)-diff.y : 0.0f) * s_pInputDevice->GetMouseSpeed().y;
    s_pInputDevice->m_InputSlotValues[plInputSlot_MouseMovePosY] += ((diff.y > 0) ? (float)diff.y : 0.0f) * s_pInputDevice->GetMouseSpeed().y;

    // plLog::Info("Mouse Move: {} / {} - {} / {}", x, y, diff.x, diff.y);
  }

  s_pInputDevice->m_LastPos = plVec2d(x, y);
}

void plStandardInputDevice::onWebMouseLeave()
{
  if (s_pInputDevice == nullptr)
    return;

  // reset mouse button states
  s_pInputDevice->onWebMouseClick(0, false);
  s_pInputDevice->onWebMouseClick(1, false);
  s_pInputDevice->onWebMouseClick(2, false);
  s_pInputDevice->onWebMouseClick(3, false);
  s_pInputDevice->onWebMouseClick(4, false);

  plLog::Info("Mouse leave");
}

void plStandardInputDevice::onWebMouseWheel(double y)
{
  if (s_pInputDevice == nullptr)
    return;

  if (y > 0)
  {
    s_pInputDevice->m_InputSlotValues[plInputSlot_MouseWheelUp] = static_cast<float>(y);
  }
  else
  {
    s_pInputDevice->m_InputSlotValues[plInputSlot_MouseWheelDown] = static_cast<float>(-y);
  }

  plLog::Info("Mouse wheel: {}", y);
}

#  include <emscripten/bind.h>
using namespace emscripten;

EMSCRIPTEN_BINDINGS(input)
{
  function("onWebChar", &plStandardInputDevice::onWebChar);
  function("onWebKey", &plStandardInputDevice::onWebKey);
  function("onWebMouseClick", &plStandardInputDevice::onWebMouseClick);
  function("onWebMouseMove", &plStandardInputDevice::onWebMouseMove);
  function("onWebMouseLeave", &plStandardInputDevice::onWebMouseLeave);
  function("onWebMouseWheel", &plStandardInputDevice::onWebMouseWheel);
}

#endif


PL_STATICLINK_FILE(Core, Core_Platform_Web_InputDevice_Web);
