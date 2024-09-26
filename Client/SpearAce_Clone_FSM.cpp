#include "pch.h"
#include "SpearAce_Clone_FSM.h"
#include "ModelAnimator.h"
#include "MotionTrailRenderer.h"
#include "MotionTrailDisappear.h"

HRESULT SpearAce_Clone_FSM::Init()
{
	if (!m_bInitialize)
	{
		m_eCurState = STATE::Skill_Use;

		m_bInitialize = true;
	}
	return S_OK;
}

void SpearAce_Clone_FSM::Tick()
{
	State_Tick();
}

void SpearAce_Clone_FSM::State_Tick()
{
	State_Init();

	m_iCurFrame = Get_CurFrame();

	switch (m_eCurState)
	{
	case SpearAce_Clone_FSM::STATE::Skill_Use:
		Skill_Use();
		break;
	case SpearAce_Clone_FSM::STATE::Skill_End:
		Skill_End();
		break;
	case SpearAce_Clone_FSM::STATE::NONE:
		break;
	}

	Update_GroupEffectWorldPos(Get_Owner()->Get_Transform()->Get_WorldMatrix());

	if (m_iPreFrame != m_iCurFrame)
		m_iPreFrame = m_iCurFrame;
}

void SpearAce_Clone_FSM::State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case SpearAce_Clone_FSM::STATE::Skill_Use:
			Skill_Use_Init();
			break;
		case SpearAce_Clone_FSM::STATE::Skill_End:
			Skill_End_Init();
			break;
		case SpearAce_Clone_FSM::STATE::NONE:
			break;
		}


		m_ePreState = m_eCurState;
	}

}

void SpearAce_Clone_FSM::Skill_Use()
{
	if (m_iCurFrame > 70)
	{
		shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();
		animator->Set_RenderState(true);
	}
	if (m_iCurFrame > 100)
	{
		shared_ptr<GameObject> motionTrail = make_shared<GameObject>();
		motionTrail->GetOrAddTransform()->Set_WorldMat(Get_Transform()->Get_WorldMatrix());
		motionTrail->Add_Component(make_shared<MotionTrailRenderer>(RESOURCES.Get<Shader>(L"Shader_Model.fx"), Get_Owner()->Get_Animator()->Get_TweenDesc(), Get_Owner()->Get_Animator()->Get_Model()));
		motionTrail->Add_Component(make_shared<MotionTrailDisappear>(1.5f, Color(0.2f, 0.5f, 4.f, 1.f)));
		motionTrail->Get_Script<MotionTrailDisappear>()->Init();
		EVENTMGR.Create_Object(motionTrail);

		EVENTMGR.Delete_Object(Get_Owner());
	}

	if (Is_AnimFinished())
		m_eCurState = STATE::Skill_End;
}

void SpearAce_Clone_FSM::Skill_Use_Init()
{
	wstring animName = L"Fx_skill_300100_";
	animName += to_wstring(m_iCloneIndex + 1);
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(animName, 0.1f, false, 1.2f);
	animator->Set_RenderState(false);
}

void SpearAce_Clone_FSM::Skill_End()
{

}

void SpearAce_Clone_FSM::Skill_End_Init()
{
	
}
