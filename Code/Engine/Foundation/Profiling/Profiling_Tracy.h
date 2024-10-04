#include <Foundation/Basics.h>
#include <Foundation/Strings/HashedString.h>

#if PL_ENABLED(PL_USE_PROFILING) && TRACY_ENABLE && !PL_DOCS

#  include <tracy/tracy/Tracy.hpp>

PL_ALWAYS_INLINE plUInt32 __tracyPlStringLength(const char* szString)
{
  return plStringUtils::GetStringElementCount(szString);
}

PL_ALWAYS_INLINE plUInt32 __tracyPlStringLength(plStringView sString)
{
  return sString.GetElementCount();
}

PL_ALWAYS_INLINE plUInt32 __tracyPlStringLength(const plString& sString)
{
  return sString.GetElementCount();
}

PL_ALWAYS_INLINE plUInt32 __tracyPlStringLength(const plStringBuilder& sString)
{
  return sString.GetElementCount();
}

PL_ALWAYS_INLINE plUInt32 __tracyPlStringLength(const plHashedString& sString)
{
  return sString.GetView().GetElementCount();
}

PL_ALWAYS_INLINE const char* __tracyPlStringToConstChar(const plString& sString)
{
  return sString.GetData();
}

PL_ALWAYS_INLINE const char* __tracyPlStringToConstChar(const plStringBuilder& sString)
{
  return sString.GetData();
}

PL_ALWAYS_INLINE const char* __tracyPlStringToConstChar(const plHashedString& sString)
{
  return sString.GetData();
}

PL_ALWAYS_INLINE const char* __tracyPlStringToConstChar(const plStringView& sString)
{
  // can just return the string views start pointer, because this is used together with __tracyPlStringLength
  return sString.GetStartPointer();
}

PL_ALWAYS_INLINE const char* __tracyPlStringToConstChar(const char* szString)
{
  return szString;
}

/// \brief Similar to PL_PROFILE_SCOPE, but only forwards to Tracy
#  define PL_TRACY_PROFILE_SCOPE(ScopeName) \
    ZoneScoped;                             \
    ZoneName(__tracyPlStringToConstChar(ScopeName), __tracyPlStringLength(ScopeName))

// Override the standard PL profiling macros and inject Tracy profiling scopes

#  undef PL_PROFILE_SCOPE
#  define PL_PROFILE_SCOPE(ScopeName)                                                                                    \
    plProfilingScope PL_PP_CONCAT(_plProfilingScope, PL_SOURCE_LINE)(ScopeName, PL_SOURCE_FUNCTION, plTime::MakeZero()); \
    PL_TRACY_PROFILE_SCOPE(ScopeName)

#  undef PL_PROFILE_SCOPE_WITH_TIMEOUT
#  define PL_PROFILE_SCOPE_WITH_TIMEOUT(ScopeName, Timeout)                                                   \
    plProfilingScope PL_PP_CONCAT(_plProfilingScope, PL_SOURCE_LINE)(ScopeName, PL_SOURCE_FUNCTION, Timeout); \
    PL_TRACY_PROFILE_SCOPE(ScopeName);

#  undef PL_PROFILE_LIST_SCOPE
#  define PL_PROFILE_LIST_SCOPE(ListName, FirstSectionName)                                                               \
    plProfilingListScope PL_PP_CONCAT(_plProfilingScope, PL_SOURCE_LINE)(ListName, FirstSectionName, PL_SOURCE_FUNCTION); \
    PL_TRACY_PROFILE_SCOPE(ScopeName);

#  undef PL_PROFILER_FRAME_MARKER
#  define PL_PROFILER_FRAME_MARKER() FrameMark

#else

/// \brief Similar to PL_PROFILE_SCOPE, but only forwards to Tracy
#  define PL_TRACY_PROFILE_SCOPE(ScopeName)

#endif
