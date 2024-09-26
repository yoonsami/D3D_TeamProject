#include "pch.h"
#include "FSM.h"

#include "ModelAnimator.h"
#include "GroupEffect.h"
#include "GroupEffectData.h"
#include "GroupEffectOwner.h"
#include "Camera.h"

FSM::FSM() : Component(COMPONENT_TYPE::FSM)
{
}


void FSM::Calculate_CamBoneMatrix()
{
	m_Dummy_CP_BoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iCenterBoneIndex) *
		_float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();
	
	m_CenterBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iCenterBoneIndex) *
		_float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

	m_CamBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iCamBoneIndex) *
		_float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

	m_SkillCamBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iSkillCamBoneIndex) *
		_float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();



	m_vDummy_CP_BonePos = _float4{ m_Dummy_CP_BoneMatrix.Translation().x, m_Dummy_CP_BoneMatrix.Translation().y, m_Dummy_CP_BoneMatrix.Translation().z , 1.f };
	m_vCenterBonePos = _float4{ m_CenterBoneMatrix.Translation().x, m_CenterBoneMatrix.Translation().y, m_CenterBoneMatrix.Translation().z , 1.f };
	m_vCamBonePos = _float4{ m_CamBoneMatrix.Translation().x, m_CamBoneMatrix.Translation().y, m_CamBoneMatrix.Translation().z , 1.f };
	m_vSkillCamBonePos = _float4{ m_SkillCamBoneMatrix.Translation().x, m_SkillCamBoneMatrix.Translation().y, m_SkillCamBoneMatrix.Translation().z , 1.f };
}

_uint FSM::Get_CurFrame()
{
	if (m_pOwner.expired())
		assert(false);

	if (!Get_Owner()->Get_Animator())
		assert(false);

	auto& desc = Get_Owner()->Get_Animator()->Get_TweenDesc();
	if (desc.next.animIndex >= 0)
		return desc.next.currentFrame;
	else
		return desc.curr.currentFrame;
}

_uint FSM::Get_FinalFrame()
{
	if (m_pOwner.expired())
		assert(false);

	if (!Get_Owner()->Get_Animator())
		assert(false);

	auto& desc = Get_Owner()->Get_Animator()->Get_TweenDesc();
	if (desc.next.animIndex >= 0)
		return Get_Owner()->Get_Animator()->Get_FinalFrame(desc.next.animIndex);
	else
		return Get_Owner()->Get_Animator()->Get_FinalFrame(desc.curr.animIndex);
}

void FSM::Reset_Frame()
{
	if (m_pOwner.expired())
		assert(false);

	if (!Get_Owner()->Get_Animator())
		assert(false);

	Get_Owner()->Get_Animator()->Reset_Frame();
}

_bool FSM::Is_AnimFinished()
{
	if (m_pOwner.expired())
		assert(false);

	if (!Get_Owner()->Get_Animator())
		assert(false);

	auto animator = Get_Owner()->Get_Animator();
	auto& desc = Get_Owner()->Get_Animator()->Get_TweenDesc();
	if (desc.next.animIndex < 0 && animator->Is_Finished())
		return true;
	return false;
}

_float3 FSM::Get_InputDirVector()
{
	auto pCamera = CUR_SCENE->Get_MainCamera();
	if (!pCamera)
		return _float3(0.f);

	_float3 vCameraLook = pCamera->Get_Transform()->Get_State(Transform_State::LOOK).xyz();
	vCameraLook.y = 0;
	vCameraLook.Normalize();
	_float3 vCameraRight = pCamera->Get_Transform()->Get_State(Transform_State::RIGHT).xyz();
	_float3 vCurLook = Get_Transform()->Get_State(Transform_State::LOOK).xyz();
	vCurLook.Normalize();

	_float2 vInput = { 0.f,0.f };
	if (KEYTAP(KEY_TYPE::W) || KEYHOLD(KEY_TYPE::W))
		vInput.x += 1.f;
	if (KEYTAP(KEY_TYPE::A) || KEYHOLD(KEY_TYPE::A))
		vInput.y -= 1.f;
	if (KEYTAP(KEY_TYPE::S) || KEYHOLD(KEY_TYPE::S))
		vInput.x -= 1.f;
	if (KEYTAP(KEY_TYPE::D) || KEYHOLD(KEY_TYPE::D))
		vInput.y += 1.f;

	vInput.Normalize();

	_float3 vInputDir = vInput.x * vCameraLook + vInput.y * vCameraRight;

	vInputDir.Normalize();
	return vInputDir;
}

