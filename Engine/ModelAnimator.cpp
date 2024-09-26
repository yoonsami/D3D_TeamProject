#include "pch.h"
#include "ModelAnimator.h"

#include "Model.h"
#include "Light.h"
#include "Camera.h"
#include "InputMgr.h"
#include "Material.h" 
#include "ModelMesh.h"
#include "ModelAnimation.h"
#include "CharacterController.h"

ModelAnimator::ModelAnimator(shared_ptr<Shader> shader)
	:Component(COMPONENT_TYPE::Animator)
	, m_pShader(shader)
{

}

ModelAnimator::~ModelAnimator()
{
}

void ModelAnimator::Tick()
{
	if (m_bAnimStop)
		return;

	m_preTweenDesc = m_TweenDesc;

	_float deltaTime = fABT;
	if(Get_Owner()->Is_TimeSlowed())
		deltaTime = fDT;

	m_TweenDesc.curr.sumTime += deltaTime;

	{
		shared_ptr<ModelAnimation> currentAnim = m_pModel->Get_AnimationByIndex(m_TweenDesc.curr.animIndex);
		if (currentAnim)
		{
			_float timePerFrame = 1 / (currentAnim->frameRate * currentAnim->speed);
			while (m_TweenDesc.curr.sumTime > timePerFrame)
			{
				m_TweenDesc.curr.sumTime -=timePerFrame;


				if (!m_bFinished)
				{
					m_TweenDesc.curr.currentFrame = (m_TweenDesc.curr.currentFrame + 1) % currentAnim->frameCount;
					m_TweenDesc.curr.nextFrame = (m_TweenDesc.curr.currentFrame + 1);
				}
				else if (m_bFinished && m_TweenDesc.curr.nextFrame == currentAnim->frameCount - 1)
				{
					if (m_iNextAnimation > 0)
					{
						m_TweenDesc.curr.animIndex = m_iNextAnimation;
						m_iNextAnimation = -1;
						m_TweenDesc.curr.currentFrame = 0;
						m_TweenDesc.curr.nextFrame = 0;
						m_bFinished = false;
					}
				}


				if (m_TweenDesc.curr.nextFrame == currentAnim->frameCount)
				{
					if (m_iNextAnimation < 0 && m_TweenDesc.next.animIndex < 0)
					{
						if (currentAnim->loop)
						{
							m_TweenDesc.curr.nextFrame %= currentAnim->frameCount;
						}
						else
						{
							m_TweenDesc.curr.currentFrame = currentAnim->frameCount - 1;
							m_TweenDesc.curr.nextFrame = currentAnim->frameCount - 1;
							m_bFinished = true;
						}
					}
					else
					{
						if (m_iNextAnimation < 0)
						{
							m_TweenDesc.curr.animIndex = m_iNextAnimation;
							m_iNextAnimation = -1;
							m_TweenDesc.curr.currentFrame = 0;
							m_TweenDesc.curr.nextFrame = 0;
							m_bFinished = false;
						}
						else
						{
							m_TweenDesc.curr = m_TweenDesc.next;
							m_TweenDesc.ClearNextAnim();
							m_bFinished = false;
						}

					}
				}
			}

			m_TweenDesc.curr.ratio = (m_TweenDesc.curr.sumTime / timePerFrame);
		}
	}


	if (m_TweenDesc.next.animIndex >= 0)
	{
		m_TweenDesc.tweenSumTime += deltaTime;
		m_TweenDesc.tweenRatio = m_TweenDesc.tweenSumTime / m_TweenDesc.tweenDuration;

		if (m_TweenDesc.tweenRatio >= 1.f || m_bFinished)
		{
			m_TweenDesc.curr = m_TweenDesc.next;
			m_TweenDesc.ClearNextAnim();
			m_bFinished = false;

		}
		else
		{
			shared_ptr<ModelAnimation> nextAnim = m_pModel->Get_AnimationByIndex(m_TweenDesc.next.animIndex);
			m_TweenDesc.next.sumTime += deltaTime;

			_float timePerFrame = 1.f / (nextAnim->frameRate * nextAnim->speed);

			if (m_TweenDesc.next.ratio >= 1.f)
			{
				m_TweenDesc.next.sumTime = 0;
				m_TweenDesc.next.currentFrame = (m_TweenDesc.next.currentFrame + 1) % nextAnim->frameCount;
				m_TweenDesc.next.nextFrame = (m_TweenDesc.next.currentFrame + 1) % nextAnim->frameCount;
			}

			m_TweenDesc.next.ratio = m_TweenDesc.next.sumTime / timePerFrame;
		}
	}

	if (m_pModel->Get_AnimationCount() > 0)
		m_TweenDesc.curr.animIndex %= m_pModel->Get_AnimationCount();

	_float3 vDistToMove;

	auto& preAnimRootPositions = m_pModel->Get_RootBonePosition()[m_preTweenDesc.curr.animIndex];
	auto& curAnimRootPositions = m_pModel->Get_RootBonePosition()[m_TweenDesc.curr.animIndex];
	{
		if (m_preTweenDesc.curr.animIndex != m_TweenDesc.curr.animIndex)
			vDistToMove = _float3(0.f);
		else if (m_TweenDesc.curr.currentFrame == 0 && m_TweenDesc.curr.nextFrame == 0)
			vDistToMove = _float3(0.f);
		else if (m_TweenDesc.curr.currentFrame <= m_TweenDesc.curr.nextFrame)
		{
			_float3 vPrePosition = _float3::Lerp(preAnimRootPositions[m_preTweenDesc.curr.currentFrame], preAnimRootPositions[m_preTweenDesc.curr.nextFrame], m_preTweenDesc.curr.ratio);
			_float3 vCurPosition = _float3::Lerp(curAnimRootPositions[m_TweenDesc.curr.currentFrame], curAnimRootPositions[m_TweenDesc.curr.nextFrame], m_TweenDesc.curr.ratio);
			vDistToMove = vCurPosition - vPrePosition;
		}
		else
			vDistToMove = _float3(0.f);
	}

	if (vDistToMove != _float3(0.f))
	{

		vDistToMove = _float3::TransformNormal(vDistToMove, Get_Transform()->Get_WorldMatrix());

		Get_Transform()->Go_Dir(vDistToMove);
	}


	//Cal_AnimTransform();
}

