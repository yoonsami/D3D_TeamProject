#ifndef _GLOBAL_FX_
#define _GLOBAL_FX_
#define GAMMA 2.2f
#define PI 3.14159265359
/////////////////
// ConstBuffer //
/////////////////

cbuffer GlobalBuffer
{
    row_major float4x4 V;
    row_major float4x4 P;
    row_major float4x4 VP;
    row_major float4x4 VInv;
};

cbuffer TransformBuffer
{
    row_major float4x4 W;
    row_major float4x4 preW;
};

////////////////
// VertexData //
////////////////

struct Vertex
{
    float3 position : POSITION;
};

struct VertexTexture
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VertexColor
{
    float3 Position : POSITION;
    float4 Color : COLOR;
};

struct VertexTextureNormal
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VTXMesh
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct VTXModel
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float4 blendIndices : BLENDINDICES;
    float4 blendWeight : BLENDWEIGHT;
    bool filtered : TEXCOORD2;
};



//////////////////
// VertexOutput //
//////////////////

struct UIOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct UIInstancingOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    uint id : ID;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION1;
    float3 viewPosition : POSITION2;
    float2 uv : TEXCOORD;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
};

struct MeshOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION1;
    float3 viewPosition : POSITION2;
    float2 uv : TEXCOORD;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
};


struct MeshInstancingOutput
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION1;
    float3 viewPosition : POSITION2;
    float2 uv : TEXCOORD;
    float3 viewNormal : NORMAL;
    float3 viewTangent : TANGENT;
    uint id : SV_InstanceID;
};

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
};

struct PS_OUT_Deferred
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 depth : SV_Target2;
    float4 diffuseColor : SV_Target3;
    float4 specularColor : SV_Target4;
    float4 emissiveColor : SV_Target5;
};

struct ShadowOutput
{
    float4 pos : SV_POSITION;
    float4 clipPos : POSITION1;
    float4 worldPos : POSITION2;
    float2 uv : TEXCOORD;
};

struct ShadowInstanceOutput
{
    float4 pos : SV_POSITION;
    float4 clipPos : POSITION1;
    float4 worldPos : POSITION2;
    float2 uv : TEXCOORD;
    uint id : SV_InstanceID;
};

//////////////////
// SamplerState //
//////////////////

SamplerState LinearSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerComparisonState samShadow
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
    BorderColor = float4(1.f,1.f,1.f,1.f);

    ComparisonFunc = LESS;
};

SamplerState LinearSamplerClamp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};

SamplerState LinearSamplerBorder
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Border;
    AddressV = Border;
    BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
};

SamplerState PointSamplerBorder
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Border;
    AddressV = Border;
    BorderColor = float4(1.f,1.f,1.f,1.f);
};

SamplerState LinearSamplerMirror
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Mirror;
    AddressV = Mirror;
};

SamplerState PointSampler
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};

SamplerState PointSamplerMirror
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

/////////////////////
// RasterizerState //
/////////////////////

RasterizerState FillModeWireFrame
{
    FillMode = WireFrame;
    CullMode = None;
};

RasterizerState RS_Default
{
    FillMode = solid;
    CullMode = back;
    FrontCounterClockwise = false;
};

RasterizerState RS_CullNone
{
    FillMode = Solid;
    CullMode = None;
};

RasterizerState RS_SkyBox
{
    FillMode = solid;
    CullMode = front;
    FrontCounterClockwise = false;
};

RasterizerState RS_CW
{
    FillMode = solid;
    CullMode = back;
    FrontCounterClockwise = true;
};
/////////////////////
//   BlendState    //
/////////////////////

BlendState BlendOff
{
    BlendEnable[0] = FALSE;
};

BlendState BlendDefault
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ONE;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
};

BlendState AlphaBlend
{
    AlphaToCoverageEnable = false;
    BlendEnable[0] = true;
    SrcBlend[0] = SRC_ALPHA;
    DestBlend[0] = INV_SRC_ALPHA;
    BlendOp = ADD;

    SrcBlendAlpha[0] = ONE;
    DestBlendAlpha[0] = ZERO;
    BlendOpAlpha[0] = ADD;
    RenderTargetWriteMask[0] = 15;
};

BlendState AlphaBlendAlphaToCoverageEnable
{
    AlphaToCoverageEnable = true;
    BlendEnable[0] = true;
    SrcBlend[0] = SRC_ALPHA;
    DestBlend[0] = INV_SRC_ALPHA;
    BlendOp = ADD;

    SrcBlendAlpha[0] = ONE;
    DestBlendAlpha[0] = ZERO;
    BlendOpAlpha[0] = ADD;
    RenderTargetWriteMask[0] = 15;
};

