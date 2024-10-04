#pragma once

Texture2D LTC1Texture;
Texture2D LTC2Texture;
SamplerState LTCSampler;

#define AREA_LIGHT_TYPE_RECTANGLE 1
#define AREA_LIGHT_TYPE_DISK 2

static const float LTC_LUT_SIZE = 64.0f;
static const float LTC_LUT_SCALE = (LTC_LUT_SIZE - 1.0f) / LTC_LUT_SIZE;
static const float LTC_LUT_BIAS = 0.5f / LTC_LUT_SIZE;

float3 IntegrateEdgeVec(float3 v1, float3 v2)
{
  float x = dot(v1, v2);
  float y = abs(x);

  float a = 0.8543985 + (0.4965155 + 0.0145206 * y) * y;
  float b = 3.4175940 + (4.1616724 + y) * y;
  float v = a / b;

  float theta_sintheta = (x > 0.0) ? v : 0.5 * rsqrt(max(1.0 - x * x, 1e-7)) - v;

  return cross(v1, v2) * theta_sintheta;
}


float3 LTC_Evaluate(float3 normalVector, float3 viewVector, float3 position, float3x3 minV, float3 points[4], bool twoSided)
{
  float3 T1, T2;
  float NdotV = saturate(dot(normalVector, viewVector)) + 1e-6;
  T1 = normalize(viewVector - normalVector *  NdotV);
  T2 = cross(normalVector, T1);

  minV = minV * (float3x3(T1, T2, normalVector));

  float3 L[4];
  L[0] = mul(minV, points[0] - position);
  L[1] = mul(minV, points[1] - position);
  L[2] = mul(minV, points[2] - position);
  L[3] = mul(minV, points[3] - position);

  float3 dir = points[0] - position; // LTC space
  float3 lightNormal = cross(points[1] - points[0], points[3] - points[0]);
  bool behind = (dot(dir, lightNormal) < 0.0);

  L[0] = normalize(L[0]);
  L[1] = normalize(L[1]);
  L[2] = normalize(L[2]);
  L[3] = normalize(L[3]);

  float3 vsum = float3(0.0f, 0.0f, 0.0f);
  vsum += IntegrateEdgeVec(L[0], L[1]);
  vsum += IntegrateEdgeVec(L[1], L[2]);
  vsum += IntegrateEdgeVec(L[2], L[3]);
  vsum += IntegrateEdgeVec(L[3], L[0]);

  float len = length(vsum);

  float z = vsum.z/len;
  if (behind)
    z = -z;

  float2 uv = float2(z*0.5f + 0.5f, len); // range [0, 1]
  uv = uv*LTC_LUT_SCALE + LTC_LUT_BIAS;

  float scale = LTC2Texture.SampleLevel(LTCSampler, uv, 0);

  float sum = len*scale;
  if (!behind && !twoSided)
    sum = 0.0;

  float3 Lo_i = float3(sum, sum, sum);
  return Lo_i;
}
AccumulatedLight AreaLightLTC_GGX(float3 view, plMaterialData matData, plPerLightData perLightData)
{
  float3 specular = 0.0f;
  float3 diffuse  = 0.0f;

  float3 normalVector = normalize(matData.worldNormal);
  float3 viewVector = normalize(GetCameraPosition() - matData.worldPosition);
  float3 position = matData.worldPosition;
  float dotNV = saturate(dot(normalVector, viewVector))  + 1e-6;

  // use roughness and sqrt(1-cos_theta) to sample M_texture
  float2 uv = float2(matData.roughness, sqrt(1.0f - dotNV));
  uv = uv * LTC_LUT_SCALE + LTC_LUT_BIAS;

  // get 4 parameters for inverse_M
  float4 t1 = LTC1Texture.SampleLevel(LTCSampler, uv, 0);

  // Get 2 parameters for Fresnel calculation
  float4 t = LTC2Texture.SampleLevel(LTCSampler, uv, 0);

  float3x3 minv = float3x3(t1.x, 0, t1.z,
                            0,   1,    0,
                            t1.y,0, t1.w);

  float3x3 matIdentity = float3x3(1, 0, 0,
                                  0, 1, 0,
                                  0, 0, 1);

  float4x4 lightParamMatrix = perLightData.lightParams;
  int type = lightParamMatrix[0][3];

  // Define Light shape
  float3 points[4];
  points[0] = float3(lightParamMatrix[0][0], lightParamMatrix[0][1], lightParamMatrix[0][2]);
  points[1] = float3(lightParamMatrix[1][0], lightParamMatrix[1][1], lightParamMatrix[1][2]);
  points[2] = float3(lightParamMatrix[2][0], lightParamMatrix[2][1], lightParamMatrix[2][2]);
  points[3] = float3(lightParamMatrix[3][0], lightParamMatrix[3][1], lightParamMatrix[3][2]);

 // uint shape = lightParamMatrix[0][3];
  bool twoSided = true;//lightParamMatrix[1][3];
  bool textured = false;//lightParamMatrix[2][3];

  if (type == AREA_LIGHT_TYPE_RECTANGLE)
  {
    diffuse = LTC_Evaluate(normalVector, viewVector, position, matIdentity, points, twoSided);
    specular = LTC_Evaluate(normalVector, viewVector, position, minv, points, twoSided);
  }
  else if (type == AREA_LIGHT_TYPE_DISK)
  {
  }

  return InitializeLight(diffuse * matData.diffuseColor, specular);
}
