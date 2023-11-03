#include "pch.h"
#include "SpearAce_FSM.h"
#include "ModelAnimator.h"

HRESULT SpearAce_FSM::Init()
{
	auto animator = Get_Owner()->Get_Animator();
	if (animator)
	{
		// ���� �ִϸ��̼� �������ִµ�, ������ ����
		animator->Set_NextTweenAnim(L"b_idle"/*�ִϸ��̼� �̸�*/, 0.2f/*���� �ð�*/, true/*�ݺ� �ִϸ��̼�*/, 1.f/*�ִϸ��̼� �ӵ�*/);
		m_eCurState = STATE::b_idle;
	}
	return S_OK;
}

void SpearAce_FSM::Tick()
{
	State_Tick();
}

void SpearAce_FSM::State_Tick()
{
	State_Init();

	switch (m_eCurState)
	{
	case STATE::b_idle:
		b_idle();
		break;
	case STATE::b_run_start:
		b_run_start();
		break;
	case STATE::b_run:
		b_run();
		break;
	case STATE::b_run_end_r:
		b_run_end_r();
		break;
	case STATE::b_run_end_l:
		b_run_end_l();
		break;
	case STATE::b_sprint:
		b_sprint();
		break;
	case STATE::b_walk:
		b_walk();
		break;
	case STATE::die:
		die();
		break;
	case STATE::skill_1100:
		skill_1100();
		break;
	case STATE::skill_1200:
		skill_1200();
		break;
	case STATE::skill_1300:
		skill_1300();
		break;
	case STATE::skill_91100:
		skill_91100();
		break;
	case STATE::skill_93100:
		skill_93100();
		break;
	case STATE::skill_100100:
		skill_100100();
		break;
	case STATE::skill_200100:
		skill_200100();
		break;
	case STATE::skill_200200:
		skill_200200();
		break;
	case STATE::skill_300100:
		skill_300100();
		break;
	case STATE::skill_502100:
		skill_502100();
		break;
	case STATE::skill_500100:
		skill_500100();
		break;
	}
}

void SpearAce_FSM::State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case STATE::b_idle:
			b_idle_Init();
			break;
		case STATE::b_run_start:
			b_run_start_Init();
			break;
		case STATE::b_run:
			b_run_Init();
			break;
		case STATE::b_run_end_r:
			b_run_end_r_Init();
			break;
		case STATE::b_run_end_l:
			b_run_end_l_Init();
			break;
		case STATE::b_sprint:
			b_sprint_Init();
			break;
		case STATE::b_walk:
			b_walk_Init();
			break;
		case STATE::die:
			die_Init();
			break;
		case STATE::skill_1100:
			skill_1100_Init();
			break;
		case STATE::skill_1200:
			skill_1200_Init();
			break;
		case STATE::skill_1300:
			skill_1300_Init();
			break;
		case STATE::skill_91100:
			skill_91100_Init();
			break;
		case STATE::skill_93100:
			skill_93100_Init();
			break;
		case STATE::skill_100100:
			skill_100100_Init();
			break;
		case STATE::skill_200100:
			skill_200100_Init();
			break;
		case STATE::skill_200200:
			skill_200200_Init();
			break;
		case STATE::skill_300100:
			skill_300100_Init();
			break;
		case STATE::skill_502100:
			skill_502100_Init();
			break;
		case STATE::skill_500100:
			skill_500100_Init();
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void SpearAce_FSM::OnCollision(shared_ptr<BaseCollider> pCollider, _float fGap)
{
}

void SpearAce_FSM::OnCollisionEnter(shared_ptr<BaseCollider> pCollider, _float fGap)
{
}

void SpearAce_FSM::OnCollisionExit(shared_ptr<BaseCollider> pCollider, _float fGap)
{
}