BlendState AdditiveBlend
{
    AlphaToCoverageEnable = true;
    BlendEnable[0] = true;
    BlendEnable[1] = true;
    BlendEnable[2] = true;
    BlendEnable[3] = true;
    BlendEnable[4] = true;
    BlendEnable[5] = true;
    BlendEnable[6] = true;
    SrcBlend = One;
    DestBlend = One;
    BlendOp = ADD;

    SrcBlendAlpha[0] = ONE;
    DestBlendAlpha[0] = ZERO;
    BlendOpAlpha[0] = ADD;
    RenderTargetWriteMask[0] = 15;
};

DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less_equal;
};

DepthStencilState DSS_LESS
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = less;
};

DepthStencilState DSS_GREATER
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = greater;
};

DepthStencilState DSS_GREATER_EQUAL
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = greater_equal;
};

DepthStencilState DSS_NO_DEPTH_TEST
{
    DepthEnable = false;
    DepthWriteMask = all;
    DepthFunc = less;
};

DepthStencilState DSS_NO_DEPTH_TEST_NO_WRITE
{
    DepthEnable = false;
    DepthWriteMask = false;
    DepthFunc = less;
};

DepthStencilState DSS_LESS_NO_WRITE
{
    DepthEnable = true;
    DepthWriteMask = false;
    DepthFunc = less;
};

DepthStencilState DSS_Sky
{
    DepthEnable = true;
    DepthWriteMask = false;
    DepthFunc = less_equal;
};

///////////
// Macro //
///////////

#define PASS_VP(name, vs, ps)					\
pass name											\
{													\
	SetVertexShader(CompileShader(vs_5_0, vs()));	\
	SetRasterizerState(RS_Default);	                   \
	SetDepthStencilState(DSS_Default,0);             \
	SetPixelShader(CompileShader(ps_5_0, ps()));	\
    SetBlendState(BlendOff,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);	 \
}

#define PASS_VP_CULLNONE(name, vs, ps)					\
pass name											\
{													\
	SetVertexShader(CompileShader(vs_5_0, vs()));	\
	SetRasterizerState(RS_CullNone);	                   \
	SetDepthStencilState(DSS_Default,0);       \
	SetPixelShader(CompileShader(ps_5_0, ps()));	\
    SetBlendState(BlendOff,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);	 \
}

#define PASS_VP_BLEND(name, vs, ps)					\
pass name											\
{													\
	SetVertexShader(CompileShader(vs_5_0, vs()));	\
	SetRasterizerState(RS_Default);	                   \
	SetDepthStencilState(DSS_Default,0);           \
	SetPixelShader(CompileShader(ps_5_0, ps()));	\
    SetBlendState(BlendDefault,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);						        \
}

#define PASS_RS_VP(name, rs, vs, ps)				\
pass name											\
{													\
    SetRasterizerState(rs);							\
    SetBlendState(BlendOff,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF);						\
  SetDepthStencilState(DSS_Default,0);              \
    SetVertexShader(CompileShader(vs_5_0, vs()));	\
    SetPixelShader(CompileShader(ps_5_0, ps()));	\
}

#define PASS_CS(name, cs)				            \
pass name											\
{													\
    SetVertexShader(NULL);	                        \
  	SetDepthStencilState(DSS_Default,0);	            \
    SetPixelShader(NULL);	                        \
    SetGeometryShader(NULL);	                        \
    SetComputeShader(CompileShader(cs_5_0, cs()));	\
}

#define PASS_BS_VP(name, bs,vs,ps)                                      \
pass name                                                               \
{                                                                       \
    SetBlendState(bs,float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFF);         \
    SetVertexShader(CompileShader(vs_5_0, vs()));	                        \
    SetPixelShader(CompileShader(ps_5_0, ps()));	                    \
}


//////////////
// Function //
//////////////

float3 CameraPosition()
{
    return VInv._41_42_43;
}

#define IDENTITY_MATRIX float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)

row_major float4x4 inverse(row_major float4x4 m)
{
    float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
    float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
    float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
    float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

    float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    float idet = 1.0f / det;

    row_major float4x4 ret;

    ret[0][0] = t11 * idet;
    ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
    ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
    ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

    ret[1][0] = t12 * idet;
    ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
    ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
    ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

    ret[2][0] = t13 * idet;
    ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
    ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
    ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

    ret[3][0] = t14 * idet;
    ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
    ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
    ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

    return ret;
}

