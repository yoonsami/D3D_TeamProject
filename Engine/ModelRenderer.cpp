#include "pch.h"
#include "ModelRenderer.h"

#include "Model.h"
#include "Utils.h"
#include "Light.h"
#include "Camera.h"
#include "Material.h"
#include "ModelMesh.h"

ModelRenderer::ModelRenderer(shared_ptr<Shader> shader)
	:Component(COMPONENT_TYPE::ModelRenderer)
	, m_pShader(shader)
{
}

ModelRenderer::~ModelRenderer()
{
}

void ModelRenderer::Render()
{
	if (m_pModel == nullptr)
		return;
	// Set VP
	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

	// Transform
	auto& world = Get_Transform()->Get_WorldMatrix();
	auto& preWorld = Get_Transform()->Get_preWorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world,preWorld });

	auto preView = CUR_SCENE->Get_MainCamera()->Get_Transform()->Get_preWorldMatrix().Invert();
	m_pShader->GetMatrix("g_preView")->SetMatrix((_float*)&preView);

	m_pShader->Push_RenderParamData(m_RenderParams);

	// Bones
	shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();

	const _uint boneCount = m_pModel->Get_BoneCount();

	for (_uint i = 0; i < boneCount; ++i)
	{
		shared_ptr<ModelBone> bone = m_pModel->Get_BoneByIndex(i);
		boneDesc->transform[i] = bone->transform * Utils::m_matPivot;
	}
	m_pShader->Push_BoneData(*boneDesc);

	m_pShader->GetVector("g_UVSliding")->SetFloatVector((_float*)(&m_vUvSilding));

	m_pShader->GetScalar("g_DepthRange")->SetFloat(CUR_SCENE->g_DepthRange);
	m_pShader->GetScalar("g_ClosestDepth")->SetFloat(CUR_SCENE->g_ClosestDepth);




	const auto& meshes = m_pModel->Get_Meshes();
	for (auto& mesh : meshes)
	{
		if (!mesh->material.expired())
		{
			mesh->material.lock()->Tick();
			mesh->material.lock()->Push_TextureMapData();

		}

		m_pShader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);
		mesh->vertexBuffer->Push_Data();
		mesh->indexBuffer->Push_Data();
		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		int techniqueIndex = CUR_SCENE->g_bPBR_On ? 4 : 0;

		switch (m_ePassType)
		{
		case ModelRenderer::PASS_MAPOBJECT:
			m_pShader->DrawIndexed(techniqueIndex, PS_MAPOBJECT, mesh->indexBuffer->Get_IndicesNum(), 0, 0);

			break;
		case ModelRenderer::PASS_MAPOBJECT_CULLNONE:
			m_pShader->DrawIndexed(techniqueIndex, PS_MAPOBJECT_CULLNONE, mesh->indexBuffer->Get_IndicesNum(), 0, 0);

			break;
		case ModelRenderer::PASS_MAPOBJECT_WORLDNORMAL:
			m_pShader->DrawIndexed(techniqueIndex, PS_MAPOBJECT_WORLDNORMAL, mesh->indexBuffer->Get_IndicesNum(), 0, 0);

			break;
		case ModelRenderer::PASS_MAPOBJECT_WORLDNORMAL_CULLNONE:
			m_pShader->DrawIndexed(techniqueIndex, PS_MAPOBJECT_CULLNONE_WORLDNORMAL, mesh->indexBuffer->Get_IndicesNum(), 0, 0);

			break;
		case ModelRenderer::PASS_WATER:
			m_pShader->DrawIndexed(techniqueIndex, PS_WATER, mesh->indexBuffer->Get_IndicesNum(), 0, 0);

			break;
		case ModelRenderer::PASS_NORMAL_CONTROL:
			m_pShader->DrawIndexed(techniqueIndex, PS_MapObject_NormalControl, mesh->indexBuffer->Get_IndicesNum(), 0, 0);

			break;
		case ModelRenderer::PASS_DEFAULT:
		{
			if(m_bCullNone)
				m_pShader->DrawIndexed(techniqueIndex, PS_NONANIM_CULLNONE, mesh->indexBuffer->Get_IndicesNum(), 0, 0);
			else
				m_pShader->DrawIndexed(techniqueIndex, PS_NONANIM, mesh->indexBuffer->Get_IndicesNum(), 0, 0);

		}
			break;

		}

	}
}

