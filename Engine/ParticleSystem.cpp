#include "pch.h"
#include "ParticleSystem.h"

#include "Model.h"
#include "Utils.h"
#include "Camera.h"
#include "Material.h"
#include "ModelMesh.h"
#include "StructuredBuffer.h"

ParticleSystem::ParticleSystem(shared_ptr<Shader> shader)
	: Component(COMPONENT_TYPE::ParticleSystem)
	, m_pShader(shader)
{

}

ParticleSystem::~ParticleSystem()
{
	m_pShader->GetUAV("g_Particle")->SetUnorderedAccessView(m_pParticleBuffer->Get_UAV().Get());
	m_pShader->Dispatch(1, 2, 1, 1, 1);
}

HRESULT ParticleSystem::Init()
{
	vector<ParticleInfo> tmp(m_ParticleSystemDesc.m_iMaxParticle);
	if (!m_pParticleBuffer)
		m_pParticleBuffer = make_shared<StructuredBuffer>(tmp.data(), static_cast<_uint>(sizeof ParticleInfo), m_ParticleSystemDesc.m_iMaxParticle);

	if (!m_pComputeSharedBuffer)
		m_pComputeSharedBuffer = make_shared<StructuredBuffer>(nullptr, static_cast <_uint>(sizeof ComputeSharedInfo), 1);

	return S_OK;
}

void ParticleSystem::Tick()
{
}

void ParticleSystem::Final_Tick()
{
	if (!m_pMesh || !m_pMaterial)
		return;

	m_ParticleSystemDesc.m_fAccTime += fDT;
	m_fSystemAge += fDT;
	_int add = 0;
	if (m_fSystemAge >= m_ParticleSystemDesc.m_fDuration)
	{
		EVENTMGR.Delete_Object(Get_Owner());
		return;

	}
	//else
	if(m_fSystemAge <= m_ParticleSystemDesc.m_fCreatingTime)
	{
		{
			if (m_ParticleSystemDesc.m_fCreateInterval < m_ParticleSystemDesc.m_fAccTime)
			{
				m_ParticleSystemDesc.m_fAccTime = m_ParticleSystemDesc.m_fAccTime - m_ParticleSystemDesc.m_fCreateInterval;
				add = _int(m_ComputeParams.vec4Params[1].x + 1);
			}
		}
	}
	auto& world = Get_Transform()->Get_WorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world });

	m_pShader->GetUAV("g_Particle")->SetUnorderedAccessView(m_pParticleBuffer->Get_UAV().Get());	// 쉐이
	m_pShader->GetUAV("g_Shared")->SetUnorderedAccessView(m_pComputeSharedBuffer->Get_UAV().Get());

	_float2 StartEndScale = { m_ParticleSystemDesc.m_fStartScale,m_ParticleSystemDesc.m_fEndScale };
	m_pShader->GetVector("StartEndScale")->SetFloatVector((_float*)&StartEndScale);

	m_pShader->Push_RenderParamData(m_ComputeParams);

	ParticleDesc desc;
	desc.maxCount = m_ParticleSystemDesc.m_iMaxParticle;
	desc.addCount = add;
	desc.DeltaAccTime = { fDT,m_ParticleSystemDesc.m_fAccTime };
	desc.MinMaxLifeTime = { m_ParticleSystemDesc.m_fMinLifeTime,m_ParticleSystemDesc.m_fMaxLifeTime };
	desc.MinMaxSpeed = { m_ParticleSystemDesc.m_fMinSpeed,m_ParticleSystemDesc.m_fMaxSpeed };
	m_pShader->Push_ParticleData(desc);

	if(m_pModel)
		m_pShader->GetScalar("isModel")->SetBool(true);
	else
		m_pShader->GetScalar("isModel")->SetBool(false);

	m_pShader->Dispatch(1, m_iCSPass, 1, 1, 1);

	vector<ParticleInfo> tmp(m_ParticleSystemDesc.m_iMaxParticle);
	m_pParticleBuffer->Copy_FromOutput(tmp.data());

	m_pParticleBuffer->Copy_ToInput(tmp.data());
}

