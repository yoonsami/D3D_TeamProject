#include "Render.fx"
#include "Light.fx"
int g_BlurCount;


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

static const float Weight[9] =
{
    0.075, 0.125, 0.075,
  0.125, 0.200, 0.125,
  0.075, 0.125, 0.075,
};

float GaussianWeight[25];

static const float Total = 1.;

//Mesh : Rect
VS_OUT VS_MAIN(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0.f;
    output.pos = float4(input.pos * 2.f, 1.f);
    output.uv = input.uv;
    
    return output;
}

float4 PS_DOWN(VS_OUT input) : SV_Target0
{
    float4 Out = 0;

    uint m_TexW, m_TexH, numMips;
    SubMap0.GetDimensions(0, m_TexW, m_TexH, numMips);
    

    float tu = 1.f / m_TexW;
    float tv = 1.f / m_TexH;

    float2 offset[25] =
    {
         float2(-2.0f * tu, 2.0f * tv), float2(-1.0f * tu, 2.0f * tv), float2(0.0f * tu, 2.0f * tv), float2(1.0f * tu, 2.0f * tv), float2(2.0f * tu, 2.0f * tv) ,
         float2(-2.0f * tu, 1.0f * tv), float2(-1.0f * tu, 1.0f * tv), float2(0.0f * tu, 1.0f * tv), float2(1.0f * tu, 1.0f * tv), float2(2.0f * tu, 1.0f * tv) ,
         float2(-2.0f * tu, 0.0f * tv), float2(-1.0f * tu, 0.0f * tv), float2(0.0f * tu, 0.0f * tv), float2(1.0f * tu, 0.0f * tv), float2(2.0f * tu, 0.0f * tv) ,
         float2(-2.0f * tu, -1.0f * tv), float2(-1.0f * tu, -1.0f * tv), float2(0.0f * tu, -1.0f * tv), float2(1.0f * tu, -1.0f * tv), float2(2.0f * tu, -1.0f * tv) ,
         float2(-2.0f * tu, -2.0f * tv), float2(-1.0f * tu, -2.0f * tv), float2(0.0f * tu, -2.0f * tv), float2(1.0f * tu, -2.0f * tv), float2(2.0f * tu, -2.0f * tv) 
    };
    
    
    
    //float3 a = SubMap0.Sample(LinearSampler, input.uv + float2(-2.f * tu, 2.f * tv)).rgb;
    //float3 b = SubMap0.Sample(LinearSampler, input.uv + float2(0.f, 2 * tv)).rgb;
    //float3 c = SubMap0.Sample(LinearSampler, input.uv + float2(2.f * tu, 2.f * tv)).rgb;
    
    //float3 d = SubMap0.Sample(LinearSampler, input.uv + float2(-2.f * tu, 0.f)).rgb;
    //float3 e = SubMap0.Sample(LinearSampler, input.uv + float2(0.f, 0.f)).rgb;
    //float3 f = SubMap0.Sample(LinearSampler, input.uv + float2(2.f * tu, 0.f)).rgb;
    
    //float3 g = SubMap0.Sample(LinearSampler, input.uv + float2(-2.f * tu, -2.f * tv)).rgb;
    //float3 h = SubMap0.Sample(LinearSampler, input.uv + float2(0.f, -2.f * tv)).rgb;
    //float3 i = SubMap0.Sample(LinearSampler, input.uv + float2(2.f * tu, -2.f * tv)).rgb;

    //float3 j = SubMap0.Sample(LinearSampler, input.uv + float2(-tu,+tv)).rgb;
    //float3 k = SubMap0.Sample(LinearSampler, input.uv + float2(+tu,+tv)).rgb;
    //float3 l = SubMap0.Sample(LinearSampler, input.uv + float2(-tu,-tv)).rgb;
    //float3 m = SubMap0.Sample(LinearSampler, input.uv + float2(+tu,-tv)).rgb;
    float3 vDownSample = 0;
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            int index = i * 5 + j;
            vDownSample += SubMap0.Sample(LinearSamplerMirror, input.uv + offset[index]).rgb * GaussianWeight[index];
        }
    }
    
    //float3 vDownSample = e * 0.125;
    //vDownSample += (a + c + g + i) * 0.03125;
    //vDownSample += (b + d + f + h) * 0.0625;
    //vDownSample += (j + k + l + m) * 0.125;
    
    Out = float4(vDownSample, 1.f);

    return Out;
    
    
}
float4 PS_JUSTDOWN(VS_OUT input) : SV_Target0
{
  

    return SubMap0.Sample(LinearSamplerMirror, input.uv);
    
    
}

