#include "pch.h"
#include "MeshRenderer.h"

#include "Model.h"
#include "Light.h"
#include "Camera.h"
#include "Material.h"
#include "ModelMesh.h"
#include "InstancingBuffer.h"

MeshRenderer::MeshRenderer(shared_ptr<Shader> shader)
	:Component(COMPONENT_TYPE::MeshRenderer)
	, m_pShader(shader)
{

}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Set_Material(shared_ptr<Material> material)
{
	m_pMaterial = material;
	m_pMaterial->Set_Shader(m_pShader);
}

void MeshRenderer::Render_Instancing(shared_ptr<InstancingBuffer>& buffer, shared_ptr<InstanceRenderParamDesc> renderParamDesc)
{
	if (!m_pMaterial || !m_pMesh || !m_pShader)
		return;

	m_pMaterial->Tick();
	m_pMaterial->Push_TextureMapData();

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

	m_pShader->Push_InstanceRenderParamData(*renderParamDesc);

	auto& LightParam = CUR_SCENE->Get_LightParams();
	m_pShader->Push_LightData(LightParam);

	m_pMesh->Get_VertexBuffer()->Push_Data();
	m_pMesh->Get_IndexBuffer()->Push_Data();
//	m_pShader->GetScalar("g_BarPercent")->SetFloat(m_fLoadingPercent);
	buffer->Push_Data();

	if (m_pShader->Get_ShaderType() == SHADER_TYPE::FORWARD)
	{

		if (m_pMesh->Get_Name() == L"Point")
			CONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		else
			CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pShader->DrawIndexedInstanced(0, m_iPass + 1, m_pMesh->Get_IndexBuffer()->Get_IndicesNum(), buffer->Get_Count());
	}
	else if (m_pShader->Get_ShaderType() == SHADER_TYPE::DEFERRED)
	{
		if (m_pMesh->Get_Name() == L"Point")
			CONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		else
			CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		int passIndex = CUR_SCENE->g_bPBR_On ? 1 : 0;
		m_pShader->DrawIndexedInstanced(1, passIndex, m_pMesh->Get_IndexBuffer()->Get_IndicesNum(), buffer->Get_Count());
	}
}

void MeshRenderer::Render()
{
	if (!m_pMaterial || !m_pMesh || !m_pShader)
		return;

	m_pMaterial->Tick();
	m_pMaterial->Push_TextureMapData();

	m_pShader->Push_RenderParamData(m_RenderParams);

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());


	auto& LightParam = CUR_SCENE->Get_LightParams();
	m_pShader->Push_LightData(LightParam);

	//	m_pShader->GetScalar("g_BarPercent")->SetFloat(m_fLoadingPercent);
	auto& world = Get_Transform()->Get_WorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world });

	m_pMesh->Get_VertexBuffer()->Push_Data();
	m_pMesh->Get_IndexBuffer()->Push_Data();

	if (m_pShader->Get_ShaderType() == SHADER_TYPE::FORWARD)
	{

		if (m_pMesh->Get_Name() == L"Point")
			CONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		else
			CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pShader->DrawIndexed(0, m_iPass, m_pMesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	}
	else if (m_pShader->Get_ShaderType() == SHADER_TYPE::DEFERRED)
	{
		if (m_pMesh->Get_Name() == L"Point")
			CONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		else
			CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		int passIndex = CUR_SCENE->g_bPBR_On ? 1 : 0;
		m_pShader->DrawIndexed(0, passIndex, m_pMesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	}


}

void MeshRenderer::Render_Water()
{
	if (!m_pMaterial || !m_pMesh || !m_pShader)
		return;

	m_pMaterial->Tick();
	m_pMaterial->Push_TextureMapData();

	m_pShader->Push_RenderParamData(m_RenderParams);

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());


	auto& LightParam = CUR_SCENE->Get_LightParams();
	m_pShader->Push_LightData(LightParam);

	//	m_pShader->GetScalar("g_BarPercent")->SetFloat(m_fLoadingPercent);
	auto& world = Get_Transform()->Get_WorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world });

	m_pMesh->Get_VertexBuffer()->Push_Data();
	m_pMesh->Get_IndexBuffer()->Push_Data();


	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_pShader->DrawIndexed(0, 0, m_pMesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
}

InstanceID MeshRenderer::Get_InstanceID()
{
	return make_pair(_ulonglong(m_pMesh.get()), _ulonglong(m_pMaterial.get()));
}