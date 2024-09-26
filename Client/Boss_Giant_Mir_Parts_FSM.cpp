#include "pch.h"
#include "Boss_Giant_Mir_Parts_FSM.h"
#include "SphereCollider.h"
#include "AttackColliderInfoScript.h"
#include "UiDamageCreate.h"
#include "MathUtils.h"

void Boss_Giant_Mir_Parts_FSM::Tick()
{
	//State_Tick();
}

void Boss_Giant_Mir_Parts_FSM::OnCollisionEnter(shared_ptr<BaseCollider> pCollider, _float fGap)
{
	if (pCollider->Get_Owner() == nullptr)
		return;

	if (!pCollider->Get_Owner()->Get_Script<AttackColliderInfoScript>())
		return;
	wstring hitEffectTag = pCollider->Get_Owner()->Get_Script<AttackColliderInfoScript>()->Get_HitEffectTag();
	if (!hitEffectTag.empty())
	{
		_float3 random = MathUtils::Get_RandomVector(_float3(0, 0, XM_PI / 6.f), _float3(0, 0, -XM_PI / 6.f));

		Quaternion q = Quaternion::CreateFromYawPitchRoll(random.x, random.y, random.z);

		Add_Effect(hitEffectTag, nullptr, _float4x4::CreateFromQuaternion(q));
	}

	if (pCollider->Get_CollisionGroup() == Player_Attack || pCollider->Get_CollisionGroup() == Player_Skill)
	{
		if (!m_bInvincible)
		{
			if (!m_pTarget.expired())
			{
				wstring strSkillName = pCollider->Get_Owner()->Get_Script<AttackColliderInfoScript>()->Get_SkillName();
				_float fAttackDamage = pCollider->Get_Owner()->Get_Script<AttackColliderInfoScript>()->Get_AttackDamage();

				shared_ptr<GameObject> targetToLook = nullptr;
				// skillName
				if (strSkillName.find(L"_Skill") != wstring::npos)
					targetToLook = pCollider->Get_Owner(); // Collider owner
				else // 
					targetToLook = pCollider->Get_Owner()->Get_Script<AttackColliderInfoScript>()->Get_ColliderOwner(); // Collider

				if (targetToLook == nullptr)
					return;

				Get_Hit(strSkillName, fAttackDamage, targetToLook);
			}
		}
	}
}

void Boss_Giant_Mir_Parts_FSM::Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget)
{
	//Calculate Damage to Giant_Mir
	if (!m_pTarget.expired())
		m_pTarget.lock()->Get_Hurt(fDamage);

	CUR_SCENE->Get_UI(L"UI_Damage_Controller")->Get_Script<UiDamageCreate>()->Create_Damage_Font(Get_Owner(), fDamage);

	if (skillname == NORMAL_ATTACK || skillname == NORMAL_SKILL)
	{
		
	}
	else if (skillname == KNOCKBACK_ATTACK || skillname == KNOCKBACK_SKILL)
	{
		
	}
	else if (skillname == KNOCKDOWN_ATTACK || skillname == KNOCKDOWN_SKILL)
	{
		
	}
	else if (skillname == AIRBORNE_ATTACK || skillname == AIRBORNE_SKILL)
	{
		
	}
}

void Boss_Giant_Mir_Parts_FSM::State_Tick()
{
	State_Init();

	m_iCurFrame = Get_CurFrame();

	switch (m_eCurState)
	{
	case STATE::Idle:
		break;
	case STATE::NONE:
		break;
	}

	if (m_iPreFrame != m_iCurFrame)
		m_iPreFrame = m_iCurFrame;
}

void Boss_Giant_Mir_Parts_FSM::State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case STATE::Idle:
			break;
		case STATE::NONE:
			break;
		}


		m_ePreState = m_eCurState;
	}

}