// http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
float4 matrix_to_quaternion(row_major float4x4 m)
{
    float tr = m[0][0] + m[1][1] + m[2][2];
    float4 q = float4(0, 0, 0, 0);

    if (tr > 0)
    {
        float s = sqrt(tr + 1.0) * 2; // S=4*qw 
        q.w = 0.25 * s;
        q.x = (m[2][1] - m[1][2]) / s;
        q.y = (m[0][2] - m[2][0]) / s;
        q.z = (m[1][0] - m[0][1]) / s;
    }
    else if ((m[0][0] > m[1][1]) && (m[0][0] > m[2][2]))
    {
        float s = sqrt(1.0 + m[0][0] - m[1][1] - m[2][2]) * 2; // S=4*qx 
        q.w = (m[2][1] - m[1][2]) / s;
        q.x = 0.25 * s;
        q.y = (m[0][1] + m[1][0]) / s;
        q.z = (m[0][2] + m[2][0]) / s;
    }
    else if (m[1][1] > m[2][2])
    {
        float s = sqrt(1.0 + m[1][1] - m[0][0] - m[2][2]) * 2; // S=4*qy
        q.w = (m[0][2] - m[2][0]) / s;
        q.x = (m[0][1] + m[1][0]) / s;
        q.y = 0.25 * s;
        q.z = (m[1][2] + m[2][1]) / s;
    }
    else
    {
        float s = sqrt(1.0 + m[2][2] - m[0][0] - m[1][1]) * 2; // S=4*qz
        q.w = (m[1][0] - m[0][1]) / s;
        q.x = (m[0][2] + m[2][0]) / s;
        q.y = (m[1][2] + m[2][1]) / s;
        q.z = 0.25 * s;
    }

    return q;
}

row_major float4x4 m_scale(row_major float4x4 m, float3 v)
{
    float x = v.x, y = v.y, z = v.z;

    m[0][0] *= x;
    m[1][0] *= y;
    m[2][0] *= z;
    m[0][1] *= x;
    m[1][1] *= y;
    m[2][1] *= z;
    m[0][2] *= x;
    m[1][2] *= y;
    m[2][2] *= z;
    m[0][3] *= x;
    m[1][3] *= y;
    m[2][3] *= z;

    return m;
}

row_major float4x4 quaternion_to_matrix(float4 quat)
{
    row_major float4x4 m = float4x4(float4(0, 0, 0, 0), float4(0, 0, 0, 0), float4(0, 0, 0, 0), float4(0, 0, 0, 0));

    float x = quat.x, y = quat.y, z = quat.z, w = quat.w;
    float x2 = x + x, y2 = y + y, z2 = z + z;
    float xx = x * x2, xy = x * y2, xz = x * z2;
    float yy = y * y2, yz = y * z2, zz = z * z2;
    float wx = w * x2, wy = w * y2, wz = w * z2;

    m[0][0] = 1.0 - (yy + zz);
    m[0][1] = xy - wz;
    m[0][2] = xz + wy;

    m[1][0] = xy + wz;
    m[1][1] = 1.0 - (xx + zz);
    m[1][2] = yz - wx;

    m[2][0] = xz - wy;
    m[2][1] = yz + wx;
    m[2][2] = 1.0 - (xx + yy);

    m[3][3] = 1.0;

    return m;
}

row_major float4x4 m_translate(row_major float4x4 m, float3 v)
{
    float x = v.x, y = v.y, z = v.z;
    m[3][0] = x;
    m[3][1] = y;
    m[3][2] = z;
    return m;
}

row_major float4x4 compose(float3 position, float4 quat, float3 scale)
{
    row_major float4x4 m = quaternion_to_matrix(quat);
    m = m_scale(m, scale);
    m = m_translate(m, position);
    return m;
}

void decompose(in row_major float4x4 m, out float3 position, out float4 rotation, out float3 scale)
{
    float sx = length(float3(m[0][0], m[0][1], m[0][2]));
    float sy = length(float3(m[1][0], m[1][1], m[1][2]));
    float sz = length(float3(m[2][0], m[2][1], m[2][2]));

    // if determine is negative, we need to invert one scale
    float det = determinant(m);
    if (det < 0)
    {
        sx = -sx;
    }

    position.x = m[3][0];
    position.y = m[3][1];
    position.z = m[3][2];

    // scale the rotation part

    float invSX = 1.0 / sx;
    float invSY = 1.0 / sy;
    float invSZ = 1.0 / sz;

    m[0][0] *= invSX;
    m[0][1] *= invSX;
    m[0][2] *= invSX;

    m[1][0] *= invSY;
    m[1][1] *= invSY;
    m[1][2] *= invSY;

    m[2][0] *= invSZ;
    m[2][1] *= invSZ;
    m[2][2] *= invSZ;

    rotation = matrix_to_quaternion(m);

    scale.x = sx;
    scale.y = sy;
    scale.z = sz;
}

row_major float4x4 axis_matrix(float3 right, float3 up, float3 forward)
{
    float3 xaxis = right;
    float3 yaxis = up;
    float3 zaxis = forward;
    return float4x4(
		xaxis.x, yaxis.x, zaxis.x, 0,
		xaxis.y, yaxis.y, zaxis.y, 0,
		xaxis.z, yaxis.z, zaxis.z, 0,
		0, 0, 0, 1
	);
}

