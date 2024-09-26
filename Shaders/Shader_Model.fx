#include "Render.fx"
#include "Light.fx"
#include "PBR.fx"

row_major float4x4 g_preView;

struct MotionBlurOutput
{
    float4 position : SV_Position;
    float4 vDir : Position1;
};


// VS_Model
MeshOutput VS_NonAnim(VTXModel input)
{
    MeshOutput output;
    output.position = mul(float4(input.position, 1.f), BoneTransform[BoneIndex]);
    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz;
    output.viewPosition = mul(float4(output.worldPosition, 1.f), V).xyz;
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    output.viewNormal = mul(input.normal, (float3x3) BoneTransform[BoneIndex]);
    output.viewNormal = mul(output.viewNormal, (float3x3) W);
    output.viewNormal = normalize(mul(output.viewNormal, (float3x3) V));
    output.viewTangent = mul(input.tangent, (float3x3) BoneTransform[BoneIndex]);
    output.viewTangent = mul(output.viewTangent, (float3x3) W);
    output.viewTangent = normalize(mul(output.viewTangent, (float3x3) V));
    

    return output;
}

MeshOutput VS_MapObject(VTXModel input, uniform bool isWorldNormal = false)
{
    MeshOutput output;
    output.position = mul(float4(input.position, 1.f), BoneTransform[BoneIndex]);
    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz;
    output.viewPosition = mul(float4(output.worldPosition, 1.f), V).xyz;
    output.position = mul(output.position, VP);

    output.uv = input.uv * g_vec4_0.x;
    if(isWorldNormal)
    {
        output.viewNormal = normalize(mul(float3(0,1,0), (float3x3) W));
        output.viewNormal = normalize(mul(output.viewNormal, (float3x3) V));
        output.viewTangent = normalize(mul(float3(1,0,0), (float3x3) W));
        output.viewTangent = normalize(mul(output.viewTangent, (float3x3) V));
    }
    else
    {
        output.viewNormal = mul(input.normal, (float3x3) BoneTransform[BoneIndex]);
        output.viewNormal = mul(output.viewNormal, (float3x3) W);
        output.viewNormal = normalize(mul(output.viewNormal, (float3x3) V));
        output.viewTangent = mul(input.tangent, (float3x3) BoneTransform[BoneIndex]);
        output.viewTangent = mul(output.viewTangent, (float3x3) W);
        output.viewTangent = normalize(mul(output.viewTangent, (float3x3) V));
    }
   

    return output;
}

MeshOutput VS_Anim(VTXModel input)
{
    MeshOutput output;
    
    row_major float4x4 m = GetAnimationMatrix(input);

    output.position = mul(float4(input.position, 1.f), m);

    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz;
    output.viewPosition = mul(float4(output.worldPosition, 1.f), V).xyz;

    output.position = mul(output.position, VP);
    output.uv = input.uv;
    output.viewNormal = mul(input.normal, (float3x3) m);
    output.viewNormal = mul(output.viewNormal, (float3x3) W);
    output.viewNormal = normalize(mul(output.viewNormal, (float3x3) V));
    output.viewTangent = mul(input.tangent, (float3x3) m);
    output.viewTangent = mul(output.viewTangent, (float3x3) W);
    output.viewTangent = normalize(mul(output.viewTangent, (float3x3) V));

    return output;
}

MeshInstancingOutput VS_NonAnimInstancing(VTXModelInstancing input)
{
    MeshInstancingOutput output;
    
    output.position = mul(float4(input.position, 1.f), BoneTransform[BoneIndex]);
    output.position = mul(output.position, input.world);
    output.worldPosition = output.position.xyz;
    output.viewPosition = mul(float4(output.worldPosition, 1.f), V).xyz;

    output.position = mul(output.position, VP);
    output.uv = input.uv;
    output.viewNormal = normalize(mul(input.normal, (float3x3) BoneTransform[BoneIndex]));
    output.viewNormal = normalize(mul(output.viewNormal, (float3x3) input.world));
    output.viewNormal = normalize(mul(output.viewNormal, (float3x3) V));
    output.viewTangent = mul(input.tangent, (float3x3) BoneTransform[BoneIndex]);
    output.viewTangent = mul(output.viewTangent, (float3x3) input.world);
    output.viewTangent = normalize(mul(output.viewTangent, (float3x3) V));
    
    output.id = input.instanceID;
    
    return output;
}

