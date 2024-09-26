#include "pch.h"
#include "TerrainRenderer.h"
#include "Camera.h"

TerrainRenderer::TerrainRenderer(shared_ptr<Shader> shader)
	: Component(COMPONENT_TYPE::TerrainRenderer)
	, m_pShader(shader)
{
}

TerrainRenderer::~TerrainRenderer()
{
}

HRESULT TerrainRenderer::Init()
{
	


	return S_OK;
}

void TerrainRenderer::CreateGrid(_uint x, _uint z)
{
	if(!m_pMesh)
		m_pMesh = make_shared<Mesh>();
	m_pMesh->CreateGrid(x, z );

	m_RenderParams.intParams[0] = x;
	m_RenderParams.intParams[1] = z;

}

void TerrainRenderer::Set_Material(shared_ptr<Material> material)
{
	m_pMaterial = material;
	m_pMaterial->Set_Shader(m_pShader);
}

void TerrainRenderer::Render()
{
	if (!m_pMaterial || !m_pMesh || !m_pShader)
		return;
	m_RenderParams.vec2Params[1] = GAMEINSTANCE.m_vMinMaxTessellationDistance;
	m_RenderParams.vec2Params[2] = GAMEINSTANCE.m_vTest;
	m_pMaterial->Tick();
	m_pMaterial->Push_TextureMapData();

	m_pShader->Push_RenderParamData(m_RenderParams);

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());


	auto& LightParam = CUR_SCENE->Get_LightParams();
	m_pShader->Push_LightData(LightParam);

	//	m_pShader->GetScalar("g_BarPercent")->SetFloat(m_fLoadingPercent);
	auto& world = Get_Transform()->Get_WorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{world });

	m_pMesh->Get_VertexBuffer()->Push_Data();
	m_pMesh->Get_IndexBuffer()->Push_Data();


	CONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	m_pShader->DrawIndexed(0, 1, m_pMesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

}
