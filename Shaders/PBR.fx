#ifndef _PBR_FX_
#define _PBR_FX_

#include "Global.fx"
#include "Light.fx"

float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 FresnelSchlickRoughness(float cosTheta, float3 F0, float roughness)
{
    return F0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)  // k is a remapping of roughness based on direct lighting or IBL lighting
{
    float r = roughness + 1.0f;
    float k = (r * r) / 8.0f;

    //float k = (roughness * roughness) / 2.0f;

    float nom = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

float GeometrySmith(float3 n, float3 v, float3 l, float k)
{
    //  Geometry Obstruction
    float NdotV = saturate(dot(n, v));
    //  Geometry Shadowing
    float NdotL = saturate(dot(n, l));

    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);

    return ggx1 * ggx2;
}

// Diffuse
float3 CalculateLambertianBRDF(in float3 albedo)
{ //램버시안 표면의 난반사 BRDF계산
    return max(albedo / PI, 0.0);

}

//Specular
float NormalDistributionGGXTR(float3 n, float3 h, float a)
{
    float a2 = a * a;
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / denom;
}


float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return (F0 + (1.0f - F0) * pow(1.0 - cosTheta, 5.0f));
}

float3 CalculateCookTorranceBRDF(
        in float3 normal,
        in float3 pointToCamera,
        in float3 halfVector,
        in float3 pointToLight,
        in float roughness,
        in float3 F
    )
{
    float D = NormalDistributionGGXTR(normal, halfVector, roughness); //미세면 분포도 NDF계산
    float G = GeometrySmith(normal, pointToCamera, pointToLight, roughness); //미세면 그림자 계산
                
    float3 numerator = D * G * F;
    float denominator = 4.0 * saturate(dot(normal, pointToCamera)) * saturate(dot(normal, pointToLight)) + 0.0001f;
    float3 specular = numerator / denominator;
                
    return specular;
}

struct PBR_OUTPUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 diffuseColor : SV_Target3;
    float4 arm : SV_Target4;
    float4 rimColor : SV_Target5;
    float4 blur : SV_Target6;
    float4 emissive : SV_Target7;
};

struct PBR_MAPOBJECT_OUTPUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 positionSSD : SV_Target2;
    float4 diffuseColor : SV_Target3;
    float4 arm : SV_Target4;
    float4 rimColor : SV_Target5;
    float4 blur : SV_Target6;
    float4 emissive : SV_Target7;
};

struct PBR_OUT
{
    float4 outColor : SV_Target0;
    float4 emissiveColor : SV_Target3;
};

int lightIndex;

float3 CalculateRimLight(in float3 normal, in float3 pointToCamera)
{

    float rimThreshold = 0.5f;
    float rimIntensity = 1.0f;
    float rimAttenuation = 4.f;

    float rim = saturate(dot(normal, pointToCamera) - rimThreshold);
    rim = pow(rim, rimAttenuation);

    return rim * rimIntensity;
}

