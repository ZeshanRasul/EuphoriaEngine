#include <Foundation/FoundationPCH.h>

#include <Foundation/Strings/Implementation/StringIterator.h>
#include <Foundation/Strings/StringBuilder.h>

const char* plPathUtils::FindPreviousSeparator(const char* szPathStart, const char* szStartSearchAt)
{
  if (plStringUtils::IsNullOrEmpty(szPathStart))
    return nullptr;

  while (szStartSearchAt > szPathStart)
  {
    plUnicodeUtils::MoveToPriorUtf8(szStartSearchAt, szPathStart).AssertSuccess();

    if (IsPathSeparator(*szStartSearchAt))
      return szStartSearchAt;
  }

  return nullptr;
}

bool plPathUtils::HasAnyExtension(plStringView sPath)
{
  return !GetFileExtension(sPath, true).IsEmpty();
}

bool plPathUtils::HasExtension(plStringView sPath, plStringView sExtension)
{
  sPath = GetFileNameAndExtension(sPath);
  plStringView fullExt = GetFileExtension(sPath, true);

  if (sExtension.IsEmpty() && fullExt.IsEmpty())
    return true;

  // if there is a single dot at the start of the extension, remove it
  if (sExtension.StartsWith("."))
    sExtension.ChopAwayFirstCharacterAscii();

  if (!fullExt.EndsWith_NoCase(sExtension))
    return false;

  // remove the checked extension
  sPath = plStringView(sPath.GetStartPointer(), sPath.GetEndPointer() - sExtension.GetElementCount());

  // checked extension didn't start with a dot -> make sure there is one at the end of sPath
  if (!sPath.EndsWith("."))
    return false;

  // now make sure the rest isn't just the dot
  return sPath.GetElementCount() > 1;
}

plStringView plPathUtils::GetFileExtension(plStringView sPath, bool bFullExtension)
{
  // get rid of any path before the filename
  sPath = GetFileNameAndExtension(sPath);

  // ignore all dots that the file name may start with (".", "..", ".file", "..file", etc)
  // filename may be empty afterwards, which means no dot will be found -> no extension
  while (sPath.StartsWith("."))
    sPath.ChopAwayFirstCharacterAscii();

  const char* szDot;

  if (bFullExtension)
  {
    szDot = sPath.FindSubString(".");
  }
  else
  {
    szDot = sPath.FindLastSubString(".");
  }

  // no dot at all -> no extension
  if (szDot == nullptr)
    return plStringView();

  // dot at the very end of the string -> not an extension
  if (szDot + 1 == sPath.GetEndPointer())
    return plStringView();

  return plStringView(szDot + 1, sPath.GetEndPointer());
}

plStringView plPathUtils::GetFileNameAndExtension(plStringView sPath)
{
  const char* szSeparator = FindPreviousSeparator(sPath.GetStartPointer(), sPath.GetEndPointer());

  if (szSeparator == nullptr)
    return sPath;

  return plStringView(szSeparator + 1, sPath.GetEndPointer());
}

plStringView plPathUtils::GetFileName(plStringView sPath, bool bRemoveFullExtension)
{
  // reduce the problem to just the filename + extension
  sPath = GetFileNameAndExtension(sPath);

  plStringView ext = GetFileExtension(sPath, bRemoveFullExtension);

  if (ext.IsEmpty())
    return sPath;

  return plStringView(sPath.GetStartPointer(), sPath.GetEndPointer() - ext.GetElementCount() - 1);
}

plStringView plPathUtils::GetFileDirectory(plStringView sPath)
{
  auto it = rbegin(sPath);

  // if it already ends in a path separator, do not return a different directory
  if (IsPathSeparator(it.GetCharacter()))
    return sPath;

  // find the last separator in the string
  const char* szSeparator = FindPreviousSeparator(sPath.GetStartPointer(), sPath.GetEndPointer());

  // no path separator -> root dir -> return the empty path
  if (szSeparator == nullptr)
    return plStringView(nullptr);

  return plStringView(sPath.GetStartPointer(), szSeparator + 1);
}

#if PL_ENABLED(PL_PLATFORM_WINDOWS)
const char plPathUtils::OsSpecificPathSeparator = '\\';
#elif PL_ENABLED(PL_PLATFORM_LINUX) || PL_ENABLED(PL_PLATFORM_ANDROID) || PL_ENABLED(PL_PLATFORM_WEB)
const char plPathUtils::OsSpecificPathSeparator = '/';
#elif PL_ENABLED(PL_PLATFORM_OSX)
const char plPathUtils::OsSpecificPathSeparator = '/';
#else
#  error "Unknown platform."
#endif

bool plPathUtils::IsAbsolutePath(plStringView sPath)
{
  if (sPath.GetElementCount() < 1)
    return false;

  const char* szPath = sPath.GetStartPointer();

  // szPath[0] will not be \0 -> so we can access szPath[1] without problems

#if PL_ENABLED(PL_PLATFORM_WINDOWS)
  if (sPath.GetElementCount() < 2)
    return false;

  /// if it is an absolute path, character 0 must be ASCII (A - Z)
  /// checks for local paths, i.e. 'C:\stuff' and UNC paths, i.e. '\\server\stuff'
  /// not sure if we should handle '//' identical to '\\' (currently we do)
  return ((szPath[1] == ':') || (IsPathSeparator(szPath[0]) && IsPathSeparator(szPath[1])));
#elif PL_ENABLED(PL_PLATFORM_LINUX) || PL_ENABLED(PL_PLATFORM_ANDROID) || PL_ENABLED(PL_PLATFORM_WEB)
  return (szPath[0] == '/');
#elif PL_ENABLED(PL_PLATFORM_OSX)
  return (szPath[0] == '/');
#else
#  error "Unknown platform."
#endif
}