void FSM::Soft_Turn_ToInputDir(const _float3& vInputDir, _float turnSpeed)
{
	Quaternion qCurRot = Get_Transform()->Get_Rotation();
	Quaternion InputQuaternion = Transform::Get_QuatFromLook(vInputDir);

	Quaternion newQ = Quaternion::Slerp(qCurRot, InputQuaternion, turnSpeed * fDT);

	Get_Transform()->Set_Quaternion(newQ);
}

_float3 FSM::Soft_Turn_ToTarget(const _float3& vTargetPos, _float turnSpeed)
{
	return Soft_Turn_ToTarget(_float4(vTargetPos, 1.f), turnSpeed);
}

_float3 FSM::Soft_Turn_ToTarget(const _float4& vTargetPos, _float turnSpeed)
{
	Quaternion qCurRot = Get_Transform()->Get_Rotation();

	_float3 vDir = (vTargetPos - Get_Transform()->Get_State(Transform_State::POS)).xyz();
	vDir.y = 0.f;
	Quaternion InputQuaternion = Transform::Get_QuatFromLook(vDir);

	Quaternion newQ = Quaternion::Slerp(qCurRot, InputQuaternion, turnSpeed * fDT);

	Get_Transform()->Set_Quaternion(newQ);

	return vDir;
}

_bool FSM::Target_In_AttackRange(_float* pGap)
{
	_bool bFlag = false;
	
	if (m_pTarget.expired())
		return false;

	_float fGap = (Get_Transform()->Get_State(Transform_State::POS).xyz() -
		m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS).xyz()).Length();

	if (pGap)
		*pGap = fGap;

	if (fGap <= m_fAttackRange)
		bFlag = true;

	return bFlag;
}

_bool FSM::LookingTarget_In_AttackRange(_float* pGap)
{
	_bool bFlag = false;

	if (m_pLookingTarget.expired())
		return false;

	_float fGap = (Get_Transform()->Get_State(Transform_State::POS).xyz() -
		m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS).xyz()).Length();

	if (pGap)
		*pGap = fGap;

	if (fGap <= m_fAttackRange)
		bFlag = true;

	return bFlag;
}

_bool FSM::Target_In_DetectRange()
{
	_bool bFlag = false;

	if (m_pTarget.expired())
		return false;

	_float fGap = (Get_Transform()->Get_State(Transform_State::POS).xyz() -
		m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS).xyz()).Length();

	if (fGap <= m_fDetectRange)
		bFlag = true;

	return bFlag;
}

_bool FSM::TargetGroup_In_DetectRange(_uint eType)
{
	_bool bFlag = false;

	auto& gameObjects = CUR_SCENE->Get_Objects();
	shared_ptr<GameObject> target;
	_float fMinDistSQ = FLT_MAX;
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->Get_ObjectGroup() != eType)
			continue;

		if (gameObject->Get_CurHp() <= 0.f)
			continue;

		if (gameObject->Get_FSM())
		{
			if (gameObject->Get_FSM()->Get_EntryTeam() == false)
				continue;
		}



		_float3 vOwnerPos = Get_Transform()->Get_State(Transform_State::POS).xyz();
		_float3 vObjectPos = gameObject->Get_Transform()->Get_State(Transform_State::POS).xyz();
		_float distSQ = (vOwnerPos - vObjectPos).LengthSquared();
		
		if (distSQ < m_fDetectRange * m_fDetectRange)
		{
			bFlag = true;
		}

		if (fMinDistSQ > distSQ)
		{
			//Target Setting
			fMinDistSQ = distSQ;
			m_pTarget = gameObject;
		}
	}

	return bFlag;
}

_bool FSM::Target_In_GazeCheckRange()
{
	_bool bFlag = false;

	if (m_pTarget.expired())
		return false;

	_float fGap = (Get_Transform()->Get_State(Transform_State::POS).xyz() -
		m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS).xyz()).Length();

	if (fGap <= m_fGazeCheckRange)
		bFlag = true;

	return bFlag;
}

