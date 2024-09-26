#include "Render.fx"

#include "PBR.fx"

float g_ShadowBias;
float g_lightAttenuation = 1.f;
float g_ambientRatio = 0.5f;

float CalcShadowFactor(float4 shadowPosH)
{
  //w로 나누어서 투영을 완료한다.
    shadowPosH.xyz /= shadowPosH.w;
    float2 uv = shadowPosH.xy;
    uv.y = -uv.y;
    uv = uv * 0.5f + 0.5f;
  //NDC 공간 기준의 깊이 값
    float depth = shadowPosH.z;

    if(uv.x < 0.f || uv.x > 1.f || uv.y <0.f || uv.y > 1.f)
        return 1.f;
    
    uint width, height, numMips;
    SubMap2.GetDimensions(0, width, height, numMips);

  //텍셀 크기
    float dx = 1.0f / (float) width;

    float percentLit = 0.0f;
    
        //const float2 offsets[25] =
    //{
    //    float2(-2.f, -2.f), float2(-1.f, -2.f), float2(0.f, -2.f), float2(1.f, -2.f), float2(2.f, -2.f),
    //    float2(-2.f, -1.f), float2(-1.f, -1.f), float2(0.f, -1.f), float2(1.f, -1.f), float2(2.f, -1.f),
    //    float2(-2.f, -0.f), float2(-1.f, -0.f), float2(0.f, -0.f), float2(1.f, -0.f), float2(2.f, -0.f),
    //    float2(-2.f, +1.f), float2(-1.f, +1.f), float2(0.f, +1.f), float2(1.f, +1.f), float2(2.f, +1.f),
    //    float2(-2.f, +2.f), float2(-1.f, +2.f), float2(0.f, +2.f), float2(1.f, +2.f), float2(2.f, +2.f)
    //};
    
    const float2 offsets[9] =
    {
       float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
    float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
    float2(-dx, dx), float2(0.0f, dx), float2(dx, dx)
    };

  [unroll]
    for (int i = 0; i < 9; ++i)
    {  
        percentLit += SubMap2.SampleCmpLevelZero(samShadow, uv + offsets[i], depth).r;
    }
     percentLit /= 9.0f;
    
    
    return percentLit;
}

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

struct PS_LIGHT_Deferred
{
    float4 ambient : SV_Target0;
    float4 diffuse : SV_Target1;
    float4 specular : SV_Target2;
    float4 emissive : SV_Target3;
};


//SubMap0 : PositionRT
//SubMap1 : NormalRT
//SubMap2 : ShadowRT
float2 RenderTargetResolution;
row_major float4x4 ShadowCameraVP;

//Mesh : Rect
VS_OUT VS_DirLight(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;
    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;
    
    return output;
}

PS_LIGHT_Deferred PS_DirLight(VS_OUT input)
{
    PS_LIGHT_Deferred output = (PS_LIGHT_Deferred) 0.f;
    
    float3 viewPos = SubMap0.Sample(LinearSampler, input.uv).xyz;
    if(viewPos.z <= 0.f)
        clip(-1);
    
    float3 viewNormal = SubMap1.Sample(LinearSampler, input.uv).xyz;
    
    LightColor color = CalculateLightColor_ViewSpace(lightIndex, viewNormal, viewPos);
  
    if(length(color.diffuse) !=0)
    {
        float4 worldPos = mul(float4(viewPos.xyz, 1.f), VInv);
        float4 shadowClipPos = mul(worldPos, ShadowCameraVP);
        float shadowFactor = CalcShadowFactor(shadowClipPos);

        color.diffuse = color.diffuse * shadowFactor;
        color.specular = color.specular * shadowFactor;

      
    }
    if(length(color.ambient) != 0 && g_SSAO_On)
    {
        float ssAO = SubMap3.Sample(LinearSampler, input.uv).r;
        
        color.ambient = color.ambient * ssAO;
    }


    output.ambient = color.ambient;
    output.ambient.a = 1.f;
    output.diffuse = color.diffuse;
    output.diffuse.a = 1.f;
    output.specular = color.specular;
    output.specular.a = 1.f;
    output.emissive = color.emissive;
    output.emissive.a = 1.f;
    
    return output;
}

//Mesh : Sphere
VS_OUT VS_PointLight(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.pos = mul(float4(input.pos * 0.5f, 1.f), W);

    output.pos = mul(output.pos, VP);
    output.uv = input.uv;
    
    
    return output;
}