MeshInstancingOutput VS_MapObject_Instancing(VTXModelInstancing input, uniform bool isWorldNormal = false)
{
    MeshInstancingOutput output;
    
    output.position = mul(float4(input.position, 1.f), BoneTransform[BoneIndex]);
    output.position = mul(output.position, input.world);
    output.worldPosition = output.position.xyz;
    output.viewPosition = mul(float4(output.worldPosition, 1.f), V).xyz;

    output.position = mul(output.position, VP);
    output.uv = input.uv * InstanceRenderParams[input.instanceID].x;

    if(isWorldNormal)
    {
       output.viewNormal = normalize(mul(float3(0, 1, 0), (float3x3) input.world));
       output.viewNormal = normalize(mul(output.viewNormal, (float3x3) V));
       
       output.viewTangent = normalize(mul(float3(1, 0, 0), (float3x3) input.world));
       output.viewTangent = normalize(mul(output.viewTangent, (float3x3) V));
    }
    else
    {
    output.viewNormal = normalize(mul(input.normal, (float3x3) BoneTransform[BoneIndex]));
    output.viewNormal = normalize(mul(output.viewNormal, (float3x3) input.world));
    output.viewNormal = normalize(mul(output.viewNormal, (float3x3) V));
    output.viewTangent = mul(input.tangent, (float3x3) BoneTransform[BoneIndex]);
    output.viewTangent = mul(output.viewTangent, (float3x3) input.world);
    output.viewTangent = normalize(mul(output.viewTangent, (float3x3) V));
        
    }
    output.id = input.instanceID;
    
    return output;
}

MeshInstancingOutput VS_AnimInstancing(VTXModelInstancing input)
{
    MeshInstancingOutput output;

    row_major float4x4 m = GetAnimationMatrix_Instance(input);
    
    output.position = mul(float4(input.position, 1.f), m);
    output.position = mul(output.position, input.world);
    output.worldPosition = output.position.xyz;
    output.viewPosition = mul(float4(output.worldPosition, 1.f), V).xyz;

    output.position = mul(output.position, VP);
    output.uv = input.uv;
    output.viewNormal = mul(input.normal, (float3x3) m);
    output.viewNormal = mul(output.viewNormal, (float3x3) input.world);
    output.viewNormal = normalize(mul(output.viewNormal, (float3x3) V));
    output.viewTangent = mul(input.tangent, (float3x3) m);
    output.viewTangent = mul(output.viewTangent, (float3x3) input.world);
    output.viewTangent = normalize(mul(output.viewTangent, (float3x3) V));
    
    output.id = input.instanceID;
    
    return output;
}

// VS_Shadow
ShadowOutput VS_Shadow_NonAnim(VTXModel input)
{
    ShadowOutput output = (ShadowOutput) 0.f;
    
    output.pos = mul(float4(input.position, 1.f), BoneTransform[BoneIndex]);
    output.pos = mul(output.pos, W);
    output.worldPos = output.pos;
    output.pos = mul(output.pos, VP);
    output.clipPos = output.pos;
    output.uv = input.uv;
    return output;
}

ShadowOutput VS_Shadow_Anim(VTXModel input)
{
    ShadowOutput output = (ShadowOutput) 0.f;
    
    row_major float4x4 m = GetAnimationMatrix(input);
    
    output.pos = mul(float4(input.position, 1.f), m);
    
    output.pos = mul(output.pos, W);
    output.worldPos = output.pos;
    output.pos = mul(output.pos, VP);
    output.clipPos = output.pos;
    output.uv = input.uv;
    return output;
}

ShadowInstanceOutput VS_Shadow_NonAnim_Instancing(VTXModelInstancing input)
{
    ShadowInstanceOutput output = (ShadowInstanceOutput) 0.f;
    
    output.pos = mul(float4(input.position, 1.f), BoneTransform[BoneIndex]);
    output.pos = mul(output.pos, input.world);
    output.worldPos = output.pos;
    output.pos = mul(output.pos, VP);
    output.clipPos = output.pos;
    output.uv = input.uv;
    output.id = input.instanceID;
    return output;
}

ShadowInstanceOutput VS_Shadow_Anim_Instancing(VTXModelInstancing input)
{
    ShadowInstanceOutput output = (ShadowInstanceOutput) 0.f;
    
    row_major float4x4 m = GetAnimationMatrix_Instance(input);
    
    output.pos = mul(float4(input.position, 1.f), m);
    
    output.pos = mul(output.pos, input.world);
    output.worldPos = output.pos;
    output.pos = mul(output.pos, VP);
    output.clipPos = output.pos;
    output.uv = input.uv;
    output.id = input.instanceID;
    return output;
}
// VS_MotionBlur
MotionBlurOutput VS_NonAnimMotionBlur(VTXModel input)
{
    MotionBlurOutput output;
    
    output.position = mul(float4(input.position, 1.f), BoneTransform[BoneIndex]);
    output.position = mul(output.position, W);
    output.position = mul(output.position, V);
    float3 viewNormal = normalize(mul(input.normal, (float3x3) BoneTransform[BoneIndex]));
    viewNormal = normalize(mul(viewNormal, (float3x3) W));
    viewNormal = normalize(mul(viewNormal, (float3x3) V));
    
    float4 vOldPos = mul(float4(input.position, 1.f), BoneTransform[BoneIndex]);
    vOldPos = mul(vOldPos, preW);
    vOldPos = mul(vOldPos, g_preView);
    vOldPos = mul(vOldPos, P);
    
    float4 vNewPos = mul(output.position, P);
    
    float3 vDir = vNewPos.xyz - vOldPos.xyz;
    
    float a = dot(normalize(vDir), viewNormal);
    if (a < 0.f)
        output.position = vOldPos;
    else
        output.position = vNewPos;
        
    //[-2~2]
    float2 velocity = (vNewPos.xy / vNewPos.w) - (vOldPos.xy / vOldPos.w);

    output.vDir.xy = velocity * 0.5f;
    output.vDir.y *= -1.f;
    output.vDir.z = output.position.z;
    output.vDir.w = output.position.w;
    
    return output;
}

