#include "Render.fx"
#include "Light.fx"
float g_BarPercent;
float4 g_DrawColor;
struct VS_OUT
{
    float4 viewPos : POSITION;
    float2 uv : TEXCOORD;
};

UIOutput VS_UI(VTXMesh input)
{
    UIOutput output;
    output.position = mul(float4(input.position, 1.f), W);
    output.position = mul(output.position, VP);
    output.uv = input.uv;

    return output;
}

VS_OUT VS_Default(VTXMesh input)
{
    VS_OUT output;
    float3 worldPos = mul(float4(input.position, 1.f), W).xyz;
    output.viewPos = mul(float4(worldPos, 1.f), V);

    output.uv = input.uv;

    return output;
}

VS_OUT VS_3D_To_2D(VTXMesh input)
{
    VS_OUT output;
    
    
    float3 viewPos = mul(float4(g_vec4_1.xyz, 1.f), g_mat_0).xyz;
    float4 projPos = mul(float4(viewPos, 1.f), g_mat_1);

    projPos.xy /= projPos.w;

    float2 screenPos;
    screenPos.x = (projPos.x + 1.f) * 0.5f * 1920.f;
    screenPos.y = ((projPos.y * -1.f) + 1.f) * 0.5f * 1080.f;

    screenPos.x -= 1920 * 0.5f;
    screenPos.y = (screenPos.y * -1.f) + 1080 * 0.5f;

    output.viewPos = mul(float4(screenPos.x, screenPos.y, W._43, 1.f), V);
    output.uv = input.uv;

    return output;
}

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

VS_OUT VS_ViewPort(VTXMesh input)
{    
    VS_OUT output;
    float3 worldPos = mul(float4(input.position, 1.f), W).xyz;
    output.viewPos = mul(float4(worldPos, 1.f), V);

    output.uv = input.uv;

    return output;
}

UIInstancingOutput VS_Instancing(VTXMeshInstancing input)
{
    UIInstancingOutput output;
    output.position = mul(float4(input.position, 1.f), input.world);
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    output.id = input.instanceID;
    return output;
}

struct GS_OUTPUT
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    float3 viewPos : POSITION1;
};

//g_int_0 spriteCountX
//g_int_1 spriteCountY
//g_vec2_0 curtime,lifetime
//g_vec2_1 sizexy
//g_vec4_0.x > 0.5f = HasDissolve
//g_vec4_0.y > 0.5f = Sprited
//g_vec4_0.z = dissolveSpeed
//g_vec4_0.w = MaintainTime

//g_vec4_0 - TerrainBrushPos
//g_float_0 - TerrainBrushRadius

struct GS_GRASS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 viewPosition : POSITION2;
    float2 uv : TEXCOORD;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
};

[maxvertexcount(6)]
void GS_Main(point VS_OUT input[1], inout TriangleStream<GS_OUTPUT> outputStream)
{
    GS_OUTPUT output[4] =
    {
        (GS_OUTPUT)0.f, (GS_OUTPUT)0.f, (GS_OUTPUT)0.f, (GS_OUTPUT)0.f
    };

    float3 trans, mscale;
    float4 q;
    decompose(W, trans, q, mscale);




    VS_OUT vtx = input[0];
    float2 scale = mscale.xy * 0.5f;

    output[0].position = vtx.viewPos +float4(-scale.x, scale.y, 0.f, 0.f);
    output[1].position = vtx.viewPos + float4(scale.x, scale.y, 0.f, 0.f);
    output[2].position = vtx.viewPos + float4(scale.x, -scale.y, 0.f, 0.f);
    output[3].position = vtx.viewPos + float4(-scale.x, -scale.y, 0.f, 0.f);

    output[0].viewPos = output[0].position.xyz;
    output[1].viewPos = output[1].position.xyz;
    output[2].viewPos = output[2].position.xyz;
    output[3].viewPos = output[3].position.xyz;

    // proj q
    for (int i = 0; i < 4; ++i)
        output[i].position = mul(output[i].position, P);

    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

    for (int j = 0; j < 4; ++j)
    {
        output[j].viewNormal = float3(0.f, 0.f, -1.f);
        output[j].viewTangent = float3(1.f, 0.f, 0.f);
    }

    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[2]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}

