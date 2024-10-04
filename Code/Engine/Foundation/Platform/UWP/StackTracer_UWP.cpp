#include <Foundation/FoundationPCH.h>

#if PL_ENABLED(PL_PLATFORM_WINDOWS_UWP)

#  include <Foundation/System/StackTracer.h>

void plStackTracer::OnPluginEvent(const plPluginEvent& e)
{
  PL_IGNORE_UNUSED(e);
}

// static
plUInt32 plStackTracer::GetStackTrace(plArrayPtr<void*>& trace, void* pContext)
{
  PL_IGNORE_UNUSED(trace);
  PL_IGNORE_UNUSED(pContext);

  return 0;
}

// static
void plStackTracer::ResolveStackTrace(const plArrayPtr<void*>& trace, PrintFunc printFunc)
{
  PL_IGNORE_UNUSED(trace);

  char szBuffer[512] = "Stack Traces are currently not supported on UWP";

  printFunc(szBuffer);
}

#endif
