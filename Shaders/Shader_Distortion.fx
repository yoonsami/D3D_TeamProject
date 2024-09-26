#include "Render.fx"
#include "Light.fx"
float g_BarPercent;



struct EffectOut
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION1;
    float3 viewPosition : POSITION2;
    float2 uv : TEXCOORD;
    float2 originUV : TEXCOORD1;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
};
struct EffectOutInstancing
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION1;
    float3 viewPosition : POSITION2;
    float2 uv : TEXCOORD;
    float2 originUV : TEXCOORD1;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    uint id : SV_InstanceID;
};

EffectOut VS_Main(VTXModel input)
{
    EffectOut output;
    output.position = mul(float4(input.position, 1.f), BoneTransform[BoneIndex]);
    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz;
    output.viewPosition = mul(float4(output.worldPosition, 1.f), V).xyz;
    output.position = mul(output.position, VP);
    
    output.uv = input.uv + uvSlidingAcc;
    output.originUV = input.uv;
    output.viewNormal = mul(input.normal, (float3x3) BoneTransform[BoneIndex]);
    output.viewNormal = mul(output.viewNormal, (float3x3) W);
    output.viewNormal = mul(output.viewNormal, (float3x3) V);
    
    output.viewTangent = mul(input.tangent, (float3x3) BoneTransform[BoneIndex]);
    output.viewTangent = mul(output.viewTangent, (float3x3) W);
    output.viewTangent = mul(output.viewTangent, (float3x3) V);

    return output;
}

EffectOutInstancing VS_Main_Instancing(VTXModelInstancing input)
{
    EffectOutInstancing output;
    output.position = mul(float4(input.position, 1.f), BoneTransform[BoneIndex]);
    output.position = mul(output.position, input.world);
    output.worldPosition = output.position.xyz;
    output.viewPosition = mul(float4(output.worldPosition, 1.f), V).xyz;
    output.position = mul(output.position, VP);
    
    output.uv = input.uv + uvSlidingAcc;
    output.originUV = input.uv;
    output.viewNormal = mul(input.normal, (float3x3) BoneTransform[BoneIndex]);
    output.viewNormal = mul(output.viewNormal, (float3x3) input.world);
    output.viewNormal = mul(output.viewNormal, (float3x3) V);
    
    output.viewTangent = mul(input.tangent, (float3x3) BoneTransform[BoneIndex]);
    output.viewTangent = mul(output.viewTangent, (float3x3) input.world);
    output.viewTangent = mul(output.viewTangent, (float3x3) V);
    
    output.id = input.instanceID;
    
    return output;
}

