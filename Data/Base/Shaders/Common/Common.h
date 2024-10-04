#pragma once

#include "Platforms.h"

// Common sampler states
SamplerState LinearSampler;
SamplerState LinearClampSampler;
SamplerState PointSampler;
SamplerState PointClampSampler;

static const float PI                  = 3.14159265f;
static const float PI2                 = 6.28318530f;
static const float PI4                 = 12.5663706f;
static const float INV_PI              = 0.31830988f;
static const float PI_HALF             = PI * 0.5f;
static const float FLT_MIN             = 0.00000001f;
static const float FLT_MAX             = 3.402823466e+38;
static const float FLT_MAX_10          = 511.0f;
static const float FLT_MAX_11          = 1023.0f;
static const float FLT_MAX_14          = 8191.0f;
static const float FLT_MAX_16          = 32767.0f;
static const float FLT_MAX_16U         = 65535.0f;
static const float RPC_16              = 0.0625f;

#define TexelSize ViewportSize.zw
#define TargetTexelSize TargetViewportSize.zw

#define sqr(a) ((a) * (a))

/*------------------------------------------------------------------------------
  MATH
------------------------------------------------------------------------------*/

float min2(float2 value)
{
  return min(value.x, value.y);
}

float min3(float3 value)
{
  return min(min(value.x, value.y), value.z);
}

float min3(float a, float b, float c)
{
  return min(min(a, b), c);
}

float min4(float a, float b, float c, float d)
{
  return min(min(min(a, b), c), d);
}

float min5(float a, float b, float c, float d, float e)
{
  return min(min(min(min(a, b), c), d), e);
}

float max2(float2 value)
{
  return max(value.x, value.y);
}

float max3(float3 value)
{
  return max(max(value.x, value.y), value.z);
}

float max4(float a, float b, float c, float d)
{
  return max(max(max(a, b), c), d);
}

float max5(float a, float b, float c, float d, float e)
{
  return max(max(max(max(a, b), c), d), e);
}

float pow2(float x)
{
  return x * x;
}

float pow3(float x)
{
  float xx = x * x;
  return xx * x;
}

float pow4(float x)
{
  float xx = x * x;
  return xx * xx;
}

bool is_saturated(float value)
{
  return value == saturate(value);
}

bool is_saturated(float2 value)
{
  return is_saturated(value.x) && is_saturated(value.y);
}

bool is_saturated(float3 value)
{
  return is_saturated(value.x) && is_saturated(value.y) && is_saturated(value.z);
}

bool is_saturated(float4 value)
{
  return is_saturated(value.x) && is_saturated(value.y) && is_saturated(value.z) && is_saturated(value.w);
}

bool is_valid_uv(float2 value)
{
  return (value.x >= 0.0f && value.x <= 1.0f) || (value.y >= 0.0f && value.y <= 1.0f);
}

float4 RGBA8ToFloat4(uint x)
{
  float4 result;
  result.r = x & 0xFF;
  result.g = (x >> 8)  & 0xFF;
  result.b = (x >> 16) & 0xFF;
  result.a = (x >> 24) & 0xFF;

  return result / 255.0;
}

// for when the input data is already float4
float4 RGBA8ToFloat4(float4 x)
{
  return x;
}

float3 RGB8ToFloat3(uint x)
{
  float3 result;
  result.r = x & 0xFF;
  result.g = (x >> 8)  & 0xFF;
  result.b = (x >> 16) & 0xFF;

  return result / 255.0;
}

float3 RGB10ToFloat3(uint x)
{
  float3 result;
  result.r = x & 0x3FF;
  result.g = (x >> 10) & 0x3FF;
  result.b = (x >> 20) & 0x3FF;

  return result / 1023.0;
}

float2 RG16FToFloat2(uint x)
{
  float2 result;
  result.r = f16tof32(x);
  result.g = f16tof32(x >> 16);

  return result;
}

float4 RGBA16FToFloat4(uint rg, uint ba)
{
  return float4(RG16FToFloat2(rg), RG16FToFloat2(ba));
}

float GetLuminance(float3 color)
{
  return dot(color, float3(0.2126, 0.7152, 0.0722));
}

