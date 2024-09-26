#pragma once
#include "Component.h"

class Model;
class Mesh;
class Shader;
class Material;

class MeshRenderer :public Component
{
public:
	enum PASS_INFO
	{
		Default_UI = 0,
		Curtain_Up_UI = 1,
		Clock_UI = 2,
		HpBar_UI = 3,
		Slide_Right_UI = 4,
		Default_UI_Instancing = 5,
		Loaing_Bar = 6,
	};

public:
	MeshRenderer(shared_ptr<Shader> shader);
	virtual ~MeshRenderer();

public:
	void Set_Material(shared_ptr<Material> material);
	void Set_Mesh(shared_ptr<Mesh> mesh)	{ m_pMesh = mesh; }
	void Set_Pass(_uint pass)				{ m_iPass = pass; }
	_uint Get_Pass()						{ return m_iPass; }
	shared_ptr<Mesh> Get_Mesh()				{ return m_pMesh; }
	shared_ptr<Material> Get_Material()		{ return  m_pMaterial; }
	shared_ptr<Shader> Get_Shader()			{ return m_pShader; }

public:
	void Render_Instancing(shared_ptr<InstancingBuffer>& buffer, shared_ptr<InstanceRenderParamDesc> renderParamDesc);
	void Render();

	void Render_Water();

	InstanceID Get_InstanceID();

private:
	shared_ptr<Shader> m_pShader;
	shared_ptr<Mesh> m_pMesh;
	shared_ptr<Material> m_pMaterial;
	_uint m_iPass = 0;

public:
	auto& Get_RenderParamDesc()						{ return m_RenderParams; }
	void SetInt(_uchar index, _int value)			{ m_RenderParams.SetInt(index, value);}
	void SetFloat(_uchar index, _float value)		{ m_RenderParams.SetFloat(index, value); }
	void SetVec2(_uchar index, _float2 value)		{ m_RenderParams.SetVec2(index, value); }
	void SetVec4(_uchar index, _float4 value)		{ m_RenderParams.SetVec4(index, value); }
	void SetMatrix(_uchar index, _float4x4& value)	{ m_RenderParams.SetMatrix(index, value); }

private:
	RenderParams m_RenderParams{};
};