PBR_OUT PBRShade(
    in float3 ambientMap,
    in float3 albedoMap,
    in float roughnessMap,
    in float metallicMap,
    in float3 viewNormal,
    in float3 viewPosition,
    in float3 lightColor,
    in float shadowAmount
)
{   
    float3 viewLightPos = mul(float4(lights[lightIndex].vPosition.xyz, 1.f), V).xyz;
    float3 pointToLight = normalize(viewLightPos - viewPosition);
    float3 cameraPosition = 0.f;
    float3 pointToCamera = normalize(cameraPosition - viewPosition);
    float3 viewLightDir = 0.f;
    
    float3 ambient = ambientMap * albedoMap * 0.2f;
    float3 color = 0.f;
     float3 eyeDir = normalize(viewPosition - cameraPosition);
    float3 halfVector = normalize(pointToLight + pointToCamera);
    
    float3 diffuse = CalculateLambertianBRDF(albedoMap);
    
    float3 F0 = float3(0.04f, 0.04f, 0.04f); 
    F0 = lerp(F0, albedoMap, metallicMap);
    float3 F = FresnelSchlick(max(dot(halfVector, pointToCamera), 0.0), F0);
    
    float3 kS = F; 
    float3 kD = float3(1.f, 1.f, 1.f) - kS; 
    kD = kD * float3(1.f - metallicMap, 1.f - metallicMap, 1.f - metallicMap);
    
    float3 specular = CalculateCookTorranceBRDF(viewNormal, pointToCamera, halfVector, pointToLight, roughnessMap, F);
   
    float NdotL = max(dot(viewNormal, pointToLight), 0.0);
    float attenuation = 1.f;
       //DIRECTIONAL_LIGHT
    if (lights[lightIndex].lightType == 0)
    {
        viewLightDir = normalize(mul(float4(lights[lightIndex].vDirection.xyz, 0.f), V).xyz);
        diffuse = saturate(diffuse * shadowAmount);
        specular = saturate(specular * shadowAmount);
    }
    //POINT_LIGHT
    else if (lights[lightIndex].lightType == 1)
    {
        viewLightDir = normalize(viewPosition - viewLightPos);
        float distance = length(viewLightPos - viewPosition);
        attenuation *= saturate(1.f - saturate(pow(distance / lights[lightIndex].range, 2)));;
        attenuation *= 2.f;

    }
        //SPOT_LIGHT
    else
    {
        float spotlightEffect = 0.0;
        float3 lightDir = normalize(-lights[lightIndex].vDirection.xyz);
        float spotCos = dot(lightDir, normalize(-pointToLight));
        viewLightDir = normalize(viewPosition - viewLightPos);
        if (spotCos > cos(lights[lightIndex].angle))
        {
        // Inside the spotlight cone
            attenuation *= pow(spotCos, 2);
        }
        else
            attenuation = 0.f;

    }
    color += (kD * diffuse + specular) * lightColor * 10.f * attenuation * NdotL;
    color += ambient * attenuation;

    PBR_OUT output = (PBR_OUT) 0.f;
    output.outColor = float4(color, 1.f);
    
    output.emissiveColor = 1.f //* saturate(dot(viewNormal, -viewLightDir))
     * pow(smoothstep(0.f, 1.f, 1.f - saturate(dot(-eyeDir, viewNormal))), 2) * attenuation;
    
    return output;
        
}

float4 PBRShadeWater(
    in float3 ambientMap,
    in float3 albedoMap,
    in float roughnessMap,
    in float metallicMap,
    in float3 viewNormal,
    in float3 viewPosition,
    in float3 lightColor,
    in float shadowAmount
)
{
    float3 viewLightPos = mul(float4(lights[0].vPosition.xyz, 1.f), V).xyz;
    float3 pointToLight = normalize(viewLightPos - viewPosition);
    float3 cameraPosition = 0.f;
    float3 pointToCamera = normalize(cameraPosition - viewPosition);
    float3 viewLightDir = 0.f;
    
    float3 ambient = ambientMap * albedoMap * 0.2f;
    float3 color = 0.f;
    float3 eyeDir = normalize(viewPosition - cameraPosition);
    float3 halfVector = normalize(pointToLight + pointToCamera);
    
    float3 diffuse = CalculateLambertianBRDF(albedoMap);
    
    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedoMap, metallicMap);
    float3 F = FresnelSchlick(max(dot(halfVector, pointToCamera), 0.0), F0);
    
    float3 kS = F;
    float3 kD = float3(1.f, 1.f, 1.f) - kS;
    kD = kD * float3(1.f - metallicMap, 1.f - metallicMap, 1.f - metallicMap);
    
    float3 specular = CalculateCookTorranceBRDF(viewNormal, pointToCamera, halfVector, pointToLight, roughnessMap, F);
   
    float NdotL = max(dot(viewNormal, pointToLight), 0.0);
    float attenuation = 1.f;
       //DIRECTIONAL_LIGHT

    viewLightDir = normalize(mul(float4(lights[0].vDirection.xyz, 0.f), V).xyz);
    diffuse = saturate(diffuse * shadowAmount);
    specular = saturate(specular * shadowAmount);


    color += (kD * diffuse + specular) * lightColor * 100.f * attenuation * NdotL;
   
    return float4(color, 1.f);
}


#endif