float3 SrgbToLinear(float3 color)
{
  return select(color < 0.04045, (color / 12.92), pow(color / 1.055 + 0.0521327, 2.4));
}

float3 LinearToSrgb(float3 color)
{
  return select(color < 0.0031308, (color * 12.92), (1.055 * pow(color, 1.0 / 2.4) - 0.055));
}

float3 CubeMapDirection(float3 inDirection)
{
  return float3(inDirection.x, inDirection.z, -inDirection.y);
}

float3 DecodeNormalTexture(float4 normalTex)
{
  float2 xy = normalTex.xy * 2.0f - 1.0f;
  float z = sqrt(max(1.0f - dot(xy, xy), 0.0));
  return float3(xy, z);
}

float InterleavedGradientNoise(float2 screenSpacePosition)
{
  float3 magic = float3(0.06711056, 0.00583715, 52.9829189);
  return frac(magic.z * frac(dot(screenSpacePosition, magic.xy)));
}

float InterleavedGradientNoise(float2 uv, uint frameCount)
{
  const float2 magicFrameScale = float2(47, 17) * 0.695;
  uv += frameCount * magicFrameScale;

  const float3 magic = float3(0.06711056, 0.00583715, 52.9829189);
  return frac(magic.z * frac(dot(uv, magic.xy)));
}

float3 NormalizeAndGetLength(float3 v, out float len)
{
  float squaredLen = dot(v, v);
  float reciprocalLen = rsqrt(squaredLen);
  len = squaredLen * reciprocalLen;
  return v * reciprocalLen;
}

float Square(float x)
{
  return x * x;
}
float2 Square(float2 x)
{
  return x * x;
}
float3 Square(float3 x)
{
  return x * x;
}
float4 Square(float4 x)
{
  return x * x;
}

float AdjustContrast(float value, float contrast)
{
  float a = -contrast;
  float b = contrast + 1;
  return saturate(lerp(a, b, value));
}

float3 Colorize(float3 baseColor, float3 color, float mask)
{
  return baseColor * lerp(1, 2 * color, mask);
}

// https://iquilplles.org/articles/smin/
float SmoothMin(float a, float b, float k = 0.1)
{
  float h = max(k - abs(a - b), 0.0) / k;
  return min(a, b) - h * h * k * (1.0 / 4.0);
}

float SmoothMinCubic(float a, float b, float k = 0.1)
{
  float h = max(k - abs(a - b), 0.0) / k;
  return min(a, b) - h * h * h * k * (1.0 / 6.0);
}

// Find good arbitrary axis vectors to represent U and V axes of a plane,
// given just the normal. 
void FindBestAxisVectors(float3 input, out float3 axis1, out float3 axis2)
{
  const float3 N = abs(input);

  // Find best basis vectors.
  if (N.z > N.x && N.z > N.y)
  {
    axis1 = float3(1, 0, 0);
  }
  else
  {
    axis1 = float3(0, 0, 1);
  }

  axis1 = normalize(axis1 - input * dot(axis1, input));
  axis2 = cross(axis1, input);
}

float saturate_11(float x)
{
  return clamp(x, FLT_MIN, FLT_MAX_11);
}

float2 saturate_11(float2 x)
{
  return clamp(x, FLT_MIN, FLT_MAX_11);
}

float3 saturate_11(float3 x)
{
  return clamp(x, FLT_MIN, FLT_MAX_11);
}

float4 saturate_11(float4 x)
{
  return clamp(x, FLT_MIN, FLT_MAX_11);
}

float saturate_16(float x)
{
  return clamp(x, FLT_MIN, FLT_MAX_16);
}

float2 saturate_16(float2 x)
{
  return clamp(x, FLT_MIN, FLT_MAX_16);
}

float3 saturate_16(float3 x)
{
  return clamp(x, FLT_MIN, FLT_MAX_16);
}

float4 saturate_16(float4 x)
{
  return clamp(x, FLT_MIN, FLT_MAX_16);
}

float2 Ndc2Uv(float2 x)
{
  return x * float2(0.5f, -0.5f) + 0.5f;
}