PS_LIGHT_Deferred PS_PointLight(VS_OUT input)
{
    PS_LIGHT_Deferred output = (PS_LIGHT_Deferred) 0.f;
    
    // input.pos = SV_Position = Screen좌표
    float2 uv = float2(input.pos.x / RenderTargetResolution.x, input.pos.y / RenderTargetResolution.y);
    
    float3 viewPos = SubMap0.Sample(LinearSampler, uv).xyz;
    if (viewPos.z <= 0.f)
        clip(-1);
    
    float3 viewLightPos = mul(float4(lights[lightIndex].vPosition.xyz, 1.f), V).xyz;
    float distance = length(viewPos - viewLightPos);
    if(distance > lights[lightIndex].range)
        clip(-1);
    
    float3 viewNormal = SubMap1.Sample(LinearSampler, uv).xyz;
    
    LightColor color = CalculateLightColor_ViewSpace(lightIndex, viewNormal, viewPos);
    
    if (length(color.ambient) != 0 && g_SSAO_On)
    {
        float ssAO = SubMap3.Sample(LinearSampler, uv).r;
      
        color.ambient = color.ambient * ssAO;
    }
    output.ambient = color.ambient;
    output.ambient.a = 1.f;
    output.diffuse = color.diffuse;
    output.diffuse.a = 1.f;
    output.specular = color.specular;
    output.specular.a = 1.f;
    output.emissive = color.emissive;
    output.emissive.a = 1.f;
    
    return output;
}

VS_OUT VS_SpotLight(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    row_major float4x4 scale = IDENTITY_MATRIX;
    float halfAngle = lights[lightIndex].angle / 2.f;
    
    float scaleRatio = 1.f/cos(halfAngle);
    
    m_scale(scale, float3(scaleRatio, scaleRatio, scaleRatio));
    
    output.pos = mul(float4(input.pos, 1.f), mul(scale, W));
    output.pos = mul(output.pos, VP);
    output.uv = input.uv;
    
    
    return output;
}

PS_LIGHT_Deferred PS_SpotLight(VS_OUT input)
{
    PS_LIGHT_Deferred output = (PS_LIGHT_Deferred) 0.f;
    
    // input.pos = SV_Position = Screen좌표
    float2 uv = float2(input.pos.x / RenderTargetResolution.x, input.pos.y / RenderTargetResolution.y);
    
    float3 viewPos = SubMap0.Sample(LinearSampler, input.uv).xyz;
    if (viewPos.z <= 0.f)
        clip(-1);
    
    float3 viewLightPos = mul(float4(lights[lightIndex].vPosition.xyz, 1.f), V).xyz;
    float distance = length(viewPos - viewLightPos);
    
    float halfAngle = lights[lightIndex].angle / 2.f;
    
    float scaleRatio = 1.f / cos(halfAngle);
    
    if (distance > lights[lightIndex].range * scaleRatio)
        clip(-1);
    
    float3 viewNormal = SubMap1.Sample(LinearSampler, input.uv).xyz;
    
    LightColor color = CalculateLightColor_ViewSpace(lightIndex, viewNormal, viewPos);
    
    if (length(color.ambient) != 0 && g_SSAO_On)
    {
        float ssAO = SubMap3.Sample(LinearSampler, input.uv).r;
        
        color.ambient = color.ambient * ssAO;
    }
    output.ambient = color.ambient;
    output.ambient.a = 1.f;
    output.diffuse = color.diffuse;
    output.diffuse.a = 1.f;
    output.specular = color.specular;
    output.specular.a = 1.f;
    output.emissive = color.emissive;
    output.emissive.a = 1.f;
    
    return output;
}

//Final

//SubMap1 : DiffuseColor
//SubMap2 : SpecularColor
//SubMap3 : EmissiveColor
//SubMap4 : AmbientLightColor
//SubMap5 : DiffuseLightColor
//SubMap6 : SpecularLightColor
//SubMap7 : EmissiveLightColor

VS_OUT VS_Final(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;

    return output;
}

