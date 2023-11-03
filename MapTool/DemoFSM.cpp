#include "pch.h"
#include "DemoFSM.h"
#include "ModelAnimator.h"

HRESULT DemoFSM::Init()
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

void DemoFSM::Tick()
{
    State_Tick();
}

void DemoFSM::State_Tick()
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
	}
}

void DemoFSM::State_Init()
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
		}
		m_ePreState = m_eCurState;
	}
}

void DemoFSM::OnCollision(shared_ptr<BaseCollider> pCollider, _float fGap)
{
}

void DemoFSM::OnCollisionEnter(shared_ptr<BaseCollider> pCollider, _float fGap)
{
}

void DemoFSM::OnCollisionExit(shared_ptr<BaseCollider> pCollider, _float fGap)
{
}

void DemoFSM::b_idle()
{
	
}

void DemoFSM::b_idle_Init()
{
	
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	
	animator->Set_NextTweenAnim(L"b_idle", 0.2f, true, 1.f);
}

void DemoFSM::b_run_start()
{
	_float3 vInputVector = Get_InputDirVector();

	// ����Ű�� �ƹ��͵� ������ ������ ���¸� ����
	if (vInputVector == _float3(0.f))
		m_eCurState = STATE::b_idle;
	else
	{	
		// �ִϸ��̼��� ������ �ε�, ���� �ð������� ����� ���� �־,
		// �ٷ� �̾����� �ִϸ��̼��� ��쿡��
		// ������ ���ϸ鼭 �ٲٰų�, 
		// �Ʒ�ó�� ������ ���� ���� �̸� �ٲٸ� �ڿ������� ��ȯ��
		// �� �� ����
		if (Is_AnimFinished())
			m_eCurState = STATE::b_run;

		if (Get_FinalFrame() - Get_CurFrame() < 5)
			m_eCurState = STATE::b_run;

		Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);
	}
}

void DemoFSM::b_run_start_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_run_start", 0.2f, false, 1.f);
}

void DemoFSM::b_run()
{
	_float3 vInputVector = Get_InputDirVector();

	// ����Ű�� �ƹ��͵� ������ ������ ���¸� ����
	if (vInputVector == _float3(0.f))
		m_eCurState = STATE::b_run_end_r;
}

void DemoFSM::b_run_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_run", 0.2f, true, 1.f);
}

void DemoFSM::b_run_end_r()
{
}

void DemoFSM::b_run_end_r_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_run_end_r", 0.2f, false, 1.f);
}

void DemoFSM::b_run_end_l()
{
}

void DemoFSM::b_run_end_l_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_run_end_l", 0.2f, false, 1.f);
}
