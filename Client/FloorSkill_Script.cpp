#include "pch.h"
#include "FloorSkill_Script.h"
#include "AttackColliderInfoScript.h"
#include "SphereCollider.h"

FloorSkill_Script::FloorSkill_Script(FLOORSKILLDESC desc)
{
	m_vLook = desc.vSkillDir;
	m_fFirstAttackDamage = desc.fAttackDamage;
	m_fAttackDamage = desc.fAttackDamage;
	m_fLastAttackDamage = desc.fLastAttackDamage;
	
	m_bFirstAttack = desc.bFirstAttack;

	m_tAttackStartTime.fCoolTime = desc.fAttackStartGap;
	m_tAttackTickTime.fCoolTime = desc.fAttackTickTime;

	if (m_bFirstAttack)
		m_tAttackTickTime.fAccTime = desc.fAttackTickTime;
	else
		m_tAttackTickTime.fAccTime = 0.f;

	m_iLimitAttackCnt = desc.iLimitAttackCnt;

	m_strAttackType = desc.strAttackType;
	m_strLastAttackType = desc.strLastAttackType;
}

FloorSkill_Script::~FloorSkill_Script()
{
}

HRESULT FloorSkill_Script::Init()
{
	Get_Transform()->Set_LookDir(m_vLook.xyz());
	m_vFirstPos = Get_Transform()->Get_State(Transform_State::POS);

	if (m_pOwner.lock()->Get_Script<AttackColliderInfoScript>())
	{
		m_pOwner.lock()->Get_Collider()->Set_Activate(false);
		m_pOwner.lock()->Get_Script<AttackColliderInfoScript>()->Set_SkillName(m_strAttackType);
		m_pOwner.lock()->Get_Script<AttackColliderInfoScript>()->Set_AttackDamage(m_fAttackDamage);
	}

	return S_OK;
}

void FloorSkill_Script::Tick()
{
	if (m_tAttackStartTime.fAccTime <= m_tAttackStartTime.fCoolTime)
		m_tAttackStartTime.fAccTime += fDT;
	else
	{
		if (!m_bAttackStart)
			m_bAttackStart = true;
	}
	
	m_tAttackTickTime.fAccTime += fDT;

	if (m_bAttackStart)
	{
		if (m_pOwner.lock()->Get_Collider())
		{
			if (m_tAttackTickTime.fAccTime >= m_tAttackTickTime.fCoolTime)
			{
				m_pOwner.lock()->Get_Collider()->Set_Activate(true);
				m_tAttackTickTime.fAccTime = 0.f;
				m_bAttackOn = true;
				m_iCurrentAttackCnt++;
				m_pOwner.lock()->Get_Script<AttackColliderInfoScript>()->Set_AttackDamage(m_fAttackDamage);

				if (m_iCurrentAttackCnt == m_iLimitAttackCnt)
				{
					m_pOwner.lock()->Get_Script<AttackColliderInfoScript>()->Set_SkillName(m_strLastAttackType);
					m_pOwner.lock()->Get_Script<AttackColliderInfoScript>()->Set_AttackDamage(m_fLastAttackDamage);
				}
			}
			else
			{
				if (m_iCurrentAttackCnt >= m_iLimitAttackCnt)
					EVENTMGR.Delete_Object(m_pOwner.lock());

				if (m_bAttackOn)
				{
					m_bAttackOn = false;
					m_pOwner.lock()->Get_Collider()->Set_Activate(false);
					m_fAttackDamage = m_fFirstAttackDamage + _float(rand() % 10);
				}
			}
		}
	}

}
