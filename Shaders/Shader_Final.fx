#include "Render.fx"
#include "Light.fx"

float g_AberrationPower;

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

VS_OUT VS_Final(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;

    return output;
}

float4 PS_Final(VS_OUT input) : SV_Target0
{
   // float4 output = SubMap0.Sample(LinearSampler, input.uv);
    float4 output = (float4) 0.f;

    output = SubMap0.Sample(LinearSampler, input.uv);

    output.rgb += g_brightness;

        // 대비 조절
    output.rgb = (output.rgb - 0.5) * g_contrast + 0.5;

        // 채도 조절
    float luminance = dot(output.rgb, float3(0.3, 0.59, 0.11));
    output.rgb = lerp(luminance, output.rgb, g_saturation);
    
    //output.rgb = pow(abs(output.rgb), 1.f / GAMMA);
    return output;
}

float4 PS_LumainanceToneMap(VS_OUT input) : SV_Target0
{
   // float4 output = SubMap0.Sample(LinearSampler, input.uv);
    float4 output = (float4) 0.f;

    output = SubMap0.Sample(LinearSampler, input.uv);
   // output.rgb = pow(output.rgb, g_gamma);
    // 밝기 조절
    output.rgb += g_brightness;

        // 대비 조절
    output.rgb = (output.rgb - 0.5) * g_contrast + 0.5;

        // 채도 조절
    float luminance = dot(output.rgb, float3(0.3, 0.59, 0.11));
    output.rgb = lerp(luminance, output.rgb, g_saturation);
    output.rgb = reinhard_extended_luminance(output.rgb,g_max_white);
    //output.rgb = pow(abs(output.rgb), 1.f / GAMMA);
    return output;
}

float4 PS_Uncharted2TMO(VS_OUT input) : SV_Target0
{
   // float4 output = SubMap0.Sample(LinearSampler, input.uv);
    float4 output = (float4) 0.f;

    output = SubMap0.Sample(LinearSampler, input.uv);
   // output.rgb = pow(output.rgb, g_gamma);
    output.rgb += g_brightness;

        // 대비 조절
    output.rgb = (output.rgb - 0.5) * g_contrast + 0.5;

        // 채도 조절
    float luminance = dot(output.rgb, float3(0.3, 0.59, 0.11));
    output.rgb = lerp(luminance, output.rgb, g_saturation);
    //output.rgb = pow(abs(output.rgb), 1.f / GAMMA);
    output.rgb = uncharted2_filmic(output.rgb);
    //output.rgb = pow(abs(output.rgb), 1.f / GAMMA);
    return output;
}

float4 PS_ACESTMO(VS_OUT input) : SV_Target0
{
   // float4 output = SubMap0.Sample(LinearSampler, input.uv);
    float4 output = (float4) 0.f;

    output = SubMap0.Sample(LinearSampler, input.uv);
    //output.rgb = pow(output.rgb, g_gamma);
    output.rgb += g_brightness;

        // 대비 조절
    output.rgb = (output.rgb - 0.5) * g_contrast + 0.5;

        // 채도 조절
    float luminance = dot(output.rgb, float3(0.3, 0.59, 0.11));
    output.rgb = lerp(luminance, output.rgb, g_saturation);
    
    //output.rgb = pow(abs(output.rgb), 1.f / GAMMA);
    
    output.rgb = aces_fitted(output.rgb);
    //output.rgb = pow(abs(output.rgb), 1.f / GAMMA);
    return output;
}

