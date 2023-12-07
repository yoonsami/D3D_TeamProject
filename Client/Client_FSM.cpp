#include "pch.h"
#include "Client_FSM.h"
#include "BaseCollider.h"
#include "AttackColliderInfoScript.h"

#include <SphereCollider.h>
#include <OBBBoxCollider.h>

void Client_FSM::AttackCollider_On(const wstring& skillname, _float fAttackDamage)
{
	if (!m_pAttackCollider.expired())
	{
		m_pAttackCollider.lock()->Get_Collider()->Set_Activate(true);
		m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_SkillName(skillname);
		m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_AttackDamage(fAttackDamage);
	}
}

void Client_FSM::AttackCollider_Off()
{
	if (!m_pAttackCollider.expired())
	{
		m_pAttackCollider.lock()->Get_Collider()->Set_Activate(false);
		m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_SkillName(L"");
		m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_AttackDamage(0.f);
	}
}

void Client_FSM::Set_ColliderOption(ElementType eType, const wstring& strHitEffectTag)
{
	if (!m_pAttackCollider.expired())
	{
		m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_AttackElementType(eType);
		m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_HitEffectTag(strHitEffectTag);
	}

}

void Client_FSM::Create_ForwardMovingSkillCollider(CollisionGroup eGroup, const wstring& strColliderName, const _float4& vPos, _float fSkillRange, FORWARDMOVINGSKILLDESC desc, const wstring& SkillType, _float fAttackDamage)
{
	shared_ptr<GameObject> SkillCollider = make_shared<GameObject>();

	m_pSkillCollider = SkillCollider;

	m_pSkillCollider.lock()->GetOrAddTransform();
	m_pSkillCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, vPos);

	auto pSphereCollider = make_shared<SphereCollider>(fSkillRange);
	pSphereCollider->Set_CenterPos(_float3{ vPos.x,vPos.y, vPos.z });
	m_pSkillCollider.lock()->Add_Component(pSphereCollider);

	m_pSkillCollider.lock()->Get_Collider()->Set_CollisionGroup(eGroup);

	m_pSkillCollider.lock()->Add_Component(make_shared<AttackColliderInfoScript>());
	m_pSkillCollider.lock()->Get_Collider()->Set_Activate(true);
	m_pSkillCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_SkillName(SkillType);
	m_pSkillCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_AttackDamage(fAttackDamage);
	m_pSkillCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_ColliderOwner(m_pOwner.lock());
	m_pSkillCollider.lock()->Set_Name(strColliderName);
	m_pSkillCollider.lock()->Add_Component(make_shared<ForwardMovingSkillScript>(desc));
	m_pSkillCollider.lock()->Get_Script<ForwardMovingSkillScript>()->Init();

	EVENTMGR.Create_Object(m_pSkillCollider.lock());
}

void Client_FSM::Create_InstallationSkillCollider(CollisionGroup eGroup, const wstring& strColliderName, const _float4& vPos, _float fSkillRange, INSTALLATIONSKILLDESC desc)
{
	shared_ptr<GameObject> InstallationSkillCollider = make_shared<GameObject>();

	InstallationSkillCollider->GetOrAddTransform();
	InstallationSkillCollider->Get_Transform()->Set_State(Transform_State::POS, vPos);

	auto pSphereCollider = make_shared<SphereCollider>(fSkillRange);
	pSphereCollider->Set_CenterPos(_float3{ vPos.x,vPos.y, vPos.z });
	InstallationSkillCollider->Add_Component(pSphereCollider);
	InstallationSkillCollider->Get_Collider()->Set_CollisionGroup(eGroup);

	InstallationSkillCollider->Add_Component(make_shared<AttackColliderInfoScript>());
	InstallationSkillCollider->Get_Script<AttackColliderInfoScript>()->Set_ColliderOwner(m_pOwner.lock());
	InstallationSkillCollider->Add_Component(make_shared<InstallationSkill_Script>(desc));
	InstallationSkillCollider->Get_Script<InstallationSkill_Script>()->Init();

	InstallationSkillCollider->Set_Name(strColliderName);

	EVENTMGR.Create_Object(InstallationSkillCollider);
}

void Client_FSM::Create_FloorSkillCollider(CollisionGroup eGroup, const wstring& strColliderName, const _float4& vPos, _float3 vSkillScale, FLOORSKILLDESC desc)
{
	shared_ptr<GameObject> FloorSkillCollider = make_shared<GameObject>();

	FloorSkillCollider->GetOrAddTransform();
	FloorSkillCollider->Get_Transform()->Set_State(Transform_State::POS, vPos);


	auto pOBBCollider = make_shared<OBBBoxCollider>(vSkillScale);
	FloorSkillCollider->Add_Component(pOBBCollider);
	FloorSkillCollider->Get_Collider()->Set_CollisionGroup(eGroup);

	FloorSkillCollider->Add_Component(make_shared<AttackColliderInfoScript>());
	FloorSkillCollider->Get_Script<AttackColliderInfoScript>()->Set_ColliderOwner(m_pOwner.lock());
	FloorSkillCollider->Add_Component(make_shared<FloorSkill_Script>(desc));
	FloorSkillCollider->Get_Script<FloorSkill_Script>()->Init();

	FloorSkillCollider->Set_Name(strColliderName);

	EVENTMGR.Create_Object(FloorSkillCollider);
}
