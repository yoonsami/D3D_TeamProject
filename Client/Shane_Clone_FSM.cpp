#include "pch.h"
#include "Shane_Clone_FSM.h"
#include "ModelAnimator.h"
#include "MotionTrailRenderer.h"
#include "MotionTrailDisappear.h"

HRESULT Shane_Clone_FSM::Init()
{
	if (!m_bInitialize)
	{
		m_eCurState = STATE::Shane_Clone1;

		m_bInitialize = true;
	}

	return S_OK;
}

void Shane_Clone_FSM::Tick()
{
	State_Tick();
}

void Shane_Clone_FSM::State_Tick()
{
	State_Init();

	m_iCurFrame = Get_CurFrame();

	switch (m_eCurState)
	{
	case Shane_Clone_FSM::STATE::Shane_Clone1:
		Skill_Use();
		break;
	case Shane_Clone_FSM::STATE::Shane_Clone2:
		Skill_Use();
		break;
	case Shane_Clone_FSM::STATE::Shane_Clone3:
		Skill_Use();
		break;
	case Shane_Clone_FSM::STATE::Shane_Clone4:
		Skill_Use();
		break;
	case Shane_Clone_FSM::STATE::Shane_Clone5:
		Skill_Use();
		break;
	case Shane_Clone_FSM::STATE::Skill_End:
		Skill_End();
		break;
	case Shane_Clone_FSM::STATE::NONE:
		break;
	}

	Update_GroupEffectWorldPos(Get_Owner()->Get_Transform()->Get_WorldMatrix());

	if (m_iPreFrame != m_iCurFrame)
		m_iPreFrame = m_iCurFrame;
}

void Shane_Clone_FSM::State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Shane_Clone_FSM::STATE::Shane_Clone1:
			Skill_Use_Init(_uint(STATE::Shane_Clone1));
			break;
		case Shane_Clone_FSM::STATE::Shane_Clone2:
			Skill_Use_Init(_uint(STATE::Shane_Clone2));
			break;
		case Shane_Clone_FSM::STATE::Shane_Clone3:
			Skill_Use_Init(_uint(STATE::Shane_Clone3));
			break;
		case Shane_Clone_FSM::STATE::Shane_Clone4:
			Skill_Use_Init(_uint(STATE::Shane_Clone4));
			break;
		case Shane_Clone_FSM::STATE::Shane_Clone5:
			Skill_Use_Init(_uint(STATE::Shane_Clone5));
			break;
		case Shane_Clone_FSM::STATE::Skill_End:
			Skill_End_Init();
			break;
		case Shane_Clone_FSM::STATE::NONE:
			break;
		}


		m_ePreState = m_eCurState;
	}

}

void Shane_Clone_FSM::Skill_Use()
{

	if (Is_AnimFinished())
	{
		shared_ptr<GameObject> motionTrail = make_shared<GameObject>();
		motionTrail->GetOrAddTransform()->Set_WorldMat(Get_Transform()->Get_WorldMatrix());
		motionTrail->Add_Component(make_shared<MotionTrailRenderer>(RESOURCES.Get<Shader>(L"Shader_Model.fx"), Get_Owner()->Get_Animator()->Get_TweenDesc(), Get_Owner()->Get_Animator()->Get_Model()));
		motionTrail->Add_Component(make_shared<MotionTrailDisappear>(2.f, Color(1.f, 0.3f, 0.3f, 1.f)));
		motionTrail->Get_Script<MotionTrailDisappear>()->Init();
		EVENTMGR.Create_Object(motionTrail);

		EVENTMGR.Delete_Object(Get_Owner());
	}
}

void Shane_Clone_FSM::Skill_Use_Init(_uint iCloneIndex)
{
	wstring animName = L"skill_2_Fx_0";
	animName += to_wstring(m_iCloneIndex + 1);
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(animName, 0.1f, false, 1.f);
}

void Shane_Clone_FSM::Skill_End()
{

}

void Shane_Clone_FSM::Skill_End_Init()
{

}