void ModelAnimator::Set_NextAnim(_int index)
{
	if (m_bFinished)
	{
		m_TweenDesc.curr.animIndex = index;
		m_iNextAnimation = -1;
		m_TweenDesc.curr.currentFrame = 0;
		m_TweenDesc.curr.nextFrame = 0;
		m_bFinished = false;
	}
	else
	{
		m_iNextAnimation = index;
		m_bFinished = false;
	}

}

void ModelAnimator::Set_Model(shared_ptr<Model> model)
{
	m_pModel = model;

	m_TweenDesc.curr = KeyFrameDesc();
	m_bFinished = false;
	m_TweenDesc.ClearNextAnim();

	m_preTweenDesc.curr = KeyFrameDesc();
	m_preTweenDesc.ClearNextAnim();

	const auto& materials = m_pModel->Get_Materials();

	for (auto& material : materials)
	{
		if(!material->Get_Shader())
			material->Set_Shader(m_pShader);
	}
}

void ModelAnimator::Render()
{
	if (!m_pModel)
		return;

	if (!m_bRenderOn)
		return;

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

	{
		m_pShader->Push_TweenData({m_TweenDesc,m_preTweenDesc});

		m_pShader->GetSRV("TransformMap")->SetResource(m_pModel->Get_TransformSRV().Get());

		auto& desc = m_pModel->Get_AnimAddonDesc();

		m_pShader->Push_AnimAddonData(desc);
	}

	m_pShader->Push_RenderParamData(m_RenderParams);


	auto& world = Get_Transform()->Get_WorldMatrix();
	auto& preWorld = Get_Transform()->Get_preWorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world,preWorld });

	if(!m_pModel->Has_Parts())
	{
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
			m_pShader->DrawIndexed(techniqueIndex, PS_ANIM, mesh->indexBuffer->Get_IndicesNum(), 0, 0);
		}
	}
	else
	{
		const auto meshes = m_pModel->Get_PartsMeshes();

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
			m_pShader->DrawIndexed(techniqueIndex, PS_ANIM, mesh->indexBuffer->Get_IndicesNum(), 0, 0);
		}
	}


}

