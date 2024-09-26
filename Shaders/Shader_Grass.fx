#include "Render.fx"
#include "Light.fx"
#include "PBR.fx"
MeshOutput VS_Grass(VTXMesh input)
{
    MeshOutput output;
    output.position = float4(input.position, 1.f);
    output.uv = input.uv;
    output.worldPosition = mul(float4(input.position, 1.f), W).xyz;
    output.viewPosition = mul(float4(output.worldPosition, 1.f), V).xyz;
    output.viewNormal = mul(float4(input.normal, 0.f), V).xyz;
    output.viewTangent = mul(float4(input.tangent, 0.f), V).xyz;
    
    return output;
}

struct InstancingOuput
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION1;
    float3 viewPosition : POSITION2;
    float2 uv : TEXCOORD;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    uint id : SV_InstanceID;
    float4x4 matWorld : POSITION3;
};

InstancingOuput VS_Grass_Instancing(VTXMeshInstancing input)
{
    InstancingOuput output;
    output.position = float4(input.position, 1.f);
    output.uv = input.uv;
    output.worldPosition = mul(float4(input.position, 1.f), input.world).xyz;
    output.viewPosition = mul(float4(output.worldPosition, 1.f), V).xyz;
    output.viewNormal = mul(float4(input.normal, 0.f), V).xyz;
    output.viewTangent = mul(float4(input.tangent, 0.f), V).xyz;
    
    output.matWorld = input.world;
    output.id = input.instanceID;
    return output;
    
}

struct GS_GRASS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 viewPosition : POSITION2;
    float2 uv : TEXCOORD;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
};

struct GS_GRASS_INSTANCING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 viewPosition : POSITION2;
    float2 uv : TEXCOORD;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    uint id : SV_InstanceID;
};

float4x4 RotateMatrix(float angle, float3 axis)
{
    float s = sin(angle);
    float c = cos(angle);
    float oneMinusC = 1.0 - c;

    float3 sq = axis * axis;
    float3x3 rotationMatrix = float3x3(
        sq.x * oneMinusC + c,
        axis.x * axis.y * oneMinusC + axis.z * s,
        axis.x * axis.z * oneMinusC - axis.y * s,

        axis.x * axis.y * oneMinusC - axis.z * s,
        sq.y * oneMinusC + c,
        axis.y * axis.z * oneMinusC + axis.x * s,

        axis.x * axis.z * oneMinusC + axis.y * s,
        axis.y * axis.z * oneMinusC - axis.x * s,
        sq.z * oneMinusC + c
    );
    
    return float4x4(
        float4(rotationMatrix[0], 0),
        float4(rotationMatrix[1], 0),
        float4(rotationMatrix[2], 0),
        float4(0, 0, 0, 1)
    );
}

