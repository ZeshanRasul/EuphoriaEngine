#include <Foundation/FoundationPCH.h>

#if PL_ENABLED(PL_PLATFORM_WEB)

#  include <Foundation/System/MiniDumpUtils.h>

plStatus plMiniDumpUtils::WriteExternalProcessMiniDump(plStringView sDumpFile, plUInt32 uiProcessID, plDumpType dumpTypeOverride)
{
  return plStatus("Not implemented on Web");
}

plStatus plMiniDumpUtils::LaunchMiniDumpTool(plStringView sDumpFile, plDumpType dumpTypeOverride)
{
  return plStatus("Not implemented on Web");
}

#endif
