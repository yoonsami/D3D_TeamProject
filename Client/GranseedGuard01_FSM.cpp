#include "pch.h"
#include "GranseedGuard01_FSM.h"
#include "ModelAnimator.h"
#include <MathUtils.h>

GranseedGuard01_FSM::GranseedGuard01_FSM()
{
}

GranseedGuard01_FSM::~GranseedGuard01_FSM()
{
}

HRESULT GranseedGuard01_FSM::Init()
{
    m_eCurState = STATE::n_idle;
    return S_OK;
}

void GranseedGuard01_FSM::Tick()
{
    State_Tick();


}

void GranseedGuard01_FSM::State_Tick()
{
	State_Init();
	switch (m_eCurState)
	{
	case GranseedGuard01_FSM::STATE::n_idle:
		n_idle();
		break;
	case GranseedGuard01_FSM::STATE::run:
		run();
		
		break;
	case GranseedGuard01_FSM::STATE::talk:
		talk();
		break;
	case GranseedGuard01_FSM::STATE::walk:
		walk();
		
		break;
	}
}

void GranseedGuard01_FSM::State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case GranseedGuard01_FSM::STATE::n_idle:
			n_idle_Init();
			break;
		case GranseedGuard01_FSM::STATE::run:
			run_Init();
			break;
		case GranseedGuard01_FSM::STATE::talk:
			talk_Init();
			break;
		case GranseedGuard01_FSM::STATE::walk:
			walk_Init();
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void GranseedGuard01_FSM::n_idle()
{
	m_fStateAcc += fDT;
	if (m_fStateAcc >= m_fIdleTimer)
	{
		m_fStateAcc = 0.f;
		m_eCurState = STATE::walk;
	}
	if (Can_Interact())
	{
		InteractWithPlayer();
	}
}

void GranseedGuard01_FSM::n_idle_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"n_idle", 0.15f, true, 1.f);

}

void GranseedGuard01_FSM::run()
{
	m_fStateAcc += fDT;

	if (m_fStateAcc >= m_fMoveTimer)
	{
		m_fStateAcc = 0.f;
		m_eCurState = STATE::n_idle;
	}
}

void GranseedGuard01_FSM::run_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"run", 0.15f, true, 1.f);
}

void GranseedGuard01_FSM::talk()
{
	Look_DirToTarget(XM_PI * 0.5f);
	if (KEYTAP(KEY_TYPE::E))
		m_eCurState = STATE::n_idle;
}

void GranseedGuard01_FSM::talk_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"talk", 0.15f, true, 1.f);

}

void GranseedGuard01_FSM::walk()
{
	m_fStateAcc += fDT;
	
	_float3 vPos = Get_Transform()->Get_State(Transform_State::POS).xyz();
	_float3 vLook = Get_Transform()->Get_State(Transform_State::LOOK).xyz();

	Get_Transform()->Go_Straight();

	if (vPos.x > m_vMaxMovingPoint.x )
	{
		vPos.x = m_vMaxMovingPoint.x;

		if(m_vDirToTarget.x >0.f)
			m_vDirToTarget.x *= -1.f;
	}

	if (vPos.x < m_vMinMovingPoint.x)
	{
		vPos.x = m_vMinMovingPoint.x;

		if (m_vDirToTarget.x < 0.f)
			m_vDirToTarget.x *= -1.f;
	}

	if (vPos.z > m_vMaxMovingPoint.z )
	{
		vPos.z = m_vMaxMovingPoint.z;

		if (m_vDirToTarget.z > 0.f)
			m_vDirToTarget.z *= -1.f;
	}

	if (vPos.z < m_vMinMovingPoint.z)
	{
		vPos.z = m_vMinMovingPoint.z;

		if (m_vDirToTarget.z < 0.f)
			m_vDirToTarget.z *= -1.f;
	}

	Look_DirToTarget(XM_PI * 0.5f);

	if (m_fStateAcc >= m_fMoveTimer)
	{
		m_fStateAcc = 0.f;
		m_eCurState = STATE::n_idle;
	}
	if (Can_Interact())
	{
		InteractWithPlayer();
	}
}

void GranseedGuard01_FSM::walk_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"walk", 0.15f, true, 1.f);
	Get_Transform()->Set_Speed(3.f);
	m_vDirToTarget = MathUtils::Get_RandomVector(_float3(-1, 0, -1), _float3(1, 0, 1));
	m_vDirToTarget.Normalize();

	_float3 vLook = Get_Transform()->Get_State(Transform_State::LOOK).xyz();
	vLook = _float3::TransformNormal(vLook, _float4x4::CreateRotationY(MathUtils::Get_RandomFloat(-XM_PI * 0.1f, XM_PI * 0.1f)));

	m_vDirToTarget = vLook;
}

void GranseedGuard01_FSM::Set_State(_uint iIndex)
{
}

void GranseedGuard01_FSM::InteractWithPlayer()
{
	if(KEYPUSH(KEY_TYPE::E))
	{
		auto pPlayer = GET_PLAYER;
		m_vDirToTarget = (pPlayer->Get_Transform()->Get_State(Transform_State::POS) - Get_Transform()->Get_State(Transform_State::POS)).xyz();
		m_vDirToTarget.y = 0;
		m_eCurState = STATE::talk;
	}
}

