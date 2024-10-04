#pragma once

#include <Shaders/Common/Common.h>
#include <Shaders/Common/GlobalConstants.h>
#include <Shaders/Common/ObjectConstants.h>
#include <Shaders/Common/BRDF.h>
#include <Shaders/Materials/MaterialData.h>
#include <Shaders/Materials/MaterialInterpolator.h>

float3 GetNormal();

#if defined(USE_SIMPLE_MATERIAL_MODEL)
  float3 GetBaseColor();
  float GetReflectance();
#else
  float3 GetDiffuseColor();
  float3 GetSpecularColor();
#endif

#if defined(USE_MATERIAL_EMISSIVE)
  float3 GetEmissiveColor();
#endif

#if defined(USE_MATERIAL_REFRACTION)
  float4 GetRefractionColor();
#endif

// Note that this function actually returns perceptualRoughness.
float GetRoughness();
float GetMetallic();
float GetOpacity();

#if defined(USE_MATERIAL_OCCLUSION)
  float GetOcclusion();
#endif

#if defined(USE_MATERIAL_SUBSURFACE_COLOR)
  float3 GetSubsurfaceColor();
#endif

#if defined(USE_MATERIAL_SUBSURFACE_PARAMS)
  void GetSubsurfaceParams(out float scatterPower, out float shadowFalloff);
#endif

#if defined(USE_MATERIAL_SPECULAR_CLEARCOAT)
  void GetSpecularClearCoatParams(out float clearcoat, out float clearcoatRoughness, out float3 normal);
#endif

#if defined(USE_MATERIAL_SPECULAR_ANISOTROPIC)
  void GetSpecularAnisotopicParams(out float anisotropic, out float rotation);
#endif

#if defined(USE_MATERIAL_SPECULAR_SHEEN)
  void GetSpecularSheenParams(out float sheen, out float tintFactor);
#endif

#if defined(USE_TRANSPARENT_BLUR)
  void GetBlurParams(out float samples, out float multiplier);
#endif

struct PS_GLOBALS
{
  PS_IN Input;
  #if defined(CUSTOM_GLOBALS)
    CUSTOM_GLOBALS
  #endif
};
static PS_GLOBALS G;

#if defined(CUSTOM_GLOBALS)
  void FillCustomGlobals();
#endif

uint CalculateCoverage()
{
#if defined(USE_ALPHA_TEST_SUPER_SAMPLING) && defined(USE_TEXCOORD0)
  uint coverage = 0;

  float2 texCoords = G.Input.TexCoord0;

  for (uint i = 0; i < NumMsaaSamples; ++i)
  {
    G.Input.TexCoord0 = plEvaluateAttributeAtSample(texCoords, i, NumMsaaSamples);

    float opacity = GetOpacity();
    coverage |= (opacity > 0.0) ? (1U << i) : 0;
  }

  G.Input.TexCoord0 = texCoords;

  return coverage;
#else
  return GetOpacity() > 0.0;
#endif
}

