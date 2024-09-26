#pragma once
#include "Component.h"
class TerrainRenderer :
    public Component
{
public:
	TerrainRenderer(shared_ptr<Shader> shader);
	virtual ~TerrainRenderer();

public:
	virtual HRESULT Init() override;

	void CreateGrid(_uint x, _uint z);

	shared_ptr<Material> Get_Material() { return m_pMaterial; }

	void Set_Material(shared_ptr<Material> material);
	void Set_Mesh(shared_ptr<Mesh> mesh) { m_pMesh = mesh; }

	void Render();
private:
	shared_ptr<Shader> m_pShader;
	shared_ptr<Mesh> m_pMesh;
	shared_ptr<Material> m_pMaterial;
	_uint m_iPass = 0;

public:
	auto& Get_RenderParamDesc() { return m_RenderParams; }
	void SetInt(_uchar index, _int value) { m_RenderParams.SetInt(index, value); }
	void SetFloat(_uchar index, _float value) { m_RenderParams.SetFloat(index, value); }
	void SetVec2(_uchar index, _float2 value) { m_RenderParams.SetVec2(index, value); }
	void SetVec4(_uchar index, _float4 value) { m_RenderParams.SetVec4(index, value); }
	void SetMatrix(_uchar index, _float4x4& value) { m_RenderParams.SetMatrix(index, value); }

private:
	RenderParams m_RenderParams{};

};