void ModelAnimator::Render_Instancing(shared_ptr<class InstancingBuffer>& buffer, shared_ptr<InstanceTweenDesc> desc, shared_ptr<InstanceRenderParamDesc> renderParamDesc)
{
	if (!m_pModel)
		return;

	{
		m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());
	}

	m_pShader->Push_InstanceTweenData(*desc);
	m_pShader->Push_InstanceRenderParamData(*renderParamDesc);
	m_pShader->GetSRV("TransformMap")->SetResource(m_pModel->Get_TransformSRV().Get());

	auto& animAddonDesc = m_pModel->Get_AnimAddonDesc();
	buffer->Push_Data();
	m_pShader->Push_AnimAddonData(animAddonDesc);
	if (!m_pModel->Has_Parts())
	{
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
			m_pShader->DrawIndexedInstanced(techniqueIndex, PS_ANIMINSTANCING, mesh->indexBuffer->Get_IndicesNum(), buffer->Get_Count());
		}
	}
	else
	{
		const auto meshes = m_pModel->Get_PartsMeshes();

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

			m_pShader->DrawIndexedInstanced(0, PS_ANIMINSTANCING, mesh->indexBuffer->Get_IndicesNum(), buffer->Get_Count());
		}

	}
}

void ModelAnimator::Render_Shadow()
{
	if (!m_pModel)
		return;

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

	{
		m_pShader->Push_TweenData({m_TweenDesc,m_preTweenDesc});

		m_pShader->GetSRV("TransformMap")->SetResource(m_pModel->Get_TransformSRV().Get());

		auto& desc = m_pModel->Get_AnimAddonDesc();

		m_pShader->Push_AnimAddonData(desc);
	}

	auto& world = Get_Transform()->Get_WorldMatrix();
	auto& preWorld = Get_Transform()->Get_preWorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world,preWorld });
	if (!m_pModel->Has_Parts())
	{
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
			m_pShader->DrawIndexed(0, PS_ANIM_SHADOW, mesh->indexBuffer->Get_IndicesNum(), 0, 0);
		}
	}
	else
	{
		const auto meshes = m_pModel->Get_PartsMeshes();

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
			m_pShader->DrawIndexed(0, PS_ANIM_SHADOW, mesh->indexBuffer->Get_IndicesNum(), 0, 0);
		}
	}

	if (KEYTAP(KEY_TYPE::F6))
		int a = 0;
}

void ModelAnimator::Render_Shadow_Instancing(shared_ptr<InstancingBuffer>& buffer, shared_ptr<InstanceTweenDesc> desc, shared_ptr<InstanceRenderParamDesc> renderParamDesc)
{
	if (!m_pModel)
		return;

	{
		m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());
	}

	m_pShader->Push_InstanceTweenData(*desc);
	m_pShader->Push_InstanceRenderParamData(*renderParamDesc);

	m_pShader->GetSRV("TransformMap")->SetResource(m_pModel->Get_TransformSRV().Get());

	auto& animAddonDesc = m_pModel->Get_AnimAddonDesc();

	m_pShader->Push_AnimAddonData(animAddonDesc);
	buffer->Push_Data();
	if (!m_pModel->Has_Parts())
	{
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

			m_pShader->DrawIndexedInstanced(0, PS_ANIM_SHADOW_INSTANCING, mesh->indexBuffer->Get_IndicesNum(), buffer->Get_Count());
		}
	}
	else
	{
		const auto meshes = m_pModel->Get_PartsMeshes();

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

			m_pShader->DrawIndexedInstanced(0, PS_ANIM_SHADOW_INSTANCING, mesh->indexBuffer->Get_IndicesNum(), buffer->Get_Count());
		}

	}
}

void ModelAnimator::Render_MotionBlur()
{
	if (!m_pModel)
		return;

	if (!m_bRenderOn)
		return;
	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());
	auto preView = CUR_SCENE->Get_MainCamera()->Get_Transform()->Get_preWorldMatrix().Invert();
	m_pShader->GetMatrix("g_preView")->SetMatrix((_float*)&preView);

	{
		m_pShader->Push_TweenData({ m_TweenDesc,m_preTweenDesc });
		m_pShader->GetSRV("TransformMap")->SetResource(m_pModel->Get_TransformSRV().Get());

		auto& desc = m_pModel->Get_AnimAddonDesc();
		m_pShader->Push_AnimAddonData(desc);
	}
	m_pShader->Push_RenderParamData(m_RenderParams);


	auto& world = Get_Transform()->Get_WorldMatrix();
	auto& preWorld = Get_Transform()->Get_preWorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world,preWorld });
	if (!m_pModel->Has_Parts())
	{
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

			m_pShader->DrawIndexed(2, 1, mesh->indexBuffer->Get_IndicesNum(), 0, 0);
		}
	}
	else
	{
		const auto meshes = m_pModel->Get_PartsMeshes();

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

			m_pShader->DrawIndexed(2, 1, mesh->indexBuffer->Get_IndicesNum(), 0, 0);
		}
	}

}