[maxvertexcount(18)]
void GS_Grass(point MeshOutput input[1], inout TriangleStream<GS_GRASS_OUTPUT> outputStream)
{
    const uint vertexCount = 4;
    const uint billboardCount = 3;
    GS_GRASS_OUTPUT output[vertexCount * billboardCount] =
    {
        (GS_GRASS_OUTPUT) 0.f, (GS_GRASS_OUTPUT) 0.f, (GS_GRASS_OUTPUT) 0.f, (GS_GRASS_OUTPUT) 0.f,
        (GS_GRASS_OUTPUT) 0.f, (GS_GRASS_OUTPUT) 0.f, (GS_GRASS_OUTPUT) 0.f, (GS_GRASS_OUTPUT) 0.f,
        (GS_GRASS_OUTPUT) 0.f, (GS_GRASS_OUTPUT) 0.f, (GS_GRASS_OUTPUT) 0.f, (GS_GRASS_OUTPUT) 0.f
    };
    MeshOutput vtx = input[0];
    
    // 바람의 방향,세기(이동량),현재가중치(0~1),속도(0~1도달속도)
    float3 vWind = g_vec4_0.xyz;
    float fWindPowerMagicNumber = 0.05f;
    float fWindWeight = g_vec4_0.w;
    
    for (uint j = 0; j < billboardCount; j++)
    {
        float4x4 matRotateByBillboard = RotateMatrix(radians(120.f * j), float3(0.f, 1.f, 0.f));
        float4x4 RotateWByBillboard = mul(matRotateByBillboard, W);
        
        output[j * 4 + 0].position = float4(vtx.position.xyz/*포지션*/ + matRotateByBillboard[2].xyz * 0.1f /*삼각편대*/ - matRotateByBillboard[0].xyz * 0.5f + matRotateByBillboard[1].xyz * 0.5f /*사각형을위한점위치*/ + matRotateByBillboard[1].xyz * 0.5f /*높이*/ + mul(mul(vWind, fWindPowerMagicNumber), fWindWeight) /*바람정보를 위쪽 버텍스에 반영*/, 1.f);
        output[j * 4 + 1].position = float4(vtx.position.xyz/*포지션*/ + matRotateByBillboard[2].xyz * 0.1f /*삼각편대*/ + matRotateByBillboard[0].xyz * 0.5f + matRotateByBillboard[1].xyz * 0.5f /*사각형을위한점위치*/ + matRotateByBillboard[1].xyz * 0.5f /*높이*/ + mul(mul(vWind, fWindPowerMagicNumber), fWindWeight) /*바람정보를 위쪽 버텍스에 반영*/, 1.f);
        output[j * 4 + 2].position = float4(vtx.position.xyz/*포지션*/ + matRotateByBillboard[2].xyz * 0.1f /*삼각편대*/ + matRotateByBillboard[0].xyz * 0.5f - matRotateByBillboard[1].xyz * 0.5f /*사각형을위한점위치*/ + matRotateByBillboard[1].xyz * 0.5f /*높이*/, 1.f);
        output[j * 4 + 3].position = float4(vtx.position.xyz/*포지션*/ + matRotateByBillboard[2].xyz * 0.1f /*삼각편대*/ - matRotateByBillboard[0].xyz * 0.5f - matRotateByBillboard[1].xyz * 0.5f /*사각형을위한점위치*/ + matRotateByBillboard[1].xyz * 0.5f /*높이*/, 1.f);
        
        output[j * 4 + 0].uv = float2(0.f, 0.001f);
        output[j * 4 + 1].uv = float2(1.f, 0.001f);
        output[j * 4 + 2].uv = float2(1.f, 0.999f);
        output[j * 4 + 3].uv = float2(0.f, 0.999f);
    
        output[j * 4 + 0].viewPosition = mul(output[j * 4 + 0].position, mul(W, V));
        output[j * 4 + 1].viewPosition = mul(output[j * 4 + 1].position, mul(W, V));
        output[j * 4 + 2].viewPosition = mul(output[j * 4 + 2].position, mul(W, V));
        output[j * 4 + 3].viewPosition = mul(output[j * 4 + 3].position, mul(W, V));

    // proj q
        output[j * 4 + 0].position = mul(float4(output[j * 4 + 0].viewPosition, 1.f), P);
        output[j * 4 + 1].position = mul(float4(output[j * 4 + 1].viewPosition, 1.f), P);
        output[j * 4 + 2].position = mul(float4(output[j * 4 + 2].viewPosition, 1.f), P);
        output[j * 4 + 3].position = mul(float4(output[j * 4 + 3].viewPosition, 1.f), P);
        
        output[j * 4 + 0].viewNormal = mul(RotateWByBillboard[2], V).xyz;
        output[j * 4 + 1].viewNormal = mul(RotateWByBillboard[2], V).xyz;
        output[j * 4 + 2].viewNormal = mul(RotateWByBillboard[2], V).xyz;
        output[j * 4 + 3].viewNormal = mul(RotateWByBillboard[2], V).xyz;
        
        output[j * 4 + 0].viewTangent = mul(RotateWByBillboard[0], V).xyz;
        output[j * 4 + 1].viewTangent = mul(RotateWByBillboard[0], V).xyz;
        output[j * 4 + 2].viewTangent = mul(RotateWByBillboard[0], V).xyz;
        output[j * 4 + 3].viewTangent = mul(RotateWByBillboard[0], V).xyz;

        outputStream.Append(output[j * 4 + 0]);
        outputStream.Append(output[j * 4 + 1]);
        outputStream.Append(output[j * 4 + 2]);
        outputStream.RestartStrip();

        outputStream.Append(output[j * 4 + 0]);
        outputStream.Append(output[j * 4 + 2]);
        outputStream.Append(output[j * 4 + 3]);
        outputStream.RestartStrip();
    }

}