float4 PS_Wrap(EffectOut input) : SV_Target
{
    float4 vOutColor = float4(1.f, 1.f, 1.f, 1.f);
    
    bool bUseFadeOut = (bool) g_int_0;
    bool bUseRimLight = (bool) g_int_1;
    bool bUseSpriteAnim = (bool) g_int_2;
    bool bLightOn = (bool) g_int_3;
    
    float fLifeTimeRatio = g_float_0;
    float fDissolveWeight = g_float_1;
    float fRimLightIntensity = g_float_2;
    float fLightIntensity = g_float_3;
    
    float2 vColorOptions_Op[3] = { g_vec2_0, g_vec2_1, g_vec2_2 };
        
    float bUseTexColor_Op[3] = { g_mat_2._11, g_mat_2._12, g_mat_2._13 };
    int iFilpOPtion_Op[3] = { g_mat_2._21, g_mat_2._22, g_mat_2._23 };
    
    /* Calc Texcoord */
    float fDistortionWeight = 0.f;
    if (bHasDistortionMap)
    {
        float4 vDistortion = DistortionMap.Sample(LinearSampler, input.uv + g_vec2_3);
        fDistortionWeight = vDistortion.r * 0.5f;
    }

    float2 vTexcoord_Op[3] =
    {
        input.uv + float2(g_vec4_0.x, g_vec4_0.y) + fDistortionWeight,
        input.uv + float2(g_vec4_0.z, g_vec4_0.w) + fDistortionWeight,
        input.uv + float2(g_vec4_1.x, g_vec4_1.y) + fDistortionWeight
    };
    
    float2 vTexcoord_Blend = input.uv + float2(g_vec4_1.z, g_vec4_1.w) + fDistortionWeight;
    float2 vTexcoord_Overlay = input.uv + float2(g_vec4_2.x, g_vec4_2.y);
    float2 vTexcoord_Dissolve = input.uv + float2(g_vec4_2.z, g_vec4_2.w);
    if (bUseSpriteAnim)
    {
        float2 vSpriteAnim_RangeX = float2(g_vec4_3.x, g_vec4_3.y);
        float2 vSpriteAnim_RangeY = float2(g_vec4_3.z, g_vec4_3.w);
        vTexcoord_Op[0] = float2(vSpriteAnim_RangeX.x + ((vSpriteAnim_RangeX.y - vSpriteAnim_RangeX.x) * input.uv.x),
                              vSpriteAnim_RangeY.x + ((vSpriteAnim_RangeY.y - vSpriteAnim_RangeY.x) * input.uv.y)) + fDistortionWeight;
    }
    
    // For. Flip option 
    for (int i = 0; i < 3; i++)
    {
        if (iFilpOPtion_Op[i] == 1) // flip up/down
            vTexcoord_Op[i].y = 1.f - vTexcoord_Op[i].y;
        else if (iFilpOPtion_Op[i] == 2) // flip left/right 
            vTexcoord_Op[i].x = 1.f - vTexcoord_Op[i].x;
        else if (iFilpOPtion_Op[i] == 3) // flip up/down and left/right 
        {
            vTexcoord_Op[i].x = 1.f - vTexcoord_Op[i].x;
            vTexcoord_Op[i].y = 1.f - vTexcoord_Op[i].y;
        }
        else if (iFilpOPtion_Op[i] == 4)
        {
            float2 vTemp = vTexcoord_Op[i];
            vTexcoord_Op[i].x = 1.f - vTemp.y;
            vTexcoord_Op[i].y = vTemp.x;
        }
        else if (iFilpOPtion_Op[i] == 5)
        {
            float2 vTemp = vTexcoord_Op[i];
            vTexcoord_Op[i].x = vTemp.y;
            vTexcoord_Op[i].y = 1 - vTemp.x;
        }
    }
  
    /* Sampled from textures */    
    float4 vSample_Op1 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Op2 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Op3 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Blend = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Overlay = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Dissolve = { 0.f, 0.f, 0.f, 0.f };
   
    if (bHasTexturemap7)
    {
        vSample_Op1 = TextureMap7.Sample(LinearSampler, vTexcoord_Op[0]);
        vSample_Op1.rgb = pow(vSample_Op1.rgb, GAMMA);
    }
    
    if (bHasTexturemap10)
        vSample_Blend = TextureMap10.Sample(LinearSampler, vTexcoord_Blend);
    if (bHasTexturemap11)
        vSample_Overlay = TextureMap11.Sample(LinearSampler, vTexcoord_Overlay);
    if (bHasDissolveMap)
        vSample_Dissolve = DissolveMap.Sample(LinearSampler, vTexcoord_Dissolve);
    
    /* Mix four option textures */
    vOutColor = vSample_Op1;
    
    /* Gamma collection */
    if (bUseTexColor_Op[0] || bUseTexColor_Op[1] || bUseTexColor_Op[2])
        vOutColor.rgb = pow(vOutColor.rgb, 1.f / GAMMA);
    
    /* Blend */
    if (bHasTexturemap10)
        vOutColor.a *= vSample_Blend.r;
    
    /* DissolveMap */
    if (bHasDissolveMap)
    {
        float fDissolve = vSample_Dissolve.r;
        if (fDissolve < sin(fDissolveWeight))//sin(fLifeTimeRatio))
            vOutColor.a = 0.f;
    }
    
    /* Fade Out */
    if (bUseFadeOut)
    {
        vOutColor.r *= (1.f - fLifeTimeRatio);
        
    }
    
    if (vOutColor.a < 0.1f)
        discard;
    
    vOutColor.r *= g_vec2_0.x;
    
    return vOutColor;
}