void ModelAnimator::Render_Forward()
{
	if (!m_pModel)
		return;

	if (!m_bRenderOn)
		return;

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

	{
		m_pShader->Push_TweenData({ m_TweenDesc,m_preTweenDesc });

		m_pShader->GetSRV("TransformMap")->SetResource(m_pModel->Get_TransformSRV().Get());

		auto& desc = m_pModel->Get_AnimAddonDesc();

		m_pShader->Push_AnimAddonData(desc);
	}

	m_pShader->Push_RenderParamData(m_RenderParams);


	auto& world = Get_Transform()->Get_WorldMatrix();
	auto& preWorld = Get_Transform()->Get_preWorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world,preWorld });
	m_pShader->Push_LightData(CUR_SCENE->Get_LightParams());
	m_pShader->GetScalar("lightIndex")->SetInt(0);
	if (!m_pModel->Has_Parts())
	{
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
			m_pShader->DrawIndexed(techniqueIndex, PS_FORWARD, mesh->indexBuffer->Get_IndicesNum(), 0, 0);
		}
	}
	else
	{
		const auto meshes = m_pModel->Get_PartsMeshes();

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
			m_pShader->DrawIndexed(techniqueIndex, PS_FORWARD, mesh->indexBuffer->Get_IndicesNum(), 0, 0);
		}
	}
}

InstanceID ModelAnimator::Get_InstanceID()
{
	return make_pair(_ulonglong(m_pModel.get()), _ulonglong(m_pShader.get()));
}

_float4x4 ModelAnimator::Get_CurAnimTransform(_int boneIndex)
{
	shared_ptr<ModelBone> bone = m_pModel->Get_BoneByIndex(boneIndex);

	_float4x4 curMat = m_pModel->Get_AnimationTransform(m_TweenDesc.curr.animIndex, m_TweenDesc.curr.currentFrame, boneIndex);
	_float4x4 nextMat = m_pModel->Get_AnimationTransform(m_TweenDesc.curr.animIndex, m_TweenDesc.curr.nextFrame, boneIndex);

	_float4x4 matAnimation = Transform::SLerpMatrix(curMat, nextMat, m_TweenDesc.curr.ratio);

	if (m_TweenDesc.next.animIndex >= 0)
	{
		curMat = m_pModel->Get_AnimationTransform(m_TweenDesc.next.animIndex, m_TweenDesc.next.currentFrame, boneIndex);
		nextMat = m_pModel->Get_AnimationTransform(m_TweenDesc.next.animIndex, m_TweenDesc.next.nextFrame, boneIndex);

		_float4x4 matNext = Transform::SLerpMatrix(curMat, nextMat, m_TweenDesc.next.ratio);

		matAnimation = Transform::SLerpMatrix(matAnimation, matNext, m_TweenDesc.tweenRatio);
	}

	_float4x4 invGlobal = bone->offsetTransform;

	return matAnimation;

}

void ModelAnimator::Set_NextTweenAnim(const wstring& animName, _float tweenDuration, _bool loop, _float animSpeed)
{
	_int animIndex = -1;
	auto anim = Get_Model()->Get_AnimationByName(animName, &animIndex);
	assert(animIndex >= 0);

	m_TweenDesc.ClearNextAnim();
	m_TweenDesc.next.animIndex = animIndex;
	m_TweenDesc.tweenDuration = tweenDuration;

	anim->loop = loop;
	anim->speed = animSpeed;
}

void ModelAnimator::Set_CurrentAnim(_int animIndex, _bool loop, _float animSpeed)
{
	auto anim = Get_Model()->Get_AnimationByIndex(animIndex);
	assert(animIndex >= 0);

	m_TweenDesc.ClearNextAnim();
	Set_CurrentAnim(animIndex);

	anim->loop = loop;
	anim->speed = animSpeed;
}

void ModelAnimator::Set_CurrentAnim(const wstring& animName, _bool loop, _float animSpeed)
{
	_int animIndex = -1;
	auto anim = Get_Model()->Get_AnimationByName(animName, &animIndex);
	assert(animIndex >= 0);

	m_TweenDesc.ClearNextAnim();
	Set_CurrentAnim(animIndex);

	anim->loop = loop;
	anim->speed = animSpeed;
}

void ModelAnimator::Set_AnimationSpeed(_float animSpeed)
{
	_int animIndex = m_TweenDesc.curr.animIndex;
	auto anim = Get_Model()->Get_AnimationByIndex(animIndex);
	assert(animIndex >= 0);

	anim->speed = animSpeed;
}

_uint ModelAnimator::Get_FinalFrame(_int animIndex)
{
	return m_pModel->Get_AnimationByIndex(animIndex)->frameCount;
}