MotionBlurOutput VS_AnimMotionBlur(VTXModel input)
{
    MotionBlurOutput output;
    
    row_major float4x4 m = GetAnimationMatrix(input);
    row_major float4x4 preM = GetPreAnimationMatrix(input);
    
    output.position = mul(float4(input.position, 1.f), m);
    output.position = mul(output.position, W);
    output.position = mul(output.position, V);
    
    float3 viewNormal = normalize(mul(input.normal, (float3x3) m));
    viewNormal = normalize(mul(viewNormal, (float3x3) W));
    viewNormal = normalize(mul(viewNormal, (float3x3) V));
    
    float4 vOldPos = mul(float4(input.position, 1.f), preM);
    vOldPos = mul(vOldPos, preW);
    vOldPos = mul(vOldPos, g_preView);
    vOldPos = mul(vOldPos, P);
    
    float4 vNewPos = mul(output.position, P);
    
    float3 vDir = vNewPos.xyz - vOldPos.xyz;
    
    float a = dot(normalize(vDir), viewNormal);
    if (a < 0.f)
        output.position = vOldPos;
    else
        output.position = vNewPos;
        
    //[-2~2]
    float2 velocity = (vNewPos.xy / vNewPos.w) - (vOldPos.xy / vOldPos.w);

    output.vDir.xy = velocity * 0.5f;
    output.vDir.y *= -1.f;
    output.vDir.z = output.position.z;
    output.vDir.w = output.position.w;
    
    return output;
}

// VS_Motion Trail
MeshOutput VS_Anim_MotionTrail(VTXModel input)
{
    MeshOutput output;
    
    row_major float4x4 m = GetAnimationMatrix(input);

    output.position = mul(float4(input.position, 1.f), m);

    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz;
    output.viewPosition = mul(float4(output.worldPosition, 1.f), V).xyz;

    output.position = mul(output.position, VP);
    output.uv = input.uv;
    output.viewNormal = mul(input.normal, (float3x3) m);
    output.viewNormal = mul(output.viewNormal, (float3x3) W);
    output.viewNormal = normalize(mul(output.viewNormal, (float3x3) V));
    output.viewTangent = mul(input.tangent, (float3x3) m);
    output.viewTangent = mul(output.viewTangent, (float3x3) W);
    output.viewTangent = normalize(mul(output.viewTangent, (float3x3) V));

    return output;
}

MeshInstancingOutput VS_Anim_MotionTrail_Instancing(VTXModelInstancing input)
{
    MeshInstancingOutput output;

    row_major float4x4 m = GetAnimationMatrix_Instance(input);
    
    output.position = mul(float4(input.position, 1.f), m);
    output.position = mul(output.position, input.world);
    output.worldPosition = output.position.xyz;
    output.viewPosition = mul(float4(output.worldPosition, 1.f), V).xyz;

    output.position = mul(output.position, VP);
    output.uv = input.uv;
    output.viewNormal = mul(input.normal, (float3x3) m);
    output.viewNormal = mul(output.viewNormal, (float3x3) input.world);
    output.viewNormal = normalize(mul(output.viewNormal, (float3x3) V));
    output.viewTangent = mul(input.tangent, (float3x3) m);
    output.viewTangent = mul(output.viewTangent, (float3x3) input.world);
    output.viewTangent = normalize(mul(output.viewTangent, (float3x3) V));
    
    output.id = input.instanceID;
    
    return output;
}

MeshOutput VS_NonAnim_MotionTrail(VTXModel input)
{
    MeshOutput output;
    output.position = mul(float4(input.position, 1.f), BoneTransform[BoneIndex]);
    output.position = mul(output.position, W);
    output.worldPosition = output.position.xyz;
    output.viewPosition = mul(float4(output.worldPosition, 1.f), V).xyz;
    output.position = mul(output.position, VP);
    output.uv = input.uv;
    output.viewNormal = mul(input.normal, (float3x3) BoneTransform[BoneIndex]);
    output.viewNormal = mul(output.viewNormal, (float3x3) W);
    output.viewNormal = normalize(mul(output.viewNormal, (float3x3) V));
    output.viewTangent = mul(input.tangent, (float3x3) BoneTransform[BoneIndex]);
    output.viewTangent = mul(output.viewTangent, (float3x3) W);
    output.viewTangent = normalize(mul(output.viewTangent, (float3x3) V));
    

    return output;
}