void SpearAce_FSM::b_idle()
{
	_float3 vInputVector = Get_InputDirVector();


	if (KEYPUSH(KEY_TYPE::W) || KEYPUSH(KEY_TYPE::S) ||
		KEYPUSH(KEY_TYPE::A) || KEYPUSH(KEY_TYPE::D))
		m_eCurState = STATE::b_run_start;

	if (KEYPUSH(KEY_TYPE::LBUTTON))
		m_eCurState = STATE::skill_1100;
	else if (KEYPUSH(KEY_TYPE::KEY_1))
		m_eCurState = STATE::skill_100100;
	else if (KEYPUSH(KEY_TYPE::KEY_2))
		m_eCurState = STATE::skill_200100;
	else if (KEYPUSH(KEY_TYPE::KEY_3))
		m_eCurState = STATE::skill_300100;
	else if (KEYPUSH(KEY_TYPE::KEY_4))
		m_eCurState = STATE::skill_502100;
	else if (KEYPUSH(KEY_TYPE::KEY_5))
		m_eCurState = STATE::skill_500100;
	else if (KEYPUSH(KEY_TYPE::SPACE))
	{
		if (vInputVector == _float3(0.f))
			m_eCurState = STATE::skill_93100;
		else
			m_eCurState = STATE::skill_91100;
	}
}

void SpearAce_FSM::b_idle_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_idle", 0.1f, true, 1.f);

	Get_Transform()->Set_Speed(1.f);
	m_tRunEndDelay.fAccTime = 0.f;
}

void SpearAce_FSM::b_run_start()
{
	Get_Transform()->Go_Straight();

	_float3 vInputVector = Get_InputDirVector();

	// ����Ű�� �ƹ��͵� ������ ������ ���¸� ����
	if (vInputVector == _float3(0.f))
	{
		m_tRunEndDelay.fAccTime += fDT;

		if (m_tRunEndDelay.fAccTime >= m_tRunEndDelay.fCoolTime)
			m_eCurState = STATE::b_idle;
	}
	else
	{
		// �ִϸ��̼��� ������ �ε�, ���� �ð������� ����� ���� �־,
		// �ٷ� �̾����� �ִϸ��̼��� ��쿡��
		// ������ ���ϸ鼭 �ٲٰų�, 
		// �Ʒ�ó�� ������ ���� ���� �̸� �ٲٸ� �ڿ������� ��ȯ��
		// �� �� ����
		if (Is_AnimFinished())
			m_eCurState = STATE::b_run;

		//if (Get_FinalFrame() - Get_CurFrame() < 5)
		//	m_eCurState = STATE::b_run;

		Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);

		if (KEYPUSH(KEY_TYPE::LBUTTON))
			m_eCurState = STATE::skill_1100;
		else if (KEYPUSH(KEY_TYPE::KEY_1))
			m_eCurState = STATE::skill_100100;
		else if (KEYPUSH(KEY_TYPE::KEY_2))
			m_eCurState = STATE::skill_200100;
		else if (KEYPUSH(KEY_TYPE::KEY_3))
			m_eCurState = STATE::skill_300100;
		else if (KEYPUSH(KEY_TYPE::KEY_4))
			m_eCurState = STATE::skill_502100;
		else if (KEYPUSH(KEY_TYPE::KEY_5))
			m_eCurState = STATE::skill_500100;
		else if (KEYPUSH(KEY_TYPE::SPACE))
			m_eCurState = STATE::skill_91100;
	}
}

void SpearAce_FSM::b_run_start_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_run_start", 0.1f, false, 1.5f);

	Get_Transform()->Set_Speed(1.f);
	m_tRunEndDelay.fAccTime = 0.f;
}