_bool FSM::CounterAttackCheck(_float fCheckDegree)
{
	// Monster to player 
	_float4 vDir = _float4(0.f);
	vDir = m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) - Get_Transform()->Get_State(Transform_State::POS);
	vDir.y = 0.f;
	vDir.Normalize();

	_float4 vDot = _float4(0.f);
	_float4 vCross = _float4(0.f);

	vDot = XMVector3Dot(Get_Transform()->Get_State(Transform_State::LOOK), vDir);
	vCross = XMVector3Cross(Get_Transform()->Get_State(Transform_State::LOOK), vDir);

	if (XMVectorGetX(vDot) >= 0.f) //forward = dot is bigger 0  
	{
		if (XMVectorGetY(vCross) < 0.f) //cross > 0 is right , cross < 0 is left
		{
			if (XMVectorGetX(vDot) >= cosf(XMConvertToRadians(fCheckDegree / 2.f)))//üũ ���� �ݸ�ŭ ���� ��
				return true;
			else
				return false;
		}
		else 
		{
			if (XMVectorGetX(vDot) >= cosf(XMConvertToRadians(fCheckDegree / 2.f)))//üũ���� �ݸ�ŭ ������ ��
				return true;
			else
				return false;
		}
	}
	else 
	{
		return false;
	}
}

void FSM::Set_DirToTarget()
{
	if (!m_pLookingTarget.expired())
	{
		m_vDirToTarget = (m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) - Get_Transform()->Get_State(Transform_State::POS)).xyz();
		m_vDirToTarget.y = 0.f;
	}
}

void FSM::Set_DirToTargetOrInput(_uint eType)
{
	m_pLookingTarget.reset();
	if (m_pTarget.expired())
		m_pLookingTarget = Find_TargetInFrustum(eType);
	else
		m_pLookingTarget = m_pTarget.lock();

	if (!m_pLookingTarget.expired())
	{
		m_vDirToTarget = (m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) - Get_Transform()->Get_State(Transform_State::POS)).xyz();
		m_vDirToTarget.y = 0.f;
	}
	else
		m_vDirToTarget = Get_InputDirVector();
}

void FSM::Set_DirToTarget_Monster(_uint eType)
{
	m_pTarget.reset();

	m_pTarget = Find_TargetInFrustum(eType, false);

	if (!m_pTarget.expired())
	{
		m_vDirToTarget = (m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) - Get_Transform()->Get_State(Transform_State::POS)).xyz();
		m_vDirToTarget.y = 0.f;
	}
}

void FSM::Look_DirToTarget(_float fTurnSpeed)
{
	if (m_vDirToTarget != _float3(0.f))
		Soft_Turn_ToInputDir(m_vDirToTarget, fTurnSpeed);
}

shared_ptr<GameObject> FSM::Find_TargetInFrustum(_uint eType, _bool bFrustumCheck)
{
	auto& gameObjects = CUR_SCENE->Get_Objects();
	shared_ptr<GameObject> target;
	_float fMinDistSQ = FLT_MAX;
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->Get_ObjectGroup() != eType)
			continue;

		if (gameObject->Get_CurHp() <= 0.f)
			continue;

		_float3 vOwnerPos = Get_Transform()->Get_State(Transform_State::POS).xyz();
		_float3 vObjectPos = gameObject->Get_Transform()->Get_State(Transform_State::POS).xyz();
		_float distSQ = (vOwnerPos - vObjectPos).LengthSquared();
		if(distSQ > m_fDetectRange * m_fDetectRange)
			continue;
		
		if (bFrustumCheck)
		{
			_float3 viewPos = _float3::Transform(vObjectPos, CUR_SCENE->Get_MainCamera()->Get_Camera()->Get_ViewMat());
			
			if(viewPos.z <0)
				continue;
		}

		if (fMinDistSQ > distSQ)
		{
			fMinDistSQ = distSQ;
			target = gameObject;
		}
		
	}

	return target;
}

_bool FSM::Init_CurFrame(const _uint curFrame)
{
	if (Get_CurFrame() == curFrame)
	{
		if (m_iPreFrame != m_iCurFrame)
			return true;
	}
	return false;
}

