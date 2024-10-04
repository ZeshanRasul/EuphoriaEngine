#include <Shaders/Common/GlobalConstants.h>

CONSTANT_BUFFER(plMaterialConstants, 1)
{
  COLOR4F(BaseColor);
  COLOR4F(EmissiveColor);
  FLOAT2(Scale);
  FLOAT1(MetallicValue);
  FLOAT1(ReflectanceValue);
  FLOAT1(RoughnessValue);
  FLOAT1(MaskThreshold);
  FLOAT1(HeightScale);
  FLOAT1(MaxSlices);
  BOOL1(UseBaseTexture);
  BOOL1(UseNormalTexture);
  BOOL1(UseRoughnessTexture);
  BOOL1(UseMetallicTexture);
  BOOL1(UseEmissiveTexture);
  BOOL1(UseOcclusionTexture);
  BOOL1(UseORMTexture);
  BOOL1(UseRMATexture);
  BOOL1(InvertOcclusion);
  BOOL1(UseDisplacementTexture);
  BOOL1(UseDisplacementStochasticTexturing);
  BOOL1(UseBaseStochasticTexturing);
  BOOL1(UseNormalStochasticTexturing);
  BOOL1(UseOcclusionStochasticTexturing);
  BOOL1(UseRoughnessStochasticTexturing);
  BOOL1(UseMetallicStochasticTexturing);
  BOOL1(UseOrmStochasticTexturing);
  BOOL1(UseEmissiveStochasticTexturing);

};