void SpearAce_FSM::b_run()
{
	Get_Transform()->Go_Straight();

	if (KEYPUSH(KEY_TYPE::LSHIFT))
		m_bIsSprint = true;
	else
		m_bIsSprint = false;

	_float3 vInputVector = Get_InputDirVector();

	// ����Ű�� �ƹ��͵� ������ ������ ���¸� ����
	if (vInputVector == _float3(0.f))
	{
		m_tRunEndDelay.fAccTime += fDT;
		
		if (m_tRunEndDelay.fAccTime >= m_tRunEndDelay.fCoolTime)
		{
			if (Get_CurFrame() % 2 == 0)
				m_eCurState = STATE::b_run_end_r;
			else
				m_eCurState = STATE::b_run_end_l;
		}
	}
	else
		Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);

	if (m_bIsSprint)
		m_eCurState = STATE::b_sprint;

	if (KEYPUSH(KEY_TYPE::LBUTTON))
		m_eCurState = STATE::skill_1100;
	else if (KEYPUSH(KEY_TYPE::KEY_1))
		m_eCurState = STATE::skill_100100;
	else if (KEYPUSH(KEY_TYPE::KEY_2))
		m_eCurState = STATE::skill_200100;
	else if (KEYPUSH(KEY_TYPE::KEY_3))
		m_eCurState = STATE::skill_300100;
	else if (KEYPUSH(KEY_TYPE::KEY_4))
		m_eCurState = STATE::skill_502100;
	else if (KEYPUSH(KEY_TYPE::KEY_5))
		m_eCurState = STATE::skill_500100;
	else if (KEYPUSH(KEY_TYPE::SPACE))
		m_eCurState = STATE::skill_91100;
}

void SpearAce_FSM::b_run_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_run", 0.1f, true, 1.f);

	Get_Transform()->Set_Speed(1.f);
}

void SpearAce_FSM::b_run_end_r()
{
	_float3 vInputVector = Get_InputDirVector();

	// ����Ű�� �ƹ��͵� ������ ������ ���¸� ����
	if (vInputVector != _float3(0.f))
		Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);
	
	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;

	if (KEYPUSH(KEY_TYPE::LBUTTON))
		m_eCurState = STATE::skill_1100;
	else if (KEYPUSH(KEY_TYPE::KEY_1))
		m_eCurState = STATE::skill_100100;
	else if (KEYPUSH(KEY_TYPE::KEY_2))
		m_eCurState = STATE::skill_200100;
	else if (KEYPUSH(KEY_TYPE::KEY_3))
		m_eCurState = STATE::skill_300100;
	else if (KEYPUSH(KEY_TYPE::KEY_4))
		m_eCurState = STATE::skill_502100;
	else if (KEYPUSH(KEY_TYPE::KEY_5))
		m_eCurState = STATE::skill_500100;
	else if (KEYPUSH(KEY_TYPE::SPACE))
		m_eCurState = STATE::skill_93100;

}

void SpearAce_FSM::b_run_end_r_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_run_end_r", 0.1f, false, 1.5f);

	Get_Transform()->Set_Speed(1.f);
	m_tRunEndDelay.fAccTime = 0.f;
}

void SpearAce_FSM::b_run_end_l()
{
	_float3 vInputVector = Get_InputDirVector();

	if (vInputVector != _float3(0.f))
		Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);

	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;

	if (KEYPUSH(KEY_TYPE::LBUTTON))
		m_eCurState = STATE::skill_1100;
	else if (KEYPUSH(KEY_TYPE::KEY_1))
		m_eCurState = STATE::skill_100100;
	else if (KEYPUSH(KEY_TYPE::KEY_2))
		m_eCurState = STATE::skill_200100;
	else if (KEYPUSH(KEY_TYPE::KEY_3))
		m_eCurState = STATE::skill_300100;
	else if (KEYPUSH(KEY_TYPE::KEY_4))
		m_eCurState = STATE::skill_502100;
	else if (KEYPUSH(KEY_TYPE::KEY_5))
		m_eCurState = STATE::skill_500100;
	else if (KEYPUSH(KEY_TYPE::SPACE))
		m_eCurState = STATE::skill_93100;
}

void SpearAce_FSM::b_run_end_l_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_run_end_l", 0.1f, false, 1.5f);

	Get_Transform()->Set_Speed(1.f);
	m_tRunEndDelay.fAccTime = 0.f;
}