void FSM::Add_Effect(const wstring& strSkilltag, shared_ptr<MonoBehaviour> pScript, const _float4x4& matPivot, _bool bUseAsItis)
{
	shared_ptr<GameObject> pGroupEffectObj = make_shared<GameObject>();

	// For. Transform 
	pGroupEffectObj->GetOrAddTransform();
	if(bUseAsItis)
		pGroupEffectObj->Get_Transform()->Set_WorldMat(matPivot);
	else
		pGroupEffectObj->Get_Transform()->Set_WorldMat(matPivot * m_pOwner.lock()->Get_Transform()->Get_WorldMatrix());

	// For. GroupEffectData 
	wstring wstrFileName = strSkilltag + L".dat";
	wstring wtsrFilePath = TEXT("..\\Resources\\EffectData\\GroupEffectData\\") + wstrFileName;
	shared_ptr<GroupEffectData> pGroupEffectData = RESOURCES.GetOrAddGroupEffectData(strSkilltag, wtsrFilePath);

	if (pGroupEffectData == nullptr)
		return;

	// For. GroupEffect component 
	shared_ptr<GroupEffect> pGroupEffect = make_shared<GroupEffect>();

	pGroupEffectObj->Add_Component(pGroupEffect);
	pGroupEffectObj->Get_GroupEffect()->Set_Tag(pGroupEffectData->Get_GroupEffectDataTag());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectData(pGroupEffectData->Get_MemberEffectData());
	pGroupEffectObj->Get_GroupEffect()->Set_InitWorldMatrix(pGroupEffectObj->Get_Transform()->Get_WorldMatrix());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectMaterials();
	pGroupEffectObj->Set_Name(strSkilltag);
	pGroupEffectObj->Init();

	// For. Script 
	if(nullptr != pScript)
		pGroupEffectObj->Get_GroupEffect()->Set_Script(pScript);

	// For. Add Effect GameObject to current scene
	EVENTMGR.Create_Object(pGroupEffectObj);
}

void FSM::Add_And_Set_Effect(const wstring& strSkilltag, shared_ptr<MonoBehaviour> pScript)
{
	shared_ptr<GameObject> pGroupEffectObj = make_shared<GameObject>();

	// For. Transform 
	pGroupEffectObj->GetOrAddTransform();
	pGroupEffectObj->Get_Transform()->Set_State(Transform_State::POS, m_pOwner.lock()->Get_Transform()->Get_State(Transform_State::POS));
	pGroupEffectObj->Get_Transform()->Set_Quaternion(Get_Transform()->Get_Rotation());
	pGroupEffectObj->Set_Name(strSkilltag);
	// For. GroupEffectData 
	wstring wstrFileName = strSkilltag + L".dat";
	wstring wtsrFilePath = TEXT("..\\Resources\\EffectData\\GroupEffectData\\") + wstrFileName;
	shared_ptr<GroupEffectData> pGroupEffectData = RESOURCES.GetOrAddGroupEffectData(strSkilltag, wtsrFilePath);

	if (pGroupEffectData == nullptr)
		return;

	// For. GroupEffect component 
	shared_ptr<GroupEffect> pGroupEffect = make_shared<GroupEffect>();
	pGroupEffectObj->Add_Component(pGroupEffect);
	pGroupEffectObj->Get_GroupEffect()->Set_Tag(pGroupEffectData->Get_GroupEffectDataTag());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectData(pGroupEffectData->Get_MemberEffectData());
	pGroupEffectObj->Get_GroupEffect()->Set_InitWorldMatrix(Get_Transform()->Get_WorldMatrix());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectMaterials();
	pGroupEffectObj->Set_Name(strSkilltag);
	m_vGroupEffect.push_back(pGroupEffectObj);
	pGroupEffectObj->Init();

	// For. Script 
	if (nullptr != pScript)
		pGroupEffectObj->Get_GroupEffect()->Set_Script(pScript);

	// For. Add Effect GameObject to current scene
	EVENTMGR.Create_Object(pGroupEffectObj);

}