float4 PS_Final(VS_OUT input) : SV_Target
{
    float4 output = (float4) 0.f;
    
    float3 viewPos = SubMap0.Sample(LinearSampler, input.uv).xyz;
    
    float4 diffuseColor = SubMap1.Sample(LinearSampler, input.uv);

    float4 specularColor = SubMap2.Sample(LinearSampler, input.uv);

    float4 emissiveColor = SubMap3.Sample(LinearSampler, input.uv);

   
    float4 ambientLightColor = SubMap4.Sample(LinearSampler, input.uv);

    float4 diffuseLightColor = SubMap5.Sample(LinearSampler, input.uv);

    float4 specularLightColor = SubMap6.Sample(LinearSampler, input.uv);

    float4 emissiveLightColor = SubMap7.Sample(LinearSampler, input.uv);

   
    diffuseColor = diffuseColor * saturate(diffuseLightColor + ambientLightColor);
    specularColor *= specularLightColor;
    emissiveColor *= emissiveLightColor;
    
    float vDistToEye = length(viewPos);
    float3 vToEye = normalize(viewPos);
        
    output = diffuseColor 
    + specularColor
    + emissiveColor;
   // +emissiveBlurColor * emissiveLightColor;
    

    
    output.a = 1.f;
    return output;
}

// Diffuse


//float3 PBRShade(
//    in float3 ambientMap,
//    in float3 albedoMap,
//    in float roughnessMap,
//    in float metallicMap,
//    in float3 normal,
//    in float3 pointToLights,
//    in float3 pointToCamera,
//    in float3 lightColor,
//    in float lightAttenuation,
//    in float shadowAmount
//)
//{
//    float3 ambient = ambientMap * albedoMap * 0.2f;
//    float3 color = float3(0.f, 0.f, 0.f);


//    float3 halfVector = normalize(pointToLights + pointToCamera);
    
//    float3 diffuse = CalculateLambertianBRDF(albedoMap);
    
//    float3 F0 = float3(0.04f, 0.04f, 0.04f); //일반적인 프레넬 상수수치를 0.04로 정의
//    F0 = lerp(F0, albedoMap, metallicMap);
//    float3 F = FresnelSchlick(max(dot(halfVector, pointToCamera), 0.0), F0); //반사정도 정의
    
//    float3 kS = F; //Specular상수
//    float3 kD = float3(1.f, 1.f, 1.f) - kS; //Diffuse 상수
//    kD = kD * float3(1.f - metallicMap, 1.f - metallicMap, 1.f - metallicMap); //Diffuse에 metallic반영
    
//    float3 specular = CalculateCookTorranceBRDF(normal, pointToCamera, halfVector, pointToLights, roughnessMap, F);
   
//    //rad = (((kD * albedo / PI) + specular) * radiance * WiDotN);

//    { //그림자 효과 반영
//        diffuse = saturate(diffuse * shadowAmount);
//        specular = saturate(specular * shadowAmount);
//    }
//    float NdotL = max(dot(normal, pointToLights), 0.0);
//    color += (kD * diffuse + specular) * lightColor * 50.f * NdotL;

//    color += ambient;

//    return color;
        
//}

//float3 PBRShadePointLightRange(
//    in float3 ambientMap,
//    in float3 albedoMap,
//    in float roughnessMap,
//    in float metallicMap,
//    in float3 normal,
//    in float3 pointToLight,
//    in float3 pointToCamera,
//    in float3 lightColor,
//    in float lightRange,
//    in float distance
//)
//{
//    float3 ambient = ambientMap * albedoMap * 0.2f;
//    float3 color = float3(0.f, 0.f, 0.f);

//    float3 halfVector = normalize(pointToLight + pointToCamera);

//    float3 diffuse = CalculateLambertianBRDF(albedoMap);

//    float3 F0 = float3(0.04f, 0.04f, 0.04f);
//    F0 = lerp(F0, albedoMap, metallicMap);
//    float3 F = FresnelSchlick(max(dot(halfVector, pointToCamera), 0.0), F0);

//    float3 kS = F;
//    float3 kD = float3(1.f, 1.f, 1.f) - kS;
//    kD = kD * float3(1.f - metallicMap, 1.f - metallicMap, 1.f - metallicMap);

//    float3 specular = CalculateCookTorranceBRDF(normal, pointToCamera, halfVector, pointToLight, roughnessMap, F);

//    float NdotL = max(dot(normal, pointToLight), 0.0);

//    // Distance attenuation
//    float attenuation = 1.0 / (1.0 + 0.1 * distance);

