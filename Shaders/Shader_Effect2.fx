#include "Render.fx"
#include "Light.fx"

Texture2D PositionTargetTex;
Texture2D originPositionTargetTex;
Texture2D NormalTargetTex;
float4x4 InvWorldTransformMatrix;

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
    
    /* Get Shared Data */
    bool bUseFadeOut = (bool) g_int_0;
    bool bUseRimLight = (bool) g_int_1;
    bool bUseSpriteAnim = (bool) g_int_2;
    bool bLightOn = (bool) g_int_3;
    bool bUseSSD = (bool) g_mat_2._31;
    
    float fLifeTimeRatio = g_float_0;
    float fDissolveWeight = g_float_1;
    float fRimLightIntensity = g_float_2;
    float fLightIntensity = g_float_3;
    
    float2 vColorOptions_Op[3] = { g_vec2_0, g_vec2_1, g_vec2_2 };
    
    float fAlphaOffset_Blend = g_mat_2._24;
    
    float4 vBaseColor1_Op1 = g_mat_0._11_12_13_14;
    float4 vBaseColor2_Op1 = g_mat_0._21_22_23_24;
    float4 vBaseColor1_Op2 = g_mat_0._31_32_33_34;
    float4 vBaseColor2_Op2 = g_mat_0._41_42_43_44;
    float4 vBaseColor1_Op3 = g_mat_1._11_12_13_14;
    float4 vBaseColor2_Op3 = g_mat_1._21_22_23_24;
    float4 vBaseColor_Overlay = g_mat_1._31_32_33_34;
    float4 vBaseColor_RimLight = g_mat_1._41_42_43_44;
    
    float bUseTexColor_Op[3] = { g_mat_2._11, g_mat_2._12, g_mat_2._13 };
    int iFilpOPtion_Op[3] = { g_mat_2._21, g_mat_2._22, g_mat_2._23 };
    
    /* Calc Texcoord */
    float2 decalUV = input.uv;
    float decalAlpha = 1.f;
    if (bUseSSD)
    {
        float4 projPos = mul(float4(input.viewPosition, 1.f), P);
        float2 vPixelPosInSS = projPos.xy / projPos.w;
        vPixelPosInSS = (vPixelPosInSS * 0.5f) + 0.5f;
        vPixelPosInSS.y = 1.f - vPixelPosInSS.y;
        float3 fPixelViewPos = PositionTargetTex.Sample(LinearSamplerClamp, vPixelPosInSS).rgb;
        float3 fPixelWorldPos = mul(float4(fPixelViewPos, 1.f), VInv);
        float3 vPixelOriginViewPos = originPositionTargetTex.Sample(LinearSamplerClamp, vPixelPosInSS).rgb;
        if (vPixelOriginViewPos.z < fPixelViewPos.z)
            discard;
        
        float3 decalLocalPos = mul(float4(fPixelWorldPos, 1.f), InvWorldTransformMatrix);
        clip(0.5f - abs(decalLocalPos.xyz));
        
        
        decalUV = decalLocalPos.xz + 0.5f;
        
        float3 vSSDNormal = NormalTargetTex.Sample(LinearSamplerClamp, vPixelPosInSS).xyz;
        float3 viewUp = mul(float3(0.f, 1.f, 0.f), (float3x3) V);
        
        decalAlpha = dot(normalize(vSSDNormal), viewUp);
        
    }
    
    float fDistortionWeight = 0.f;
    if (bHasDistortionMap)
    {
        float4 vDistortion = DistortionMap.Sample(LinearSampler, decalUV + g_vec2_3);
        fDistortionWeight = vDistortion.r * 0.5f;
    }

    float2 vTexcoord_Op[3] =
    {
        decalUV + float2(g_vec4_0.x, g_vec4_0.y) + fDistortionWeight,
        decalUV + float2(g_vec4_0.z, g_vec4_0.w) + fDistortionWeight,
        decalUV + float2(g_vec4_1.x, g_vec4_1.y) + fDistortionWeight
    };
    
    float2 vTexcoord_Blend = decalUV + float2(g_vec4_1.z, g_vec4_1.w) + fDistortionWeight;
    float2 vTexcoord_Overlay = decalUV + float2(g_vec4_2.x, g_vec4_2.y);
    float2 vTexcoord_Dissolve = decalUV + float2(g_vec4_2.z, g_vec4_2.w);
    if (bUseSpriteAnim)
    {
        float2 vSpriteAnim_RangeX = float2(g_vec4_3.x, g_vec4_3.y);
        float2 vSpriteAnim_RangeY = float2(g_vec4_3.z, g_vec4_3.w);
        vTexcoord_Op[0] = float2(vSpriteAnim_RangeX.x + ((vSpriteAnim_RangeX.y - vSpriteAnim_RangeX.x) * decalUV.x),
                              vSpriteAnim_RangeY.x + ((vSpriteAnim_RangeY.y - vSpriteAnim_RangeY.x) * decalUV.y)) + fDistortionWeight;
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
        if (0.f == bUseTexColor_Op[0])
        {
            vSample_Op1.a = TextureMap7.Sample(LinearSampler, vTexcoord_Op[0]).r;
            vSample_Op1.rgb = lerp(vBaseColor2_Op1, vBaseColor1_Op1, vSample_Op1.a);
        }
        else
        {
            vSample_Op1 = TextureMap7.Sample(LinearSampler, vTexcoord_Op[0]);
            vSample_Op1.rgb = pow(vSample_Op1.rgb, GAMMA);
        }
        
        float luminance = dot(vSample_Op1.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op1.rgb = lerp(vSample_Op1.rgb, vSample_Op1.rgb * vColorOptions_Op[0].x, saturate(luminance));
        vSample_Op1.a = saturate(vSample_Op1.a * vColorOptions_Op[0].y);
    }
    if (bHasTexturemap8)
    {
        if (0.f == bUseTexColor_Op[1])
        {
            vSample_Op2.a = TextureMap8.Sample(LinearSampler, vTexcoord_Op[1]).r;
            vSample_Op2.rgb = lerp(vBaseColor2_Op2, vBaseColor1_Op2, vSample_Op2.a);
        }
        else
        {
            vSample_Op2 = TextureMap8.Sample(LinearSampler, vTexcoord_Op[1]);
            vSample_Op2.rgb = pow(vSample_Op2.rgb, GAMMA);
        }
        
        float luminance = dot(vSample_Op2.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op2.rgb = lerp(vSample_Op2.rgb, vSample_Op2.rgb * vColorOptions_Op[1].x, saturate(luminance));
        vSample_Op2.a = saturate(vSample_Op2.a * vColorOptions_Op[1].y);
    }
    if (bHasTexturemap9)
    {
        if (0.f == bUseTexColor_Op[2])
        {
            vSample_Op3.a = TextureMap9.Sample(LinearSampler, vTexcoord_Op[2]).r;
            vSample_Op3.rgb = lerp(vBaseColor2_Op3, vBaseColor1_Op3, vSample_Op3.a);
        }
        else
        {
            vSample_Op3 = TextureMap9.Sample(LinearSampler, vTexcoord_Op[2]);
            vSample_Op3.rgb = pow(vSample_Op3.rgb, GAMMA);
        }

        float luminance = dot(vSample_Op3.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op3.rgb = lerp(vSample_Op3.rgb, vSample_Op3.rgb * vColorOptions_Op[2].x, saturate(luminance));
        vSample_Op3.a = saturate(vSample_Op3.a * vColorOptions_Op[2].y);
    }
    if (bHasTexturemap10)
    {
        vSample_Blend = TextureMap10.Sample(LinearSampler, vTexcoord_Blend);
        vSample_Blend.r = saturate(vSample_Blend.r *fAlphaOffset_Blend);
    }
    if (bHasTexturemap11)
        vSample_Overlay = TextureMap11.Sample(LinearSampler, vTexcoord_Overlay);
    if (bHasDissolveMap)                                                                
        vSample_Dissolve = DissolveMap.Sample(LinearSampler, vTexcoord_Dissolve);
    
    /* Mix four option textures */
    vOutColor = vSample_Op1;
    vOutColor = lerp(vOutColor, vSample_Op2, vSample_Op2.a);
    vOutColor = lerp(vOutColor, vSample_Op3, vSample_Op3.a);
    
    /* Gamma collection */
    if (bUseTexColor_Op[0] || bUseTexColor_Op[1] || bUseTexColor_Op[2])
        vOutColor.rgb = pow(vOutColor.rgb, 1.f / GAMMA);

    /* Normal Mapping */
    if(bHasNormalMap)
        ComputeNormalMapping_ViewSpace(input.viewNormal,input.viewTangent,vTexcoord_Op[0]);
    
     /* Lighting */
    if (bLightOn)
    {
        float diffuseRatio = 0.f;
        {
            float3 viewLightDir = 0.f;
            
            viewLightDir = normalize(mul(float4(lights[0].vDirection.xyz, 0.f), V).xyz);
            diffuseRatio = saturate(dot(-viewLightDir, normalize(input.viewNormal)));
                    
        }
        float3 vLightingColor = vOutColor.rgb * diffuseRatio;
        vOutColor.rgb = lerp(vOutColor.rgb, vLightingColor, fLightIntensity);
    }
    
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
    
    /* Overlay */
    if (bHasTexturemap11)
    {
        float fOverlayIntensity = vSample_Overlay.r;
        float4 vFinalOverlayColor = float4(0.f, 0.f, 0.f, 0.f);

        vFinalOverlayColor.r = (vOutColor.r <= 0.5) ? 2 * vOutColor.r * vSample_Overlay.r : 1 - 2 * (1 - vOutColor.r) * (1 - vSample_Overlay.r);
        vFinalOverlayColor.g = (vOutColor.g <= 0.5) ? 2 * vOutColor.g * vSample_Overlay.g : 1 - 2 * (1 - vOutColor.g) * (1 - vSample_Overlay.g);
        vFinalOverlayColor.b = (vOutColor.b <= 0.5) ? 2 * vOutColor.b * vSample_Overlay.b : 1 - 2 * (1 - vOutColor.b) * (1 - vSample_Overlay.b);
        
        vOutColor.rgb = lerp(vOutColor.rgb, vFinalOverlayColor.rgb, fOverlayIntensity);
    }
    
    /* Rim Light */
        // TODO 
    
    /* Fade Out */
    if (bUseFadeOut)
        vOutColor.a *= (1.f - fLifeTimeRatio);
    
    if (vOutColor.a < 0.07f)
        discard;
    
     vOutColor.a *= decalAlpha;
    
    return vOutColor;
}

float4 PS_Clamp(EffectOut input) : SV_Target
{
    float4 vOutColor = float4(1.f, 1.f, 1.f, 1.f);
    
    /* Get Shared Data */
    bool bUseFadeOut = (bool) g_int_0;
    bool bUseRimLight = (bool) g_int_1;
    bool bUseSpriteAnim = (bool) g_int_2;
    bool bLightOn = (bool) g_int_3;
    bool bUseSSD = (bool) g_mat_2._31;
    
    float fLifeTimeRatio = g_float_0;
    float fDissolveWeight = g_float_1;
    float fRimLightIntensity = g_float_2;
    float fLightIntensity = g_float_3;
    
    float2 vColorOptions_Op[3] = { g_vec2_0, g_vec2_1, g_vec2_2 };
    
    float fAlphaOffset_Blend = g_mat_2._24;
    
    float4 vBaseColor1_Op1 = g_mat_0._11_12_13_14;
    float4 vBaseColor2_Op1 = g_mat_0._21_22_23_24;
    float4 vBaseColor1_Op2 = g_mat_0._31_32_33_34;
    float4 vBaseColor2_Op2 = g_mat_0._41_42_43_44;
    float4 vBaseColor1_Op3 = g_mat_1._11_12_13_14;
    float4 vBaseColor2_Op3 = g_mat_1._21_22_23_24;
    float4 vBaseColor_Overlay = g_mat_1._31_32_33_34;
    float4 vBaseColor_RimLight = g_mat_1._41_42_43_44;
    
    float bUseTexColor_Op[3] = { g_mat_2._11, g_mat_2._12, g_mat_2._13 };
    int iFilpOPtion_Op[3] = { g_mat_2._21, g_mat_2._22, g_mat_2._23 };
    
    /* Calc Texcoord */
    float2 decalUV = input.uv;
    float decalAlpha = 1.f;
    if (bUseSSD)
    {
        
        
        float4 projPos = mul(float4(input.viewPosition, 1.f), P);
        float2 vPixelPosInSS = projPos.xy / projPos.w;
        vPixelPosInSS = (vPixelPosInSS * 0.5f) + 0.5f;
        vPixelPosInSS.y = 1.f - vPixelPosInSS.y;
        float3 fPixelViewPos = PositionTargetTex.Sample(LinearSamplerClamp, vPixelPosInSS).rgb;
        
        float3 vPixelOriginViewPos = originPositionTargetTex.Sample(LinearSamplerClamp, vPixelPosInSS).rgb;
        
        if (vPixelOriginViewPos.z < fPixelViewPos.z)
            discard;
        
        
        float3 fPixelWorldPos = mul(float4(fPixelViewPos, 1.f), VInv);
     
        float3 decalLocalPos = mul(float4(fPixelWorldPos, 1.f), InvWorldTransformMatrix);
        clip(0.5f - abs(decalLocalPos.xyz));
        
        decalUV = decalLocalPos.xz + 0.5f;
        float3 vSSDNormal = NormalTargetTex.Sample(LinearSamplerClamp, vPixelPosInSS).xyz;
        
        float3 viewUp = mul(float3(0.f, 1.f, 0.f), (float3x3) V);
        
        decalAlpha = dot(normalize(vSSDNormal), viewUp);

    }
    
    float fDistortionWeight = 0.f;
    if (bHasDistortionMap)
    {
        float4 vDistortion = DistortionMap.Sample(LinearSamplerClamp, decalUV + g_vec2_3);
        fDistortionWeight = vDistortion.r * 0.5f;
    }

    float2 vTexcoord_Op[3] =
    {
        decalUV + float2(g_vec4_0.x, g_vec4_0.y) + fDistortionWeight,
        decalUV + float2(g_vec4_0.z, g_vec4_0.w) + fDistortionWeight,
        decalUV + float2(g_vec4_1.x, g_vec4_1.y) + fDistortionWeight
    };
    
    
    float2 vTexcoord_Blend = decalUV + float2(g_vec4_1.z, g_vec4_1.w) + fDistortionWeight;
    float2 vTexcoord_Overlay = decalUV + float2(g_vec4_2.x, g_vec4_2.y);
    float2 vTexcoord_Dissolve = decalUV + float2(g_vec4_2.z, g_vec4_2.w);
    if (bUseSpriteAnim)
    {
        float2 vSpriteAnim_RangeX = float2(g_vec4_3.x, g_vec4_3.y);
        float2 vSpriteAnim_RangeY = float2(g_vec4_3.z, g_vec4_3.w);
        vTexcoord_Op[0] = float2(vSpriteAnim_RangeX.x + ((vSpriteAnim_RangeX.y - vSpriteAnim_RangeX.x) * decalUV.x),
                              vSpriteAnim_RangeY.x + ((vSpriteAnim_RangeY.y - vSpriteAnim_RangeY.x) * decalUV.y)) + fDistortionWeight;
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
        if (0.f == bUseTexColor_Op[0])
        {
            vSample_Op1.a = TextureMap7.Sample(LinearSamplerClamp, vTexcoord_Op[0]).r;
            vSample_Op1.rgb = lerp(vBaseColor2_Op1, vBaseColor1_Op1, vSample_Op1.a);
        }
        else
        {
            vSample_Op1 = TextureMap7.Sample(LinearSamplerClamp, vTexcoord_Op[0]);
            vSample_Op1.rgb = pow(vSample_Op1.rgb, GAMMA);
        }
      
        float luminance = dot(vSample_Op1.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op1.rgb = lerp(vSample_Op1.rgb, vSample_Op1.rgb * vColorOptions_Op[0].x, saturate(luminance));
        vSample_Op1.a = saturate(vSample_Op1.a * vColorOptions_Op[0].y);
    }
    if (bHasTexturemap8)
    {
        if (0.f == bUseTexColor_Op[1])
        {
            vSample_Op2.a = TextureMap8.Sample(LinearSamplerClamp, vTexcoord_Op[1]).r;
            vSample_Op2.rgb = lerp(vBaseColor2_Op2, vBaseColor1_Op2, vSample_Op2.a);
        }
        else
        {
            vSample_Op2 = TextureMap8.Sample(LinearSamplerClamp, vTexcoord_Op[1]);
            vSample_Op2.rgb = pow(vSample_Op2.rgb, GAMMA);
        }
      
        float luminance = dot(vSample_Op2.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op2.rgb = lerp(vSample_Op2.rgb, vSample_Op2.rgb * vColorOptions_Op[1].x, saturate(luminance));
        vSample_Op2.a = saturate(vSample_Op2.a * vColorOptions_Op[1].y);
    }
    if (bHasTexturemap9)
    {
        if (0.f == bUseTexColor_Op[2])
        {
            vSample_Op3.a = TextureMap9.Sample(LinearSamplerClamp, vTexcoord_Op[2]).r;
            vSample_Op3.rgb = lerp(vBaseColor2_Op3, vBaseColor1_Op3, vSample_Op3.a);
        }
        else
        {
            vSample_Op3 = TextureMap9.Sample(LinearSamplerClamp, vTexcoord_Op[2]);
            vSample_Op3.rgb = pow(vSample_Op3.rgb, GAMMA);
        }

        float luminance = dot(vSample_Op3.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op3.rgb = lerp(vSample_Op3.rgb, vSample_Op3.rgb * vColorOptions_Op[2].x, saturate(luminance));
        vSample_Op3.a = saturate(vSample_Op3.a * vColorOptions_Op[2].y);
    }
    if (bHasTexturemap10)
    {
        vSample_Blend = TextureMap10.Sample(LinearSamplerClamp, vTexcoord_Blend);
        vSample_Blend.r = saturate(vSample_Blend.r * fAlphaOffset_Blend);
    }
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
    
    /* DissolveMap */
    if (bHasDissolveMap)
    {
        float fDissolve = vSample_Dissolve.r;
        if (fDissolve < sin(fDissolveWeight))//sin(fLifeTimeRatio))
            vOutColor.a = 0.f;
    }
    
    /* Overlay */
    if (bHasTexturemap11)
    {
        float fOverlayIntensity = vSample_Overlay.r;
        float4 vFinalOverlayColor = float4(0.f, 0.f, 0.f, 0.f);

        vFinalOverlayColor.r = (vOutColor.r <= 0.5) ? 2 * vOutColor.r * vSample_Overlay.r : 1 - 2 * (1 - vOutColor.r) * (1 - vSample_Overlay.r);
        vFinalOverlayColor.g = (vOutColor.g <= 0.5) ? 2 * vOutColor.g * vSample_Overlay.g : 1 - 2 * (1 - vOutColor.g) * (1 - vSample_Overlay.g);
        vFinalOverlayColor.b = (vOutColor.b <= 0.5) ? 2 * vOutColor.b * vSample_Overlay.b : 1 - 2 * (1 - vOutColor.b) * (1 - vSample_Overlay.b);
      
        vOutColor.rgb = lerp(vOutColor.rgb, vFinalOverlayColor.rgb, fOverlayIntensity);
    }
    
    if (bHasNormalMap)
        ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, vTexcoord_Op[0]);
    
    /* Lighting */
    if (bLightOn)
    {
        float diffuseRatio = 0.f;
        {
            float3 viewLightDir = 0.f;
          
            viewLightDir = normalize(mul(float4(lights[0].vDirection.xyz, 0.f), V).xyz);
            diffuseRatio = saturate(dot(-viewLightDir, normalize(input.viewNormal)));
                  
        }
        float3 vLightingColor = vOutColor.rgb * diffuseRatio;
        vOutColor.rgb = lerp(vOutColor.rgb, vLightingColor, fLightIntensity);
        //vOutColor.rgb = (vOutColor.rgb * diffuseRatio) + fLightIntensity;

    }
    
    /* Rim Light */
        // TODO 
    
    /* Fade Out */
    if (bUseFadeOut)
        vOutColor.a *= (1.f - fLifeTimeRatio);
    
        /* Blend */
    if (bHasTexturemap10)
        vOutColor.a *= vSample_Blend.r;
    
    if (vOutColor.a < 0.07f)
        discard;
    
    vOutColor.a *= decalAlpha;
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
    /* Get Shared Data */
    bool bUseFadeOut = (bool) g_effectData[id].g_int_0;
    bool bUseRimLight = (bool) g_effectData[id].g_int_1;
    bool bUseSpriteAnim = (bool) g_effectData[id].g_int_2;
    bool bLightOn = (bool) g_effectData[id].g_int_3;
    bool bUseSSD = (bool) g_effectData[id].g_mat_2._31;
    
    float fLifeTimeRatio = g_effectData[id].g_float_0;
    float fDissolveWeight = g_effectData[id].g_float_1;
    float fRimLightIntensity = g_effectData[id].g_float_2;
    float fLightIntensity = g_effectData[id].g_float_3;
    
    float2 vColorOptions_Op[3] = { g_effectData[id].g_vec2_0, g_effectData[id].g_vec2_1, g_effectData[id].g_vec2_2 };
    
    float fAlphaOffset_Blend = g_effectData[id].g_mat_2._24;
    
    float4 vBaseColor1_Op1 = g_effectData[id].g_mat_0._11_12_13_14;
    float4 vBaseColor2_Op1 = g_effectData[id].g_mat_0._21_22_23_24;
    float4 vBaseColor1_Op2 = g_effectData[id].g_mat_0._31_32_33_34;
    float4 vBaseColor2_Op2 = g_effectData[id].g_mat_0._41_42_43_44;
    float4 vBaseColor1_Op3 = g_effectData[id].g_mat_1._11_12_13_14;
    float4 vBaseColor2_Op3 = g_effectData[id].g_mat_1._21_22_23_24;
    float4 vBaseColor_Overlay = g_effectData[id].g_mat_1._31_32_33_34;
    float4 vBaseColor_RimLight = g_effectData[id].g_mat_1._41_42_43_44;
    
    float bUseTexColor_Op[3] = { g_effectData[id].g_mat_2._11, g_effectData[id].g_mat_2._12, g_effectData[id].g_mat_2._13 };
    int iFilpOPtion_Op[3] = { g_effectData[id].g_mat_2._21, g_effectData[id].g_mat_2._22, g_effectData[id].g_mat_2._23 };
    
    /* Calc Texcoord */
    float2 decalUV = input.uv;
    float decalAlpha = 1.f;
    if (bUseSSD)
    {
        float4 projPos = mul(float4(input.viewPosition, 1.f), P);
        float2 vPixelPosInSS = projPos.xy / projPos.w;
        vPixelPosInSS = (vPixelPosInSS * 0.5f) + 0.5f;
        vPixelPosInSS.y = 1.f - vPixelPosInSS.y;
        float3 vPixelViewPos = PositionTargetTex.Sample(LinearSamplerClamp, vPixelPosInSS).rgb;
        float3 vPixelWorldPos = mul(float4(vPixelViewPos, 1.f), VInv);
        float3 vPixelOriginViewPos = originPositionTargetTex.Sample(LinearSamplerClamp, vPixelPosInSS).rgb;
        
        if(vPixelOriginViewPos.z < vPixelViewPos.z)
            discard;
                
        float3 decalLocalPos = mul(float4(vPixelWorldPos, 1.f), InvWorldTransformMatrix);
        clip(0.5f - abs(decalLocalPos.xyz));
        
        decalUV = decalLocalPos.xz + 0.5f;
        float3 vSSDNormal = NormalTargetTex.Sample(LinearSamplerClamp, vPixelPosInSS).xyz;
        float3 viewUp = mul(float3(0.f, 1.f, 0.f), (float3x3) V);
        
        decalAlpha = dot(normalize(vSSDNormal), viewUp);

    }
    
    float fDistortionWeight = 0.f;
    if (bHasDistortionMap)
    {
        float4 vDistortion = DistortionMap.Sample(LinearSampler, decalUV + g_effectData[id].g_vec2_3);
        fDistortionWeight = vDistortion.r * 0.5f;
    }

    float2 vTexcoord_Op[3] =
    {
        decalUV + float2(g_effectData[id].g_vec4_0.x, g_effectData[id].g_vec4_0.y) + fDistortionWeight,
        decalUV + float2(g_effectData[id].g_vec4_0.z, g_effectData[id].g_vec4_0.w) + fDistortionWeight,
        decalUV + float2(g_effectData[id].g_vec4_1.x, g_effectData[id].g_vec4_1.y) + fDistortionWeight
    };
    
    float2 vTexcoord_Blend = decalUV + float2(g_effectData[id].g_vec4_1.z, g_effectData[id].g_vec4_1.w) + fDistortionWeight;
    float2 vTexcoord_Overlay = decalUV + float2(g_effectData[id].g_vec4_2.x, g_effectData[id].g_vec4_2.y);
    float2 vTexcoord_Dissolve = decalUV + float2(g_effectData[id].g_vec4_2.z, g_effectData[id].g_vec4_2.w);
    if (bUseSpriteAnim)
    {
        float2 vSpriteAnim_RangeX = float2(g_effectData[id].g_vec4_3.x, g_effectData[id].g_vec4_3.y);
        float2 vSpriteAnim_RangeY = float2(g_effectData[id].g_vec4_3.z, g_effectData[id].g_vec4_3.w);
        vTexcoord_Op[0] = float2(vSpriteAnim_RangeX.x + ((vSpriteAnim_RangeX.y - vSpriteAnim_RangeX.x) * decalUV.x),
                              vSpriteAnim_RangeY.x + ((vSpriteAnim_RangeY.y - vSpriteAnim_RangeY.x) * decalUV.y)) + fDistortionWeight;
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
        if (0.f == bUseTexColor_Op[0])
        {
            vSample_Op1.a = TextureMap7.Sample(LinearSampler, vTexcoord_Op[0]).r;
            vSample_Op1.rgb = lerp(vBaseColor2_Op1, vBaseColor1_Op1, vSample_Op1.a);
        }
        else
        {
            vSample_Op1 = TextureMap7.Sample(LinearSampler, vTexcoord_Op[0]);
            vSample_Op1.rgb = pow(vSample_Op1.rgb, GAMMA);
        }
        
        float luminance = dot(vSample_Op1.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op1.rgb = lerp(vSample_Op1.rgb, vSample_Op1.rgb * vColorOptions_Op[0].x, saturate(luminance));
        vSample_Op1.a = saturate(vSample_Op1.a * vColorOptions_Op[0].y);
    }
    if (bHasTexturemap8)
    {
        if (0.f == bUseTexColor_Op[1])
        {
            vSample_Op2.a = TextureMap8.Sample(LinearSampler, vTexcoord_Op[1]).r;
            vSample_Op2.rgb = lerp(vBaseColor2_Op2, vBaseColor1_Op2, vSample_Op2.a);
        }
        else
        {
            vSample_Op2 = TextureMap8.Sample(LinearSampler, vTexcoord_Op[1]);
            vSample_Op2.rgb = pow(vSample_Op2.rgb, GAMMA);
        }
        
        float luminance = dot(vSample_Op2.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op2.rgb = lerp(vSample_Op2.rgb, vSample_Op2.rgb * vColorOptions_Op[1].x, saturate(luminance));
        vSample_Op2.a = saturate(vSample_Op2.a * vColorOptions_Op[1].y);
    }
    if (bHasTexturemap9)
    {
        if (0.f == bUseTexColor_Op[2])
        {
            vSample_Op3.a = TextureMap9.Sample(LinearSampler, vTexcoord_Op[2]).r;
            vSample_Op3.rgb = lerp(vBaseColor2_Op3, vBaseColor1_Op3, vSample_Op3.a);
        }
        else
        {
            vSample_Op3 = TextureMap9.Sample(LinearSampler, vTexcoord_Op[2]);
            vSample_Op3.rgb = pow(vSample_Op3.rgb, GAMMA);
        }

        float luminance = dot(vSample_Op3.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op3.rgb = lerp(vSample_Op3.rgb, vSample_Op3.rgb * vColorOptions_Op[2].x, saturate(luminance));
        vSample_Op3.a = saturate(vSample_Op3.a * vColorOptions_Op[2].y);
    }
    if (bHasTexturemap10)
    {
        vSample_Blend = TextureMap10.Sample(LinearSampler, vTexcoord_Blend);
        vSample_Blend.r = saturate(vSample_Blend.r * fAlphaOffset_Blend);
    }
    if (bHasTexturemap11)
        vSample_Overlay = TextureMap11.Sample(LinearSampler, vTexcoord_Overlay);
    if (bHasDissolveMap)
        vSample_Dissolve = DissolveMap.Sample(LinearSampler, vTexcoord_Dissolve);
    
    /* Mix four option textures */
    vOutColor = vSample_Op1;
    vOutColor = lerp(vOutColor, vSample_Op2, vSample_Op2.a);
    vOutColor = lerp(vOutColor, vSample_Op3, vSample_Op3.a);
    
    /* Gamma collection */
    if (bUseTexColor_Op[0] || bUseTexColor_Op[1] || bUseTexColor_Op[2])
        vOutColor.rgb = pow(vOutColor.rgb, 1.f / GAMMA);
    
    if (bHasNormalMap)
        ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, vTexcoord_Op[0]);
    
     /* Lighting */
    if (bLightOn)
    {
        float diffuseRatio = 0.f;
        {
            float3 viewLightDir = 0.f;
            
            viewLightDir = normalize(mul(float4(lights[0].vDirection.xyz, 0.f), V).xyz);
            diffuseRatio = saturate(dot(-viewLightDir, normalize(input.viewNormal)));
                    
        }
        float3 vLightingColor = vOutColor.rgb * diffuseRatio;
        vOutColor.rgb = lerp(vOutColor.rgb, vLightingColor, fLightIntensity);
    }
    
    ///* Blend */
    //if (bHasTexturemap10)
    //    vOutColor.a *= vSample_Blend.r;
    
    /* DissolveMap */
    if (bHasDissolveMap)
    {
        float fDissolve = vSample_Dissolve.r;
        if (fDissolve < sin(fDissolveWeight))//sin(fLifeTimeRatio))
            vOutColor.a = 0.f;
    }
    
    /* Overlay */
    if (bHasTexturemap11)
    {
        float fOverlayIntensity = vSample_Overlay.r;
        float4 vFinalOverlayColor = float4(0.f, 0.f, 0.f, 0.f);

        vFinalOverlayColor.r = (vOutColor.r <= 0.5) ? 2 * vOutColor.r * vSample_Overlay.r : 1 - 2 * (1 - vOutColor.r) * (1 - vSample_Overlay.r);
        vFinalOverlayColor.g = (vOutColor.g <= 0.5) ? 2 * vOutColor.g * vSample_Overlay.g : 1 - 2 * (1 - vOutColor.g) * (1 - vSample_Overlay.g);
        vFinalOverlayColor.b = (vOutColor.b <= 0.5) ? 2 * vOutColor.b * vSample_Overlay.b : 1 - 2 * (1 - vOutColor.b) * (1 - vSample_Overlay.b);
        
        vOutColor.rgb = lerp(vOutColor.rgb, vFinalOverlayColor.rgb, fOverlayIntensity);
    }
    
    /* Rim Light */
    if (bUseRimLight)
    {
        //RimLightIntensity
        //vBaseColor_RimLight
        
        //float3 eyeDir = normalize(input.viewPosition - );
        

    }
        
    
    /* Fade Out */
    if (bUseFadeOut)
        vOutColor.a *= (1.f - fLifeTimeRatio);
    
        /* Blend */
    if (bHasTexturemap10)
        vOutColor.a *= vSample_Blend.r;
    
    if (vOutColor.a < 0.07f)
        discard;
    
    vOutColor.a *= decalAlpha;
    
    return vOutColor;
}

float4 PS_Clamp_Instancing(EffectOutInstancing input) : SV_Target
{   
    float4 vOutColor = float4(1.f, 1.f, 1.f, 1.f);
    int id = input.id;
    /* Get Shared Data */
    bool bUseFadeOut = (bool) g_effectData[id].g_int_0;
    bool bUseRimLight = (bool) g_effectData[id].g_int_1;
    bool bUseSpriteAnim = (bool) g_effectData[id].g_int_2;
    bool bLightOn = (bool) g_effectData[id].g_int_3;
    bool bUseSSD = (bool)g_effectData[id].g_mat_2._31;
    
    float fLifeTimeRatio = g_effectData[id].g_float_0;
    float fDissolveWeight = g_effectData[id].g_float_1;
    float fRimLightIntensity = g_effectData[id].g_float_2;
    float fLightIntensity = g_effectData[id].g_float_3;
    
    float2 vColorOptions_Op[3] = { g_effectData[id].g_vec2_0, g_effectData[id].g_vec2_1, g_effectData[id].g_vec2_2 };
    
    float fAlphaOffset_Blend = g_effectData[id].g_mat_2._24;

    float4 vBaseColor1_Op1 = g_effectData[id].g_mat_0._11_12_13_14;
    float4 vBaseColor2_Op1 = g_effectData[id].g_mat_0._21_22_23_24;
    float4 vBaseColor1_Op2 = g_effectData[id].g_mat_0._31_32_33_34;
    float4 vBaseColor2_Op2 = g_effectData[id].g_mat_0._41_42_43_44;
    float4 vBaseColor1_Op3 = g_effectData[id].g_mat_1._11_12_13_14;
    float4 vBaseColor2_Op3 = g_effectData[id].g_mat_1._21_22_23_24;
    float4 vBaseColor_Overlay = g_effectData[id].g_mat_1._31_32_33_34;
    float4 vBaseColor_RimLight = g_effectData[id].g_mat_1._41_42_43_44;
    
    float bUseTexColor_Op[3] = { g_effectData[id].g_mat_2._11, g_effectData[id].g_mat_2._12, g_effectData[id].g_mat_2._13 };
    int iFilpOPtion_Op[3] = { g_effectData[id].g_mat_2._21, g_effectData[id].g_mat_2._22, g_effectData[id].g_mat_2._23 };
    
    /* Calc Texcoord */
     /* Calc Decal Textcoord */
    float2 decalUV = input.uv;
    float decalAlpha = 1.f;
    if (bUseSSD)
    {
        float4 projPos = mul(float4(input.viewPosition, 1.f), P);
        float2 vPixelPosInSS = projPos.xy / projPos.w;
        vPixelPosInSS = (vPixelPosInSS * 0.5f) + 0.5f;
        vPixelPosInSS.y = 1.f - vPixelPosInSS.y;
        float3 fPixelViewPos = PositionTargetTex.Sample(LinearSamplerClamp, vPixelPosInSS).rgb;
        float3 fPixelWorldPos = mul(float4(fPixelViewPos, 1.f), VInv);
        float3 vPixelOriginViewPos = originPositionTargetTex.Sample(LinearSamplerClamp, vPixelPosInSS).rgb;
        
        if (vPixelOriginViewPos.z < fPixelViewPos.z)
            discard;
        float3 decalLocalPos = mul(float4(fPixelWorldPos, 1.f), InvWorldTransformMatrix);
        clip(0.5f - abs(decalLocalPos.xyz));
        
        decalUV = decalLocalPos.xz + 0.5f;
        float3 vSSDNormal = NormalTargetTex.Sample(LinearSamplerClamp, vPixelPosInSS).xyz;
        float3 viewUp = mul(float3(0.f, 1.f, 0.f), (float3x3) V);
        
        decalAlpha = dot(normalize(vSSDNormal), viewUp);

    }
    
    float fDistortionWeight = 0.f;
    if (bHasDistortionMap)
    {
        float4 vDistortion = DistortionMap.Sample(LinearSamplerClamp, decalUV + g_effectData[id].g_vec2_3);
        fDistortionWeight = vDistortion.r * 0.5f;
    }

    float2 vTexcoord_Op[3] =
    {
        decalUV + float2(g_effectData[id].g_vec4_0.x, g_effectData[id].g_vec4_0.y) + fDistortionWeight,
        decalUV + float2(g_effectData[id].g_vec4_0.z, g_effectData[id].g_vec4_0.w) + fDistortionWeight,
        decalUV + float2(g_effectData[id].g_vec4_1.x, g_effectData[id].g_vec4_1.y) + fDistortionWeight
    };
    
    float2 vTexcoord_Blend = decalUV + float2(g_effectData[id].g_vec4_1.z, g_effectData[id].g_vec4_1.w) + fDistortionWeight;
    float2 vTexcoord_Overlay = decalUV + float2(g_effectData[id].g_vec4_2.x, g_effectData[id].g_vec4_2.y);
    float2 vTexcoord_Dissolve = decalUV + float2(g_effectData[id].g_vec4_2.z, g_effectData[id].g_vec4_2.w);
    if (bUseSpriteAnim)
    {
        float2 vSpriteAnim_RangeX = float2(g_effectData[id].g_vec4_3.x, g_effectData[id].g_vec4_3.y);
        float2 vSpriteAnim_RangeY = float2(g_effectData[id].g_vec4_3.z, g_effectData[id].g_vec4_3.w);
        vTexcoord_Op[0] = float2(vSpriteAnim_RangeX.x + ((vSpriteAnim_RangeX.y - vSpriteAnim_RangeX.x) * decalUV.x),
                              vSpriteAnim_RangeY.x + ((vSpriteAnim_RangeY.y - vSpriteAnim_RangeY.x) * decalUV.y)) + fDistortionWeight;
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
        if (0.f == bUseTexColor_Op[0])
        {
            vSample_Op1.a = TextureMap7.Sample(LinearSamplerClamp, vTexcoord_Op[0]).r;
            vSample_Op1.rgb = lerp(vBaseColor2_Op1, vBaseColor1_Op1, vSample_Op1.a);
        }
        else
        {
            vSample_Op1 = TextureMap7.Sample(LinearSamplerClamp, vTexcoord_Op[0]);
            vSample_Op1.rgb = pow(vSample_Op1.rgb, GAMMA);
        }
        
        float luminance = dot(vSample_Op1.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op1.rgb = lerp(vSample_Op1.rgb, vSample_Op1.rgb * vColorOptions_Op[0].x, saturate(luminance));
        vSample_Op1.a = saturate(vSample_Op1.a * vColorOptions_Op[0].y);
    }
    if (bHasTexturemap8)
    {
        if (0.f == bUseTexColor_Op[1])
        {
            vSample_Op2.a = TextureMap8.Sample(LinearSamplerClamp, vTexcoord_Op[1]).r;
            vSample_Op2.rgb = lerp(vBaseColor2_Op2, vBaseColor1_Op2, vSample_Op2.a);
        }
        else
        {
            vSample_Op2 = TextureMap8.Sample(LinearSamplerClamp, vTexcoord_Op[1]);
            vSample_Op2.rgb = pow(vSample_Op2.rgb, GAMMA);
        }
        
        float luminance = dot(vSample_Op2.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op2.rgb = lerp(vSample_Op2.rgb, vSample_Op2.rgb * vColorOptions_Op[1].x, saturate(luminance));
        vSample_Op2.a = saturate(vSample_Op2.a * vColorOptions_Op[1].y);
    }
    if (bHasTexturemap9)
    {
        if (0.f == bUseTexColor_Op[2])
        {
            vSample_Op3.a = TextureMap9.Sample(LinearSamplerClamp, vTexcoord_Op[2]).r;
            vSample_Op3.rgb = lerp(vBaseColor2_Op3, vBaseColor1_Op3, vSample_Op3.a);
        }
        else
        {
            vSample_Op3 = TextureMap9.Sample(LinearSamplerClamp, vTexcoord_Op[2]);
            vSample_Op3.rgb = pow(vSample_Op3.rgb, GAMMA);
        }

        float luminance = dot(vSample_Op3.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op3.rgb = lerp(vSample_Op3.rgb, vSample_Op3.rgb * vColorOptions_Op[2].x, saturate(luminance));
        vSample_Op3.a = saturate(vSample_Op3.a * vColorOptions_Op[2].y);
    }
    if (bHasTexturemap10)
    {
        vSample_Blend = TextureMap10.Sample(LinearSamplerClamp, vTexcoord_Blend);
        vSample_Blend.r = saturate(vSample_Blend.r * fAlphaOffset_Blend);
    }
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

    ///* Blend */
    //if (bHasTexturemap10)
    //    vOutColor.a *= vSample_Blend.r;
    
    /* DissolveMap */
    if (bHasDissolveMap)
    {
        float fDissolve = vSample_Dissolve.r;
        if (fDissolve < sin(fDissolveWeight))//sin(fLifeTimeRatio))
            vOutColor.a = 0.f;
    }
    
    /* Overlay */
    if (bHasTexturemap11)
    {
        float fOverlayIntensity = vSample_Overlay.r;
        float4 vFinalOverlayColor = float4(0.f, 0.f, 0.f, 0.f);

        vFinalOverlayColor.r = (vOutColor.r <= 0.5) ? 2 * vOutColor.r * vSample_Overlay.r : 1 - 2 * (1 - vOutColor.r) * (1 - vSample_Overlay.r);
        vFinalOverlayColor.g = (vOutColor.g <= 0.5) ? 2 * vOutColor.g * vSample_Overlay.g : 1 - 2 * (1 - vOutColor.g) * (1 - vSample_Overlay.g);
        vFinalOverlayColor.b = (vOutColor.b <= 0.5) ? 2 * vOutColor.b * vSample_Overlay.b : 1 - 2 * (1 - vOutColor.b) * (1 - vSample_Overlay.b);
        
        vOutColor.rgb = lerp(vOutColor.rgb, vFinalOverlayColor.rgb, fOverlayIntensity);
    }
    
    if (bHasNormalMap)
        ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, vTexcoord_Op[0]);
    
    /* Lighting */
    if (bLightOn)
    {
        float diffuseRatio = 0.f;
        {
            float3 viewLightDir = 0.f;
            
            viewLightDir = normalize(mul(float4(lights[0].vDirection.xyz, 0.f), V).xyz);
            diffuseRatio = saturate(dot(-viewLightDir, normalize(input.viewNormal)));
                    
        }
        float3 vLightingColor = vOutColor.rgb * diffuseRatio;
        vOutColor.rgb = lerp(vOutColor.rgb, vLightingColor, fLightIntensity);
        //vOutColor.rgb = (vOutColor.rgb * diffuseRatio) + fLightIntensity;

    }
    
    /* Rim Light */
        // TODO 
    
    /* Fade Out */
    if (bUseFadeOut)
        vOutColor.a *= (1.f - fLifeTimeRatio);
    
        /* Blend */
    if (bHasTexturemap10)
        vOutColor.a *= vSample_Blend.r;
    
    if (vOutColor.a < 0.07f)
        discard;
    
    vOutColor.a *= decalAlpha;
    
    return vOutColor;
}

technique11 T0 // 0
{
    pass pass_Wrap // 0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Wrap()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass pass_Clamp // 1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Clamp()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass pass_Mirror // 2
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Mirror()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass pass_Border // 3
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Border()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
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
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass pass_Clamp_Instancing // 1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main_Instancing()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Clamp_Instancing()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

};

technique11 T2_SSD // 2
{
    pass pass_Wrap // 0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_LESS_NO_WRITE, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Wrap()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass pass_Clamp // 1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_LESS_NO_WRITE, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Clamp()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass pass_Mirror // 2
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_LESS_NO_WRITE, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Mirror()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass pass_Border // 3
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_LESS_NO_WRITE, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Border()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
};
  
technique11 T3_SSD_Instancing // 3
{
    pass pass_Wrap_Instancing // 0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main_Instancing()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_LESS_NO_WRITE, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Wrap_Instancing()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

    pass pass_Clamp_Instancing // 1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Main_Instancing()));
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_LESS_NO_WRITE, 0);
        SetPixelShader(CompileShader(ps_5_0, PS_Clamp_Instancing()));
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

};