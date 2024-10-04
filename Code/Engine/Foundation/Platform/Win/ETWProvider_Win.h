#pragma once

#include <Foundation/Basics.h>

#if PL_ENABLED(PL_PLATFORM_WINDOWS)

#  include <Foundation/FoundationInternal.h>
#  include <Foundation/Logging/Log.h>

PL_FOUNDATION_INTERNAL_HEADER

class plETWProvider
{
public:
  plETWProvider();
  ~plETWProvider();

  void LogMessage(plLogMsgType::Enum eventType, plUInt8 uiIndentation, plStringView sText);

  static plETWProvider& GetInstance();
};

#endif