//// 거리가 lightRange와 같아질 때, 감쇠가 0이 되도록 보정
//    attenuation *= saturate(1.f - saturate(pow(distance / lightRange, 2)));;
//    {
//        color += (kD * diffuse + specular) * lightColor * 50.f * attenuation * NdotL;

//        color += ambient * attenuation;
//    }
    
    

//    return color;
//}

//float3 PBRShadeSpotLight(
//    in float3 ambientMap,
//    in float3 albedoMap,
//    in float roughnessMap,
//    in float metallicMap,
//    in float3 normal,
//    in float3 pointToLight,
//    in float3 pointToCamera,
//    in float3 lightColor,
//    in float lightAttenuation,
//    in float3 lightDirection,
//    in float spotAngle,
//    in float spotExponent
//)
//{
//    float3 ambient = ambientMap * albedoMap * g_ambientRatio;
//    float3 color = float3(0.f, 0.f, 0.f);

//    float3 halfVector = normalize(pointToLight + pointToCamera);

//    float3 diffuse = CalculateLambertianBRDF(albedoMap);

//    float3 F0 = float3(0.04f, 0.04f, 0.04f);
//    F0 = lerp(F0, albedoMap, metallicMap);
//    float3 F = FresnelSchlick(max(dot(halfVector, pointToCamera), 0.0), F0);

//    float3 kS = F;
//    float3 kD = float3(1.f, 1.f, 1.f) - kS;
//    kD = kD * float3(1.f - metallicMap, 1.f - metallicMap, 1.f - metallicMap);

//    float3 specular = CalculateCookTorranceBRDF(normal, pointToCamera, halfVector, pointToLight, roughnessMap, F);

//    float NdotL = max(dot(normal, pointToLight), 0.0);
    
//    // Spotlight cone calculations
//    float spotlightEffect = 0.0;
//    float3 lightDir = normalize(-lightDirection);
//    float spotCos = dot(lightDir, normalize(-pointToLight));
    
//    if (spotCos > cos(spotAngle))
//    {
//        // Inside the spotlight cone
//        spotlightEffect = pow(spotCos, spotExponent);
//    }

//    color += (kD * diffuse + specular) * lightColor * lightAttenuation * NdotL * spotlightEffect;

//    color += ambient;

//    return color;
//}

//SubMap0 -> viewPos
//SubMap1 -> viewNormal
//SubMap2 -> Shadow
//SubMap3 -> SSAO
//SubMap4 -> diffuse
//SubMap5 -> ARM



PBR_OUT PS_PBR_DirLight(VS_OUT input)
{
    PBR_OUT output = (PBR_OUT) 0.f;
    
    float3 viewPos = SubMap0.Sample(LinearSampler, input.uv).xyz;
    if(viewPos.z <= 0.f)
        clip(-1);
    
    float3 viewNormal = SubMap1.Sample(LinearSampler, input.uv).xyz;
    
    float3 diffuseColor = SubMap4.Sample(LinearSampler, input.uv).xyz;
    
    float3 ambientColor = SubMap5.Sample(LinearSampler, input.uv).xxx;
    float roughness = SubMap5.Sample(LinearSampler, input.uv).y;
    float metalic = SubMap5.Sample(LinearSampler, input.uv).z;
    if (g_SSAO_On)
        ambientColor = SubMap3.Sample(LinearSampler, input.uv).x;
    
    float4 worldPos = mul(float4(viewPos.xyz, 1.f), VInv);
    float4 shadowClipPos = mul(worldPos, ShadowCameraVP);
    float shadowFactor = CalcShadowFactor(shadowClipPos);
    
    output = PBRShade(
    ambientColor,
    diffuseColor,
    roughness,
    metalic,
    viewNormal,
    viewPos,
    lights[lightIndex].color.diffuse.xyz,
   shadowFactor
    );
      
    return output;
}
//SubMap0 -> viewPos
//SubMap1 -> viewNormal
//SubMap2 -> Shadow
//SubMap3 -> SSAO
//SubMap4 -> diffuse
//SubMap5 -> ARM
PBR_OUT PS_PBR_PointLight(VS_OUT input)
{
    PBR_OUT output = (PBR_OUT) 0.f;
    float2 uv = float2(input.pos.x / RenderTargetResolution.x, input.pos.y / RenderTargetResolution.y);

    float3 viewPos = SubMap0.Sample(LinearSampler, uv).xyz;
    if (viewPos.z <= 0.f)
        clip(-1);
    
    float3 viewNormal = SubMap1.Sample(LinearSampler, uv).xyz;
    
    float3 diffuseColor = SubMap4.Sample(LinearSampler, uv).xyz;
    
    float ambientColor = SubMap5.Sample(LinearSampler, uv).x;
    float roughness = SubMap5.Sample(LinearSampler, uv).y;
    float metalic = SubMap5.Sample(LinearSampler, uv).z;
    if (g_SSAO_On)
        ambientColor = SubMap3.Sample(LinearSampler, uv).x;

    output = PBRShade(
    ambientColor,
    diffuseColor,
    roughness,
    metalic,
    viewNormal,
    viewPos,
    lights[lightIndex].color.diffuse.xyz,
    1.f
    );

    
    return output;
}

