
#include <Shaders/Materials/MaterialTessellationData.h>

Texture2D SnowDiffuseDisplacement;
SamplerState SnowDisplacementSampler;


[domain("tri")]
VS_OUT main(TessellationFactors factors, OutputPatch<VS_OUT, 3> patch, float3 barycentricCoordinates : SV_DomainLocation)
{
  VS_OUT output = patch[0];

#define INTERPOLATE(fieldname) output.fieldname = \
    patch[0].fieldname * barycentricCoordinates.x + \
    patch[1].fieldname * barycentricCoordinates.y + \
    patch[2].fieldname * barycentricCoordinates.z;

  INTERPOLATE(Position);

#if defined(USE_VELOCITY)
  INTERPOLATE(ScreenPosition);
  INTERPOLATE(LastScreenPosition);
#endif

#if defined(USE_WORLDPOS)
  INTERPOLATE(WorldPosition);
#endif

#if defined(USE_NORMAL)
  INTERPOLATE(Normal);
#endif

#if defined(USE_TANGENT)
  INTERPOLATE(Tangent);
  INTERPOLATE(BiTangent);
#endif

#if defined(USE_TEXCOORD0)
  INTERPOLATE(TexCoord0);
# if defined(USE_TEXCOORD1)
    INTERPOLATE(TexCoord1);
# endif
#endif

#if defined(USE_COLOR0)
    INTERPOLATE(Color0);
# if defined(USE_COLOR1)
    INTERPOLATE(Color1);
# endif
#endif

#if defined(SNOW_DISPLACEMENT)
  #if defined(USE_WORLDPOS) && defined(USE_NORMAL)
    float displacement = SampleTexture3WayStochastic(SnowDiffuseDisplacement, SnowDisplacementSampler, output.Normal, output.WorldPosition, SnowTiling, true).r;
    output.Position += normalize(output.Position) * displacement;
  #endif
#endif
  return output;
}
