#pragma once

#include <Shaders/Common/Platforms.h>
#include <Shaders/Common/ConstantBufferMacros.h>

CONSTANT_BUFFER(plDepthOfFieldConstants, 3)
{
  FLOAT1(Radius);
};