void ModelRenderer::Render_Instancing(shared_ptr<class InstancingBuffer>& buffer, shared_ptr<InstanceRenderParamDesc> renderParamDesc)
{
	if (m_pModel == nullptr)
		return;

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

	m_pShader->Push_InstanceRenderParamData(*renderParamDesc);
	// Bones
	auto preView = CUR_SCENE->Get_MainCamera()->Get_Transform()->Get_preWorldMatrix().Invert();
	m_pShader->GetMatrix("g_preView")->SetMatrix((_float*)&preView);

	shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();
	const _uint boneCount = m_pModel->Get_BoneCount();

	for (_uint i = 0; i < boneCount; ++i)
	{
		shared_ptr<ModelBone> bone = m_pModel->Get_BoneByIndex(i);
		boneDesc->transform[i] = (bone->transform) * Utils::m_matPivot;
	}
	m_pShader->Push_BoneData(*boneDesc);
	m_pShader->GetVector("g_UVSliding")->SetFloatVector((_float*)(&m_vUvSilding));
	
	m_pShader->GetScalar("g_DepthRange")->SetFloat(CUR_SCENE->g_DepthRange);
	m_pShader->GetScalar("g_ClosestDepth")->SetFloat(CUR_SCENE->g_ClosestDepth);

	
	buffer->Push_Data();
	const auto& meshes = m_pModel->Get_Meshes();
	for (auto& mesh : meshes)
	{
		if (!mesh->material.expired())
		{
			mesh->material.lock()->Tick();
			mesh->material.lock()->Push_TextureMapData();

		}

		// BoneIndex
		m_pShader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		mesh->vertexBuffer->Push_Data();
		mesh->indexBuffer->Push_Data();




		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		int techniqueIndex = CUR_SCENE->g_bPBR_On ? 4 : 0;

		switch (m_ePassType)
		{
		case ModelRenderer::PASS_MAPOBJECT:
			m_pShader->DrawIndexedInstanced(techniqueIndex, PS_MAPOBJECT_INSTANCE, mesh->indexBuffer->Get_IndicesNum(), buffer->Get_Count());

			break;
		case ModelRenderer::PASS_MAPOBJECT_CULLNONE:
			m_pShader->DrawIndexedInstanced(techniqueIndex, PS_MAPOBJECT_INSTANCE_CULLNONE, mesh->indexBuffer->Get_IndicesNum(), buffer->Get_Count());

			break;
		case ModelRenderer::PASS_MAPOBJECT_WORLDNORMAL:
			m_pShader->DrawIndexedInstanced(techniqueIndex, PS_MAPOBJECT_INSTANCE_WORLDNORMAL, mesh->indexBuffer->Get_IndicesNum(), buffer->Get_Count());

			break;
		case ModelRenderer::PASS_MAPOBJECT_WORLDNORMAL_CULLNONE:
			m_pShader->DrawIndexedInstanced(techniqueIndex, PS_MAPOBJECT_INSTANCE_CULLNONE_WORLDNORMAL, mesh->indexBuffer->Get_IndicesNum(), buffer->Get_Count());

			break;
		case ModelRenderer::PASS_NORMAL_CONTROL:
			m_pShader->DrawIndexedInstanced(techniqueIndex, PS_MapObject_Instancing_NormalControl, mesh->indexBuffer->Get_IndicesNum(), buffer->Get_Count());

			break;
		case ModelRenderer::PASS_DEFAULT:
		{
			if (m_bCullNone)
				m_pShader->DrawIndexedInstanced(techniqueIndex, PS_NONANIM_CULLNONE_INSTANCE, mesh->indexBuffer->Get_IndicesNum(), buffer->Get_Count());
			else
				m_pShader->DrawIndexedInstanced(techniqueIndex, PS_NONANIMINSTANCE, mesh->indexBuffer->Get_IndicesNum(), buffer->Get_Count());

		}
		break;

		}
	}
}

