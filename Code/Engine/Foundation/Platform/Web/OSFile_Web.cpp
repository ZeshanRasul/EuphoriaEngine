#include <Foundation/FoundationPCH.h>

#if PL_ENABLED(PL_PLATFORM_WEB)

#  define PL_SKIP_FOLDER_PATHS
#  include <Foundation/Platform/Posix/OSFile_Posix.h>

plStringView plOSFile::GetApplicationPath()
{
  s_sApplicationPath = "/";
  return s_sApplicationPath;
}

plString plOSFile::GetUserDataFolder(plStringView sSubFolder)
{
  return "/web-app-user";
}

plString plOSFile::GetTempDataFolder(plStringView sSubFolder)
{
  return "/web-app-temp";
}

plString plOSFile::GetUserDocumentsFolder(plStringView sSubFolder)
{
  return "/web-app-docs";
}

const plString plOSFile::GetCurrentWorkingDirectory()
{
  return ".";
}

#endif