// 회전행렬 생성
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
    
    for (uint j = 0; j < billboardCount; j++)
    {
        float4x4 matRotateByBillboard = RotateMatrix(radians(120.f * j), float3(0.f, 1.f, 0.f));
        float4x4 RotateWByBillboard = mul(matRotateByBillboard, W);
        MeshOutput vtx = input[0];
        
        output[j * 4 + 0].position = float4(vtx.position.xyz/*포지션*/ + matRotateByBillboard[2].xyz * 0.1f /*삼각편대*/ - matRotateByBillboard[0].xyz * 0.5f + matRotateByBillboard[1].xyz * 0.5f/*사각형을위한점위치*/ + matRotateByBillboard[1].xyz * 0.5f/*높이*/, 1.f);
        output[j * 4 + 1].position = float4(vtx.position.xyz/*포지션*/ + matRotateByBillboard[2].xyz * 0.1f /*삼각편대*/ + matRotateByBillboard[0].xyz * 0.5f + matRotateByBillboard[1].xyz * 0.5f/*사각형을위한점위치*/ + matRotateByBillboard[1].xyz * 0.5f/*높이*/, 1.f);
        output[j * 4 + 2].position = float4(vtx.position.xyz/*포지션*/ + matRotateByBillboard[2].xyz * 0.1f /*삼각편대*/ + matRotateByBillboard[0].xyz * 0.5f - matRotateByBillboard[1].xyz * 0.5f/*사각형을위한점위치*/ + matRotateByBillboard[1].xyz * 0.5f/*높이*/, 1.f);
        output[j * 4 + 3].position = float4(vtx.position.xyz/*포지션*/ + matRotateByBillboard[2].xyz * 0.1f /*삼각편대*/ - matRotateByBillboard[0].xyz * 0.5f - matRotateByBillboard[1].xyz * 0.5f/*사각형을위한점위치*/ + matRotateByBillboard[1].xyz * 0.5f/*높이*/, 1.f);
        
        output[j * 4 + 0].uv = float2(0.f, 0.f);
        output[j * 4 + 1].uv = float2(1.f, 0.f);
        output[j * 4 + 2].uv = float2(1.f, 1.f);
        output[j * 4 + 3].uv = float2(0.f, 1.f);
    
        output[j * 4 + 0].viewPosition = mul(output[j * 4 + 0].position, mul(W,V));
        output[j * 4 + 1].viewPosition = mul(output[j * 4 + 1].position, mul(W,V));
        output[j * 4 + 2].viewPosition = mul(output[j * 4 + 2].position, mul(W,V));
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

[maxvertexcount(6)]
void GS_Sprite(point VS_OUT input[1], inout TriangleStream<GS_OUTPUT> outputStream)
{
    GS_OUTPUT output[4] =
    {
        (GS_OUTPUT)0.f, (GS_OUTPUT)0.f, (GS_OUTPUT)0.f, (GS_OUTPUT)0.f
    };

    float3 trans, mscale;
    float4 q;
    decompose(W, trans, q, mscale);

    VS_OUT vtx = input[0];
    float2 scale = g_vec2_1 * mscale.x;

    output[0].position = vtx.viewPos + float4(-scale.x, scale.y, 0.f, 0.f);
    output[1].position = vtx.viewPos + float4(scale.x, scale.y, 0.f, 0.f);
    output[2].position = vtx.viewPos + float4(scale.x, -scale.y, 0.f, 0.f);
    output[3].position = vtx.viewPos + float4(-scale.x, -scale.y, 0.f, 0.f);

    output[0].viewPos = output[0].position.xyz;
    output[1].viewPos = output[1].position.xyz;
    output[2].viewPos = output[2].position.xyz;
    output[3].viewPos = output[3].position.xyz;

    // proj space
    for (int i = 0; i < 4; ++i)
        output[i].position = mul(output[i].position, P);

    float ratio = g_vec2_0.x / g_vec2_0.y;

    uint spriteCountX = g_int_0;
    uint spriteCountY = g_int_1;
    uint curFrame = 0;

    curFrame = uint(ratio * spriteCountX * spriteCountY);


    int indexX = curFrame % spriteCountX;
    int indexY = curFrame / spriteCountX;


    output[0].uv = float2(indexX * 1.f / spriteCountX, indexY * 1.f / spriteCountY);
    output[1].uv = float2((indexX + 1) * 1.f / spriteCountX, indexY * 1.f / spriteCountY);
    output[2].uv = float2((indexX + 1) * 1.f / spriteCountX, (indexY + 1) * 1.f / spriteCountY);
    output[3].uv = float2(indexX * 1.f / spriteCountX, (indexY + 1) * 1.f / spriteCountY);




    for (int j = 0; j < 4; ++j)
    {
        output[j].viewNormal = float3(0.f, 0.f, -1.f);
        output[j].viewTangent = float3(1.f, 0.f, 0.f);
    }

    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[2]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}

[maxvertexcount(6)]
void GS_Ui(point VS_OUT input[1], inout TriangleStream<UIOutput> outputStream)
{
    UIOutput output[4] =
    {
        (UIOutput)0.f, (UIOutput)0.f, (UIOutput)0.f, (UIOutput)0.f
    };

    float3 trans, mscale;
    float4 q;
    decompose(W, trans, q, mscale);

    VS_OUT vtx = input[0];
    float2 scale = mscale.xy * 0.5f;

    output[0].position = vtx.viewPos + float4(-scale.x, scale.y,  0.f, 0.f);
    output[1].position = vtx.viewPos + float4(scale.x,  scale.y,  0.f, 0.f);
    output[2].position = vtx.viewPos + float4(scale.x,  -scale.y, 0.f, 0.f);
    output[3].position = vtx.viewPos + float4(-scale.x, -scale.y, 0.f, 0.f);

    for (int i = 0; i < 4; ++i)
        output[i].position = mul(output[i].position, P);

    output[0].uv = float2(0.f, 0.f);
    output[1].uv = float2(1.f, 0.f);
    output[2].uv = float2(1.f, 1.f);
    output[3].uv = float2(0.f, 1.f);

    outputStream.Append(output[0]);
    outputStream.Append(output[1]);
    outputStream.Append(output[2]);
    outputStream.RestartStrip();

    outputStream.Append(output[0]);
    outputStream.Append(output[2]);
    outputStream.Append(output[3]);
    outputStream.RestartStrip();
}


float4 PS_UI(UIOutput input) : SV_TARGET
{
    float4 diffuseColor = g_vec4_0;
    if (bHasDiffuseMap)
    {
        diffuseColor = pow(abs(DiffuseMap.Sample(LinearSamplerMirror, input.uv)), GAMMA) * g_vec4_0;
        
    }

    return diffuseColor;
}

float4 PS_UI1(UIOutput input) : SV_TARGET
{
    float4 diffuseColor = g_vec4_0;
    if (bHasDiffuseMap)
    {
        diffuseColor = pow(abs(DiffuseMap.Sample(LinearSamplerMirror, input.uv)), GAMMA) * g_vec4_0;
        
    }

    if (g_float_0 < 100.f)
    {
        if (1.f - input.uv.y >= g_float_0 / 100.f)
            diffuseColor.xyz *= 0.2f;

        if (bHasTexturemap7)
        {
            float gauge_Color = TextureMap7.Sample(LinearSampler, input.uv + float2(0.f, 0.5f + g_float_0 / 100.f)).x;
            diffuseColor.xyz += gauge_Color;
        }
    }

    return diffuseColor;
}

float4 PS_UI2(UIOutput input) : SV_TARGET
{
    float4 diffuseColor = g_vec4_0;
    if (bHasDiffuseMap)
    {
        diffuseColor = pow(abs(DiffuseMap.Sample(LinearSamplerMirror, input.uv)), GAMMA) * g_vec4_0;
        
    }

    if (g_float_0 < 1.f)
    {
        float2 uvPos = float2(input.uv.x - 0.5f, 0.5f - input.uv.y);
        float theta = atan2(uvPos.x, uvPos.y);

        if (theta <= 0.f)
            theta += 2.f * PI;

        if (theta >= 2.f * PI * g_float_0)
        {
            diffuseColor.xyz *= 0.2f;
        }
    }

    else
    {
        diffuseColor.xyz = (pow(abs(DiffuseMap.Sample(LinearSamplerMirror, input.uv)), GAMMA) * g_vec4_0).xyz;
    }

    return diffuseColor;
}

float4 PS_UI3(UIOutput input) : SV_TARGET
{
    float4 diffuseColor = g_vec4_0;
    if (bHasDiffuseMap)
    {
        diffuseColor = pow(abs(DiffuseMap.Sample(LinearSamplerMirror, input.uv)), GAMMA) * g_vec4_0;
    }

    if (input.uv.x > g_float_0)
        discard;

    float2 newUV;
    newUV.x = input.uv.x - frac(g_float_1);
    newUV.y = input.uv.y;

    float falpha;
    if(bHasTexturemap7)
        falpha = TextureMap7.Sample(LinearSampler, newUV).a + 0.3f;
    
    diffuseColor.xyz *= falpha;

    return diffuseColor;
}

float4 PS_UI4(UIOutput input) : SV_TARGET
{
    float4 diffuseColor = g_vec4_0;
    if (bHasDiffuseMap)
    {
        diffuseColor = pow(abs(DiffuseMap.Sample(LinearSamplerMirror, input.uv)), GAMMA) * g_vec4_0;
        
    }

    if (input.uv.x >= g_float_0 / 100.f)
        diffuseColor = float4(0.3f, 0.3f, 0.3f, 1.f);

    return diffuseColor;
}

float4 PS_UI5(UIOutput input) : SV_TARGET
{
    float4 diffuseColor = g_vec4_0;
    diffuseColor = pow(abs(DiffuseMap.Sample(LinearSamplerMirror, input.uv)), GAMMA) * g_vec4_0;

    return diffuseColor;
}

float4 PS_UI6(UIOutput input) : SV_TARGET
{
    float4 diffuseColor = g_vec4_0;
    if (bHasDiffuseMap)
    {
        diffuseColor = pow(DiffuseMap.Sample(LinearSamplerMirror, input.uv), GAMMA) * g_vec4_0;

    }

    if (g_float_0 <= 1.f)
    {
        float2 uvPos = float2(input.uv.x - 0.5f, 0.5f - input.uv.y);
        float theta = atan2(uvPos.x, uvPos.y);

        if (theta <= 0.f)
            theta += 2.f * PI;

        if (theta >= 2.f * PI * g_float_0)
        {
            discard;
        }

        uvPos = float2(input.uv.x - 0.5f, 0.5f - input.uv.y);
        theta = atan2(uvPos.x, uvPos.y);

        if (theta <= 0.f)
            theta += 2.f * PI;

        if (theta < 2.f * PI * g_float_1)
        {
            discard;
        }
       
    }

    else
    {
        discard;
    }

    return diffuseColor;
}


float4 PS_UI7(UIOutput input) : SV_TARGET
{
    float4 diffuseColor = g_vec4_0;
    if (bHasDiffuseMap)
    {
        diffuseColor = pow(abs(DiffuseMap.Sample(LinearSamplerMirror, input.uv)), GAMMA) * g_vec4_0;

    }

    if (g_float_0 < 1.f)
    {
        float2 uvPos = float2(input.uv.x - 0.5f, 0.5f - input.uv.y);
        float theta = atan2(uvPos.x, uvPos.y);

        if (theta <= 0.f)
            theta += 2.f * PI;

        if (theta < 2.f * PI * g_float_0)
        {
            discard;
        }
    }

    else
    {
        diffuseColor.xyz = (pow(abs(DiffuseMap.Sample(LinearSamplerMirror, input.uv)), GAMMA) * g_vec4_0).xyz;
    }

    return diffuseColor;
}


float4 PS_UIInstancing(UIInstancingOutput input) : SV_TARGET
{

    float4 diffuseColor = InstanceRenderParams[input.id];
    if (bHasDiffuseMap)
    {
        diffuseColor = pow(abs(DiffuseMap.Sample(LinearSamplerMirror, input.uv)), GAMMA) * g_vec4_0;
        
    }

    if (bHasOpacityMap)
    {
        diffuseColor.a = pow(abs(OpacityMap.Sample(LinearSamplerMirror, input.uv)), GAMMA).x * g_vec4_0.w;
        if (diffuseColor.a <= 0.01f)
            discard;
    }

    return diffuseColor;
}


float4 PS_UIBAR(UIOutput input) : SV_TARGET
{
    float4 diffuseColor = pow(abs(DiffuseMap.Sample(LinearSamplerMirror, input.uv)), GAMMA);

    if (input.uv.x >= g_BarPercent / 100.f)
        diffuseColor = diffuseColor * float4(0.1, 0.1, 0.1, 0.1);

    return diffuseColor;
}

//g_float_0 dissolveTime

float4 PS_CustomEffect1(GS_OUTPUT input) : SV_Target
{
    float4 color = (float4) 0.f;
    float2 distortionUV = (float2)0.f;
    float3 viewNormal = (float3) 0.f;
    if (bHasDistortionMap != 0)
    {
        distortionUV = DistortionMap.Sample(LinearSampler, input.uv).xy * g_vec2_2;
    }
    LightColor totalLightColor = (LightColor)0.f;
    float4 diffuseColor = Material.diffuse;
    float4 emissiveColor = Material.emissive;
    float opacity = 1.f;
    if (bHasDiffuseMap)
        diffuseColor.xyz *= pow(abs(DiffuseMap.Sample(LinearSamplerMirror, input.uv)), GAMMA).xyz;

    //if(g_bHasNormalMap)
    //{
    //    viewNormal = NormalMap.Sample(LinearSampler, input.uv + distortionUV).xyz;

    //    for (uint i = 0; i < lightCount; ++i)
    //    {
    //        LightColor color = CalculateLightColor_ViewSpace(i, viewNormal, input.viewPos);
    //        totalLightColor.ambient += color.ambient;
    //        totalLightColor.diffuse += color.diffuse;
    //        totalLightColor.specular += color.specular;
    //        totalLightColor.emissive += color.emissive;

    //    }

    //}
    //else
    //    totalLightColor = (LightColor) 1.f;

    if (bHasOpacityMap)
    {
        opacity = OpacityMap.Sample(LinearSampler, input.uv + distortionUV).x;
    }

    if (bHasEmissiveMap)
    {
        emissiveColor.xyz *= EmissiveMap.Sample(LinearSampler, input.uv + distortionUV).xyz;
    }

    color.xyz = diffuseColor.xyz
    + emissiveColor.xyz;
    color.w = opacity * diffuseColor.a;

    if (bHasDissolveMap != 0)
    {
        float dissolveColor = DissolveMap.Sample(LinearSampler, input.uv + distortionUV).x;
        if (dissolveColor < g_float_0)
            discard;
    }


    return color;
}

float4 PS_FRAME(UIOutput input) : SV_TARGET
{
    return g_DrawColor;
}

float4 PS_Test(UIOutput input) : SV_TARGET
{
    float2 uvSliding = g_vec2_0;

    float4 color = (float4) 0.f;
    float2 distortionUV = (float2) 0.f;
    float3 viewNormal = (float3) 0.f;

    if (bHasSubmap0 != 0)
    {
        distortionUV = SubMap0.Sample(LinearSampler, input.uv + uvSliding).xy * g_vec2_1;
    }

    float4 diffuseColor = g_vec4_0;
    float4 emissiveColor = g_vec4_1;
    float opacity = 1.f;

    if (bHasDiffuseMap)
        diffuseColor.xyz *= DiffuseMap.Sample(LinearSampler, input.uv + distortionUV).xyz;

    if (bHasOpacityMap)
    {
        opacity = OpacityMap.Sample(LinearSampler, input.uv + distortionUV).x;
    }

    if (bHasEmissiveMap)
    {
        emissiveColor.xyz *= EmissiveMap.Sample(LinearSampler, input.uv + distortionUV).xyz;
    }

    color.xyz = diffuseColor.xyz
    + emissiveColor.xyz;
    color.w = opacity * diffuseColor.a;

    return color;
}

float4 PS_CustomEffect2(GS_OUTPUT input) : SV_TARGET
{
    float2 uvSliding = g_vec2_0;

    float4 color = (float4) 0.f;
    float2 distortionUV = (float2) 0.f;
    float3 viewNormal = (float3) 0.f;

    if (bHasSubmap0 != 0)
    {
        distortionUV = SubMap0.Sample(LinearSampler, input.uv + uvSliding).xy * g_vec2_2;
    }

    float4 diffuseColor = Material.diffuse;
    float4 emissiveColor = Material.emissive;
    float opacity = 1.f;

    if (bHasDiffuseMap)
        diffuseColor.xyz *= DiffuseMap.Sample(LinearSampler, input.uv + distortionUV).xyz;

    if (bHasOpacityMap)
    {
        opacity = OpacityMap.Sample(LinearSampler, input.uv + distortionUV).x;
    }

    if (bHasEmissiveMap)
    {
        emissiveColor.xyz *= EmissiveMap.Sample(LinearSampler, input.uv + distortionUV).xyz;
    }

    color.xyz = diffuseColor.xyz
    + emissiveColor.xyz;
    color.w = opacity * diffuseColor.a;

    return color;
}

float4 PS_CustomEffect3(GS_OUTPUT input) : SV_TARGET
{

    float4 color = (float4) 0.f;
    float distortion = 0.f;

    if (bHasDistortionMap != 0)
    {
        distortion = DistortionMap.Sample(LinearSampler, input.uv).x * g_vec2_2.x;



    }

    float2 centerUV = float2(0.5f, 0.5f);
    float2 dir = normalize(centerUV - input.uv);
    float dist = length(centerUV - input.uv);
    float f = exp(distortion * (dist - 0.5f)) - 1.f;
    if (dist > 0.5)
        f = 0.f;

    float2 newUV = input.uv + f * dir;

    float4 diffuseColor = Material.diffuse;
    float4 emissiveColor = Material.emissive;
    float opacity = 1.f;

    if (bHasDiffuseMap)
        diffuseColor.xyz *= DiffuseMap.Sample(LinearSampler, newUV).xyz;

    if (bHasOpacityMap)
    {
        opacity = OpacityMap.Sample(LinearSampler, newUV).x;
    }

    if (bHasEmissiveMap)
    {
        emissiveColor.xyz *= EmissiveMap.Sample(LinearSampler, newUV).xyz;
    }

     color.xyz = diffuseColor.xyz
                 + emissiveColor.xyz;
                 color.w = opacity * diffuseColor.a;

     return color;
}
float4 PS_CustomEffect4(GS_OUTPUT input) : SV_TARGET
{
    float4 diffuseColor = DiffuseMap.Sample(LinearSampler,input.uv).aaaa * Material.diffuse;


    return diffuseColor;
}

float4 PS_Test2(GS_OUTPUT input) : SV_TARGET
{
    float2 uvSliding = g_vec2_0;

    float4 color = (float4) 0.f;
    float2 distortionUV = (float2) 0.f;
    float3 viewNormal = (float3) 0.f;

    if (bHasSubmap0 != 0)
    {
        distortionUV = SubMap0.Sample(LinearSampler, input.uv + uvSliding).xy * g_vec2_1;
    }

    float4 diffuseColor = g_vec4_0;
    float4 emissiveColor = g_vec4_1;
    float opacity = 1.f;

    if (bHasDiffuseMap)
        diffuseColor.xyz *= DiffuseMap.Sample(LinearSampler, input.uv + distortionUV).xyz;

    if (bHasOpacityMap)
    {
        opacity = OpacityMap.Sample(LinearSampler, input.uv + distortionUV).x;
    }

    if (bHasEmissiveMap)
    {
        emissiveColor.xyz *= EmissiveMap.Sample(LinearSampler, input.uv + distortionUV).xyz;
    }

    color.xyz = diffuseColor.xyz
    + emissiveColor.xyz;
    color.w = opacity * diffuseColor.a;

    return color;
}
float4 PS_Terrain(GS_GRASS_OUTPUT input) : SV_TARGET
{   
    float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
    
    return color;
}

float4 PS_ViewPort(UIOutput input) : SV_Target
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.uv);
    
    if (color.w < 0.1f)
        discard;
    
    return color;
}


