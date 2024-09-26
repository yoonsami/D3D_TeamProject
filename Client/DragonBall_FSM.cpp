#include "pch.h"
#include "DragonBall_FSM.h"
#include "Boss_Mir_FSM.h"
#include "ModelAnimator.h"
#include "MotionTrailRenderer.h"
#include "MotionTrailDisappear.h"
#include "SphereCollider.h"
#include "RigidBody.h"
#include "Model.h"
#include "AttackColliderInfoScript.h"
#include <MathUtils.h>


void DragonBall_FSM::Tick()
{
	State_Tick();
}

void DragonBall_FSM::OnCollisionEnter(shared_ptr<BaseCollider> pCollider, _float fGap)
{
	if (pCollider->Get_Owner() == nullptr)
		return;

	if (pCollider->Get_CollisionGroup() == Monster_Attack)
	{
		m_pOwner.lock()->Get_Animator()->Reset_Frame();

		m_eCurState = STATE::Crash;

		if (!m_pTarget.expired())
			m_pTarget.lock()->Get_FSM()->Set_State((_uint)Boss_Mir_FSM::STATE::groggy_start);
	}
}

void DragonBall_FSM::State_Tick()
{
	State_Init();

	m_iCurFrame = Get_CurFrame();

	switch (m_eCurState)
	{
	case STATE::Idle:
		Idle();
		break;
	case STATE::Crash:
		Crash();
		break;
	case STATE::NONE:
		break;
	}

	if (m_iPreFrame != m_iCurFrame)
		m_iPreFrame = m_iCurFrame;
}

void DragonBall_FSM::State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case STATE::Idle:
			Idle_Init();
			break;
		case STATE::Crash:
			Crash_Init();
			break;
		case STATE::NONE:
			break;
		}


		m_ePreState = m_eCurState;
	}

}

void DragonBall_FSM::Idle()
{
}

void DragonBall_FSM::Idle_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_CurrentAnim(L"anim_R02_DecoBall_00_Anim", true, 1.f);
}

void DragonBall_FSM::Crash()
{
	if (Is_AnimFinished())
		EVENTMGR.Delete_Object(m_pOwner.lock());
}

void DragonBall_FSM::Crash_Init()
{
	shared_ptr<Model> model = RESOURCES.Get<Model>(L"Anim_P_R02_DecoBall_01_Break");

	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();
	animator->Set_Model(model);


	animator->Set_CurrentAnim(L"P_R02_DecoBall_01_Fire", false, 1.f);

	{
		Ray ray;
		ray.position = Get_Transform()->Get_State(Transform_State::POS).xyz();
		
			Get_Transform()->Set_State(Transform_State::POS, _float4(ray.position.x, 0.f, ray.position.z, 1.f));

	}
	{

		if (Get_Owner()->Get_RigidBody())
			Get_Owner()->Get_RigidBody()->RemoveRigidBody();
	}
}