bool plPathUtils::IsRelativePath(plStringView sPath)
{
  if (sPath.IsEmpty())
    return true;

  // if it starts with a separator, it is not a relative path, ever
  if (plPathUtils::IsPathSeparator(*sPath.GetStartPointer()))
    return false;

  return !IsAbsolutePath(sPath) && !IsRootedPath(sPath);
}

bool plPathUtils::IsRootedPath(plStringView sPath)
{
  return !sPath.IsEmpty() && *sPath.GetStartPointer() == ':';
}

void plPathUtils::GetRootedPathParts(plStringView sPath, plStringView& ref_sRoot, plStringView& ref_sRelPath)
{
  ref_sRoot = plStringView();
  ref_sRelPath = sPath;

  if (!IsRootedPath(sPath))
    return;

  const char* szStart = sPath.GetStartPointer();
  const char* szPathEnd = sPath.GetEndPointer();

  do
  {
    plUnicodeUtils::MoveToNextUtf8(szStart, szPathEnd).AssertSuccess();

    if (*szStart == '\0')
      return;

  } while (IsPathSeparator(*szStart));

  const char* szEnd = szStart;
  plUnicodeUtils::MoveToNextUtf8(szEnd, szPathEnd).AssertSuccess();

  while (*szEnd != '\0' && !IsPathSeparator(*szEnd))
    plUnicodeUtils::MoveToNextUtf8(szEnd, szPathEnd).AssertSuccess();

  ref_sRoot = plStringView(szStart, szEnd);
  if (*szEnd == '\0')
  {
    ref_sRelPath = plStringView();
  }
  else
  {
    // skip path separator for the relative path
    plUnicodeUtils::MoveToNextUtf8(szEnd, szPathEnd).AssertSuccess();
    ref_sRelPath = plStringView(szEnd, szPathEnd);
  }
}

plStringView plPathUtils::GetRootedPathRootName(plStringView sPath)
{
  plStringView root, relPath;
  GetRootedPathParts(sPath, root, relPath);
  return root;
}

bool plPathUtils::IsValidFilenameChar(plUInt32 uiCharacter)
{
  /// \test Not tested yet

  // Windows: https://msdn.microsoft.com/library/windows/desktop/aa365247(v=vs.85).aspx
  // Unix: https://en.wikipedia.org/wiki/Filename#Reserved_characters_and_words
  // Details can be more complicated (there might be reserved names depending on the filesystem), but in general all platforms behave like
  // this:
  static const plUInt32 forbiddenFilenameChars[] = {'<', '>', ':', '"', '|', '?', '*', '\\', '/', '\t', '\b', '\n', '\r', '\0'};

  for (int i = 0; i < PL_ARRAY_SIZE(forbiddenFilenameChars); ++i)
  {
    if (forbiddenFilenameChars[i] == uiCharacter)
      return false;
  }

  return true;
}

bool plPathUtils::ContainsInvalidFilenameChars(plStringView sPath)
{
  /// \test Not tested yet

  plStringIterator it = sPath.GetIteratorFront();

  for (; it.IsValid(); ++it)
  {
    if (!IsValidFilenameChar(it.GetCharacter()))
      return true;
  }

  return false;
}

void plPathUtils::MakeValidFilename(plStringView sFilename, plUInt32 uiReplacementCharacter, plStringBuilder& out_sFilename)
{
  PL_ASSERT_DEBUG(IsValidFilenameChar(uiReplacementCharacter), "Given replacement character is not allowed for filenames.");

  out_sFilename.Clear();

  for (auto it = sFilename.GetIteratorFront(); it.IsValid(); ++it)
  {
    plUInt32 currentChar = it.GetCharacter();

    if (IsValidFilenameChar(currentChar) == false)
      out_sFilename.Append(uiReplacementCharacter);
    else
      out_sFilename.Append(currentChar);
  }
}

bool plPathUtils::IsSubPath(plStringView sPrefixPath, plStringView sFullPath0)
{
  if (sPrefixPath.IsEmpty())
  {
    if (sFullPath0.IsAbsolutePath())
      return true;

    PL_REPORT_FAILURE("Prefixpath is empty and checked path is not absolute.");
    return false;
  }

  plStringBuilder tmp = sPrefixPath;
  tmp.MakeCleanPath();
  tmp.Trim("", "/");

  plStringBuilder sFullPath = sFullPath0;
  sFullPath.MakeCleanPath();

  if (sFullPath.StartsWith(tmp))
  {
    if (tmp.GetElementCount() == sFullPath.GetElementCount())
      return true;

    return sFullPath.GetData()[tmp.GetElementCount()] == '/';
  }

  return false;
}

bool plPathUtils::IsSubPath_NoCase(plStringView sPrefixPath, plStringView sFullPath)
{
  plStringBuilder tmp = sPrefixPath;
  tmp.MakeCleanPath();
  tmp.Trim("", "/");

  if (sFullPath.StartsWith_NoCase(tmp))
  {
    if (tmp.GetElementCount() == sFullPath.GetElementCount())
      return true;

    return sFullPath.GetStartPointer()[tmp.GetElementCount()] == '/';
  }

  return false;
}