technique11 T0
{
//0
    pass UI_DEFAULT
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Default()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_UI()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Ui()));
    }
//1
    pass UI_CURTAIN_UP
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Default()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_UI1()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Ui()));
    }
//2
    pass UI_CLOCK
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Default()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_UI2()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Ui()));
    }
//3
    pass UI_HPBAR
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Default()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_UI3()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Ui()));
    }
//4
    pass UI_SLIDE_RIGHT
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Default()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_UI4()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Ui()));
    }
//5
    pass UI_LOADING
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Default()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_UI5()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Ui()));
    }
//6
    pass UI_SKILL_COOL_END
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Default()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_UI6()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Ui()));
    }
//7
    pass UI_TARGET_LOCKON
    {
        SetVertexShader(CompileShader(vs_5_0, VS_3D_To_2D()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_UI()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Ui()));
    }
//8
    pass UI_TARGET_HP
    {
        SetVertexShader(CompileShader(vs_5_0, VS_3D_To_2D()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_UI3()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Ui()));
    }
//9
    pass UI_SKILL_USE_GAUGE
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Default()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_UI7()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Ui()));
    }
//10
    PASS_VP(p1_instancing, VS_Instancing, PS_UI)
//11
    PASS_VP_BLEND(P2, VS_UI, PS_UIBAR)

//12
    pass p7
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Default()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_CustomEffect1()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Sprite()));

    }
//13
    pass p8
    {
        SetVertexShader(CompileShader(vs_5_0, VS_UI()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Test()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(NULL);

    }
//14
    pass p9
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Default()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_CustomEffect2()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Sprite()));

    }
//15
    pass p10
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Default()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_CustomEffect3()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Sprite()));

    }
//16
    pass p11
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Default()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_CustomEffect4()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Sprite()));

    }
//17
    pass p12
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Default()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Test2()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Main()));

    }
//18
// 지오메트리셰이더를 활용해서 만든 풀
    pass GeometryWeed
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Grass()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Terrain()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Grass()));
    }
//19
    pass UIModel
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Default()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_ViewPort()));
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(CompileShader(gs_5_0, GS_Ui()));
    }
};

technique11 T1
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_UI()));
        SetRasterizerState(FillModeWireFrame);
        SetDepthStencilState(DSS_LESS_NO_WRITE, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_FRAME()));
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetGeometryShader(NULL);

    }

};