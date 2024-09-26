#include "pch.h"
#include "Model.h"
#include "DistortionRenderer.h"
#include "Camera.h"
#include "Utils.h"
#include "ModelMesh.h"

DistortionRenderer::DistortionRenderer(shared_ptr<Shader> shader)
    : Component(COMPONENT_TYPE::DistortionRenderer)
	, m_pShader(shader)
{
}

DistortionRenderer::~DistortionRenderer()
{
}

HRESULT DistortionRenderer::Init()
{
    return S_OK;
}

void DistortionRenderer::Render()
{
	if (!m_pMaterial || !m_pShader)
		return;

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

	auto& world = Get_Transform()->Get_WorldMatrix();
	auto& preWorld = Get_Transform()->Get_preWorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world,preWorld });

	auto preView = CUR_SCENE->Get_MainCamera()->Get_Transform()->Get_preWorldMatrix().Invert();
	m_pShader->GetMatrix("g_preView")->SetMatrix((_float*)&preView);

	m_pShader->Push_RenderParamData(m_RenderParams);

	if (m_pModel)
	{
		shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();

		const _uint boneCount = m_pModel->Get_BoneCount();

		for (_uint i = 0; i < boneCount; ++i)
		{
			shared_ptr<ModelBone> bone = m_pModel->Get_BoneByIndex(i);
			boneDesc->transform[i] = bone->transform * Utils::m_matPivot;
		}
		m_pShader->Push_BoneData(*boneDesc);

		const auto& meshes = m_pModel->Get_Meshes();
		for (auto& mesh : meshes)
		{
			mesh->vertexBuffer->Push_Data();
			mesh->indexBuffer->Push_Data();

			m_pMaterial->Tick();
			m_pMaterial->Push_TextureMapData();

			m_pShader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

			// For. Setting Context Topology
			CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			// For. Draw call
			m_pShader->DrawIndexed(0, m_iPass, mesh->indexBuffer->Get_IndicesNum(), 0, 0);
		}
	}
	else
	{
		m_pMesh->Get_VertexBuffer()->Push_Data();
		m_pMesh->Get_IndexBuffer()->Push_Data();

		if (m_pMesh->Get_Name() == L"Point")
			CONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		else
			CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		m_pShader->DrawIndexed(1, m_iPass, m_pMesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

	}
}

void DistortionRenderer::Set_Material(shared_ptr<Material> material)
{
	m_pMaterial = material;
	m_pMaterial->Set_Shader(m_pShader);
}

void DistortionRenderer::Set_Model(shared_ptr<Model> model)
{
	m_pModel = model;
}