[maxvertexcount(18)]
void GS_Grass_Instancing(point InstancingOuput input[1], inout TriangleStream<GS_GRASS_INSTANCING_OUTPUT> outputStream)
{
    const uint vertexCount = 4;
    const uint billboardCount = 3;
    GS_GRASS_INSTANCING_OUTPUT output[vertexCount * billboardCount] =
    {
        (GS_GRASS_INSTANCING_OUTPUT) 0.f, (GS_GRASS_INSTANCING_OUTPUT) 0.f, (GS_GRASS_INSTANCING_OUTPUT) 0.f, (GS_GRASS_INSTANCING_OUTPUT) 0.f,
        (GS_GRASS_INSTANCING_OUTPUT) 0.f, (GS_GRASS_INSTANCING_OUTPUT) 0.f, (GS_GRASS_INSTANCING_OUTPUT) 0.f, (GS_GRASS_INSTANCING_OUTPUT) 0.f,
        (GS_GRASS_INSTANCING_OUTPUT) 0.f, (GS_GRASS_INSTANCING_OUTPUT) 0.f, (GS_GRASS_INSTANCING_OUTPUT) 0.f, (GS_GRASS_INSTANCING_OUTPUT) 0.f
    };
    
    InstancingOuput vtx = input[0];
    
    // 바람의 방향,세기(이동량),현재가중치(0~1),속도(0~1도달속도)
    float3 vWind = InstanceRenderParams[vtx.id].xyz;
    float fWindPowerMagicNumber = 0.05f;
    float fWindWeight = InstanceRenderParams[vtx.id].w;
    
    for (uint j = 0; j < billboardCount; j++)
    {
        float4x4 matRotateByBillboard = RotateMatrix(radians(120.f * j), float3(0.f, 1.f, 0.f));
        float4x4 RotateWByBillboard = mul(matRotateByBillboard, vtx.matWorld);
        
        output[j * 4 + 0].position = float4(vtx.position.xyz/*포지션*/ + matRotateByBillboard[2].xyz * 0.1f /*삼각편대*/ - matRotateByBillboard[0].xyz * 0.5f + matRotateByBillboard[1].xyz * 0.5f /*사각형을위한점위치*/ + matRotateByBillboard[1].xyz * 0.5f /*높이*/ + mul(mul(vWind, fWindPowerMagicNumber), fWindWeight) /*바람정보를 위쪽 버텍스에 반영*/, 1.f);
        output[j * 4 + 1].position = float4(vtx.position.xyz/*포지션*/ + matRotateByBillboard[2].xyz * 0.1f /*삼각편대*/ + matRotateByBillboard[0].xyz * 0.5f + matRotateByBillboard[1].xyz * 0.5f /*사각형을위한점위치*/ + matRotateByBillboard[1].xyz * 0.5f /*높이*/ + mul(mul(vWind, fWindPowerMagicNumber), fWindWeight) /*바람정보를 위쪽 버텍스에 반영*/, 1.f);
        output[j * 4 + 2].position = float4(vtx.position.xyz/*포지션*/ + matRotateByBillboard[2].xyz * 0.1f /*삼각편대*/ + matRotateByBillboard[0].xyz * 0.5f - matRotateByBillboard[1].xyz * 0.5f /*사각형을위한점위치*/ + matRotateByBillboard[1].xyz * 0.5f /*높이*/, 1.f);
        output[j * 4 + 3].position = float4(vtx.position.xyz/*포지션*/ + matRotateByBillboard[2].xyz * 0.1f /*삼각편대*/ - matRotateByBillboard[0].xyz * 0.5f - matRotateByBillboard[1].xyz * 0.5f /*사각형을위한점위치*/ + matRotateByBillboard[1].xyz * 0.5f /*높이*/, 1.f);
        
        output[j * 4 + 0].uv = float2(0.f, 0.001f);
        output[j * 4 + 1].uv = float2(1.f, 0.001f);
        output[j * 4 + 2].uv = float2(1.f, 0.999f);
        output[j * 4 + 3].uv = float2(0.f, 0.999f);
    
        output[j * 4 + 0].viewPosition = mul(output[j * 4 + 0].position, mul(vtx.matWorld, V));
        output[j * 4 + 1].viewPosition = mul(output[j * 4 + 1].position, mul(vtx.matWorld, V));
        output[j * 4 + 2].viewPosition = mul(output[j * 4 + 2].position, mul(vtx.matWorld, V));
        output[j * 4 + 3].viewPosition = mul(output[j * 4 + 3].position, mul(vtx.matWorld, V));

    // proj q
        output[j * 4 + 0].position = mul(float4(output[j * 4 + 0].viewPosition, 1.f), P);
        output[j * 4 + 1].position = mul(float4(output[j * 4 + 1].viewPosition, 1.f), P);
        output[j * 4 + 2].position = mul(float4(output[j * 4 + 2].viewPosition, 1.f), P);
        output[j * 4 + 3].position = mul(float4(output[j * 4 + 3].viewPosition, 1.f), P);
        
        output[j * 4 + 0].viewNormal = mul(RotateWByBillboard[2], V).xyz;
        output[j * 4 + 1].viewNormal = mul(RotateWByBillboard[2], V).xyz;
        output[j * 4 + 2].viewNormal = mul(RotateWByBillboard[2], V).xyz;
        output[j * 4 + 3].viewNormal = mul(RotateWByBillboard[2], V).xyz;
        
        output[j * 4 + 0].viewTangent = mul(RotateWByBillboard[0], V).xyz;
        output[j * 4 + 1].viewTangent = mul(RotateWByBillboard[0], V).xyz;
        output[j * 4 + 2].viewTangent = mul(RotateWByBillboard[0], V).xyz;
        output[j * 4 + 3].viewTangent = mul(RotateWByBillboard[0], V).xyz;
        
        output[j * 4 + 0].id = vtx.id;
        output[j * 4 + 1].id = vtx.id;
        output[j * 4 + 2].id = vtx.id;
        output[j * 4 + 3].id = vtx.id;
        
        outputStream.Append(output[j * 4 + 0]);
        outputStream.Append(output[j * 4 + 1]);
        outputStream.Append(output[j * 4 + 2]);
        outputStream.RestartStrip();

        outputStream.Append(output[j * 4 + 0]);
        outputStream.Append(output[j * 4 + 2]);
        outputStream.Append(output[j * 4 + 3]);
        outputStream.RestartStrip();
    }

}

