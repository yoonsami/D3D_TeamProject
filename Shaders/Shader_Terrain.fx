#include "Render.fx"
#include "Light.fx"
#include "PBR.fx"


float CalculateTessLevel(float3 cameraWorldPos, float3 patchPos, float min, float max, float maxLv)
{
    float distance = length(patchPos - cameraWorldPos);

    if (distance < min)
        return maxLv;
    if (distance > max)
        return 1.f;

    float ratio = (distance - min) / (max - min);
    float level = (maxLv - 1.f) * (1.f - ratio);
    return level;
}
//MeshOutput VS_Terrain(VTXMesh input)
//{
//    MeshOutput output;
    
//    output.position = mul(float4(input.position, 1.f), W);
//    output.worldPosition = output.position.xyz;
//    output.viewPosition = mul(float4(output.worldPosition, 1.f), V).xyz;
//    output.viewNormal = mul(input.normal, (float3x3) W);
//    output.viewNormal = normalize(mul(output.viewNormal, (float3x3) V));
//    output.viewTangent = mul(input.tangent, (float3x3) W);
//    output.viewTangent = normalize(mul(output.viewTangent, (float3x3) V));
    
//    output.position = mul(output.position, VP);
//    output.uv = input.uv;
//    return output;
//}

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Terrain(VS_IN input)
{

    return input;
}

struct PatchTess
{
    float edgeTess[3] : SV_TessFactor;
    float insideTess : SV_InsideTessFactor;
};

struct HS_OUT
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};
// [Terrain Shader]
// g_int_0      : TileX
// g_int_1      : TileZ
// g_float_1    : Max Tessellation Level
// g_vec2_1     : Min/Max Tessellation Distance
// g_vec2_2

//Constant HS
PatchTess ConstantHS(InputPatch<VS_OUT, 3> input, int patchID : SV_PrimitiveID)
{
    PatchTess output = (PatchTess) 0.f;
    
    float minDistance = g_vec2_1.x;
    float maxDistance = g_vec2_1.y;

    float3 edge0Pos = (input[1].pos + input[2].pos) / 2.f;
    float3 edge1Pos = (input[2].pos + input[0].pos) / 2.f;
    float3 edge2Pos = (input[0].pos + input[1].pos) / 2.f;

    edge0Pos = mul(float4(edge0Pos, 1.f), W).xyz;
    edge1Pos = mul(float4(edge1Pos, 1.f), W).xyz;
    edge2Pos = mul(float4(edge2Pos, 1.f), W).xyz;

    float edge0TessLevel = CalculateTessLevel(CameraPosition().xyz, edge0Pos, minDistance, maxDistance, 4.f);
    float edge1TessLevel = CalculateTessLevel(CameraPosition().xyz, edge1Pos, minDistance, maxDistance, 4.f);
    float edge2TessLevel = CalculateTessLevel(CameraPosition().xyz, edge2Pos, minDistance, maxDistance, 4.f);

    output.edgeTess[0] = edge0TessLevel;
    output.edgeTess[1] = edge1TessLevel;
    output.edgeTess[2] = edge2TessLevel;
    output.insideTess = edge2TessLevel;

    return output;
}

// ControlPoint HS
[domain("tri")] // 패치의 종류 (tri, quad, isoline)
[partitioning("fractional_odd")] // subdivision mode (integer 소수점 무시, fractional_even, fractional_odd)
[outputtopology("triangle_cw")] // (triangle_cw, triangle_ccw, line)
[outputcontrolpoints(3)] // 하나의 입력 패치에 대해, HS가 출력할 제어점 개수
[patchconstantfunc("ConstantHS")] // ConstantHS 함수 이름
HS_OUT HS_Main(InputPatch<VS_OUT, 3> input, int vertexIdx : SV_OutputControlPointID, int patchID : SV_PrimitiveID)
{
    HS_OUT output = (HS_OUT) 0.f;

    output.pos = input[vertexIdx].pos;
    output.uv = input[vertexIdx].uv;

    return output;
}

// maskTexture TextureMap7
// roadColor   TextureMap8
// HeightMap   TextureMap9

