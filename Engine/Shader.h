#pragma once
#include "ResourceBase.h"
#include "Pass.h"
#include "Technique.h"
#include "BindShaderDesc.h"

enum class SHADER_TYPE
{
	DEFERRED,
	FORWARD,
	LIGHTING,
	PARTICLE,
	SHADOW,
	DISTORTION,
	SKYBOX,
	WATER,
	NONE
};

struct ShaderDesc
{
	ComPtr<ID3DBlob> blob;
	ComPtr<ID3DX11Effect> effect;
};
class ConstantBuffer;
class Shader : public ResourceBase
{
public:
	friend struct Pass;

public:
	Shader();
	~Shader();

	virtual void Load(const wstring& path) override;
	wstring Get_ShaderTag() { return m_strShaderTag; }
	wstring GetFile() { return _file; }
	ComPtr<ID3DX11Effect> Effect() { return _shaderDesc.effect; }
	SHADER_TYPE Get_ShaderType() { return m_eShaderType; }
	void Set_ShaderType(SHADER_TYPE eType) { m_eShaderType = eType; }

	void Draw(UINT technique, UINT pass, UINT vertexCount, UINT startVertexLocation = 0);
	void DrawIndexed(UINT technique, UINT pass, UINT indexCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0);
	void DrawInstanced(UINT technique, UINT pass, UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation = 0, UINT startInstanceLocation = 0);
	void DrawIndexedInstanced(UINT technique, UINT pass, UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0, UINT startInstanceLocation = 0);

	void Dispatch(UINT technique, UINT pass, UINT x, UINT y, UINT z);

	ComPtr<ID3DX11EffectVariable> GetVariable(string name);
	ComPtr<ID3DX11EffectScalarVariable> GetScalar(string name);
	ComPtr<ID3DX11EffectVectorVariable> GetVector(string name);
	ComPtr<ID3DX11EffectMatrixVariable> GetMatrix(string name);
	ComPtr<ID3DX11EffectStringVariable> GetString(string name);
	ComPtr<ID3DX11EffectShaderResourceVariable> GetSRV(string name);
	ComPtr<ID3DX11EffectRenderTargetViewVariable> GetRTV(string name);
	ComPtr<ID3DX11EffectDepthStencilViewVariable> GetDSV(string name);
	ComPtr<ID3DX11EffectUnorderedAccessViewVariable> GetUAV(string name);
	ComPtr<ID3DX11EffectConstantBuffer> GetConstantBuffer(string name);
	ComPtr<ID3DX11EffectShaderVariable> GetShader(string name);
	ComPtr<ID3DX11EffectBlendVariable> GetBlend(string name);
	ComPtr<ID3DX11EffectDepthStencilVariable> GetDepthStencil(string name);
	ComPtr<ID3DX11EffectRasterizerVariable> GetRasterizer(string name);
	ComPtr<ID3DX11EffectSamplerVariable> GetSampler(string name);

private:
	void CreateEffect();
	ComPtr<ID3D11InputLayout> CreateInputLayout(ComPtr<ID3DBlob> fxBlob, D3DX11_EFFECT_SHADER_DESC* effectVsDesc, vector<D3D11_SIGNATURE_PARAMETER_DESC>& params);

private:
	wstring m_strShaderTag = L"";
	wstring _file;
	ShaderDesc _shaderDesc;
	D3DX11_EFFECT_DESC _effectDesc{};
	shared_ptr<StateBlock> _initialStateBlock;
	vector<Technique> _techniques;
	SHADER_TYPE m_eShaderType = SHADER_TYPE::NONE;

public:
	void Push_GlobalData(const _float4x4& matView, const _float4x4& matProj);
	void Push_TransformData(const TransformDesc& desc);
	void Push_LightData(const LightParams& desc);
	void Push_MaterialData(const MaterialDesc& desc);
	void Push_BoneData(const BoneDesc& desc);
	void Push_KeyframeData(const KeyFrameDesc& desc);
	void Push_TweenData(const preTweenDesc& desc);
	void Push_InstanceTweenData(const InstanceTweenDesc& desc);
	void Push_InstanceRenderParamData(const InstanceRenderParamDesc& desc);
	void Push_AnimAddonData(const AnimAddonDesc& desc);
	void Push_EffectData(const EffectDesc& desc);
	void Push_ParticleData(const ParticleDesc& desc);
	void Push_CreateParticleData(const CreateParticleDesc& desc);
	void Push_TextureMapData(const TextureMapDesc& boolArray);
	void Push_SubmapData(const SubMapDesc& boolArray);
	void Push_RenderParamData(const RenderParams& params);

private:
	GlobalDesc m_GlobalDesc;
	shared_ptr<ConstantBuffer> m_pGlobalBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pGlobalEffectBuffer;

	TransformDesc m_TransformDesc;
	shared_ptr<ConstantBuffer> m_pTransformBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pTransformEffectBuffer;

	LightParams m_LightDesc;
	shared_ptr<ConstantBuffer> m_pLightBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pLightEffectBuffer;

	MaterialDesc m_MaterialDesc;
	shared_ptr<ConstantBuffer> m_pMaterialBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pMaterialEffectBuffer;

	BoneDesc m_BoneDesc;
	shared_ptr<ConstantBuffer> m_pBoneBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pBoneEffectBuffer;

	KeyFrameDesc m_KeyFrameDesc;
	shared_ptr<ConstantBuffer> m_pKeyFrameBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pKeyFrameEffectBuffer;

	preTweenDesc m_TweenDesc;
	shared_ptr<ConstantBuffer> m_pTweenBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pTweenEffectBuffer;

	InstanceTweenDesc m_InstanceTweenDesc;
	shared_ptr<ConstantBuffer> m_pInstanceTweenBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pInstanceTweenEffectBuffer;

	InstanceRenderParamDesc m_InstanceRenderParamDesc;
	shared_ptr<ConstantBuffer> m_pInstanceRenderParamBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pInstanceRenderParamEffectBuffer;

	AnimAddonDesc m_BoneIndexDesc;
	shared_ptr<ConstantBuffer> m_pBoneIndexBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pBoneIndexEffectBuffer;

	EffectDesc m_EffectDesc;
	shared_ptr<ConstantBuffer> m_pEffectBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pEffectEffectBuffer;

	ParticleDesc m_ParticleDesc;
	shared_ptr<ConstantBuffer> m_pParticleBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pParticleEffectBuffer;

	CreateParticleDesc m_CreateParticleDesc;
	shared_ptr<ConstantBuffer> m_pCreateParticleBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pCreateParticleEffectBuffer;

	shared_ptr<ConstantBuffer> m_pTextureMapBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pTextureMapEffectBuffer;

	shared_ptr<ConstantBuffer> m_pSubmapBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pSubmapEffectBuffer;

	shared_ptr<ConstantBuffer> m_pRenderParamBuffer;
	ComPtr<ID3DX11EffectConstantBuffer> m_pRenderParamEffectBuffer;
};

class ShaderManager
{
public:
	static ShaderDesc GetEffect(wstring fileName);

private:
	static unordered_map<wstring, ShaderDesc> shaders;
};