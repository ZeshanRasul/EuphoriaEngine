
#include <Shaders/Materials/MaterialTessellationData.h>


TessellationFactors patchConstantFunc(InputPatch<VS_OUT, 3> patch)
{
    TessellationFactors f;

      // Calculate the average normal of the patch
    float3 avgNormal = normalize(patch[0].Normal + patch[1].Normal + patch[2].Normal);

    float3 upDirection = float3(0.0, 0.0, 1.0);
    float angle = acos(dot(avgNormal, upDirection));

    // Map the angle to a tessellation factor
    float angleFactor = (1.0 - saturate(angle / (PI / 2.0)));

    float adjustedTessFactor = (10.0f) * angleFactor ;

    f.edge[0] = adjustedTessFactor;
    f.edge[1] = adjustedTessFactor;
    f.edge[2] = adjustedTessFactor;
    f.inside = adjustedTessFactor;

    return f;
}

[domain("tri")]
[outputcontrolpoints(3)]
[outputtopology("triangle_cw")]
[partitioning("integer")]
[patchconstantfunc("patchConstantFunc")]
VS_OUT main(InputPatch<VS_OUT, 3> patch, uint id : SV_OutputControlPointID)
{
    return patch[id];
}