void ParticleSystem::Render()
{
	if(!m_pModel)
	{
		if (!m_pMesh || !m_pMaterial)
			return;

		m_pShader->GetScalar("isModel")->SetBool(false);	// 보류

		m_RenderParams.SetInt(3, m_ComputeParams.intParams[3]);// 보류
		m_pMaterial->Tick();// 보류

		m_pShader->Push_RenderParamData(m_RenderParams);// 보류
		m_pShader->GetSRV("g_Data")->SetResource(m_pParticleBuffer->Get_SRV().Get());

		m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

		_float2 StartEndScale = { m_ParticleSystemDesc.m_fStartScale,m_ParticleSystemDesc.m_fEndScale }; // 커스텀인듯 일단 제외
		m_pShader->GetVector("StartEndScale")->SetFloatVector((_float*)&StartEndScale); // 커스텀인듯 일단 제외

		auto LightParam = CUR_SCENE->Get_LightParams();
		m_pShader->Push_LightData(LightParam);

		auto world = Get_Transform()->Get_WorldMatrix();
		m_pShader->Push_TransformData(TransformDesc{ world });

		m_pShader->Push_EffectData(m_EffectDesc);	// 보류

		m_pMesh->Get_VertexBuffer()->Push_Data();
		m_pMesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

		m_pShader->DrawIndexedInstanced(0, m_iPass, m_pMesh->Get_IndexBuffer()->Get_IndicesNum(), m_ParticleSystemDesc.m_iMaxParticle);
	}
	else
	{
		m_pShader->GetScalar("isModel")->SetBool(true);

		m_pShader->GetSRV("g_Data")->SetResource(m_pParticleBuffer->Get_SRV().Get());
		m_pShader->Push_RenderParamData(m_RenderParams);
		auto world = Get_Transform()->Get_WorldMatrix();
		m_pShader->Push_TransformData(TransformDesc{ world });
		m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

		_float2 StartEndScale = { m_ParticleSystemDesc.m_fStartScale,m_ParticleSystemDesc.m_fEndScale };
		m_pShader->GetVector("StartEndScale")->SetFloatVector((_float*)&StartEndScale);
		
		m_pShader->Push_EffectData(m_EffectDesc);

		shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();
		const _uint boneCount = m_pModel->Get_BoneCount();

		for (_uint i = 0; i < boneCount; ++i)
		{
			shared_ptr<ModelBone> bone = m_pModel->Get_BoneByIndex(i);
			boneDesc->transform[i] = (bone->transform) * Utils::m_matPivot;
		}
		m_pShader->Push_BoneData(*boneDesc);

		for (auto& mesh : m_pModel->Get_Meshes())
		{
			if (!mesh->material.expired())
				mesh->material.lock()->Tick();

			// BoneIndex
			m_pShader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

			mesh->vertexBuffer->Push_Data();
			mesh->indexBuffer->Push_Data();

			CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			m_pShader->DrawIndexedInstanced(2, m_iPass, mesh->indexBuffer->Get_IndicesNum(), m_ParticleSystemDesc.m_iMaxParticle * mesh->indexBuffer->Get_IndicesNum());

		}
	}
}

void ParticleSystem::Create_Buffer()
{
	vector<ParticleInfo> tmp(m_ParticleSystemDesc.m_iMaxParticle);
	if (!m_pParticleBuffer)
		m_pParticleBuffer = make_shared<StructuredBuffer>(tmp.data(), static_cast<_uint>(sizeof ParticleInfo), m_ParticleSystemDesc.m_iMaxParticle);

	if (!m_pComputeSharedBuffer)
		m_pComputeSharedBuffer = make_shared<StructuredBuffer>(nullptr, static_cast <_uint>(sizeof ComputeSharedInfo), 1);
}

void ParticleSystem::Set_Material(shared_ptr<Material> material)
{
	m_pMaterial = material;
	m_pMaterial->Set_Shader(m_pShader);
}

void ParticleSystem::Set_ComputeMaterial(shared_ptr<Material> computeMaterial)
{
	m_pComputeMaterial = computeMaterial;
	m_pComputeMaterial->Set_Shader(m_pShader);
}

void ParticleSystem::Restart_System()
{
	m_pShader->GetUAV("g_Particle")->SetUnorderedAccessView(m_pParticleBuffer->Get_UAV().Get());
	m_pShader->Dispatch(1, 2, 1, 1, 1);

	m_ParticleSystemDesc.m_fAccTime = 0.f;
	m_fSystemAge = 0.f;
	
}

void ParticleSystem::Set_Model(shared_ptr<Model> model)
{
	m_pModel = model;

	if (!m_pModel)
		return;
	const auto& materials = m_pModel->Get_Materials();

	for (auto& material : materials)
	{
		material->Set_Shader(m_pShader);
	}
}