void SpearAce_FSM::b_sprint()
{
	Get_Transform()->Go_Straight();

	if (KEYPUSH(KEY_TYPE::LSHIFT))
		m_bIsSprint = true;
	else
		m_bIsSprint = false;

	_float3 vInputVector = Get_InputDirVector();

	// ����Ű�� �ƹ��͵� ������ ������ ���¸� ����
	if (vInputVector == _float3(0.f))
	{
		m_tRunEndDelay.fAccTime += fDT;

		if (m_tRunEndDelay.fAccTime >= m_tRunEndDelay.fCoolTime)
		{
			if (Get_CurFrame() % 2 == 0)
				m_eCurState = STATE::b_run_end_r;
			else
				m_eCurState = STATE::b_run_end_l;
		}
	}
	else
		Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);

	if (!m_bIsSprint)
		m_eCurState = STATE::b_run;

	if (KEYPUSH(KEY_TYPE::LBUTTON))
		m_eCurState = STATE::skill_1100;
	else if (KEYPUSH(KEY_TYPE::KEY_1))
		m_eCurState = STATE::skill_100100;
	else if (KEYPUSH(KEY_TYPE::KEY_2))
		m_eCurState = STATE::skill_200100;
	else if (KEYPUSH(KEY_TYPE::KEY_3))
		m_eCurState = STATE::skill_300100;
	else if (KEYPUSH(KEY_TYPE::KEY_4))
		m_eCurState = STATE::skill_502100;
	else if (KEYPUSH(KEY_TYPE::KEY_5))
		m_eCurState = STATE::skill_500100;
	else if (KEYPUSH(KEY_TYPE::SPACE))
		m_eCurState = STATE::skill_91100;
}

void SpearAce_FSM::b_sprint_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_CurrentAnim(L"b_sprint", true, 1.f);

	Get_Transform()->Set_Speed(1.5f);
}

void SpearAce_FSM::b_walk()
{
}

void SpearAce_FSM::b_walk_Init()
{
}

void SpearAce_FSM::die()
{
}

void SpearAce_FSM::die_Init()
{
}

void SpearAce_FSM::skill_1100()
{
	_float3 vInputVector = Get_InputDirVector();

	if (m_vInputTurnVector != _float3(0.f))
		Soft_Turn_ToInputDir(m_vInputTurnVector, XM_PI * 5.f);

	if (_float(Get_CurFrame()) / _float(Get_FinalFrame()) >= 0.25f)
		m_bCanCombo = true;

	if (m_bCanCombo)
	{
		if (KEYTAP(KEY_TYPE::LBUTTON))
			m_eCurState = STATE::skill_1200;
	}

	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;

	if (KEYPUSH(KEY_TYPE::KEY_1))
		m_eCurState = STATE::skill_100100;
	else if (KEYPUSH(KEY_TYPE::KEY_2))
		m_eCurState = STATE::skill_200100;
	else if (KEYPUSH(KEY_TYPE::KEY_3))
		m_eCurState = STATE::skill_300100;
	else if (KEYPUSH(KEY_TYPE::KEY_4))
		m_eCurState = STATE::skill_502100;
	else if (KEYPUSH(KEY_TYPE::KEY_5))
		m_eCurState = STATE::skill_500100;
	else if (KEYPUSH(KEY_TYPE::SPACE))
	{
		if (vInputVector != _float3(0.f))
			m_eCurState = STATE::skill_91100;
		else
			m_eCurState = STATE::skill_93100;
	}
}

void SpearAce_FSM::skill_1100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_1100", 0.15f, false, 1.f);

	m_bIsSprint = false;
	m_bCanCombo = false;

	m_vInputTurnVector = _float3(0.f);
	m_vInputTurnVector = Get_InputDirVector();
}

