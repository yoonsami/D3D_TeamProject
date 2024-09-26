#ifndef _LIGHT_FX_
#define _LIGHT_FX_

#include "Global.fx"


////////////
// Struct //
////////////

struct LightDesc
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
    float3 direction;
    float padding;
};

struct LightColor
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
};

struct MaterialDesc
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 emissive;
};

struct LightInfo
{
    LightColor color;
    float4 vPosition;
    float4 vDirection;
    int lightType;
    float range;
    float angle;
    int padding;
};

/////////////////
// ConstBuffer //
/////////////////

cbuffer LightBuffer
{
    uint lightCount;
    float3 padding;
    LightInfo lights[50];
    float specularPower;
    float rimPower;
    float2 padding2;
};

cbuffer MaterialBuffer
{
    MaterialDesc Material;
};



/////////
// SRV //
/////////

// 변경 시 Engine의 enum.h의 TextureMapType 변경
// 변경 시 Render.fx의 TextureMapBuffer도 변경
// 변경 시 Material 의 Set_Shader에서 m_pMapEffectBuffers SRV 변경
Texture2D DiffuseMap;
Texture2D NormalMap;
Texture2D SpecularMap;
Texture2D OpacityMap;
Texture2D EmissiveMap;
Texture2D DissolveMap;
Texture2D DistortionMap;
Texture2D TextureMap7;
Texture2D TextureMap8;
Texture2D TextureMap9;
Texture2D TextureMap10;
Texture2D TextureMap11;
Texture2D TextureMap12;
Texture2D TextureMap13;
Texture2D TextureMap14;
Texture2D TextureMap15;


Texture2D SubMap0;
Texture2D SubMap1;
Texture2D SubMap2;
Texture2D SubMap3;
Texture2D SubMap4;
Texture2D SubMap5;
Texture2D SubMap6;
Texture2D SubMap7;
Texture2D SubMap8;
Texture2D SubMap9;
Texture2D SubMap10;
Texture2D SubMap11;
Texture2D SubMap12;
Texture2D SubMap13;
Texture2D SubMap14;
Texture2D SubMap15;

//////////////
// Function //
//////////////

LightColor CalculateLightColor(int lightIndex, float3 normal, float3 worldPosition)
{
    LightColor color = (LightColor) 0.f;
    
    float3 worldLightDir = 0.f;
    float3 worldLightPos = 0.f;
    float diffuseRatio = 0.f;
    float specularRatio = 0.f;
    float distanceRatio = 1.f;
    float emissiveRatio = 0.f;
    //DIRECTIONAL_LIGHT
    if(lights[lightIndex].lightType == 0)
    {
        worldLightDir = normalize(lights[lightIndex].vDirection.xyz);
        diffuseRatio = saturate(dot(-worldLightDir, normalize(normal)));
    }
    //POINT_LIGHT
    else if(lights[lightIndex].lightType == 1)
    {
        worldLightPos = lights[lightIndex].vPosition.xyz;
        worldLightDir = normalize(worldPosition - worldLightPos);
        diffuseRatio = saturate(dot(-worldLightDir, normalize(normal)));
        
        float dist = distance(worldPosition, worldLightPos);
        if(lights[lightIndex].range ==0.f)
            distanceRatio = 0.f;
        else
            distanceRatio = saturate(1.f - pow(dist / lights[lightIndex].range, 2));
    }
    //SPOT_LIGHT
    else
    {
        worldLightPos = lights[lightIndex].vPosition.xyz;
        worldLightDir = normalize(worldPosition - worldLightPos);
        diffuseRatio = saturate(dot(-worldLightDir, normalize(normal)));
        
        if(lights[lightIndex].range ==0.f)
            distanceRatio = 0.f;
        else
        {
            float halfAngle = lights[lightIndex].angle / 2.f;
            
            float3 LightVec = worldPosition - worldLightPos;
            float3 CenterLightDir = normalize(lights[lightIndex].vDirection.xyz);
            
            float centerDist = dot(LightVec, CenterLightDir);
            distanceRatio = saturate(1.f - centerDist / lights[lightIndex].range);
            
            float lightAngle = acos(dot(normalize(LightVec), CenterLightDir));
            
            if(centerDist < 0.f || centerDist > lights[lightIndex].range)
                distanceRatio = 0.f;
            else if (lightAngle > halfAngle)
                distanceRatio = 0.f;
            else
                distanceRatio = saturate(1.f - pow(centerDist / lights[lightIndex].range, 2));
            
        }
    }
    
    float3 reflectionDir = normalize(worldLightDir + 2 * normalize(normal) * (saturate(dot(-worldLightDir, normalize(normal)))));
    float3 cameraPosition = CameraPosition();
    float3 eyeDir = normalize(worldPosition - cameraPosition);
    
    
    specularRatio = pow(saturate(dot(-eyeDir, reflectionDir)),10);
    
    emissiveRatio =pow(smoothstep(0.f, 1.f, 1.f - saturate(dot(-eyeDir, normal))), 2);
    
    
    color.diffuse = lights[lightIndex].color.diffuse * diffuseRatio * distanceRatio;
    color.ambient = lights[lightIndex].color.ambient * distanceRatio;
    color.specular = lights[lightIndex].color.specular * specularRatio * distanceRatio;
    color.emissive = lights[lightIndex].color.emissive * emissiveRatio * distanceRatio;

    return color;
}

