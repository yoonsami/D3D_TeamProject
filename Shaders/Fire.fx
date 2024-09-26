#include "Render.fx"
#include "Light.fx"

struct VS_IN
{
    float4 position : POSITIONT;
    float2 uv : TEXCOORD;
    float2 scale : SCALE;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VS_OUT VS(VS_IN input)
{
    VS_OUT output;
    
    float4 position = mul(input.position, W);
    
    float3 up = float3(0, 1, 0);
    float3 forward = position.xyz - CameraPosition();
    float3 right = normalize(cross(up, forward));
    
    position.xyz += (input.uv.x - 0.5f) * right * input.scale.x;
    position.xyz += (1.0f - input.uv.y - 0.5f) * up * input.scale.y;
    position.w = 1.f;
    
    output.position = mul(mul(position, V), P);
    output.uv = input.uv;
    return output;
}

float4 PS(VS_OUT input) : SV_TARGET
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.uv);
    return diffuse;
}

technique11 T0
{
    pass p0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));

    }
};