float4 PS_UP(VS_OUT input) : SV_Target0
{
    float4 Out = 0;

    uint m_TexW, m_TexH, numMips;
    SubMap0.GetDimensions(0, m_TexW, m_TexH, numMips);
    

    float tu = 1.f / m_TexW;
    float tv = 1.f / m_TexH;

    float3 a = SubMap0.Sample(LinearSamplerMirror, clamp(float2(input.uv.x - tu, input.uv.y + tv), 0, 1)).rgb;
    float3 b = SubMap0.Sample(LinearSamplerMirror, clamp(float2(input.uv.x, input.uv.y + tv), 0, 1)).rgb;
    float3 c = SubMap0.Sample(LinearSamplerMirror, clamp(float2(input.uv.x + tu, input.uv.y + tv), 0, 1)).rgb;
    
    float3 d = SubMap0.Sample(LinearSamplerMirror, clamp(float2(input.uv.x - tu, input.uv.y), 0, 1)).rgb;
    float3 e = SubMap0.Sample(LinearSamplerMirror, clamp(float2(input.uv.x, input.uv.y), 0, 1)).rgb;
    float3 f = SubMap0.Sample(LinearSamplerMirror, clamp(float2(input.uv.x + tu, input.uv.y), 0, 1)).rgb;
    
    float3 g = SubMap0.Sample(LinearSamplerMirror, clamp(float2(input.uv.x - tu, input.uv.y - tv), 0, 1)).rgb;
    float3 h = SubMap0.Sample(LinearSamplerMirror, clamp(float2(input.uv.x, input.uv.y - tv), 0, 1)).rgb;
    float3 i = SubMap0.Sample(LinearSamplerMirror, clamp(float2(input.uv.x + tu, input.uv.y - tv), 0, 1)).rgb;
    
    float3 vUpSample = e * 4.f;
    vUpSample += (b + d + f + h) * 2.f;
    vUpSample += (a + c + g + i);
    vUpSample *= 1.f / 16.f;
    Out = float4(vUpSample, 1.f);

    return Out;
}

float4 PS_UPGaussian(VS_OUT input) : SV_Target0
{
    float4 Out = 0;

    uint m_TexW, m_TexH, numMips;
    SubMap0.GetDimensions(0, m_TexW, m_TexH, numMips);
    

    float tu = 1.f / m_TexW;
    float tv = 1.f / m_TexH;

    float2 offset[25] =
    {
        float2(-2.0f * tu, 2.0f * tv), float2(-1.0f * tu, 2.0f * tv), float2(0.0f * tu, 2.0f * tv), float2(1.0f * tu, 2.0f * tv), float2(2.0f * tu, 2.0f * tv),
         float2(-2.0f * tu, 1.0f * tv), float2(-1.0f * tu, 1.0f * tv), float2(0.0f * tu, 1.0f * tv), float2(1.0f * tu, 1.0f * tv), float2(2.0f * tu, 1.0f * tv),
         float2(-2.0f * tu, 0.0f * tv), float2(-1.0f * tu, 0.0f * tv), float2(0.0f * tu, 0.0f * tv), float2(1.0f * tu, 0.0f * tv), float2(2.0f * tu, 0.0f * tv),
         float2(-2.0f * tu, -1.0f * tv), float2(-1.0f * tu, -1.0f * tv), float2(0.0f * tu, -1.0f * tv), float2(1.0f * tu, -1.0f * tv), float2(2.0f * tu, -1.0f * tv),
         float2(-2.0f * tu, -2.0f * tv), float2(-1.0f * tu, -2.0f * tv), float2(0.0f * tu, -2.0f * tv), float2(1.0f * tu, -2.0f * tv), float2(2.0f * tu, -2.0f * tv)
    };
    
    float3 vUpSample = 0;
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            int index = i * 5 + j;
            vUpSample += SubMap0.Sample(LinearSamplerMirror, input.uv + offset[index]).rgb * GaussianWeight[index];
        }
    }
    
    Out = float4(vUpSample, 1.f);

    return Out;
}