void FSM::Add_GroupEffectOwner(const wstring& strSkilltag, _float3 vPosOffset, _bool usePosAs, shared_ptr<MonoBehaviour> pScript)
{
	shared_ptr<GameObject> pGroupEffectOwnerObj = make_shared<GameObject>();

	// For. Transform 
	pGroupEffectOwnerObj->GetOrAddTransform();
	if(!usePosAs)
	{
		_float4 vOwnerLook = m_pOwner.lock()->Get_Transform()->Get_State(Transform_State::LOOK);
		vOwnerLook.Normalize();
		_float4 vOwnerRight = m_pOwner.lock()->Get_Transform()->Get_State(Transform_State::RIGHT);
		vOwnerRight.Normalize();
		_float4 vOwnerUp = m_pOwner.lock()->Get_Transform()->Get_State(Transform_State::UP);
		vOwnerUp.Normalize();
		_float4 vOwnerPos = m_pOwner.lock()->Get_Transform()->Get_State(Transform_State::POS)
			+ vOwnerRight * vPosOffset.x
			+ vOwnerUp * vPosOffset.y
			+ vOwnerLook * vPosOffset.z;
		pGroupEffectOwnerObj->Get_Transform()->Set_State(Transform_State::POS, vOwnerPos);
	}
	else
		pGroupEffectOwnerObj->Get_Transform()->Set_State(Transform_State::POS, _float4(vPosOffset,1.f));
	pGroupEffectOwnerObj->Get_Transform()->Set_Quaternion(Get_Transform()->Get_Rotation());

	// For. GroupEffect component 
	shared_ptr<GroupEffectOwner> pGroupEffect = make_shared<GroupEffectOwner>();
	pGroupEffectOwnerObj->Add_Component(pGroupEffect);
	pGroupEffectOwnerObj->Get_GroupEffectOwner()->Set_GroupEffectTag(strSkilltag);
	if(nullptr != pScript)
		pGroupEffectOwnerObj->Get_GroupEffectOwner()->Set_GroupEffectScript(pScript);

	// For. Add Effect GameObject to current scene
	EVENTMGR.Create_Object(pGroupEffectOwnerObj);
}

void FSM::KillAllEffect()
{
	for (auto& iter : m_vGroupEffect)
	{
		if (!iter.expired())
		{
			iter.lock()->Get_GroupEffect()->Kill_All();
			EVENTMGR.Delete_Object(iter.lock());
			iter.reset();
		}
	}
}

void FSM::Update_GroupEffectWorldPos(const _float4x4& mWorldMatrix)
{
	for (auto& iter : m_vGroupEffect)
	{
		if(!iter.expired())
			iter.lock()->Get_Transform()->Set_WorldMat(mWorldMatrix);
	}
}

void FSM::Add_FDistortion_Effect(const wstring& strSkilltag)
{
	shared_ptr<GameObject> pGroupEffectObj = make_shared<GameObject>();

	// For. Transform 
	pGroupEffectObj->GetOrAddTransform();
	//pGroupEffectObj->Get_Transform()->Set_State(Transform_State::POS, m_pOwner.lock()->Get_Transform()->Get_State(Transform_State::POS));
	//pGroupEffectObj->Get_Transform()->Set_Quaternion(Get_Transform()->Get_Rotation());
	pGroupEffectObj->Get_Transform()->Set_WorldMat(m_pOwner.lock()->Get_Transform()->Get_WorldMatrix());

	// For. GroupEffectData 
	wstring wstrFileName = strSkilltag + L".dat";
	wstring wtsrFilePath = TEXT("..\\Resources\\EffectData\\FDistortionGroupEffectData\\") + wstrFileName;
	shared_ptr<GroupEffectData> pGroupEffectData = RESOURCES.GetOrAddGroupEffectData(strSkilltag, wtsrFilePath);


	if (pGroupEffectData == nullptr)
		return;

	// For. GroupEffect component 
	shared_ptr<GroupEffect> pGroupEffect = make_shared<GroupEffect>();

	pGroupEffectObj->Add_Component(pGroupEffect);
	pGroupEffectObj->Get_GroupEffect()->Set_Tag(pGroupEffectData->Get_GroupEffectDataTag());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectData(pGroupEffectData->Get_MemberEffectData());
	pGroupEffectObj->Get_GroupEffect()->Set_InitWorldMatrix(pGroupEffectObj->Get_Transform()->Get_WorldMatrix());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectMaterials();
	pGroupEffectObj->Set_Name(strSkilltag);
	pGroupEffectObj->Init();

	// For. Add Effect GameObject to current scene
	EVENTMGR.Create_Object(pGroupEffectObj);
}