void ModelRenderer::Render_Skybox()
{
	if (m_pModel == nullptr)
		return;

	if (!m_bRenderOn)
		return;
	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

	shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();
	const _uint boneCount = m_pModel->Get_BoneCount();
	_float4 tmp = _float4(0.f);

	for (_uint i = 0; i < boneCount; ++i)
	{
		shared_ptr<ModelBone> bone = m_pModel->Get_BoneByIndex(i);
		boneDesc->transform[i] = bone->transform * _float4x4::CreateScale(0.01f) * Utils::m_matPivot;
	}
	m_pShader->Push_BoneData(*boneDesc);

	m_pShader->GetVector("g_UVSliding")->SetFloatVector((_float*)(&m_vUvSilding));
	// Transform
	auto& world = Get_Transform()->Get_WorldMatrix();
	auto& preWorld = Get_Transform()->Get_preWorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world,preWorld });

	const auto& meshes = m_pModel->Get_Meshes();

	for (auto& mesh : meshes)
	{
		if (!mesh->material.expired())
		{
			mesh->material.lock()->Tick();
			mesh->material.lock()->Push_TextureMapData();

		}

		// BoneIndex
		m_pShader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		mesh->vertexBuffer->Push_Data();
		mesh->indexBuffer->Push_Data();
		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pShader->DrawIndexed(0, m_iPass, mesh->indexBuffer->Get_IndicesNum(), 0, 0);

	}
}

void ModelRenderer::Render_Shadow()
{
	if (m_pModel == nullptr)
		return;

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

	// Bones
	shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();

	const _uint boneCount = m_pModel->Get_BoneCount();

	for (_uint i = 0; i < boneCount; ++i)
	{
		shared_ptr<ModelBone> bone = m_pModel->Get_BoneByIndex(i);
		boneDesc->transform[i] = bone->transform * Utils::m_matPivot;
	}
	m_pShader->Push_BoneData(*boneDesc);

	// Transform
	auto& world = Get_Transform()->Get_WorldMatrix();
	auto& preWorld = Get_Transform()->Get_preWorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world,preWorld });

	const auto& meshes = m_pModel->Get_Meshes();
	for (auto& mesh : meshes)
	{
		if (!mesh->material.expired())
		{
			mesh->material.lock()->Tick();
			mesh->material.lock()->Push_TextureMapData();

		}

		// BoneIndex
		m_pShader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		mesh->vertexBuffer->Push_Data();
		mesh->indexBuffer->Push_Data();
		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pShader->DrawIndexed(0, PS_NONANIM_SHADOW, mesh->indexBuffer->Get_IndicesNum(), 0, 0);
	}
}

void ModelRenderer::Render_Shadow_Instancing(shared_ptr<InstancingBuffer>& buffer, shared_ptr<InstanceRenderParamDesc> renderParamDesc)
{
	if (m_pModel == nullptr)
		return;

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

	m_pShader->Push_InstanceRenderParamData(*renderParamDesc);
	// Bones
	shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();

	const _uint boneCount = m_pModel->Get_BoneCount();

	for (_uint i = 0; i < boneCount; ++i)
	{
		shared_ptr<ModelBone> bone = m_pModel->Get_BoneByIndex(i);
		boneDesc->transform[i] = (bone->transform) * Utils::m_matPivot;
	}
	m_pShader->Push_BoneData(*boneDesc);

	const auto& meshes = m_pModel->Get_Meshes();
	for (auto& mesh : meshes)
	{
		if (!mesh->material.expired())
		{
			mesh->material.lock()->Tick();
			mesh->material.lock()->Push_TextureMapData();

		}

		// BoneIndex
		m_pShader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		mesh->vertexBuffer->Push_Data();
		mesh->indexBuffer->Push_Data();

		buffer->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pShader->DrawIndexedInstanced(0, PS_NONANIM_SHADOW_INSTANCE, mesh->indexBuffer->Get_IndicesNum(), buffer->Get_Count());
	}
}

void ModelRenderer::Render_MotionBlur()
{
	if (m_pModel == nullptr)
		return;

	if (!m_bRenderOn)
		return;
	// Set VP
	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

	// Transform
	auto& world = Get_Transform()->Get_WorldMatrix();
	auto& preWorld = Get_Transform()->Get_preWorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world,preWorld });

	auto preView = CUR_SCENE->Get_MainCamera()->Get_Transform()->Get_preWorldMatrix().Invert();
	m_pShader->GetMatrix("g_preView")->SetMatrix((_float*)&preView);
	m_pShader->Push_RenderParamData(m_RenderParams);

	// Bones
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
		if (!mesh->material.expired())
		{
			mesh->material.lock()->Tick();
			mesh->material.lock()->Push_TextureMapData();

		}

		m_pShader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		mesh->vertexBuffer->Push_Data();
		mesh->indexBuffer->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pShader->DrawIndexed(2, PS_NONANIM, mesh->indexBuffer->Get_IndicesNum(), 0, 0);
	}
}