LightColor CalculateLightColor_ViewSpace(int lightIndex, float3 viewNormal, float3 viewPosition)
{
    LightColor color = (LightColor) 0.f;
    
    float3 viewLightDir = 0.f;
    float3 viewLightPos = 0.f;
    float diffuseRatio = 0.f;
    float specularRatio = 0.f;
    float distanceRatio = 1.f;
    float emissiveRatio = 0.f;
    //DIRECTIONAL_LIGHT
    if (lights[lightIndex].lightType == 0)
    {
        viewLightDir = normalize(mul(float4(lights[lightIndex].vDirection.xyz, 0.f), V).xyz);
        diffuseRatio = saturate(dot(-viewLightDir, normalize(viewNormal)));
        
       // diffuseRatio = ceil(diffuseRatio * 3) / 3.f;
        
    }
    //POINT_LIGHT
    else if (lights[lightIndex].lightType == 1)
    {
        viewLightPos = mul(float4(lights[lightIndex].vPosition.xyz, 1.f), V).xyz;
        viewLightDir = normalize(viewPosition - viewLightPos);
        diffuseRatio = saturate(dot(-viewLightDir, normalize(viewNormal)));
        
        float dist = distance(viewPosition, viewLightPos);
        if (lights[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
            distanceRatio = saturate(1.f - saturate(pow(dist / lights[lightIndex].range, 2)));
    }
    //SPOT_LIGHT
    else
    {
        viewLightPos = mul(float4(lights[lightIndex].vPosition.xyz, 1.f), V).xyz;
        viewLightDir = normalize(viewPosition - viewLightPos);
        diffuseRatio = saturate(dot(-viewLightDir, normalize(viewNormal)));
        
        if (lights[lightIndex].range == 0.f)
            distanceRatio = 0.f;
        else
        {
            float halfAngle = lights[lightIndex].angle / 2.f;
            
            float3 viewLightVec = viewPosition - viewLightPos;
            float3 viewCenterLightDir = normalize(mul(float4(lights[lightIndex].vDirection.xyz, 0.f), V).xyz);
            
            float centerDist = dot(viewLightVec, viewCenterLightDir);
            distanceRatio = saturate(1.f - centerDist / lights[lightIndex].range);
            
            float lightAngle = acos(dot(normalize(viewLightVec), viewCenterLightDir));
            
            if (centerDist < 0.f || centerDist > lights[lightIndex].range)
                distanceRatio = 0.f;
            else if (lightAngle > halfAngle)
                distanceRatio = 0.f;
            else
                distanceRatio = saturate(1.f - pow(centerDist / lights[lightIndex].range, 2));
            
        }
    }
    
    float3 reflectionDir = normalize(viewLightDir + 2 * dot(-viewLightDir, normalize(viewNormal)) * normalize(viewNormal));
    float3 cameraPosition = float3(0.f, 0.f, 0.f);
    float3 eyeDir = normalize(viewPosition - cameraPosition);
    
    
    specularRatio = pow(saturate(dot(-eyeDir, reflectionDir)), specularPower) +
    saturate(dot(viewNormal, -viewLightDir)) * pow(smoothstep(0.f, 1.f, 1.f - saturate(dot(-eyeDir, viewNormal))), rimPower);
    
    emissiveRatio = saturate(dot(viewNormal, -viewLightDir))
     * pow(smoothstep(0.f, 1.f, 1.f - saturate(dot(-eyeDir, viewNormal))), 2);
    
    
    color.diffuse = lights[lightIndex].color.diffuse * diffuseRatio * distanceRatio;
    color.ambient = lights[lightIndex].color.ambient * distanceRatio;
    color.specular = lights[lightIndex].color.specular * specularRatio * distanceRatio;
    color.emissive = lights[lightIndex].color.emissive * emissiveRatio * distanceRatio;

    return color;
}

float Cal_EffectLightDiffuse(int lightIndex, float3 viewNormal, float3 viewPosition)
{

    float3 viewLightDir = 0.f;
    float3 viewLightPos = 0.f;
    float diffuseRatio = 0.f;
    float specularRatio = 0.f;
    float distanceRatio = 1.f;
    float emissiveRatio = 0.f;
    //DIRECTIONAL_LIGHT
    if (lights[lightIndex].lightType == 0)
    {
        viewLightDir = normalize(mul(float4(lights[lightIndex].vDirection.xyz, 0.f), V).xyz);
        diffuseRatio = saturate(dot(-viewLightDir, normalize(viewNormal)));
        
        diffuseRatio = ceil(diffuseRatio * 3) / 3.f;
        
    }
   
    
   return diffuseRatio * distanceRatio;

   
}

LightColor CalculateLightColor_ViewSpace_VirtualLight(LightColor virtualLightColor,float3 virtualDir, float3 viewNormal, float3 viewPosition)
{
    LightColor color = (LightColor) 0.f;
    
    float3 viewLightDir = 0.f;
    float3 viewLightPos = 0.f;
    float diffuseRatio = 0.f;
    float specularRatio = 0.f;
    float distanceRatio = 1.f;
    float emissiveRatio = 0.f;
    //DIRECTIONAL_LIGHT

    viewLightDir = normalize(virtualDir);
    diffuseRatio = saturate(dot(-viewLightDir, normalize(viewNormal)));
    
    float3 reflectionDir = normalize(viewLightDir + 2 * normalize(viewNormal) * (saturate(dot(-viewLightDir, normalize(viewNormal)))));
    float3 cameraPosition = float3(0.f, 0.f, 0.f);
    float3 eyeDir = normalize(viewPosition - cameraPosition);

    color.diffuse = virtualLightColor.diffuse * diffuseRatio * distanceRatio;
    color.ambient =  virtualLightColor.ambient * distanceRatio;
    color.specular = virtualLightColor.specular * specularRatio * distanceRatio;
    color.emissive = virtualLightColor.emissive * emissiveRatio * distanceRatio;

    return color;
}



void ComputeNormalMapping_ViewSpace(inout float3 viewNormal, float3 viewTangent, float2 uv)
{
	// [0,255] 범위에서 [0,1]로 변환
 //   float4 map = NormalMap.Sample(LinearSampler, uv);
        
 //   if (any(map.rgb) == false)
 //       return;

 //   float3 N = normalize(viewNormal); // z
 //   float3 T = normalize(viewTangent); // x
 //   float3 B = normalize(cross(N, T)); // y
 //   float3x3 TBN = float3x3(T, B, N); // TS -> WS

	//// [0,1] 범위에서 [-1,1] 범위로 변환
 //   float3 tangentSpaceNormal = (map.rgb * 2.0f - 1.0f);
 //   viewNormal = normalize(mul(tangentSpaceNormal, TBN));
    float3 normalT = 2.f * NormalMap.Sample(LinearSampler, uv).rgb - 1.f;
    
    float3 N = normalize(viewNormal);
    float3 T = normalize(viewTangent - dot(viewTangent, N) * N);
    float3 B = cross(N, T);
    
    float3x3 TBN = float3x3(T, B, N);
    
    viewNormal = normalize(mul(normalT, TBN));
    
    
}

#endif