PS_OUT_Deferred PS_Deferred(GS_GRASS_OUTPUT input)
{
    PS_OUT_Deferred output = (PS_OUT_Deferred) 0.f;
    float4 diffuseColor;
    float4 specularColor;
    float4 emissiveColor;
    
    float2 distortedUV = input.uv;
    
    if (bHasDissolveMap != 0)
    {
        float dissolve = DissolveMap.Sample(LinearSampler, input.uv).r;
        if (dissolve < g_vec4_0.w)
            discard;
    }
    
    
    if (bHasNormalMap)
    {
        ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, input.uv);
        
    }

    if (bHasDiffuseMap)
    {
        diffuseColor = DiffuseMap.Sample(LinearSamplerMirror, input.uv);
        diffuseColor.rgb = pow(abs(diffuseColor.rgb), GAMMA);
    }
    else
        diffuseColor = Material.diffuse;

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
    
    diffuseColor = diffuseColor;
    
    output.position = float4(input.viewPosition.xyz, 1.f);
    output.normal = float4(input.viewNormal.xyz, 0.f);
    output.depth = input.position.z;
    output.depth.w = input.viewPosition.z;
    output.diffuseColor = diffuseColor;
    output.specularColor = specularColor;
    output.emissiveColor = emissiveColor;
    return output;
}

PS_OUT_Deferred PS_Deferred_Instancing(GS_GRASS_INSTANCING_OUTPUT input)
{
    PS_OUT_Deferred output = (PS_OUT_Deferred) 0.f;
    float4 diffuseColor;
    float4 specularColor;
    float4 emissiveColor;
    
    float2 distortedUV = input.uv;
    
    if (bHasDissolveMap != 0)
    {
        float dissolve = DissolveMap.Sample(LinearSampler, input.uv).r;
        if (dissolve < InstanceRenderParams[input.id].x)
            discard;
    }
    
    if (bHasNormalMap)
    {
        ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, input.uv);
        
    }

    if (bHasDiffuseMap)
    {
        diffuseColor = DiffuseMap.Sample(LinearSamplerMirror, input.uv);
        diffuseColor.rgb = pow(abs(diffuseColor.rgb), GAMMA);
    }
    else
        diffuseColor = Material.diffuse;
    
    if (diffuseColor.a <= 0.1f)
        discard;

    diffuseColor.a = 1.f;
    
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

    diffuseColor = diffuseColor;
    
    output.position = float4(input.viewPosition.xyz, 1.f);
    output.normal = float4(input.viewNormal.xyz, 0.f);
    output.depth = input.position.z;
    output.depth.w = input.viewPosition.z;
    output.diffuseColor = diffuseColor;
    output.specularColor = specularColor;
    output.emissiveColor = emissiveColor;
    return output;
}