// PS_Model
PS_OUT_Deferred PS_Deferred(MeshOutput input)
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
        diffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
        diffuseColor.rgb = pow(abs(diffuseColor.rgb), GAMMA);
    }
    else
        diffuseColor = Material.diffuse;
    
    if(diffuseColor.a <= 0.1f)
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
    output.emissiveColor = emissiveColor + g_vec4_1;
    return output;
}

PS_OUT_Deferred PS_Deferred_Instancing(MeshInstancingOutput input)
{
    PS_OUT_Deferred output = (PS_OUT_Deferred) 0.f;
    float4 diffuseColor;
    float4 specularColor;
    float4 emissiveColor;
    
    float2 distortedUV = input.uv;
    
    if (bHasDissolveMap != 0)
    {
        float dissolve = DissolveMap.Sample(LinearSampler, input.uv).r;
        if (dissolve < InstanceRenderParams[input.id].w)
            discard;
    }
    
    if (bHasNormalMap)
    {
            ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, input.uv);
        
    }

    if (bHasDiffuseMap)
    {
        diffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
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

PS_OUT_Deferred PS_WATER(MeshOutput input)
{
    PS_OUT_Deferred output = (PS_OUT_Deferred) 0.f;
    float4 diffuseColor;
    float4 specularColor;
    float4 emissiveColor;
    float4 ARM_Map;
    
    /* Get Shared Data */   
    float fLightIntensity = 1.05f;
        
    float4 vBaseColor1_Op1 = g_vec4_1;
    float4 vBaseColor2_Op1 = g_vec4_2;
        
    /* Calc Texcoord */
    float fDistortionWeight = 0.f;
    if (bHasDistortionMap)
    {
        float4 vDistortion = DistortionMap.Sample(LinearSampler, input.uv * 0.01f + g_vec2_0 /*uvsliding*/);
        fDistortionWeight = vDistortion.r * 0.5f;
    }

    /* Sampled from textures */    
    float4 vSample_Op1 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Op2 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Op3 = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Blend = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Overlay = { 0.f, 0.f, 0.f, 0.f };
    float4 vSample_Dissolve = { 0.f, 0.f, 0.f, 0.f };
    if (bHasDiffuseMap)
    {
        vSample_Op1.a = DiffuseMap.Sample(LinearSampler, input.uv + g_vec2_1 /*uvsliding*/ + fDistortionWeight).r;
        vSample_Op1.rgb = lerp(vBaseColor2_Op1, vBaseColor1_Op1, vSample_Op1.a);

        float luminance = dot(vSample_Op1.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op1.rgb = lerp(vSample_Op1.rgb, vSample_Op1.rgb * 1.5f, saturate(luminance));
        vSample_Op1.a = saturate(vSample_Op1.a * 4.f);
    }
   
    /* Mix four option textures */
    diffuseColor = vSample_Op1;

    if (bHasNormalMap)
        ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, input.uv + g_vec2_1 /*uvsliding*/ + fDistortionWeight);
    
    ARM_Map = float4(1.f, 0.8f, 0.0f, 1.f);
    
    if (bHasTexturemap7)
    {
        ARM_Map = TextureMap7.Sample(LinearSampler, input.uv);
        ARM_Map = pow(abs(ARM_Map), GAMMA);
    }

    if (bHasEmissiveMap)
    {
        emissiveColor = EmissiveMap.Sample(LinearSampler, input.uv);
        emissiveColor.rgb = pow(abs(emissiveColor.rgb), GAMMA);
    }
    else
        emissiveColor = 0.f;
    


    output.position = float4(input.viewPosition.xyz, 1.f);
    output.normal = float4(input.viewNormal.xyz, 0.f);
    output.depth = input.position.z;
    output.depth.w = input.viewPosition.z;
    output.diffuseColor = diffuseColor;
    output.specularColor = float4(1.f, 1.f, 1.f, 1.f);
    output.emissiveColor = emissiveColor;
    return output;
}

// PS_Shadow
float4 PS_Shadow(ShadowOutput input) : SV_Target
{
    if (bHasDissolveMap != 0)
    {
        float dissolve = DissolveMap.Sample(LinearSampler, input.uv).r;
        if (dissolve < g_vec4_0.w)
            discard;
    }
  //  return float4(input.clipPos.z, input.clipPos.w, 0.f, 0.f);
    return float4(input.clipPos.z / input.clipPos.w, 0.f, 0.f, 0.f);
}

float4 PS_ShadowInstancing(ShadowInstanceOutput input) : SV_Target
{
    if (bHasDissolveMap != 0)
    {
        float dissolve = DissolveMap.Sample(LinearSampler, input.uv).r;
        if (dissolve < InstanceRenderParams[input.id].w)
            discard;
    }
    return float4(input.clipPos.z / input.clipPos.w, 0.f, 0.f, 0.f);
}

// PS_MotionBlur
float4 PS_MotionBlur(MotionBlurOutput input) : SV_Target
{
    float4 output = (float4) 0.f;
    
    output.xy = input.vDir.xy;
    //output.xy = input.vDir.xy;
    output.z = 1.f;
    output.w = input.vDir.z / input.vDir.w;
    
    
    return output;
}

// PS_MotionTrail
float4 PS_MotionTrail(MeshOutput input) :SV_Target
{
    float4 output = (float4) 0.f;
    float4 diffuseColor;
    float4 emissiveColor = g_vec4_0;
    if (bHasNormalMap)
        ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, input.uv);


    //diffuseColor = Material.diffuse;
    
    float3 eyeDir = normalize(input.viewPosition);
    
    output = emissiveColor * pow(smoothstep(0.f, 1.f, 1.f - saturate(dot(-eyeDir, input.viewNormal))), 2);
    
    return output;
}