float4 PS_Aberration(VS_OUT input) : SV_Target
{
    uint width, height, numMips;
    SubMap0.GetDimensions(0, width, height, numMips);
    
    float2 texel = float2(1.f / (float) width, 1.f / (float) height);
   // float2 coords = (input.uv - 0.5f) * 2.f;
    
    float2 coords = abs(input.uv - 0.5f) * 2.f;
    coords *= -1.f;
    coords += 0.5f;
    
    float coordDot = dot(coords, coords);
    
    float2 preCompute = g_AberrationPower * coordDot * coords;
    float2 uvR = input.uv - texel.xy * preCompute;
    float2 uvB = input.uv + texel.xy * preCompute;
    
    float4 color;
    color.r = SubMap0.Sample(LinearSamplerClamp, uvR).r;
    color.g = SubMap0.Sample(LinearSamplerClamp, input.uv).g;
    color.b = SubMap0.Sample(LinearSamplerClamp, uvB).b;
    color.a = 1.f;
    
    return color;
    
}

float g_FocusDepth;
float g_DOFRange;

float4 PS_ExtractDOF(VS_OUT input) : SV_Target
{
    float3 originalColor = SubMap0.Sample(PointSampler, input.uv).rgb;

    return float4(originalColor, 1.f);
    
}

float4 PS_DOF(VS_OUT input) : SV_Target
{
    float viewZ = SubMap2.Sample(PointSampler, input.uv).z;
    
    float3 originalColor = SubMap0.Sample(PointSampler, input.uv).rgb;
    float3 blurColor = SubMap1.Sample(PointSampler, input.uv).rgb;
    
    float3 outColor = 0.f;
    
    //if ((1.f / g_DOFRange * abs(g_FocusDepth - depth)) >= 1.f)
    //    outColor = blurColor;
    //else
    //    outColor = originalColor;
    
    float blendFactor = saturate(1.f / g_DOFRange * abs(g_FocusDepth - viewZ));
 
    outColor = lerp(originalColor, blurColor, blendFactor);
    
    return float4(outColor, 1.f);
}

float g_fVignettePower;

float4 PS_Vignette(VS_OUT input) : SV_Target
{
    float4 originColor = SubMap0.Sample(LinearSampler, input.uv);
    
    float2 uv = input.uv;
    uv *= 1.f - uv.yx;
    float vig = uv.x * uv.y * 15.f;
    
    vig = pow(vig, g_fVignettePower);
    
    
    return originColor * vig;
}

cbuffer FogBuffer
{
    float fogStart;
    float fogEnd;
    float fogDensity;
    int fogMode;
    float4 fogColor;
};

float4 PS_Fog(VS_OUT input) : SV_Target
{
    float4 color = SubMap0.Sample(LinearSampler, input.uv);
    float viewZ = SubMap1.Sample(LinearSampler, input.uv).z;
    float fogFactor = 1.f;
    
    if(fogMode == 0)//Linear
        fogFactor = saturate((fogEnd - viewZ) / (fogEnd - fogStart));
    else if(fogMode == 1)
        fogFactor = 1.f / (pow(2.71828, viewZ * fogDensity));
    else if(fogMode == 2)
        fogFactor = 1.f / (pow(2.71828, viewZ * fogDensity * viewZ * fogDensity));
    
    color = fogFactor * color + (1.f - fogFactor) * fogColor;
    
    return float4(color.rgb, 1.f);
}

float4 PS_RenderFinal(VS_OUT input) : SV_Target
{
    return SubMap0.Sample(LinearSampler, input.uv);
}

technique11 T0
{
    Pass brightcontrast
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Final()));
    }
    Pass LumainanceToneMap
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_LumainanceToneMap()));
    }
    Pass Uncharted2TMO
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Uncharted2TMO()));
    }
    Pass ACESTMO
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_ACESTMO()));
    }
};

technique11 T1_AfterEffect
{
    Pass pass_Aberration
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Aberration()));
    }
    Pass pass_DOF
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_DOF()));
    }
    Pass pass_ExtractDOF
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_ExtractDOF()));
    }
    Pass pass_Vignette
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Vignette()));
    }
};

technique11 T2_RenderFinal
{
    Pass pass_RenderFinal
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_RenderFinal()));
    }
  
};

technique11 T3_Fog
{
    Pass pass_LinearFog
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_Fog()));
    }

};