void SpearAce_FSM::skill_1200()
{
	_float3 vInputVector = Get_InputDirVector();

	if (m_vInputTurnVector != _float3(0.f))
		Soft_Turn_ToInputDir(m_vInputTurnVector, XM_PI * 5.f);


	if (_float(Get_CurFrame()) / _float(Get_FinalFrame()) >= 0.25f)
		m_bCanCombo = true;

	if (m_bCanCombo)
	{
		if (KEYTAP(KEY_TYPE::LBUTTON))
			m_eCurState = STATE::skill_1300;
	}

	if (Is_AnimFinished())
	{
		m_bCanCombo = false;
		m_eCurState = STATE::b_idle;
	}

	if (KEYPUSH(KEY_TYPE::KEY_1))
		m_eCurState = STATE::skill_100100;
	else if (KEYPUSH(KEY_TYPE::KEY_2))
		m_eCurState = STATE::skill_200100;
	else if (KEYPUSH(KEY_TYPE::KEY_3))
		m_eCurState = STATE::skill_300100;
	else if (KEYPUSH(KEY_TYPE::KEY_4))
		m_eCurState = STATE::skill_502100;
	else if (KEYPUSH(KEY_TYPE::KEY_5))
		m_eCurState = STATE::skill_500100;
	else if (KEYPUSH(KEY_TYPE::SPACE))
	{
		if (vInputVector != _float3(0.f))
			m_eCurState = STATE::skill_91100;
		else
			m_eCurState = STATE::skill_93100;
	}
}

void SpearAce_FSM::skill_1200_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_1200", 0.15f, false, 1.f);

	m_bCanCombo = false;

	m_vInputTurnVector = _float3(0.f);
	m_vInputTurnVector = Get_InputDirVector();
}

void SpearAce_FSM::skill_1300()
{
	_float3 vInputVector = Get_InputDirVector();

	if (m_vInputTurnVector != _float3(0.f))
		Soft_Turn_ToInputDir(m_vInputTurnVector, XM_PI * 5.f);

	if (Is_AnimFinished())
	{
		m_bCanCombo = false;
		m_eCurState = STATE::b_idle;
	}

	if (KEYPUSH(KEY_TYPE::KEY_1))
		m_eCurState = STATE::skill_100100;
	else if (KEYPUSH(KEY_TYPE::KEY_2))
		m_eCurState = STATE::skill_200100;
	else if (KEYPUSH(KEY_TYPE::KEY_3))
		m_eCurState = STATE::skill_300100;
	else if (KEYPUSH(KEY_TYPE::KEY_4))
		m_eCurState = STATE::skill_502100;
	else if (KEYPUSH(KEY_TYPE::KEY_5))
		m_eCurState = STATE::skill_500100;
	else if (KEYPUSH(KEY_TYPE::SPACE))
	{
		if (vInputVector != _float3(0.f))
			m_eCurState = STATE::skill_91100;
		else
			m_eCurState = STATE::skill_93100;
	}
}

void SpearAce_FSM::skill_1300_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_1300", 0.15f, false, 1.f);

	m_bCanCombo = false;

	m_vInputTurnVector = _float3(0.f);
	m_vInputTurnVector = Get_InputDirVector();
}

void SpearAce_FSM::skill_91100()
{
	if (m_vInputTurnVector != _float3(0.f))
		Soft_Turn_ToInputDir(m_vInputTurnVector, XM_PI * 5.f);

	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;
}

void SpearAce_FSM::skill_91100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_91100", 0.15f, false, 1.f);

	m_bCanCombo = false;
	m_bIsSprint = false;

	m_vInputTurnVector = _float3(0.f);
	m_vInputTurnVector = Get_InputDirVector();
}

void SpearAce_FSM::skill_93100()
{
	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;
}

void SpearAce_FSM::skill_93100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_93100", 0.15f, false, 1.f);

	m_bCanCombo = false;
	m_bIsSprint = false;
}

void SpearAce_FSM::skill_100100()
{
	_float3 vInputVector = Get_InputDirVector();

	if (m_vInputTurnVector != _float3(0.f))
		Soft_Turn_ToInputDir(m_vInputTurnVector, XM_PI * 5.f);

	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;

	if (KEYPUSH(KEY_TYPE::SPACE))
	{
		if (vInputVector != _float3(0.f))
			m_eCurState = STATE::skill_91100;
		else
			m_eCurState = STATE::skill_93100;
	}
}

void SpearAce_FSM::skill_100100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_100100", 0.15f, false, 1.f);

	m_bCanCombo = false;
	m_bIsSprint = false;

	m_vInputTurnVector = _float3(0.f);
	m_vInputTurnVector = Get_InputDirVector();
}