[domain("tri")]
MeshOutput DS_Main(const OutputPatch<HS_OUT, 3> input, float3 location : SV_DomainLocation, PatchTess patch)
{
    MeshOutput output = (MeshOutput) 0.f;

    float3 localPos = input[0].pos * location[0] + input[1].pos * location[1] + input[2].pos * location[2];
    float2 uv = input[0].uv * location[0] + input[1].uv * location[1] + input[2].uv * location[2];

    int tileCountX = g_int_0;
    int tileCountZ = g_int_1;
    uint mapWidth = 0;
    uint mapHeight = 0;
    uint mipLevel = 0;
    TextureMap9.GetDimensions(mapWidth, mapHeight);
    
    float2 fullUV = float2(uv.x / (float) tileCountX, uv.y / (float) tileCountZ);
    float height = TextureMap9.SampleLevel(LinearSampler, fullUV, 0).x;
    localPos.y = height * 100.f - 50.f;
    // 높이맵 높이 적용

    float2 deltaUV = float2(1.f / mapWidth, 1.f / mapHeight);
    float2 deltaPos = float2(tileCountX * deltaUV.x, tileCountZ * deltaUV.y);

    float upHeight = TextureMap9.SampleLevel(LinearSampler, float2(fullUV.x, fullUV.y - deltaUV.y), 0).x * 100.f - 50.f;
    float downHeight = TextureMap9.SampleLevel(LinearSampler, float2(fullUV.x, fullUV.y + deltaUV.y), 0).x * 100.f - 50.f;
    float rightHeight = TextureMap9.SampleLevel(LinearSampler, float2(fullUV.x + deltaUV.x, fullUV.y), 0).x * 100.f - 50.f;
    float leftHeight = TextureMap9.SampleLevel(LinearSampler, float2(fullUV.x - deltaUV.x, fullUV.y), 0).x * 100.f - 50.f;
    //localPos.y = (upHeight + downHeight + rightHeight + leftHeight) / 4.f;

    float3 localTangent = float3(localPos.x + deltaPos.x, rightHeight, localPos.z) - float3(localPos.x - deltaPos.x, leftHeight, localPos.z);
    float3 localBinormal = float3(localPos.x, upHeight, localPos.z + deltaPos.y) - float3(localPos.x, downHeight, localPos.z - deltaPos.y);

    output.position = mul(float4(localPos, 1.f),W);
    output.worldPosition = output.position;
    output.viewPosition = mul(float4(output.worldPosition, 1.f),V).xyz;
    output.position = mul(float4(output.viewPosition, 1.f), P);

    output.viewTangent = normalize(mul(float4(localTangent, 0.f), mul(W, V))).xyz;
    float3 viewBinormal = normalize(mul(float4(localBinormal, 0.f), mul(W, V))).xyz;
    output.viewNormal = normalize(cross(viewBinormal, output.viewTangent));

    output.uv = uv;

    return output;
}

PS_OUT_Deferred PS_Deferred(MeshOutput input)
{
    PS_OUT_Deferred output = (PS_OUT_Deferred) 0.f;
    float4 specularColor;
    float4 emissiveColor;
    
    //if (bHasNormalMap)
    //{
    //    ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, input.uv);
        
    //}
    float4 maskColor = TextureMap7.Sample(LinearSampler, float2(input.uv.x / g_int_0, input.uv.y / g_int_1));
    float4 diffuseColor = 1.f;
    
    // 길이 있으면 길색깔잡초색깔 비율맞춤
    if (maskColor.g > 0)
    {
        float4 RoadColor = TextureMap8.Sample(LinearSampler, input.uv);
        float4 tempDiffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
        
        RoadColor = mul(RoadColor, maskColor.g);
        tempDiffuseColor = mul(tempDiffuseColor, 1 - maskColor.g);
        
        diffuseColor = RoadColor + tempDiffuseColor;
    }
    else
        diffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
    
    ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, input.uv);
    
    // 브러시관련
    float3 vBrushPos = g_vec4_0.xyz;
    float fBrushRadius = g_float_0;
    float4 colBrushColor = float4(0.f, 0.f, 0.f, 0.f);
    
    vBrushPos.y = input.worldPosition.y; // 높이는 같다고 가정.
    
    // 브러시포지션과 거리가 브러시Radius이하라면 색깔을 흰색으로 함.
    if (length((input.worldPosition) - vBrushPos) <= fBrushRadius)
    {
        colBrushColor = float4(0.2f, 0.2f, 0.2, 0.f);
    }
    diffuseColor = diffuseColor + colBrushColor;
    
    if (bHasSpecularMap)
    {
        specularColor = SpecularMap.Sample(LinearSampler, input.uv);
        specularColor.rgb = pow(abs(specularColor.rgb), GAMMA);
    }
    else
        specularColor = Material.specular;
   
    if (bHasEmissiveMap)
    {
        emissiveColor = EmissiveMap.Sample(LinearSampler, input.uv);
        emissiveColor.rgb = pow(abs(emissiveColor.rgb), GAMMA);
    }
    else
        emissiveColor = Material.emissive;
    
    output.position = float4(input.viewPosition.xyz, 0.f);
    output.normal = float4(input.viewNormal.xyz, 0.f);
    output.depth = input.position.z;
    output.depth.w = input.viewPosition.z;
    output.diffuseColor = diffuseColor;
    output.specularColor = specularColor;
    output.emissiveColor = emissiveColor;
    return output;
}


//PS_OUT_Deferred PS_Deferred(MeshOutput input)
//{
//    PS_OUT_Deferred output = (PS_OUT_Deferred) 0.f;
//    float4 specularColor;
//    float4 emissiveColor;
    
//    //if (bHasNormalMap)
//    //{
//    //    ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, input.uv);
        