float4 PS_MotionTrail_Instancing(MeshInstancingOutput input) : SV_Target
{
    float4 output = (float4) 0.f;
    float4 diffuseColor;
    float4 emissiveColor = InstanceRenderParams[input.id];
    if (bHasNormalMap)
        ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, input.uv);


    //diffuseColor = Material.diffuse;
    
    float3 eyeDir = normalize(input.viewPosition);
    
    output = emissiveColor * pow(smoothstep(0.f, 1.f, 1.f - saturate(dot(-eyeDir, input.viewNormal))), 2);
    
    return output;
}

// PS_Debug
float4 PS_FRAME(VertexOutput input) : SV_TARGET
{
    float4 color = float4(1.f, 1.f, 1.f, 1.f);
    return color;
}


// PBR
PBR_OUTPUT PS_PBR_Deferred(MeshOutput input)
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
    
    if(bHasTexturemap8)
    {
        if ((10.f - g_float_0) + W._42 < input.worldPosition.y)
            discard;
        
    }
    
    ARM_Map = float4(1.f, 0.8f, 0.0f, 1.f);
    
    if(bHasTexturemap7)
    {
        ARM_Map = TextureMap7.Sample(LinearSampler, input.uv);
        ARM_Map = pow(abs(ARM_Map), GAMMA);
    }

    if (bHasNormalMap)
        ComputeNormalMapping_ViewSpace(input.viewNormal, input.viewTangent, input.uv);

    if (bHasDiffuseMap)
    {
        diffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
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
    output.diffuseColor = diffuseColor + float4(g_vec4_0.xyz, 0.f);
    output.emissive = emissiveColor;
    output.rimColor = Material.emissive + g_vec4_1;
    output.blur = 0;
    return output;
}

PBR_OUTPUT PS_PBR_Deferred_Instancing(MeshInstancingOutput input)
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
        diffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
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
    output.diffuseColor = diffuseColor + float4(InstanceRenderParams[input.id].xyz, 0.f);;
    output.emissive = emissiveColor;
    output.rimColor = Material.emissive;
    output.blur = 0;
    return output;
}

PBR_MAPOBJECT_OUTPUT PS_PBR_Deferred_MapObject(MeshOutput input)
{
    PBR_MAPOBJECT_OUTPUT output = (PBR_MAPOBJECT_OUTPUT) 0.f;
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
        diffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
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
    output.positionSSD = float4(input.viewPosition.xyz, 1.f);
    output.arm = ARM_Map;
    output.diffuseColor = diffuseColor;
    output.emissive = emissiveColor;
    output.rimColor = Material.emissive + g_vec4_1;
    output.blur = 0;
    return output;
}

PBR_MAPOBJECT_OUTPUT PS_PBR_Deferred_MapObject_Instancing(MeshInstancingOutput input)
{
    PBR_MAPOBJECT_OUTPUT output = (PBR_MAPOBJECT_OUTPUT) 0.f;
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
        diffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
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
    output.positionSSD = float4(input.viewPosition.xyz, 1.f);
    output.arm = ARM_Map;
    output.diffuseColor = diffuseColor;
    output.emissive = emissiveColor ;
    output.rimColor = Material.emissive;
    output.blur = 0;
    return output;
}