inline uint flatten2D(uint2 coord, uint2 dim)
{
  return coord.x + coord.y * dim.x;
}
// flattened array index to 2D array index
inline uint2 unflatten2D(uint idx, uint2 dim)
{
  return uint2(idx % dim.x, idx / dim.x);
}

float ScreenFade(float2 uv)
{
    float2 fade = max(0.0f, 12.0f * abs(uv - 0.5f) - 5.0f);
    return saturate(1.0f - dot(fade, fade));
}

bool IsValidUv(float2 value) 
{ 
  return (value.x >= 0.0f && value.x <= 1.0f) && (value.y >= 0.0f && value.y <= 1.0f); 
}

/*------------------------------------------------------------------------------
    PACKING/UNPACKING
------------------------------------------------------------------------------*/
float3 unpack(float3 value)
{
  return value * 2.0f - 1.0f;
}

float3 pack(float3 value)
{
  return value * 0.5f + 0.5f;
}

float2 unpack(float2 value)
{
  return value * 2.0f - 1.0f;
}

float2 pack(float2 value)
{
  return value * 0.5f + 0.5f;
}

float unpack(float value)
{
  return value * 2.0f - 1.0f;
}

float pack(float value)
{
  return value * 0.5f + 0.5f;
}

float pack_floats(float x, float y)
{
  uint xScaled = x * 0xFFFF;
  uint yScaled = y * 0xFFFF;
  uint xyPacked = (xScaled << 16) | (yScaled & 0xFFFF);
  return asfloat(xyPacked);
}

void unpack_floats(out float x, out float y, float packedFloat)
{
  uint packedUint = asuint(packedFloat);
  x = (packedUint >> 16) / 65535.0f;
  y = (packedUint & 0xFFFF) / 65535.0f;
}

float pack_uint32_to_float16(uint i)
{
  return (float)i / FLT_MAX_16;
}

uint unpack_float16_to_uint32(float f)
{
  return round(f * FLT_MAX_16);
}

float pack_float_int(float f, uint i, uint numBitI, uint numBitTarget)
{
  // Constant optimize by compiler
  float precision = float(1U << numBitTarget);
  float maxi = float(1U << numBitI);
  float precisionMinusOne = precision - 1.0;
  float t1 = ((precision / maxi) - 1.0) / precisionMinusOne;
  float t2 = (precision / maxi) / precisionMinusOne;

  // Code
  return t1 * f + t2 * float(i);
}

void unpack_float_int(float val, uint numBitI, uint numBitTarget, out float f, out uint i)
{
  // Constant optimize by compiler
  float precision = float(1U << numBitTarget);
  float maxi = float(1U << numBitI);
  float precisionMinusOne = precision - 1.0;
  float t1 = ((precision / maxi) - 1.0) / precisionMinusOne;
  float t2 = (precision / maxi) / precisionMinusOne;

  // Code
  // extract integer part
  // + rcp(precisionMinusOne) to deal with precision issue
  i = int((val / t2) + rcp(precisionMinusOne));
  // Now that we have i, solve formula in PackFloatInt for f
  // f = (val - t2 * float(i)) / t1 => convert in mads form
  f = saturate((-t2 * float(i) + val) / t1); // Saturate in case of precision issue
}

// Hash a 2D vector for randomness
float2 Hash2D(float2 s)
{
  return frac(sin(fmod(float2(dot(s, float2(127.1, 311.7)), dot(s, float2(269.5, 183.3))), 3.14159)) * 43758.5453);
}

