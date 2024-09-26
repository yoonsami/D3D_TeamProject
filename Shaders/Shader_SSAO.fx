#include "Render.fx"
#include "Light.fx"

cbuffer cbPerFrame
{
    row_major float4x4 gViewToTexSpace;
    float4 gOffsetVectors[14];
    float4 gFrustumCorners[4];


    float gOcclusionRadius = 0.5f;
    float gOcclusionFadeStart = 0.2f;
    float gOcclusionFadeEnd = 2.0f;
    float gSurfaceEpsilon = 0.05f;
};

cbuffer cbSettings
{
    float gWeights[11] =
    {
        0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f
    };
};



cbuffer cbFixed
{
    static const int gBlurRadius = 5;
};

SamplerState samNormalDepth
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;

	// Set a very far depth value if sampling outside of the NormalDepth map
	// so we do not get false occlusions.
    AddressU = BORDER;
    AddressV = BORDER;
    BorderColor = float4(0.0f, 0.0f, 0.0f, 1e5f);
};

SamplerState samNormalDepth2
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

SamplerState samInputImage
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;

    AddressU = CLAMP;
    AddressV = CLAMP;
};

SamplerState samRandomVec
{
    Filter = MIN_MAG_LINEAR_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 ToFarPlane : TEXCOORD0;
    float2 uv : TEXCOORD1;
};

VS_OUT VS_Final(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = float4(input.pos * 2.f, 1.f);
    
    int index = 0;
    if(input.uv.x < 0.5f && input.uv.y < 0.5f)
        index = 0;
    else if (input.uv.x > 0.5f && input.uv.y < 0.5f)
        index = 1;
    else if (input.uv.x > 0.5f && input.uv.y > 0.5f)
        index = 2;
    else
        index = 3;
    
    output.ToFarPlane = gFrustumCorners[index].xyz;
    
    output.uv = input.uv;

    return output;
}
float OcclusionFunction(float distZ)
{
    float occlusion = 0.0f;
    if (distZ > gSurfaceEpsilon)
    {
        float fadeLength = gOcclusionFadeEnd - gOcclusionFadeStart;

		// Linearly decrease occlusion from 1 to 0 as distZ goes 
		// from gOcclusionFadeStart to gOcclusionFadeEnd.	
        occlusion = saturate((gOcclusionFadeEnd - distZ) / fadeLength);
    }

    return occlusion;
}

float4 PS_SSAO(VS_OUT pin, uniform int gSampleCount) : SV_Target
{

    float3 n = SubMap0.SampleLevel(samNormalDepth, pin.uv, 0.0f).xyz;
    float pz = SubMap1.SampleLevel(samNormalDepth, pin.uv, 0.0f).z;

    float3 p = (pz / pin.ToFarPlane.z) * pin.ToFarPlane;

    float3 randVec = 2.0f * SubMap2.SampleLevel(samRandomVec, 4.0f * pin.uv, 0.0f).rgb - 1.0f;

    float occlusionSum = 0.0f;

	[unroll]
    for (int i = 0; i < gSampleCount; ++i)
    {
        float3 offset = reflect(gOffsetVectors[i].xyz, randVec);

        float flip = sign(dot(offset, n));

        float3 q = p + flip * gOcclusionRadius * offset;

        float4 projQ = mul(float4(q, 1.0f), gViewToTexSpace);
        projQ /= projQ.w;

        float rz = SubMap1.SampleLevel(samNormalDepth, projQ.xy, 0.0f).z;



        float3 r = (rz / q.z) * q;



        float distZ = p.z - r.z;
        float dp = max(dot(n, normalize(r - p)), 0.0f);
        float occlusion = dp * OcclusionFunction(distZ);

        occlusionSum += occlusion;
    }

    occlusionSum /= gSampleCount;

    float access = 1.0f - occlusionSum;

	// Sharpen the contrast of the SSAO map to make the SSAO affect more dramatic.
    return saturate(pow(access, 4.0f));
}

technique11 t0
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Final()));
        SetGeometryShader(NULL);
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_DEPTH_TEST_NO_WRITE, 0);
        SetBlendState(BlendOff, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetPixelShader(CompileShader(ps_5_0, PS_SSAO(14)));
    }
}