plMaterialData FillMaterialData()
{
  plMaterialData matData;

  #if defined(USE_WORLDPOS)
    matData.worldPosition = G.Input.WorldPosition;
  #else
    matData.worldPosition = float3(0.0, 0.0, 0.0);
  #endif

#if SHADING_MODE == SHADING_MODE_FULLBRIGHT
    matData.worldNormal = float3(0, 0, 1);
#else
    float3 worldNormal = normalize(GetNormal());
#  if TWO_SIDED == TRUE && defined(USE_TWO_SIDED_LIGHTING)
#    if FLIP_WINDING == TRUE
    matData.worldNormal = G.Input.FrontFace ? -worldNormal : worldNormal;
#    else
    matData.worldNormal = G.Input.FrontFace ? worldNormal : -worldNormal;
#    endif
#  else
    matData.worldNormal = worldNormal;
#  endif
#endif

#if defined(USE_NORMAL)
    matData.vertexNormal = normalize(G.Input.Normal);
#else
    matData.vertexNormal = float3(0, 0, 1);
#endif

#if defined(USE_SIMPLE_MATERIAL_MODEL)
    float3 baseColor = GetBaseColor();

#  if SHADING_MODE == SHADING_MODE_FULLBRIGHT
    matData.diffuseColor = baseColor;
    matData.specularColor = float3(0, 0, 0);
#  else
    float metallic = GetMetallic();
    float reflectance = GetReflectance();
    float f0 = 0.16f * reflectance * reflectance;

    matData.diffuseColor = lerp(baseColor, 0.0f, metallic);
    matData.specularColor = lerp(float3(f0, f0, f0), baseColor, metallic);
#  endif

#else
    matData.diffuseColor = GetDiffuseColor();

#  if SHADING_MODE == SHADING_MODE_FULLBRIGHT
    matData.specularColor = float3(0, 0, 0);
#  else
    matData.specularColor = GetSpecularColor();
#  endif
#endif

#if defined(USE_MATERIAL_EMISSIVE)
    matData.emissiveColor = GetEmissiveColor();
  #else
    matData.emissiveColor = 0.0f;
  #endif
  
  #if defined(USE_MATERIAL_REFRACTION)
    matData.refractionColor = GetRefractionColor();
  #else
    matData.refractionColor = float4(0, 0, 0, 1);
  #endif

#if SHADING_MODE == SHADING_MODE_FULLBRIGHT
    matData.perceptualRoughness = MIN_PERCEPTUAL_ROUGHNESS;
#else
    matData.perceptualRoughness = max(GetRoughness(), MIN_PERCEPTUAL_ROUGHNESS);
#endif

    matData.roughness = RoughnessFromPerceptualRoughness(matData.perceptualRoughness);
    matData.metalness = GetMetallic();

#if defined(USE_MATERIAL_OCCLUSION)
#  if defined(USE_NORMAL)
    float3 viewVector = normalize(GetCameraPosition() - matData.worldPosition);
    float occlusionFade = saturate(dot(matData.vertexNormal, viewVector));
#  else
    float occlusionFade = 1.0f;
#  endif
    matData.occlusion = lerp(1.0f, GetOcclusion(), occlusionFade);
  #else
    matData.occlusion = 1.0f;
  #endif

  #if BLEND_MODE != BLEND_MODE_OPAQUE && BLEND_MODE != BLEND_MODE_MASKED
    matData.opacity = GetOpacity();
  #else
    matData.opacity = 1.0f;
  #endif

#if defined(USE_VELOCITY)
    // TODO: Set these values in the global constants buffer when implementing TAA
    float2 CurrentTAAJitter = 0.0f;
    float2 PreviousTAAJitter = 0.0f;

    float2 uv_current = Ndc2Uv((G.Input.ScreenPosition.xy / G.Input.ScreenPosition.w) - CurrentTAAJitter);
    float2 uv_previous = Ndc2Uv((G.Input.LastScreenPosition.xy / G.Input.LastScreenPosition.w) - PreviousTAAJitter);

    matData.velocity = uv_current - uv_previous;
#else
    matData.velocity = float2(0, 0);
#endif

  #if defined(USE_MATERIAL_SUBSURFACE_COLOR)
    matData.subsurfaceColor = GetSubsurfaceColor() * matData.diffuseColor;
  #else
    matData.subsurfaceColor = 0.0;
  #endif

  #if defined(USE_MATERIAL_SUBSURFACE_PARAMS)
    GetSubsurfaceParams(matData.subsurfaceScatterPower, matData.subsurfaceShadowFalloff);
  #else
    matData.subsurfaceScatterPower = 9.0;
    matData.subsurfaceShadowFalloff = 0.0;
  #endif

  #if defined(USE_MATERIAL_SPECULAR_CLEARCOAT)
    GetSpecularClearCoatParams(matData.clearcoat, matData.clearcoatRoughness, matData.clearcoatNormal);
  #endif

  #if defined(USE_MATERIAL_SPECULAR_ANISOTROPIC)
    GetSpecularAnisotopicParams(matData.anisotropic, matData.anisotropicRotation);
  #endif

  #if defined(USE_MATERIAL_SPECULAR_SHEEN)
    GetSpecularSheenParams(matData.sheen, matData.sheenTintFactor);
  #endif

#if defined(USE_TRANSPARENT_BLUR)
    GetBlurParams(matData.blurSamples, matData.blurMultiplier);
#endif

  return matData;
}

#if defined(USE_NORMAL)
float3 TangentToWorldSpace(float3 normalTS)
{
#if defined(USE_TANGENT)
  return normalTS.x * G.Input.Tangent + normalTS.y * G.Input.BiTangent + normalTS.z * G.Input.Normal;
#elif defined(USE_NORMAL)
  return normalTS.z * G.Input.Normal;
#else
  return normalTS;
#endif
}
#endif

float4 ColorizeGameObjectId(uint gameObjectId)
{
  float intensity = saturate(0.5f + (gameObjectId & 0xF) / 31.0f);
  bool isDynamic = gameObjectId & (1 << 31);
  return float4(isDynamic ? intensity : 0.0f, isDynamic ? 0.0f : intensity, 0.0f, 1.0f);
}


float4 SampleColorPalette(Texture2D paletteTex, uint row, float column)
{
  uint width, height;
  paletteTex.GetDimensions(width, height);

  float2 uv = float2(column, (row + 0.5f) / height);
  return paletteTex.SampleLevel(LinearClampSampler, uv, 0);
}