// Stochastic sampling
// Based on the following:
// https://drive.google.com/file/d/1QecekuuyWgw68HU9tg6ENfrCTCVIjm6l/view
float4 Sample(in Texture2D tex, in SamplerState sam, in float2 uv, in bool useStochastic = false)
{
  float4 c;

  if (useStochastic)
  {
    // Triangle vertices and blend weights
    // BW_vx[0...2].xyz = triangle verts
    // BW_vx[3].xy = blend weights (z is unused)
    float4x3 BW_vx;

    // UV transformed into triangular grid space with UV scaled by approximation of 2*sqrt(3)
    float2 skewUV = mul(float2x2 (1.0 , 0.0 , -0.57735027 , 1.15470054), uv * 3.464);

    // Vertex IDs and barycentric coords
    float2 vxID = float2(floor(skewUV));
    float3 barry = float3(frac(skewUV), 0);
    barry.z = 1.0f - barry.x - barry.y;

    BW_vx = ((barry.z>0) ?
                           float4x3(float3(vxID, 0), float3(vxID + float2(0, 1), 0), float3(vxID + float2(1, 0), 0), barry.zyx) :
                           float4x3(float3(vxID + float2 (1, 1), 0), float3(vxID + float2 (1, 0), 0), float3(vxID + float2 (0, 1), 0), float3(-barry.z, 1.0 - barry.y, 1.0 - barry.x)));

    // Calculate derivatives to avoid triangular grid artifacts
    float2 dx = ddx_coarse(uv);
    float2 dy = ddy_coarse(uv);

    // Blend samples with calculated weights
    c = mul(tex.SampleGrad(sam, uv + Hash2D(BW_vx[0].xy), dx, dy), BW_vx[3].x) +
        mul(tex.SampleGrad(sam, uv + Hash2D(BW_vx[1].xy), dx, dy), BW_vx[3].y) +
        mul(tex.SampleGrad(sam, uv + Hash2D(BW_vx[2].xy), dx, dy), BW_vx[3].z);
  }
  else
  {
    c = tex.Sample(sam, uv);
  }

  return c;
}

float3 BlendNormals(float3 baseNormal, float3 detailNormal)
{
  float3 t = baseNormal + float3(0, 0, 1);
  float3 u = detailNormal * float3(-1, -1, 1);
  return t * dot(t, u) - u * t.z;
}

float4 SampleTexture3WayStochastic(Texture2D tex, SamplerState samplerState, float3 worldNormal, float3 worldPosition, float tiling, bool useStochastic = false)
{
  float3 blendWeights = abs(worldNormal);
  blendWeights = max((blendWeights - 0.2) * 7.0, 0.0);
  blendWeights /= (blendWeights.x + blendWeights.y + blendWeights.z );

  float3 ns = sign(worldNormal) * tiling;

  float4 color1 = Sample(tex, samplerState, worldPosition.yz * float2(-ns.x, -tiling), useStochastic);
  float4 color2 = Sample(tex, samplerState, worldPosition.xz * float2(ns.y, -tiling), useStochastic);
  float4 color3 = Sample(tex, samplerState, worldPosition.xy * float2(ns.z, tiling), useStochastic);

  return color1 * blendWeights.x + color2 * blendWeights.y + color3 * blendWeights.z;
}

float4 SampleTexture3Way(Texture2D tex, SamplerState samplerState, float3 worldNormal, float3 worldPosition, float tiling)
{
  float3 blendWeights = abs(worldNormal);
  blendWeights = max((blendWeights - 0.2) * 7.0, 0.0);
  blendWeights /= (blendWeights.x + blendWeights.y + blendWeights.z );

  float3 ns = sign(worldNormal) * tiling;

  float4 color1 = tex.Sample(samplerState, worldPosition.yz * float2(-ns.x, -tiling));
  float4 color2 = tex.Sample(samplerState, worldPosition.xz * float2(ns.y, -tiling));
  float4 color3 = tex.Sample(samplerState, worldPosition.xy * float2(ns.z, tiling));

  return color1 * blendWeights.x + color2 * blendWeights.y + color3 * blendWeights.z;
}

float4x4 ApproximateInverseMatrix(float4x4 m) {
  float4x4 I = float4x4(1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        0, 0, 0, 1);

  float4x4 x = I;

  for (int i = 0; i < 3; ++i) {
    x = x * (2 * I - m * x);
  }

  return x;
}

float4x4 InverseMatrix(float4x4 m) {
  float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
  float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
  float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
  float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

  float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
  float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
  float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
  float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

  float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
  float idet = 1.0f / det;

  float4x4 ret;

  ret[0][0] = t11 * idet;
  ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
  ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
  ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

  ret[1][0] = t12 * idet;
  ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
  ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
  ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

  ret[2][0] = t13 * idet;
  ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
  ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
  ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

  ret[3][0] = t14 * idet;
  ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
  ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
  ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

  return ret;
}