#include "pch.h"
#include "DestroyBuilding_FSM.h"
#include "Boss_Mir_FSM.h"
#include "ModelAnimator.h"
#include "MotionTrailRenderer.h"
#include "MotionTrailDisappear.h"
#include "SphereCollider.h"
#include "RigidBody.h"
#include "Model.h"
#include "Get_Gravity.h"
#include "MathUtils.h"

HRESULT DestroyBuilding_FSM::Init()
{
	if (!m_bInitialize)
	{
		auto animator = Get_Owner()->Get_Animator();
		if (animator)
		{
			animator->Set_CurrentAnim(L"Mir_Intro_v02_BuildCenter_Anim", false, 1.f);
			m_eCurState = STATE::Crash;
		}

		animator->Set_RenderState(false);

		_float3 vRandomLook = MathUtils::Get_RandomVector(_float3(-1.f,0.f,-1.f), _float3 (1.f,0.f,1.f));
		Get_Transform()->Set_LookDir(vRandomLook);

		Get_Owner()->Add_Component(make_shared<Get_Gravity>(0.f));

		m_bInitialize = true;
	}

	return S_OK;
}

void DestroyBuilding_FSM::Tick()
{
	State_Tick();
}

void DestroyBuilding_FSM::State_Tick()
{
	State_Init();

	m_iCurFrame = Get_CurFrame();

	switch (m_eCurState)
	{
	case STATE::Crash:
		Crash();
		break;
	case STATE::NONE:
		break;
	}

	if (m_iPreFrame != m_iCurFrame)
		m_iPreFrame = m_iCurFrame;
}

void DestroyBuilding_FSM::State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case STATE::Crash:
			Crash_Init();
			break;
		case STATE::NONE:
			break;
		}


		m_ePreState = m_eCurState;
	}

}

void DestroyBuilding_FSM::Crash()
{
	if (Init_CurFrame(1))
	{
		if (!m_pOwner.expired())
			Get_Owner()->Get_Animator()->Set_RenderState(true);
	}

	if (Is_AnimFinished())
		EVENTMGR.Delete_Object(m_pOwner.lock());
}

void DestroyBuilding_FSM::Crash_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_CurrentAnim(L"Mir_Intro_v02_BuildCenter_Anim", false, 1.f);
}