void ModelRenderer::Render_MotionBlur_Instancing(shared_ptr<InstancingBuffer>& buffer, shared_ptr<InstanceRenderParamDesc> renderParamDesc)
{
	if (m_pModel == nullptr)
		return;

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());
	m_pShader->Push_InstanceRenderParamData(*renderParamDesc);
	// Bones
	auto preView = CUR_SCENE->Get_MainCamera()->Get_Transform()->Get_preWorldMatrix().Invert();
	m_pShader->GetMatrix("g_preView")->SetMatrix((_float*)&preView);
	
	shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();
	const _uint boneCount = m_pModel->Get_BoneCount();

	for (_uint i = 0; i < boneCount; ++i)
	{
		shared_ptr<ModelBone> bone = m_pModel->Get_BoneByIndex(i);
		boneDesc->transform[i] = (bone->transform) * Utils::m_matPivot;
	}
	m_pShader->Push_BoneData(*boneDesc);
	m_pShader->GetVector("g_UVSliding")->SetFloatVector((_float*)(&m_vUvSilding));
	const auto& meshes = m_pModel->Get_Meshes();
	for (auto& mesh : meshes)
	{
		if (!mesh->material.expired())
		{
			mesh->material.lock()->Tick();
			mesh->material.lock()->Push_TextureMapData();

		}

		// BoneIndex
		m_pShader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		mesh->vertexBuffer->Push_Data();
		mesh->indexBuffer->Push_Data();

		buffer->Push_Data();


		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		m_pShader->DrawIndexedInstanced(2, PS_NONANIMINSTANCE, mesh->indexBuffer->Get_IndicesNum(), buffer->Get_Count());
	}
}

void ModelRenderer::Render_Forward()
{
	if (m_pModel == nullptr)
		return;
	// Set VP
	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());
	m_pShader->Push_LightData(CUR_SCENE->Get_LightParams());
	m_pShader->GetScalar("lightIndex")->SetInt(0);
	// Transform
	auto& world = Get_Transform()->Get_WorldMatrix();
	auto& preWorld = Get_Transform()->Get_preWorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world,preWorld });

	auto preView = CUR_SCENE->Get_MainCamera()->Get_Transform()->Get_preWorldMatrix().Invert();
	m_pShader->GetMatrix("g_preView")->SetMatrix((_float*)&preView);

	m_pShader->Push_RenderParamData(m_RenderParams);

	// Bones
	shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();

	const _uint boneCount = m_pModel->Get_BoneCount();

	for (_uint i = 0; i < boneCount; ++i)
	{
		shared_ptr<ModelBone> bone = m_pModel->Get_BoneByIndex(i);
		boneDesc->transform[i] = bone->transform * Utils::m_matPivot;
	}
	m_pShader->Push_BoneData(*boneDesc);

	m_pShader->GetVector("g_UVSliding")->SetFloatVector((_float*)(&m_vUvSilding));

	const auto& meshes = m_pModel->Get_Meshes();
	for (auto& mesh : meshes)
	{
		if (!mesh->material.expired())
		{
			mesh->material.lock()->Tick();
			mesh->material.lock()->Push_TextureMapData();

		}

		m_pShader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);
		mesh->vertexBuffer->Push_Data();
		mesh->indexBuffer->Push_Data();
		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		int techniqueIndex = CUR_SCENE->g_bPBR_On ? 4 : 0;

		m_pShader->DrawIndexed(techniqueIndex, 20, mesh->indexBuffer->Get_IndicesNum(), 0, 0);


	}
}

void ModelRenderer::Set_Model(shared_ptr<Model> model)
{
	m_pModel = model;

	const auto& materials = m_pModel->Get_Materials();

	for (auto& material : materials)
	{
		material->Set_Shader(m_pShader);
	}

}

InstanceID ModelRenderer::Get_InstanceID()
{
	return make_pair((_ulonglong)m_pModel.get(), (_ulonglong)m_pShader.get());
}
