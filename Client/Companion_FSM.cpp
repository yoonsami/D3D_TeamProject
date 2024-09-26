#include "pch.h"
#include "Companion_FSM.h"
#include "Camera.h"

_bool Companion_FSM::Can_Interact()
{
	_float3 pos = Get_Transform()->Get_State(Transform_State::POS).xyz();

	if (_float3::Transform(pos, CUR_SCENE->Get_MainCamera()->Get_Camera()->Get_ViewMat()).z < 0)
		return false;

	auto pPlayer = GET_PLAYER;

	if (pPlayer)
	{
		_float lengthSQ = (pPlayer->Get_Transform()->Get_State(Transform_State::POS).xyz() - pos).LengthSquared();

		if (lengthSQ < 3.f * 3.f)
			return true;
	}


	return false;
}

_bool Companion_FSM::Go_Follow()
{
	_float3 pos = Get_Transform()->Get_State(Transform_State::POS).xyz();

	/*if (_float3::Transform(pos, CUR_SCENE->Get_MainCamera()->Get_Camera()->Get_ViewMat()).z < 0)
		return false;*/

	auto pPlayer = GET_PLAYER;

	if (pPlayer)
	{
		_float lengthSQ = (pPlayer->Get_Transform()->Get_State(Transform_State::POS).xyz() - pos).LengthSquared();

		if (lengthSQ > 5.f * 5.f)
			return true;
	}


	return false;
}

void Companion_FSM::Set_DirToTarget_Companion(_uint eType)
{
	m_pLookingTarget.reset();

	m_pLookingTarget = Find_TargetInFrustum(eType, false);

	if (!m_pLookingTarget.expired())
	{
		m_vDirToTarget = (m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) - Get_Transform()->Get_State(Transform_State::POS)).xyz();
		m_vDirToTarget.y = 0.f;
		m_bDetected = true;
	}
	else
	{
		m_bDetected = false;
	}
}

void Companion_FSM::Detect_Target()
{
	if (m_bEntryTeam)
	{
		m_tDetectCoolTime.fAccTime += fDT;

		if (m_tDetectCoolTime.fAccTime >= m_tDetectCoolTime.fCoolTime)
		{
			m_tDetectCoolTime.fAccTime = 0.f;
			Set_DirToTarget_Companion(OBJ_MONSTER);
		}
	}
}

void Companion_FSM::Calculate_EvadeCool()
{
	if (!m_bCanEvade)
	{
		m_tEvadeCoolTime.fAccTime += fDT;
		
		if (m_tEvadeCoolTime.fAccTime >= m_tEvadeCoolTime.fCoolTime)
		{
			m_tEvadeCoolTime.fAccTime = 0.f;
			m_bCanEvade = true;
		}
	}
}