float4 PS_Clamp(EffectOut input) : SV_Target
{
    float4 vOutColor = float4(1.f, 1.f, 1.f, 1.f);
    
    bool bUseFadeOut = (bool) g_int_0;
    bool bUseRimLight = (bool) g_int_1;
    bool bUseSpriteAnim = (bool) g_int_2;
    bool bLightOn = (bool) g_int_3;
    
    float fLifeTimeRatio = g_float_0;
    float fDissolveWeight = g_float_1;
    float fRimLightIntensity = g_float_2;
    float fLightIntensity = g_float_3;
    
    float2 vColorOptions_Op[3] = { g_vec2_0, g_vec2_1, g_vec2_2 };
        
    float bUseTexColor_Op[3] = { g_mat_2._11, g_mat_2._12, g_mat_2._13 };
    int iFilpOPtion_Op[3] = { g_mat_2._21, g_mat_2._22, g_mat_2._23 };
    
    /* Calc Texcoord */
    float fDistortionWeight = 0.f;
    if (bHasDistortionMap)
    {
        float4 vDistortion = DistortionMap.Sample(LinearSamplerClamp, input.uv + g_vec2_3);
        fDistortionWeight = vDistortion.r * 0.5f;
    }

    float2 vTexcoord_Op[3] =
    {
        input.uv + float2(g_vec4_0.x, g_vec4_0.y) + fDistortionWeight,
        input.uv + float2(g_vec4_0.z, g_vec4_0.w) + fDistortionWeight,
        input.uv + float2(g_vec4_1.x, g_vec4_1.y) + fDistortionWeight
    };
    
    float2 vTexcoord_Blend = input.uv + float2(g_vec4_1.z, g_vec4_1.w) + fDistortionWeight;
    float2 vTexcoord_Overlay = input.uv + float2(g_vec4_2.x, g_vec4_2.y);
    float2 vTexcoord_Dissolve = input.uv + float2(g_vec4_2.z, g_vec4_2.w);
    if (bUseSpriteAnim)
    {
        float2 vSpriteAnim_RangeX = float2(g_vec4_3.x, g_vec4_3.y);
        float2 vSpriteAnim_RangeY = float2(g_vec4_3.z, g_vec4_3.w);
        vTexcoord_Op[0] = float2(vSpriteAnim_RangeX.x + ((vSpriteAnim_RangeX.y - vSpriteAnim_RangeX.x) * input.uv.x),
                              vSpriteAnim_RangeY.x + ((vSpriteAnim_RangeY.y - vSpriteAnim_RangeY.x) * input.uv.y)) + fDistortionWeight;
    }
    
    // For. Flip option 
    for (int i = 0; i < 3; i++)
    {
        if (iFilpOPtion_Op[i] == 1) // flip up/down
            vTexcoord_Op[i].y = 1.f - vTexcoord_Op[i].y;
        else if (iFilpOPtion_Op[i] == 2) // flip left/right 
            vTexcoord_Op[i].x = 1.f - vTexcoord_Op[i].x;
        else if (iFilpOPtion_Op[i] == 3) // flip up/down and left/right 
        {
            vTexcoord_Op[i].x = 1.f - vTexcoord_Op[i].x;
            vTexcoord_Op[i].y = 1.f - vTexcoord_Op[i].y;
        }
        else if (iFilpOPtion_Op[i] == 4)
        {
            float2 vTemp = vTexcoord_Op[i];
            vTexcoord_Op[i].x = 1.f - vTemp.y;
            vTexcoord_Op[i].y = vTemp.x;
        }
        else if (iFilpOPtion_Op[i] == 5)
        {
            float2 vTemp = vTexcoord_Op[i];
            vTexcoord_Op[i].x = vTemp.y;
            vTexcoord_Op[i].y = 1 - vTemp.x;
        }
    }
  
    /* Sampled from textures */    
    float4 vSample_Op1 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Op2 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Op3 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Blend = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Overlay = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Dissolve = { 0.f, 0.f, 0.f, 0.f };
   
    if (bHasTexturemap7)
    {
        vSample_Op1 = TextureMap7.Sample(LinearSamplerClamp, vTexcoord_Op[0]);
        vSample_Op1.rgb = pow(vSample_Op1.rgb, GAMMA);
    }
    
    if (bHasTexturemap10)
        vSample_Blend = TextureMap10.Sample(LinearSamplerClamp, vTexcoord_Blend);
    if (bHasTexturemap11)
        vSample_Overlay = TextureMap11.Sample(LinearSamplerClamp, vTexcoord_Overlay);
    if (bHasDissolveMap)
        vSample_Dissolve = DissolveMap.Sample(LinearSamplerClamp, vTexcoord_Dissolve);
    
    /* Mix four option textures */
    vOutColor = vSample_Op1;

    
    /* Gamma collection */
    if (bUseTexColor_Op[0] || bUseTexColor_Op[1] || bUseTexColor_Op[2])
        vOutColor.rgb = pow(vOutColor.rgb, 1.f / GAMMA);
    
    /* Blend */
    if (bHasTexturemap10)
        vOutColor.a *= vSample_Blend.r;
    
    /* DissolveMap */
    if (bHasDissolveMap)
    {
        float fDissolve = vSample_Dissolve.r;
        if (fDissolve < sin(fDissolveWeight))//sin(fLifeTimeRatio))
            vOutColor.a = 0.f;
    }
    
    if (bUseFadeOut)
    {
        vOutColor.r *= (1.f - fLifeTimeRatio);
        
    }
    
    if (vOutColor.a < 0.1f)
        discard;
    
    vOutColor.r *= g_vec2_0.x;
    
    return vOutColor;
}