PBR_OUTPUT PS_PBR_WATER(MeshOutput input)
{
    PBR_OUTPUT output = (PBR_OUTPUT) 0.f;
    float4 diffuseColor;
    float4 specularColor;
    float4 emissiveColor;
    float4 ARM_Map;
    
    /* Get Shared Data */   
    float fLightIntensity = 1.05f;
        
    float4 vBaseColor1_Op1 = g_vec4_1;
    float4 vBaseColor2_Op1 = g_vec4_2;
        
    /* Calc Texcoord */
    float fDistortionWeight = 0.f;
    if (bHasDistortionMap)
    {
        float4 vDistortion = DistortionMap.Sample(LinearSampler, input.uv + g_vec2_0/*uvsliding*/);
        fDistortionWeight = vDistortion.r * 0.5f;
    }

    /* Sampled from textures */    
    float4 vSample_Op1 = { 0.f, 0.f, 0.f, 0.f };

    if (bHasDiffuseMap)
    {
        vSample_Op1.a = DiffuseMap.Sample(LinearSampler, input.uv + g_vec2_1 /*uvsliding*/ + fDistortionWeight).r;
        vSample_Op1.rgb = lerp(vBaseColor2_Op1, vBaseColor1_Op1, vSample_Op1.a);

        float luminance = dot(vSample_Op1.rgb, float3(0.299, 0.587, 0.114));
        vSample_Op1.rgb = lerp(vSample_Op1.rgb, vSample_Op1.rgb * 1.5f, saturate(luminance));
        vSample_Op1.a = saturate(vSample_Op1.a * 4.f);
    }
   
    /* Mix four option textures */
    diffuseColor = vSample_Op1;

    float3 normal1 = input.viewNormal;
    ComputeNormalMapping_ViewSpace(normal1, input.viewTangent, input.uv * 0.4f + g_vec2_1 /*uvsliding*/ + fDistortionWeight);
    float3 normal2 = input.viewNormal;
    ComputeNormalMapping_ViewSpace(normal2, input.viewTangent, input.uv * 0.4f - g_vec2_1 /*uvsliding*/ + fDistortionWeight);

    float3 normal = (normal1 + normal2) * 0.5f;
    normal *= float3(0.5, 0.5, 1.);
    
    
    ARM_Map = float4(1.f, 0.8f, 0.0f, 1.f);
    
    if (bHasTexturemap7)
    {
        ARM_Map = TextureMap7.Sample(LinearSampler, input.uv);
        ARM_Map = pow(abs(ARM_Map), GAMMA);
    }

    if (bHasEmissiveMap)
    {
        emissiveColor = EmissiveMap.Sample(LinearSampler, input.uv);
        emissiveColor.rgb = pow(abs(emissiveColor.rgb), GAMMA);
    }
    else
        emissiveColor = 0.f;
    


    output.position = float4(input.viewPosition.xyz, 1.f);
    output.normal = float4(normal, 0.f);

    output.arm = ARM_Map;
    output.diffuseColor = diffuseColor;
    output.emissive = emissiveColor;
    output.rimColor = Material.emissive;
    output.blur = 0;
    return output; 
}

float4 PS_Forward(MeshOutput input) : SV_Target
{
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
        diffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
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
    
    LightColor color = CalculateLightColor_ViewSpace(0, input.viewNormal, input.viewPosition);
    float3 ambient = 0.4f;

    float4 finalDiffuseColor = diffuseColor * saturate(color.diffuse + color.ambient) + float4(ambient, 0.f);
    float4 finalSpecularColor = specularColor * color.specular;
    float4 finalEmissiveColor = emissiveColor * color.emissive;
    
    float3 finalColor = finalDiffuseColor.rgb + finalSpecularColor.rgb + finalEmissiveColor.rgb;
    
    
    return float4(finalColor, 1.f);
}

float4 PS_PBR_Forward(MeshOutput input) : SV_Target
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
        diffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
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
    output.diffuseColor = diffuseColor + float4(g_vec4_0.xyz, 0.f);
    output.emissive = emissiveColor;
    output.rimColor = Material.emissive;
    output.blur = 0;

    float3 ambient = ARM_Map.rrr;
    
    PBR_OUT lightingOut
    = PBRShade(ARM_Map.rrr,
    output.diffuseColor.rgb,
    ARM_Map.y,
    ARM_Map.z,
    input.viewNormal.xyz,
    input.viewPosition.xyz,
    lights[0].color.diffuse.xyz,
    1.f);
    
    float4 ambientLightColor = lightingOut.outColor;
    
    float4 rimColor = output.rimColor;
   
    float4 rimLightColor = lightingOut.emissiveColor;
    
    rimColor *= rimLightColor;
    float luminance = dot(rimColor.rgb, float3(0.299, 0.587, 0.114));
    rimColor.rgb = lerp(rimColor.rgb, rimColor.rgb * 20.f, saturate(luminance));
    
    return rimColor + emissiveColor + ambientLightColor;
}

float g_DepthRange = 15.f;
float g_ClosestDepth = 1.f;

