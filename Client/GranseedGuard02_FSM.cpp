#include "pch.h"
#include "GranseedGuard02_FSM.h"
#include "ModelAnimator.h"
#include "MathUtils.h"

GranseedGuard02_FSM::GranseedGuard02_FSM()
{
}

GranseedGuard02_FSM::~GranseedGuard02_FSM()
{
}

HRESULT GranseedGuard02_FSM::Init()
{
    m_eCurState = STATE::n_idle;
    return S_OK;
}

void GranseedGuard02_FSM::Tick()
{
    State_Tick();
}

void GranseedGuard02_FSM::State_Tick()
{
	State_Init();
	switch (m_eCurState)
	{
	case GranseedGuard02_FSM::STATE::n_idle:
		n_idle();
		break;
	case GranseedGuard02_FSM::STATE::n_run:
		n_run();
		break;
	case GranseedGuard02_FSM::STATE::talk_01:
		talk_01();
		break;
	case GranseedGuard02_FSM::STATE::n_walk:
		n_walk();
		break;
	}
}

void GranseedGuard02_FSM::State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case GranseedGuard02_FSM::STATE::n_idle:
			n_idle_Init();
			break;
		case GranseedGuard02_FSM::STATE::n_run:
			n_run_Init();
			break;
		case GranseedGuard02_FSM::STATE::talk_01:
			talk_01_Init();
			break;
		case GranseedGuard02_FSM::STATE::n_walk:
			n_walk_Init();
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void GranseedGuard02_FSM::n_idle()
{
	m_fStateAcc += fDT;
	if (m_fStateAcc >= m_fIdleTimer)
	{
		m_fStateAcc = 0.f;
		m_eCurState = STATE::n_walk;
	}
}

void GranseedGuard02_FSM::n_idle_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"n_idle", 0.15f, true, 1.f);

	m_fIdleTimer = MathUtils::Get_RandomFloat(2.f, 4.f);
}

void GranseedGuard02_FSM::n_run()
{
	m_fStateAcc += fDT;

	if (m_fStateAcc >= m_fMoveTimer)
	{
		m_fStateAcc = 0.f;
		m_eCurState = STATE::n_idle;
	}
}

void GranseedGuard02_FSM::n_run_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"n_run", 0.15f, true, 1.f);

	m_fMoveTimer = MathUtils::Get_RandomFloat(7.f, 10.f);
}

void GranseedGuard02_FSM::talk_01()
{
	Look_DirToTarget(XM_PI * 0.5f);
	if (KEYTAP(KEY_TYPE::E))
		m_eCurState = STATE::n_idle;
}

void GranseedGuard02_FSM::talk_01_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"talk_01", 0.15f, true, 1.f);
}

void GranseedGuard02_FSM::n_walk()
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
}

void GranseedGuard02_FSM::n_walk_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"n_walk", 0.15f, true, 1.f);
	Get_Transform()->Set_Speed(3.f);
	m_vDirToTarget = MathUtils::Get_RandomVector(_float3(-1, 0, -1), _float3(1, 0, 1));
	m_vDirToTarget.Normalize();

	_float3 vLook = Get_Transform()->Get_State(Transform_State::LOOK).xyz();
	vLook = _float3::TransformNormal(vLook, _float4x4::CreateRotationY(MathUtils::Get_RandomFloat(-XM_PI * 0.1f, XM_PI * 0.1f)));

	m_vDirToTarget = vLook;
}

void GranseedGuard02_FSM::Set_State(_uint iIndex)
{
}

void GranseedGuard02_FSM::InteractWithPlayer()
{
	auto pPlayer = GET_PLAYER;
	m_vDirToTarget = (pPlayer->Get_Transform()->Get_State(Transform_State::POS) - Get_Transform()->Get_State(Transform_State::POS)).xyz();
	m_vDirToTarget.y = 0;
	m_eCurState = STATE::talk_01;
}