//    //}
//    float4 maskColor = TextureMap7.Sample(LinearSampler, float2(input.uv.x / g_vec2_0.x, input.uv.y / g_vec2_0.y));
//    float4 diffuseColor = 1.f;
    
//    // 길이 있으면 길색깔잡초색깔 비율맞춤
//    if (maskColor.g > 0)
//    {
//        float4 RoadColor = TextureMap8.Sample(LinearSampler, input.uv);
//        float4 tempDiffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
        
//        RoadColor = mul(RoadColor, maskColor.g);
//        tempDiffuseColor = mul(tempDiffuseColor, 1 - maskColor.g);
        
//        diffuseColor = RoadColor + tempDiffuseColor;
//    }
//    else
//        diffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
    
//    ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, input.uv);
    
//    // 브러시관련
//    float3 vBrushPos = g_vec4_0.xyz;
//    float fBrushRadius = g_float_0;
//    float4 colBrushColor = float4(0.f, 0.f, 0.f, 0.f);
    
//    vBrushPos.y = input.worldPosition.y; // 높이는 같다고 가정.
    
//    // 브러시포지션과 거리가 브러시Radius이하라면 색깔을 흰색으로 함.
//    if (length((input.worldPosition) - vBrushPos) <= fBrushRadius)
//    {
//        colBrushColor = float4(0.2f, 0.2f, 0.2, 0.f);
//    }
//    diffuseColor = diffuseColor + colBrushColor;
    
//    if (bHasSpecularMap)
//    {
//        specularColor = SpecularMap.Sample(LinearSampler, input.uv);
//        specularColor.rgb = pow(abs(specularColor.rgb), GAMMA);
//    }
//    else
//        specularColor = Material.specular;
   
//    if (bHasEmissiveMap)
//    {
//        emissiveColor = EmissiveMap.Sample(LinearSampler, input.uv);
//        emissiveColor.rgb = pow(abs(emissiveColor.rgb), GAMMA);
//    }
//    else
//        emissiveColor = Material.emissive;
    
//    output.position = float4(input.viewPosition.xyz, 0.f);
//    output.normal = float4(input.viewNormal.xyz, 0.f);
//    output.depth = input.position.z;
//    output.depth.w = input.viewPosition.z;
//    output.diffuseColor = diffuseColor;
//    output.specularColor = specularColor;
//    output.emissiveColor = emissiveColor;
//    return output;
//}

PBR_MAPOBJECT_OUTPUT PS_Terrain_PBR(MeshOutput input)
{
    PBR_MAPOBJECT_OUTPUT output = (PBR_MAPOBJECT_OUTPUT) 0.f;
    float4 color;
    
    float4 maskColor = TextureMap7.Sample(LinearSampler, float2(input.uv.x / g_int_0, input.uv.y / g_int_1));
    float4 diffuseColor = 1.f;
    
    // 길이 있으면 길색깔잡초색깔 비율맞춤
    if (maskColor.g > 0)
    {
        float4 RoadColor = TextureMap8.Sample(LinearSampler, input.uv);
        float4 tempDiffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
        
        RoadColor = mul(RoadColor, maskColor.g);
        tempDiffuseColor = mul(tempDiffuseColor, 1 - maskColor.g);
        
        diffuseColor = RoadColor + tempDiffuseColor;
    }
    else
        diffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
        
    // 브러시관련
    float3 vBrushPos = g_vec4_0.xyz;
    float fBrushRadius = g_float_0;
    float4 colBrushColor = float4(0.f, 0.f, 0.f, 0.f);
    
    vBrushPos.y = input.worldPosition.y; // 높이는 같다고 가정.
    
    // 브러시포지션과 거리가 브러시Radius이하라면 색깔을 흰색으로 함.
    if (length((input.worldPosition) - vBrushPos) <= fBrushRadius)
    {
        colBrushColor = float4(0.2f, 0.2f, 0.2, 0.f);
    }
    color = diffuseColor + colBrushColor;
    
    if (bHasNormalMap)
        ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, input.uv);

    
    
    output.position = float4(input.viewPosition.xyz, 1.f);
    output.normal = float4(input.viewNormal.xyz, 0.f);
    output.positionSSD = float4(input.viewPosition.xyz, 1.f);
    output.arm = float4(1.f, 0.8f, 0.0f, 1.f);
    output.diffuseColor = color;
    output.emissive = 0.f;
    output.rimColor = Material.emissive;
    output.blur = 0;
    return output;

}


technique11 T0
{
    pass Terrain_Deferred
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Terrain()));
        SetHullShader(CompileShader(hs_5_0, HS_Main()));
        SetDomainShader(CompileShader(ds_5_0, DS_Main()));
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred())); 
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(NULL);
    }
    pass Terrain_Deferred_PBR
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Terrain()));
        SetHullShader(CompileShader(hs_5_0, HS_Main()));
        SetDomainShader(CompileShader(ds_5_0, DS_Main()));
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Terrain_PBR()));
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(NULL);
    }

};