PBR_MAPOBJECT_OUTPUT PS_PBR_Deferred_MapObject_NormalControl(MeshOutput input)
{
    PBR_MAPOBJECT_OUTPUT output = (PBR_MAPOBJECT_OUTPUT) 0.f;
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

    float3 mappingNormal = input.viewNormal;
    if (bHasNormalMap)
        ComputeNormalMapping_ViewSpace(mappingNormal, input.viewTangent, input.uv);

    float depthValue = input.viewPosition.z;
    float t = saturate((depthValue - g_ClosestDepth) / g_DepthRange);
    
    input.viewNormal = lerp(mappingNormal, input.viewNormal, t);
    
    if (bHasDiffuseMap)
    {
        diffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
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
    output.positionSSD = float4(input.viewPosition.xyz, 1.f);
    output.arm = ARM_Map;
    output.diffuseColor = diffuseColor;
    output.emissive = emissiveColor;
    output.rimColor = Material.emissive + g_vec4_1;
    output.blur = 0;
    return output;
}

PBR_MAPOBJECT_OUTPUT PS_PBR_Deferred_MapObject_Instancing_NormalControl(MeshInstancingOutput input)
{
    PBR_MAPOBJECT_OUTPUT output = (PBR_MAPOBJECT_OUTPUT) 0.f;
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

    float3 mappingNormal = input.viewNormal;
    if (bHasNormalMap)
        ComputeNormalMapping_ViewSpace(mappingNormal, input.viewTangent, input.uv);

    float depthValue = input.viewPosition.z;
    float t = saturate((depthValue - g_ClosestDepth) / g_DepthRange);
    
    input.viewNormal = lerp(mappingNormal, input.viewNormal, t);
    
    if (bHasDiffuseMap)
    {
        diffuseColor = DiffuseMap.Sample(LinearSampler, input.uv);
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
    output.positionSSD = float4(input.viewPosition.xyz, 1.f);
    output.arm = ARM_Map;
    output.diffuseColor = diffuseColor;
    output.emissive = emissiveColor;
    output.rimColor = Material.emissive;
    output.blur = 0;
    return output;
}


RasterizerState Depth
{
	// [From MSDN]
	// If the depth buffer currently bound to the output-merger stage has a UNORM format or
	// no depth buffer is bound the bias value is calculated like this: 
	//
	// Bias = (float)DepthBias * r + SlopeScaledDepthBias * MaxDepthSlope;
	//
	// where r is the minimum representable value > 0 in the depth-buffer format converted to float32.
	// [/End MSDN]
	// 
	// For a 24-bit depth buffer, r = 1 / 2^24.
	//
	// Example: DepthBias = 100000 ==> Actual DepthBias = 100000/2^24 = .006

	// You need to experiment with these values for your scene.
    DepthBias = 1000;
    DepthBiasClamp = 0.0f;
    SlopeScaledDepthBias = 1.0f;
};
technique11 T0_ModelRender
{
    pass NonAnim_NonInstancing //0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_NonAnim()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred()));
    }
    pass NonAnim_Instancing //1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_NonAnimInstancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred_Instancing()));
    }
    pass Anim_NonInstancing //2
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Anim()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred()));
    }
    pass Anim_Instancing //3
    {
        SetVertexShader(CompileShader(vs_5_0, VS_AnimInstancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred_Instancing()));
    }
    pass NonAnim_NonInstancing_CullNone //4
    {
        SetVertexShader(CompileShader(vs_5_0, VS_NonAnim()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred()));
    }
    pass NonAnim_Instancing_CullNone //5
    {
        SetVertexShader(CompileShader(vs_5_0, VS_NonAnimInstancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred_Instancing()));
    }
    pass NonAnimShadow //6
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Shadow_NonAnim()));
        SetGeometryShader(NULL);
        SetRasterizerState(Depth);
        SetDepthStencilState(DSS_LESS, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Shadow()));
    }
    pass NonAnimShadowInstancing //7
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Shadow_NonAnim_Instancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(Depth);
        SetDepthStencilState(DSS_LESS, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_ShadowInstancing()));
    }
    pass AnimShadow //8
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Shadow_Anim()));
        SetGeometryShader(NULL);
        SetRasterizerState(Depth);
        SetDepthStencilState(DSS_LESS, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Shadow()));
    }
    pass AnimShadowInstancing //9
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Shadow_Anim_Instancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(Depth);
        SetDepthStencilState(DSS_LESS, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_ShadowInstancing()));
    }

    pass MapObject //10
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred()));
    }

    pass MapObject_Instancing //11
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject_Instancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred_Instancing()));
    }

    pass MapObject_NonCull //12
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred()));
    }

    pass MapObject_Instancing_NonCull //13
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject_Instancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred_Instancing()));
    }

    pass MapObject_WorldNormal //14
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject(true)));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred()));
    }
    pass MapObject_WorldNormalInstancing //15
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject_Instancing(true)));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred_Instancing()));
    }
    pass MapObject_WorldNormal_CullNone //16
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject(true)));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred()));
    }
    pass MapObject_WorldNormalInstancing_CullNone //17
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject_Instancing(true)));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred_Instancing()));
    }
    pass MapObject_Water //18
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_WATER()));
    }
    pass forwardAnim //19
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Anim()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Forward()));
    }
    pass forwardNonAnim //20
    {
        SetVertexShader(CompileShader(vs_5_0, VS_NonAnim()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Forward()));
    }
    pass MapObject_1 //21
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred()));
    }

    pass MapObject_Instancing_1 //22
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject_Instancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Deferred_Instancing()));
    }
    PASS_RS_VP(P4_WIREFRAME, FillModeWireFrame, VS_NonAnim, PS_FRAME)
    PASS_RS_VP(P6_WIREFRAME, FillModeWireFrame, VS_NonAnimInstancing, PS_FRAME)
};

