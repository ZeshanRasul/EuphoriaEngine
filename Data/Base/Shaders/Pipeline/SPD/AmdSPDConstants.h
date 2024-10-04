#pragma once

#include <Shaders/Common/ConstantBufferMacros.h>
#include <Shaders/Common/Platforms.h>

CONSTANT_BUFFER(plAmdSPDConstants, 3)
{
  UINT1(MipCount);
  UINT1(WorkGroupCount);
};