#pragma once
//#include "ConstantBuffer.h"

#define MAX_MODEL_TRANSFORMS 300
#define MAX_MODEL_KEYFRAMES 600
#define MAX_MODEL_INSTANCE 400

class Shader;

struct GlobalDesc
{
	_float4x4 V = _float4x4::Identity;
	_float4x4 P = _float4x4::Identity;
	_float4x4 VP = _float4x4::Identity;
	_float4x4 VInv = _float4x4::Identity;
};

struct TransformDesc
{
	_float4x4 W = _float4x4::Identity;
	_float4x4 preW = _float4x4::Identity;
};

struct LightColor
{
	Color ambient = Color(1.f, 1.f, 1.f, 1.f);
	Color diffuse = Color(1.f, 1.f, 1.f, 1.f);
	Color specular = Color(1.f, 1.f, 1.f, 1.f);
	Color emissive = Color(1.f, 1.f, 1.f, 1.f);
};

struct LightInfo
{
	LightColor	color;
	_float4		vPosition;
	_float4		vDirection;
	_int		lightType;
	_float		range;
	_float		angle;
	_int		padding;
};

struct LightParams
{
	_uint lightCount;
	_float3 padding;
	LightInfo lights[50];
	_float specularPower = 10.f;
	_float rimPower = 10.f;
	_float2 dpadding;
};

struct MaterialDesc
{
	Color ambient = Color(0.f, 0.f, 0.f, 1.f);
	Color diffuse = Color(1.f, 1.f, 1.f, 1.f);
	Color specular = Color(0.f, 0.f, 0.f, 1.f);
	Color emissive = Color(0.f, 0.f, 0.f, 1.f);
};

struct KeyFrameDesc
{
	_int animIndex = 0;
	_uint currentFrame =0;
	_uint nextFrame = 0;
	_float ratio = 0.f;
	_float sumTime = 0.f;
	_float3 padding;

};

struct TweenDesc
{
	TweenDesc() { curr.animIndex = 0; next.animIndex = -1; }
	_float tweenDuration = 1.f;
	_float tweenRatio = 0.f;
	_float tweenSumTime = 0.f;
	_float padding;
	KeyFrameDesc curr;
	KeyFrameDesc next;

	void ClearNextAnim()
	{
		next.animIndex = -1;
		next.currentFrame = 0;
		next.nextFrame = 0;
		next.sumTime = 0;
		tweenSumTime = 0;
		tweenRatio = 0;
	}
};

struct preTweenDesc
{
	TweenDesc curTween;
	TweenDesc preTween;
};

struct InstanceTweenDesc
{
	TweenDesc tweens[MAX_MODEL_INSTANCE];
};


struct BoneDesc
{
	_float4x4 transform[MAX_MODEL_TRANSFORMS];
};

struct AnimAddonDesc
{
	_float4x4 transform[MAX_MODEL_TRANSFORMS];
	_float4x4 m_matPivot = _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI);
};

struct EffectDesc
{
	_float2 UVSliding = _float2(0.f);
	int maxGradationIndex = 1;
	int gradationMapIndex = 0;

	int maxSubGradationIndex;
	int SubGradationMapIndex;
	_float2 uvSlidingAcc = _float2(0.f);
};

struct ParticleDesc
{
	int maxCount = 0;
	int addCount = 0;
	int frameNumber = 0;
	int padding0;

	_float2 DeltaAccTime = { 0,0 };
	_float2 MinMaxLifeTime = { 0,0 };

	_float2 MinMaxSpeed = { 0,0 };
	_float2 padding1;
};

struct CreateParticleDesc
{
	int			iEndScaleOption;
	int			iEndSpeedOption;

	_float2		vMinMaxLifeTime;
	
	_float2		vMinMaxRotationSpeed_X;
	_float2		vMinMaxRotationSpeed_Y;
	_float2		vMinMaxRotationSpeed_Z;

	_float2		vMinMaxRotationAngle_X;
	_float2		vMinMaxRotationAngle_Y;
	_float2		vMinMaxRotationAngle_Z;

	_float4		vStartColor;
	_float4		vEndColor;

	_float2		vMinMaxStartScale;
	_float2		vMinMaxEndScale;

	_float2		vMinMaxStartSpeed;
	_float2		vMinMaxEndSpeed;

	_float4		vCreateRange;
	_float4		vCreateOffsets;

	int			iNewlyAddCnt = 0;
	int			iRandomRotationOn;
	_float2		Padding;
};

struct TextureMapDesc
{
	int bHasTextureMap[MAX_TEXTURE_MAP_COUONT];
};

struct SubMapDesc
{
	int bHasSubMap[SRV_MAP_COUNT];
};

enum
{
	MATERIAL_ARG_COUNT = 4,

};

struct RenderParams
{
	RenderParams()
	{
		for (_int i = 0; i < MATERIAL_ARG_COUNT; i++)
		{
			SetInt(i, 0);
			SetFloat(i, 0.f);
			SetVec2(i, _float2(0.f));
			SetVec4(i, _float4(0.f));
		};
	}
	void SetInt(_uchar index, _int value) { intParams[index] = value; }
	void SetFloat(_uchar index, _float value) { floatParams[index] = value; }
	void SetVec2(_uchar index, _float2 value) { vec2Params[index] = value; }
	void SetVec4(_uchar index, _float4 value) { vec4Params[index] = value; }
	void SetMatrix(_uchar index, _float4x4& value) { matParams[index] = value; }
	void ResetParams()
	{
		for (_int i = 0; i < MATERIAL_ARG_COUNT; ++i)
		{
			SetInt(i, 0);
			SetFloat(i, 0.f);
		}
	}

	array<_uint, MATERIAL_ARG_COUNT> intParams;
	array<_float, MATERIAL_ARG_COUNT> floatParams;
	array<_float2, MATERIAL_ARG_COUNT> vec2Params;
	array<_float4, MATERIAL_ARG_COUNT> vec4Params;
	array<_float4x4, MATERIAL_ARG_COUNT> matParams;
};



struct InstanceRenderParamDesc
{
	_float4 params[MAX_MODEL_INSTANCE];
};