float4 PS_Mirror(EffectOut input) : SV_Target
{
    float4 vOutColor = float4(0.f, 0.f, 0.f, 0.f);
    // TODO
    return vOutColor;
}

float4 PS_Border(EffectOut input) : SV_Target
{
    float4 vOutColor = float4(0.f, 0.f, 0.f, 0.f);
    // TODO
    return vOutColor;
}

StructuredBuffer<RenderParam> g_effectData;

float4 PS_Wrap_Instancing(EffectOutInstancing input) : SV_Target
{
    float4 vOutColor = float4(1.f, 1.f, 1.f, 1.f);
    int id = input.id;
    bool bUseFadeOut = (bool) g_effectData[id].g_int_0;
    bool bUseRimLight = (bool) g_effectData[id].g_int_1;
    bool bUseSpriteAnim = (bool) g_effectData[id].g_int_2;
    bool bLightOn = (bool) g_effectData[id].g_int_3;
    
    float fLifeTimeRatio = g_effectData[id].g_float_0;
    float fDissolveWeight = g_effectData[id].g_float_1;
    float fRimLightIntensity = g_effectData[id].g_float_2;
    float fLightIntensity = g_effectData[id].g_float_3;
    
    float2 vColorOptions_Op[3] = { g_effectData[id].g_vec2_0, g_effectData[id].g_vec2_1, g_effectData[id].g_vec2_2 };
        
    float bUseTexColor_Op[3] = { g_effectData[id].g_mat_2._11, g_effectData[id].g_mat_2._12, g_effectData[id].g_mat_2._13 };
    int iFilpOPtion_Op[3] = { g_effectData[id].g_mat_2._21, g_effectData[id].g_mat_2._22, g_effectData[id].g_mat_2._23 };
    
    /* Calc Texcoord */
    float fDistortionWeight = 0.f;
    if (bHasDistortionMap)
    {
        float4 vDistortion = DistortionMap.Sample(LinearSampler, input.uv + g_effectData[id].g_vec2_3);
        fDistortionWeight = vDistortion.r * 0.5f;
    }

    float2 vTexcoord_Op[3] =
    {
        input.uv + float2(g_effectData[id].g_vec4_0.x, g_effectData[id].g_vec4_0.y) + fDistortionWeight,
        input.uv + float2(g_effectData[id].g_vec4_0.z, g_effectData[id].g_vec4_0.w) + fDistortionWeight,
        input.uv + float2(g_effectData[id].g_vec4_1.x, g_effectData[id].g_vec4_1.y) + fDistortionWeight
    };
    
    float2 vTexcoord_Blend = input.uv + float2(g_effectData[id].g_vec4_1.z, g_effectData[id].g_vec4_1.w) + fDistortionWeight;
    float2 vTexcoord_Overlay = input.uv + float2(g_effectData[id].g_vec4_2.x, g_effectData[id].g_vec4_2.y);
    float2 vTexcoord_Dissolve = input.uv + float2(g_effectData[id].g_vec4_2.z, g_effectData[id].g_vec4_2.w);
    if (bUseSpriteAnim)
    {
        float2 vSpriteAnim_RangeX = float2(g_effectData[id].g_vec4_3.x, g_effectData[id].g_vec4_3.y);
        float2 vSpriteAnim_RangeY = float2(g_effectData[id].g_vec4_3.z, g_effectData[id].g_vec4_3.w);
        vTexcoord_Op[0] = float2(vSpriteAnim_RangeX.x + ((vSpriteAnim_RangeX.y - vSpriteAnim_RangeX.x) * input.uv.x),
                              vSpriteAnim_RangeY.x + ((vSpriteAnim_RangeY.y - vSpriteAnim_RangeY.x) * input.uv.y)) + fDistortionWeight;
    }
    
    // For. Flip option 
    for (int i = 0; i < 3; i++)
    {
        if (iFilpOPtion_Op[i] == 1) // flip up/down
            vTexcoord_Op[i].y = 1.f - vTexcoord_Op[i].y;
        else if (iFilpOPtion_Op[i] == 2) // flip left/right 
            vTexcoord_Op[i].x = 1.f - vTexcoord_Op[i].x;
        else if (iFilpOPtion_Op[i] == 3) // flip up/down and left/right 
        {
            vTexcoord_Op[i].x = 1.f - vTexcoord_Op[i].x;
            vTexcoord_Op[i].y = 1.f - vTexcoord_Op[i].y;
        }
        else if (iFilpOPtion_Op[i] == 4)
        {
            float2 vTemp = vTexcoord_Op[i];
            vTexcoord_Op[i].x = 1.f - vTemp.y;
            vTexcoord_Op[i].y = vTemp.x;
        }
        else if (iFilpOPtion_Op[i] == 5)
        {
            float2 vTemp = vTexcoord_Op[i];
            vTexcoord_Op[i].x = vTemp.y;
            vTexcoord_Op[i].y = 1 - vTemp.x;
        }
    }
  
    /* Sampled from textures */    
    float4 vSample_Op1 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Op2 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Op3 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Blend = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Overlay = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Dissolve = { 0.f, 0.f, 0.f, 0.f };
   
    if (bHasTexturemap7)
    {
        vSample_Op1 = TextureMap7.Sample(LinearSampler, vTexcoord_Op[0]);
        vSample_Op1.rgb = pow(vSample_Op1.rgb, GAMMA);
    }
    
    if (bHasTexturemap10)
        vSample_Blend = TextureMap10.Sample(LinearSampler, vTexcoord_Blend);
    if (bHasTexturemap11)
        vSample_Overlay = TextureMap11.Sample(LinearSampler, vTexcoord_Overlay);
    if (bHasDissolveMap)
        vSample_Dissolve = DissolveMap.Sample(LinearSampler, vTexcoord_Dissolve);
    
    /* Mix four option textures */
    vOutColor = vSample_Op1;
    
    /* Gamma collection */
    if (bUseTexColor_Op[0] || bUseTexColor_Op[1] || bUseTexColor_Op[2])
        vOutColor.rgb = pow(vOutColor.rgb, 1.f / GAMMA);
    
    /* Blend */
    if (bHasTexturemap10)
        vOutColor.a *= vSample_Blend.r;
    
    /* DissolveMap */
    if (bHasDissolveMap)
    {
        float fDissolve = vSample_Dissolve.r;
        if (fDissolve < sin(fDissolveWeight))//sin(fLifeTimeRatio))
            vOutColor.a = 0.f;
    }
    
    /* Fade Out */
    if (bUseFadeOut)
    {
        vOutColor.r *= (1.f - fLifeTimeRatio);
        
    }
    
    if (vOutColor.a < 0.1f)
        discard;
    
    vOutColor.r *= g_effectData[id].g_vec2_0.x;
    
    return vOutColor;
}

