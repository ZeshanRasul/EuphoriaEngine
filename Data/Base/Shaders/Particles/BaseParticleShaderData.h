#pragma once

#include "ParticleSystemConstants.h"

struct PL_SHADER_STRUCT plBaseParticleShaderData
{
  PACKEDCOLOR4H(Color);
  PACKEDHALF2(Life, Size, LifeAndSize); // Life: 1 to 0
  UINT1(Variation); // only lower 8 bit
};

// this is only defined during shader compilation
#if PL_ENABLED(PLATFORM_SHADER)

StructuredBuffer<plBaseParticleShaderData> particleBaseData;

#else // C++

static_assert(sizeof(plBaseParticleShaderData) == 16);

#endif

