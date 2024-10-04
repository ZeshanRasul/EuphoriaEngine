struct TessellationFactors
{
  float edge[3] : SV_TessFactor;
  float inside : SV_InsideTessFactor;
};