// http://stackoverflow.com/questions/349050/calculating-a-lookat-matrix
row_major float4x4 look_at_matrix(float3 forward, float3 up)
{
    float3 xaxis = normalize(cross(forward, up));
    float3 yaxis = up;
    float3 zaxis = forward;
    return axis_matrix(xaxis, yaxis, zaxis);
}

row_major float4x4 look_at_matrix(float3 at, float3 eye, float3 up)
{
    float3 zaxis = normalize(at - eye);
    float3 xaxis = normalize(cross(up, zaxis));
    float3 yaxis = cross(zaxis, xaxis);
    return axis_matrix(xaxis, yaxis, zaxis);
}

row_major float4x4 extract_rotation_matrix(row_major float4x4 m)
{
    float sx = length(float3(m[0][0], m[0][1], m[0][2]));
    float sy = length(float3(m[1][0], m[1][1], m[1][2]));
    float sz = length(float3(m[2][0], m[2][1], m[2][2]));

    // if determine is negative, we need to invert one scale
    float det = determinant(m);
    if (det < 0)
    {
        sx = -sx;
    }

    float invSX = 1.0 / sx;
    float invSY = 1.0 / sy;
    float invSZ = 1.0 / sz;

    m[0][0] *= invSX;
    m[0][1] *= invSX;
    m[0][2] *= invSX;
    m[0][3] = 0;

    m[1][0] *= invSY;
    m[1][1] *= invSY;
    m[1][2] *= invSY;
    m[1][3] = 0;

    m[2][0] *= invSZ;
    m[2][1] *= invSZ;
    m[2][2] *= invSZ;
    m[2][3] = 0;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;

    return m;
}
#define QUATERNION_IDENTITY float4(0, 0, 0, 1)
float4 q_slerp(in float4 a, in float4 b, float t)
{
    float4 src = (float4)0.f;
    float4 dest = (float4) 0.f;
    
    
    src = a;
    dest = b;
    float4 Identity = 0;
    Identity = QUATERNION_IDENTITY;
    // if either input is zero, return the other.
    if (length(src) == 0.0)
    {
        if (length(dest) == 0.0)
        {
            return Identity;
        }
        return dest;
    }
    else if (length(dest) == 0.0)
    {
        return src;
    }

    float cosHalfAngle = src.w * dest.w + dot(src.xyz, dest.xyz);

    if (cosHalfAngle >= 1.0 || cosHalfAngle <= -1.0)
    {
        return src;
    }
    else if (cosHalfAngle < 0.0)
    {
        b.xyz = -b.xyz;
        b.w = -b.w;
        cosHalfAngle = -cosHalfAngle;
    }

    float blendA;
    float blendB;
    if (cosHalfAngle < 0.99)
    {
        // do proper slerp for big angles
        float halfAngle = acos(cosHalfAngle);
        float sinHalfAngle = sin(halfAngle);
        float oneOverSinHalfAngle = 1.0 / sinHalfAngle;
        blendA = sin(halfAngle * (1.0 - t)) * oneOverSinHalfAngle;
        blendB = sin(halfAngle * t) * oneOverSinHalfAngle;
    }
    else
    {
        // do lerp if angle is really small.
        blendA = 1.0 - t;
        blendB = t;
    }

    float4 result = float4(blendA * a.xyz + blendB * b.xyz, blendA * a.w + blendB * b.w);
    if (length(result) > 0.0)
    {
        return normalize(result);
    }
    return QUATERNION_IDENTITY;
}

row_major float4x4 slerpMat(row_major float4x4 m0, row_major float4x4 m1, float t)
{
    float3 curTrans, curScale;
    float4 curRot;
    
    float3 nextTrans, nextScale;
    float4 nextRot;
    
    decompose(m0, curTrans, curRot, curScale);
    decompose(m1, nextTrans, nextRot, nextScale);

    return compose(lerp(curTrans, nextTrans, t), q_slerp(curRot, nextRot, t), lerp(curScale, nextScale, t));
}

float Rand(float2 co)
{
    return 0.5 + (frac(sin(dot(co.xy, float2(12.9898, 78.233))) * 43758.5453)) * 0.5;
}

//float noise(in float3 x)
//{
//    float3 p = floor(x);
//    float3 f = fract(x);
//    f = f * f * (3.0 - 2.0 * f);
    
//    vec2 uv = (p.xy + vec2(37.0, 17.0) * p.z) + f.xy;
//    vec2 rg = textureLod(iChannel0, (uv + 0.5) / 256.0, 0.).yx;

//    return -1.0 + 2.0 * mix(rg.x, rg.y, f.z);
//}

#endif