void SpearAce_FSM::skill_200100()
{
	_float3 vInputVector = Get_InputDirVector();

	if (m_vInputTurnVector != _float3(0.f))
		Soft_Turn_ToInputDir(m_vInputTurnVector, XM_PI * 5.f);

	if (_float(Get_CurFrame()) / _float(Get_FinalFrame()) >= 0.25f)
		m_bCanCombo = true;

	if (m_bCanCombo)
	{
		if (KEYTAP(KEY_TYPE::KEY_2))
			m_eCurState = STATE::skill_200200;
	}

	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;

	if (KEYPUSH(KEY_TYPE::SPACE))
	{
		if (vInputVector != _float3(0.f))
			m_eCurState = STATE::skill_91100;
		else
			m_eCurState = STATE::skill_93100;
	}
}

void SpearAce_FSM::skill_200100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_200100", 0.15f, false, 1.f);

	m_bCanCombo = false;
	m_bIsSprint = false;

	m_vInputTurnVector = _float3(0.f);
	m_vInputTurnVector = Get_InputDirVector();
}

void SpearAce_FSM::skill_200200()
{
	_float3 vInputVector = Get_InputDirVector();

	if (m_vInputTurnVector != _float3(0.f))
		Soft_Turn_ToInputDir(m_vInputTurnVector, XM_PI * 5.f);

	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;

	if (KEYPUSH(KEY_TYPE::SPACE))
	{
		if (vInputVector != _float3(0.f))
			m_eCurState = STATE::skill_91100;
		else
			m_eCurState = STATE::skill_93100;
	}
}

void SpearAce_FSM::skill_200200_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_200200", 0.15f, false, 1.f);

	m_bCanCombo = false;

	m_vInputTurnVector = _float3(0.f);
	m_vInputTurnVector = Get_InputDirVector();
}

void SpearAce_FSM::skill_300100()
{
	_float3 vInputVector = Get_InputDirVector();

	if (m_vInputTurnVector != _float3(0.f))
		Soft_Turn_ToInputDir(m_vInputTurnVector, XM_PI * 5.f);

	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;

	if (KEYPUSH(KEY_TYPE::SPACE))
	{
		if (vInputVector != _float3(0.f))
			m_eCurState = STATE::skill_91100;
		else
			m_eCurState = STATE::skill_93100;
	}
}

void SpearAce_FSM::skill_300100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_300100", 0.15f, false, 1.f);

	m_bCanCombo = false;
	m_bIsSprint = false;

	m_vInputTurnVector = _float3(0.f);
	m_vInputTurnVector = Get_InputDirVector();
}

void SpearAce_FSM::skill_502100()
{
	_float3 vInputVector = Get_InputDirVector();

	if (m_vInputTurnVector != _float3(0.f))
		Soft_Turn_ToInputDir(m_vInputTurnVector, XM_PI * 5.f);

	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;

	if (KEYPUSH(KEY_TYPE::SPACE))
	{
		if (vInputVector != _float3(0.f))
			m_eCurState = STATE::skill_91100;
		else
			m_eCurState = STATE::skill_93100;
	}
}

void SpearAce_FSM::skill_502100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_502100", 0.15f, false, 1.f);

	m_bCanCombo = false;
	m_bIsSprint = false;

	m_vInputTurnVector = _float3(0.f);
	m_vInputTurnVector = Get_InputDirVector();
}

void SpearAce_FSM::skill_500100()
{
	_float3 vInputVector = Get_InputDirVector();

	if (m_vInputTurnVector != _float3(0.f))
		Soft_Turn_ToInputDir(m_vInputTurnVector, XM_PI * 5.f);

	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;

	if (KEYPUSH(KEY_TYPE::SPACE))
	{
		if (vInputVector != _float3(0.f))
			m_eCurState = STATE::skill_91100;
		else
			m_eCurState = STATE::skill_93100;
	}
}

void SpearAce_FSM::skill_500100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_500100", 0.15f, false, 1.f);

	m_bCanCombo = false;
	m_bIsSprint = false;

	m_vInputTurnVector = _float3(0.f);
	m_vInputTurnVector = Get_InputDirVector();
}