PBR_OUT PS_PBR_SpotLight(VS_OUT input)
{
    PBR_OUT output = (PBR_OUT) 0.f;
    float2 uv = float2(input.pos.x / RenderTargetResolution.x, input.pos.y / RenderTargetResolution.y);

    float3 viewPos = SubMap0.Sample(LinearSampler, uv).xyz;
    if (viewPos.z <= 0.f)
        clip(-1);
    
    float3 viewNormal = SubMap1.Sample(LinearSampler, uv).xyz;
    
    float3 diffuseColor = SubMap4.Sample(LinearSampler, uv).xyz;
    
    float ambientColor = SubMap5.Sample(LinearSampler, uv).x;
    float roughness = SubMap5.Sample(LinearSampler, uv).y;
    float metalic = SubMap5.Sample(LinearSampler, uv).z;
    if (g_SSAO_On)
        ambientColor = SubMap3.Sample(LinearSampler, uv).x;

    
    float3 viewLightPos = mul(float4(lights[lightIndex].vPosition.xyz, 1.f), V).xyz;
    float3 pointToLight = normalize(viewLightPos - viewPos);
    float3 cameraPosition = float3(0.f, 0.f, 0.f);
    float3 pointToCamera = normalize(cameraPosition - viewPos);
        
    output = PBRShade(
    ambientColor,
    diffuseColor,
    roughness,
    metalic,
    viewNormal,
    viewPos,
    lights[lightIndex].color.diffuse.xyz,
    1.f
    );
  
    return output;
}

float4 PS_PBR_Final(VS_OUT input) : SV_Target
{
    float4 ambientLightColor = SubMap4.Sample(LinearSampler, input.uv);
    
    float4 rimColor = SubMap3.Sample(LinearSampler, input.uv);
    float4 emissiveColor = SubMap9.Sample(LinearSampler, input.uv);
    float4 rimLightColor = SubMap7.Sample(LinearSampler, input.uv);
    
    rimColor *= rimLightColor;
    float luminance = dot(rimColor.rgb, float3(0.299, 0.587, 0.114));
    rimColor.rgb = lerp(rimColor.rgb, rimColor.rgb * 20.f, saturate(luminance));
    
    float4 output = 0.f;
    output = rimColor + emissiveColor + ambientLightColor;
        
    return float4(output.xyz,1.F);
}


technique11 t0
{
    pass dirLight
    {
        SetVertexShader(CompileShader(vs_5_0, VS_DirLight()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(AdditiveBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_DirLight()));
    }
    pass pointLight
    {
        SetVertexShader(CompileShader(vs_5_0, VS_PointLight()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(AdditiveBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PointLight()));
    }
    pass spotLight
    {
        SetVertexShader(CompileShader(vs_5_0, VS_SpotLight()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(AdditiveBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_SpotLight()));
    }
    pass Final
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Final()));
    }
}

technique11 t1
{
    pass dirLight
    {
        SetVertexShader(CompileShader(vs_5_0, VS_DirLight()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(AdditiveBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_DirLight()));
    }
    pass pointLight
    {
        SetVertexShader(CompileShader(vs_5_0, VS_PointLight()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(AdditiveBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_PointLight()));
    }
    pass spotLight
    {
        SetVertexShader(CompileShader(vs_5_0, VS_SpotLight()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(AdditiveBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_SpotLight()));
    }
    pass Final
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_PBR_Final()));
    }
}