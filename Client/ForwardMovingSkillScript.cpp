#include "pch.h"
#include "ForwardMovingSkillScript.h"

ForwardMovingSkillScript::ForwardMovingSkillScript(FORWARDMOVINGSKILLDESC desc)
{
	m_vLook = desc.vSkillDir;
	m_fMoveSpeed = desc.fMoveSpeed;
	m_tLifeTime.fCoolTime = desc.fLifeTime;

	if (desc.fLimitDistance > 0.f)
		m_fLimitDistance = desc.fLimitDistance;
}

ForwardMovingSkillScript::~ForwardMovingSkillScript()
{
}

HRESULT ForwardMovingSkillScript::Init()
{
	Get_Transform()->Set_State(Transform_State::LOOK, m_vLook);
	Get_Transform()->Set_Speed(m_fMoveSpeed);
	m_vFirstPos = Get_Transform()->Get_State(Transform_State::POS);

	return S_OK;
}

void ForwardMovingSkillScript::Tick()
{
	m_tLifeTime.fAccTime += fDT;

	if (m_fLimitDistance == 0.f)
		Get_Transform()->Go_Straight();
	else
	{
		//IF LIMITDISTANCE IS NOT ZERO, THIS SKILL GO STRAIGHT AND STOP ON LIMIT

		_float fGap = (Get_Transform()->Get_State(Transform_State::POS).xyz() - m_vFirstPos.xyz()).Length();

		if (fGap < m_fLimitDistance)
			Get_Transform()->Go_Straight();
	}

	if (m_tLifeTime.fAccTime >= m_tLifeTime.fCoolTime)
		EVENTMGR.Delete_Object(m_pOwner.lock());
}