float4 PS_Clamp_Instancing(EffectOutInstancing input) : SV_Target
{
    float4 vOutColor = float4(1.f, 1.f, 1.f, 1.f);
    int id = input.id;
    bool bUseFadeOut = (bool) g_effectData[id].g_int_0;
    bool bUseRimLight = (bool) g_effectData[id].g_int_1;
    bool bUseSpriteAnim = (bool) g_effectData[id].g_int_2;
    bool bLightOn = (bool) g_effectData[id].g_int_3;
    
    float fLifeTimeRatio = g_effectData[id].g_float_0;
    float fDissolveWeight = g_effectData[id].g_float_1;
    float fRimLightIntensity = g_effectData[id].g_float_2;
    float fLightIntensity = g_effectData[id].g_float_3;
    
    float2 vColorOptions_Op[3] = { g_effectData[id].g_vec2_0, g_effectData[id].g_vec2_1, g_effectData[id].g_vec2_2 };
        
    float bUseTexColor_Op[3] = { g_effectData[id].g_mat_2._11, g_effectData[id].g_mat_2._12, g_effectData[id].g_mat_2._13 };
    int iFilpOPtion_Op[3] = { g_effectData[id].g_mat_2._21, g_effectData[id].g_mat_2._22, g_effectData[id].g_mat_2._23 };
    
    /* Calc Texcoord */
    float fDistortionWeight = 0.f;
    if (bHasDistortionMap)
    {
        float4 vDistortion = DistortionMap.Sample(LinearSamplerClamp, input.uv + g_effectData[id].g_vec2_3);
        fDistortionWeight = vDistortion.r * 0.5f;
    }

    float2 vTexcoord_Op[3] =
    {
        input.uv + float2(g_effectData[id].g_vec4_0.x, g_effectData[id].g_vec4_0.y) + fDistortionWeight,
        input.uv + float2(g_effectData[id].g_vec4_0.z, g_effectData[id].g_vec4_0.w) + fDistortionWeight,
        input.uv + float2(g_effectData[id].g_vec4_1.x, g_effectData[id].g_vec4_1.y) + fDistortionWeight
    };
    
    float2 vTexcoord_Blend = input.uv + float2(g_effectData[id].g_vec4_1.z, g_effectData[id].g_vec4_1.w) + fDistortionWeight;
    float2 vTexcoord_Overlay = input.uv + float2(g_effectData[id].g_vec4_2.x, g_effectData[id].g_vec4_2.y);
    float2 vTexcoord_Dissolve = input.uv + float2(g_effectData[id].g_vec4_2.z, g_effectData[id].g_vec4_2.w);
    if (bUseSpriteAnim)
    {
        float2 vSpriteAnim_RangeX = float2(g_effectData[id].g_vec4_3.x, g_effectData[id].g_vec4_3.y);
        float2 vSpriteAnim_RangeY = float2(g_effectData[id].g_vec4_3.z, g_effectData[id].g_vec4_3.w);
        vTexcoord_Op[0] = float2(vSpriteAnim_RangeX.x + ((vSpriteAnim_RangeX.y - vSpriteAnim_RangeX.x) * input.uv.x),
                              vSpriteAnim_RangeY.x + ((vSpriteAnim_RangeY.y - vSpriteAnim_RangeY.x) * input.uv.y)) + fDistortionWeight;
    }
    
    // For. Flip option 
    for (int i = 0; i < 3; i++)
    {
        if (iFilpOPtion_Op[i] == 1) // flip up/down
            vTexcoord_Op[i].y = 1.f - vTexcoord_Op[i].y;
        else if (iFilpOPtion_Op[i] == 2) // flip left/right 
            vTexcoord_Op[i].x = 1.f - vTexcoord_Op[i].x;
        else if (iFilpOPtion_Op[i] == 3) // flip up/down and left/right 
        {
            vTexcoord_Op[i].x = 1.f - vTexcoord_Op[i].x;
            vTexcoord_Op[i].y = 1.f - vTexcoord_Op[i].y;
        }
        else if (iFilpOPtion_Op[i] == 4)
        {
            float2 vTemp = vTexcoord_Op[i];
            vTexcoord_Op[i].x = 1.f - vTemp.y;
            vTexcoord_Op[i].y = vTemp.x;
        }
        else if (iFilpOPtion_Op[i] == 5)
        {
            float2 vTemp = vTexcoord_Op[i];
            vTexcoord_Op[i].x = vTemp.y;
            vTexcoord_Op[i].y = 1 - vTemp.x;
        }
    }
  
    /* Sampled from textures */    
    float4 vSample_Op1 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Op2 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Op3 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Blend = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Overlay = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Dissolve = { 0.f, 0.f, 0.f, 0.f };
   
    if (bHasTexturemap7)
    {
        vSample_Op1 = TextureMap7.Sample(LinearSamplerClamp, vTexcoord_Op[0]);
        vSample_Op1.rgb = pow(vSample_Op1.rgb, GAMMA);

        float luminance = dot(vSample_Op1.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op1.rgb = lerp(vSample_Op1.rgb, vSample_Op1.rgb * vColorOptions_Op[0].x, saturate(luminance));
        vSample_Op1.a = saturate(vSample_Op1.a * vColorOptions_Op[0].y);
    }
    
    if (bHasTexturemap10)
        vSample_Blend = TextureMap10.Sample(LinearSamplerClamp, vTexcoord_Blend);
    if (bHasTexturemap11)
        vSample_Overlay = TextureMap11.Sample(LinearSamplerClamp, vTexcoord_Overlay);
    if (bHasDissolveMap)
        vSample_Dissolve = DissolveMap.Sample(LinearSamplerClamp, vTexcoord_Dissolve);
    
    /* Mix four option textures */
    vOutColor = vSample_Op1;
    vOutColor = lerp(vOutColor, vSample_Op2, vSample_Op2.a);
    vOutColor = lerp(vOutColor, vSample_Op3, vSample_Op3.a);
    
    /* Gamma collection */
    if (bUseTexColor_Op[0] || bUseTexColor_Op[1] || bUseTexColor_Op[2])
        vOutColor.rgb = pow(vOutColor.rgb, 1.f / GAMMA);
    
    /* Blend */
    if (bHasTexturemap10)
        vOutColor.a *= vSample_Blend.r;
    
    /* DissolveMap */
    if (bHasDissolveMap)
    {
        float fDissolve = vSample_Dissolve.r;
        if (fDissolve < sin(fDissolveWeight))//sin(fLifeTimeRatio))
            vOutColor.a = 0.f;
    }
    
    /* Fade Out */
    if (bUseFadeOut)
    {
        vOutColor.r *= (1.f - fLifeTimeRatio);
        
    }
    
    if (vOutColor.a < 0.1f)
        discard;
    
    vOutColor.r *= g_effectData[id].g_vec2_0.x;
    
    return vOutColor;
}


technique11 T0
{
    pass pass_Wrap // 0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Wrap()));
        SetBlendState(BlendDefault, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass pass_Clamp // 1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Clamp()));
        SetBlendState(BlendDefault, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass pass_Mirror // 2
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Mirror()));
        SetBlendState(BlendDefault, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass pass_Border // 3
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Border()));
        SetBlendState(BlendDefault, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

};
technique11 T1_Instancing // 0
{
    pass pass_Wrap_Instancing // 0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main_Instancing()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Wrap_Instancing()));
        SetBlendState(BlendDefault, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass pass_Clamp_Instancing // 1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main_Instancing()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Wrap_Instancing()));
        SetBlendState(BlendDefault, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

};