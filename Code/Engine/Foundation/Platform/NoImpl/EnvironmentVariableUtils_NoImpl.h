#include <Foundation/FoundationInternal.h>
PL_FOUNDATION_INTERNAL_HEADER

#include <Foundation/System/EnvironmentVariableUtils.h>

plString plEnvironmentVariableUtils::GetValueStringImpl(plStringView sName, plStringView sDefault)
{
  PL_IGNORE_UNUSED(sName);
  PL_IGNORE_UNUSED(sDefault);
  PL_ASSERT_NOT_IMPLEMENTED
  return "";
}

plResult plEnvironmentVariableUtils::SetValueStringImpl(plStringView sName, plStringView sValue)
{
  PL_IGNORE_UNUSED(sName);
  PL_IGNORE_UNUSED(sValue);
  PL_ASSERT_NOT_IMPLEMENTED
  return PL_FAILURE;
}

bool plEnvironmentVariableUtils::IsVariableSetImpl(plStringView sName)
{
  PL_IGNORE_UNUSED(sName);
  return false;
}

plResult plEnvironmentVariableUtils::UnsetVariableImpl(plStringView sName)
{
  PL_IGNORE_UNUSED(sName);
  return PL_FAILURE;
}