PBR_OUTPUT PS_PBR_Deferred(GS_GRASS_OUTPUT input)
{
    PBR_OUTPUT output = (PBR_OUTPUT) 0.f;
    float4 diffuseColor;
    float4 specularColor;
    float4 emissiveColor;
    float4 ARM_Map;

    if (bHasDissolveMap)
    {
        float dissolve = DissolveMap.Sample(LinearSampler, input.uv).w;
        if (dissolve < g_vec4_0.w)
            discard;
    }
    
    ARM_Map = float4(1.f, 0.8f, 0.0f, 1.f);
    
    if (bHasTexturemap7)
    {
        ARM_Map = TextureMap7.Sample(LinearSampler, input.uv);
        ARM_Map = pow(abs(ARM_Map), GAMMA);
    }

    if (bHasNormalMap)
        ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, input.uv);

    if (bHasDiffuseMap)
    {
        diffuseColor = DiffuseMap.Sample(LinearSamplerMirror, input.uv);
        diffuseColor.rgb = pow(abs(diffuseColor.rgb), GAMMA);
    }
    else
        diffuseColor = Material.diffuse;
    
    if (diffuseColor.a <= 0.1f)
        discard;

    diffuseColor.a = 1.f;
       
    if (bHasEmissiveMap)
    {
        emissiveColor = EmissiveMap.Sample(LinearSampler, input.uv);
        emissiveColor.rgb = pow(abs(emissiveColor.rgb), GAMMA);
    }
    else
        emissiveColor = 0.f;
    


    output.position = float4(input.viewPosition.xyz, 1.f);
    output.normal = float4(input.viewNormal.xyz, 0.f);
    output.arm = ARM_Map;
    output.diffuseColor = diffuseColor;
    output.emissive = emissiveColor;
    output.rimColor = Material.emissive;
    output.blur = 0;
    return output;
}

PBR_OUTPUT PS_PBR_Deferred_Instancing(GS_GRASS_INSTANCING_OUTPUT input)
{
    PBR_OUTPUT output = (PBR_OUTPUT) 0.f;
    float4 diffuseColor;
    float4 specularColor;
    float4 emissiveColor;
    float4 ARM_Map;

    if (bHasDissolveMap != 0)
    {
        float dissolve = DissolveMap.Sample(LinearSampler, input.uv).w;
        if (dissolve < InstanceRenderParams[input.id].w)
            discard;
    }
    
    ARM_Map = float4(1.f, 0.8f, 0.0f, 1.f);
    
    if (bHasTexturemap7)
    {
        ARM_Map = TextureMap7.Sample(LinearSampler, input.uv);
        ARM_Map = pow(abs(ARM_Map), GAMMA);
    }

    if (bHasNormalMap)
        ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, input.uv);

    if (bHasDiffuseMap)
    {
        diffuseColor = DiffuseMap.Sample(LinearSamplerMirror, input.uv);
        diffuseColor.rgb = pow(abs(diffuseColor.rgb), GAMMA);
    }
    else
        diffuseColor = Material.diffuse;
    
    if (diffuseColor.a <= 0.1f)
        discard;

    diffuseColor.a = 1.f;
       
    if (bHasEmissiveMap)
    {
        emissiveColor = EmissiveMap.Sample(LinearSampler, input.uv);
        emissiveColor.rgb = pow(abs(emissiveColor.rgb), GAMMA);
    }
    else
        emissiveColor = 0.f;

    
    
    output.position = float4(input.viewPosition.xyz, 1.f);
    output.normal = float4(input.viewNormal.xyz, 0.f);
    output.arm = ARM_Map;
    output.diffuseColor = diffuseColor;
    output.emissive = emissiveColor;
    output.rimColor = Material.emissive;
    output.blur = 0;
    return output;
}


technique11 NonInstancing
{
    pass GeometryWeed_NonPBR
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Grass()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Grass()));
    }
    pass GeometryWeed_PBR
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Grass()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Grass()));
    }
};

technique11 Instancing
{
    pass GeometryWeed_NonPBR
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Grass_Instancing()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred_Instancing()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Grass_Instancing()));
    }
    pass GeometryWeed_PBR
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Grass_Instancing()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred_Instancing()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Grass_Instancing()));
    }
};