float g_MotionBlurPower;

float4 PS_MotionBlur(VS_OUT input) : SV_Target0
{
    float4 output = (float4) 0.f;
    
    int NumBlurSample = g_BlurCount;
    float4 velocity = SubMap0.Sample(LinearSamplerMirror, input.uv);
            
    velocity.xy /= (float) NumBlurSample;
    velocity.xy *= g_MotionBlurPower;
    
    int iCnt = 1;
    
    float4 BColor;
    
    uint width, height, numMips;
    SubMap1.GetDimensions(0, width, height, numMips);
   
    float2 texel = { 1.f / (float) width, 1.f / (float) height };
    
    for (int i = iCnt; i < NumBlurSample; ++i)
    {
        
        BColor = SubMap1.Sample(LinearSamplerMirror, input.uv + velocity.xy * (float) i);
       
        float4 vProjPos = mul(float4(SubMap2.Sample(LinearSamplerMirror, input.uv).rgb, 1.f), P);
        
        float depth = SubMap2.Sample(LinearSamplerMirror, input.uv).z / SubMap2.Sample(PointSamplerMirror, input.uv).w;
        if (velocity.a < depth + 0.04f)
        {
           iCnt++;
            output += BColor;
        }


      
    }
    output /= (float) iCnt;

    if (velocity.z == 0.f)
        output = SubMap1.Sample(LinearSamplerMirror, input.uv);

    if (!all(output))
        output = SubMap1.Sample(LinearSamplerMirror, input.uv);
    
    output.a = 1.f;
    
    return output;
}

float g_fRadialBlurStrength;
int g_iSamples = 64;
float2 g_vCenterPos;
float g_fNormalRadius;

float4 PS_RadialBlur(VS_OUT input) : SV_Target
{
    uint width, height, numMips;
    SubMap0.GetDimensions(0, width, height, numMips);
    
    float2 centerUV = 0;
    centerUV = g_vCenterPos;
    centerUV.y *= -1.f;
    centerUV += 0.5f;
    
    float2 dir = input.uv - centerUV;
    float4 normalColor = SubMap0.Sample(LinearSamplerClamp, input.uv);
    float4 color = (float4) 0;


    for (int i = 0; i < g_iSamples; i += 2) //operating at 2 samples for better performance
    {
            color += SubMap0.Sample(LinearSamplerClamp, input.uv + float(i) / float(g_iSamples) * dir * g_fRadialBlurStrength);
            color += SubMap0.Sample(LinearSamplerClamp, input.uv + float(i + 1) / float(g_iSamples) * dir * g_fRadialBlurStrength);
    }

    
    return color / float(g_iSamples);
    
  
}

technique11 t0
{
    pass smaller
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_DOWN()));
    }
    pass bigger
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_UP()));
    }
    pass justDown
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_JUSTDOWN()));
    }
    pass GaussianUp
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_UPGaussian()));
    }
}

technique11 t1
{
    pass motionBlur
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_LESS_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_MotionBlur()));
    }
    pass RadialBlur
    {
        SetVertexShader(CompileShader(vs_5_0, VS_MAIN()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_LESS_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_RadialBlur()));
    }
    
}