void FSM::Add_And_Set_FDistortion_Effect(const wstring& strSkilltag)
{
}

void FSM::Add_FDistortion_GroupEffectOwner(const wstring& strSkilltag, _float3 vPosOffset)
{
}

void FSM::Update_FDistortion_GroupEffectWorldPos()
{
}

void FSM::FreeLoopMembers()
{
	for (auto& iter : m_vGroupEffect)
	{
		if (!iter.expired())
			iter.lock()->Get_GroupEffect()->FreeLoopMember();
	}
}

void FSM::Cal_SkillCamDirection(const _float dist)
{
	_float3 vDir = _float3(0.f);
	vDir = (m_pCamera.lock()->Get_Transform()->Get_State(Transform_State::POS) - Get_Transform()->Get_State(Transform_State::POS)).xyz();
	vDir.y = 0.f;
	vDir.Normalize();

	_float3 vCross = _float3(0.f);
	vCross = vDir.Cross(Get_Transform()->Get_State(Transform_State::LOOK).xyz());

	if ((vCross.y) < 0.f) // LEFT 
		m_vSkillCamRight = (Get_Transform()->Get_State(Transform_State::RIGHT) * -dist);
	else //RIGHT	
		m_vSkillCamRight = (Get_Transform()->Get_State(Transform_State::RIGHT) * dist);
}

_bool FSM::Check_Combo(_uint minFrame, KEY_TYPE eKeyType)
{
	if (Get_CurFrame() < minFrame)
	{
		if (KEYTAP(eKeyType))
			m_bCanCombo = true;
	}

	if (m_bCanCombo)
	{
		if (Get_CurFrame() > minFrame)
			return true;
	}

	return false;
}

_bool FSM::DeadCheck()
{
	if (!m_pOwner.expired())
	{
		if (m_pOwner.lock()->Get_CurHp() > 0.f)
			m_bIsDead = false;
		else
			m_bIsDead = true;
	}

	return m_bIsDead;
}

void FSM::Set_HitColor()
{
	if(Get_Owner()->Get_Animator())
	{
		_float4& renderParam = Get_Owner()->Get_Animator()->Get_RenderParamDesc().vec4Params[0];
		renderParam.x = 1.f;
		renderParam.y = 1.f;
		renderParam.z = 1.f;
	}
}

void FSM::Recovery_Color()
{
	if (Get_Owner()->Get_Animator())
	{
		_float4& param = Get_Owner()->Get_Animator()->Get_RenderParamDesc().vec4Params[0];
		param.x = clamp(param.x - 10.f * fDT, 0.f, FLT_MAX);
		param.y = clamp(param.y - 10.f * fDT, 0.f, FLT_MAX);
		param.z = clamp(param.z - 10.f * fDT, 0.f, FLT_MAX);
	}
}

void FSM::Set_Target(shared_ptr<GameObject> pTarget)
{
	m_pTarget = pTarget;
}

void FSM::Set_Camera(shared_ptr<GameObject> pCamera)
{
	m_pCamera = pCamera;
}

void FSM::Set_Vehicle(shared_ptr<GameObject> pVehicle)
{
	m_pVehicle = pVehicle;
}

void FSM::Reset_Target()
{
	m_pTarget.reset();
}

void FSM::Reset_Weapon()
{
	//if (!m_pWeapon.expired())
	//	CUR_SCENE->Remove_GameObject(m_pWeapon.lock());
}

void FSM::Reset_Vehicle()
{
	if (!m_pVehicle.expired())
		EVENTMGR.Delete_Object(m_pVehicle.lock());
}

void FSM::Remove_Object()
{
	if (!m_pAttackCollider.expired())
		EVENTMGR.Delete_Object(m_pAttackCollider.lock());
	
	if (!m_pOwner.expired())
		EVENTMGR.Delete_Object(m_pOwner.lock());
}