technique11 T1
{
 
};

technique11 T2_MotionBlur
{
    pass NonAnimMotionBlur
    {
        SetVertexShader(CompileShader(vs_5_0, VS_NonAnimMotionBlur()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_LESS, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_MotionBlur()));
    }
    //pass NonAnim_Instancing_MotionBlur
    //{
    //    SetVertexShader(CompileShader(vs_5_0, VS_NonAnim_Instancing_MotionBlur()));
    //    SetGeometryShader(NULL);
    //    SetRasterizerState(RS_Default);
    //    SetDepthStencilState(DSS_LESS, 0);
    //    SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    //    SetPixelShader(CompileShader(ps_5_0, PS_MotionBlur()));
    //}
    pass AnimMotionBlur
    {
        SetVertexShader(CompileShader(vs_5_0, VS_AnimMotionBlur()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_LESS, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_MotionBlur()));
    }
    //pass Anim_Instancing_MotionBlur
    //{
    //    SetVertexShader(CompileShader(vs_5_0, VS_Anim_Instancing_MotionBlur()));
    //    SetGeometryShader(NULL);
    //    SetRasterizerState(RS_Default);
    //    SetDepthStencilState(DSS_LESS, 0);
    //    SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    //    SetPixelShader(CompileShader(ps_5_0, PS_MotionBlur()));
    //}
   
};

technique11 T3_MotionTrail
{
    pass Anim_NonInstancing
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Anim()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_MotionTrail()));
    }
    pass NonAnim_NonInstancing
    {
        SetVertexShader(CompileShader(vs_5_0, VS_NonAnim()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(AlphaBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_MotionTrail()));
    }
};

technique11 T4_PBR
{
    pass NonAnim_NonInstancing//0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_NonAnim()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred()));
    }
    pass NonAnim_Instancing //1
    {
        SetVertexShader(CompileShader(vs_5_0, VS_NonAnimInstancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred_Instancing()));
    }
    pass Anim_NonInstancing //2
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Anim()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred()));
    }
    pass Anim_Instancing //3
    {
        SetVertexShader(CompileShader(vs_5_0, VS_AnimInstancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred_Instancing()));
    }
    pass NonAnim_NonInstancing_CullNone //4
    {
        SetVertexShader(CompileShader(vs_5_0, VS_NonAnim()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred()));
    }
    pass NonAnim_Instancing_CullNone //5
    {
        SetVertexShader(CompileShader(vs_5_0, VS_NonAnimInstancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred_Instancing()));
    }
    pass NonAnimShadow //6
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Shadow_NonAnim()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_LESS, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Shadow()));
    }
    pass NonAnimShadowInstancing //7
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Shadow_NonAnim_Instancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_LESS, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_ShadowInstancing()));
    }
    pass AnimShadow //8
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Shadow_Anim()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_LESS, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Shadow()));
    }
    pass AnimShadowInstancing //9
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Shadow_Anim_Instancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_LESS, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_ShadowInstancing()));
    }

    pass MapObject //10
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred_MapObject()));
    }

    pass MapObject_Instancing //11
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject_Instancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred_MapObject_Instancing()));
    }
    pass MapObject_NonCull //12
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred_MapObject()));
    }

    pass MapObject_Instancing_NonCull //13
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject_Instancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred_MapObject_Instancing()));
    }

    pass MapObject_WorldNormal //14
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject(true)));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred_MapObject()));
    }
    pass MapObject_WorldNormalInstancing //15
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject_Instancing(true)));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred_MapObject_Instancing()));
    }
    pass MapObject_WorldNormal_CullNone //16
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject(true)));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred_MapObject()));
    }
    pass MapObject_WorldNormalInstancing_CullNone //17
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject_Instancing(true)));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred_MapObject_Instancing()));
    }
    pass MapObject_Water//18
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_WATER()));
    }
    pass forwardAnim //19
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Anim()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Forward()));
    }
    pass forwardNonAnim //20
    {
        SetVertexShader(CompileShader(vs_5_0, VS_NonAnim()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Forward()));
    }
    pass MapObject_NormalControl //21
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred_MapObject_NormalControl()));
    }

    pass MapObject_Instancing_NormalControl //22
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MapObject_Instancing()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Deferred_MapObject_Instancing_NormalControl()));
    }
};