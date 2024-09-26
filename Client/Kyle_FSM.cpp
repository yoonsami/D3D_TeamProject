#include "pch.h"
#include "Kyle_FSM.h"
#include "ModelAnimator.h"
#include "SphereCollider.h"
#include "AttackColliderInfoScript.h"
#include "Model.h"
#include "MainCameraScript.h"
#include "Camera.h"
#include "CoolTimeCheckScript.h"
#include "GroupEffect.h"
#include "CharacterController.h"

HRESULT Kyle_FSM::Init()
{
	if (!m_bInitialize)
	{
		auto animator = Get_Owner()->Get_Animator();
		if (animator)
		{
		
			animator->Set_CurrentAnim(L"b_idle", true, 1.f);
			m_eCurState = STATE::b_idle;
		}

		m_iCenterBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_Center");
		m_iCamBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_Cam");
		m_iSkillCamBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_SkillCam");

		m_pCamera = CUR_SCENE->Get_MainCamera();

		m_fSkillAttack_AnimationSpeed =1.f;
		m_fDetectRange = 5.f;

		m_bEntryTeam = true;

		m_bInitialize = true;
	}

	m_fNormalAttack_AnimationSpeed = 1.2f;
	m_fSkillAttack_AnimationSpeed = 1.0f;
	m_fEvade_AnimationSpeed = 1.5f;

	return S_OK;
}

void Kyle_FSM::Tick()
{
	State_Tick();
	
	if (!m_pAttackCollider.expired())
	{
		//m_pAttack transform set forward
		m_pAttackCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 1.5f + _float3::Up);
	}

}

void Kyle_FSM::State_Tick()
{
	State_Init();
	Recovery_Color();
	m_iCurFrame = Get_CurFrame();

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
	case STATE::airborne_start:
		airborne_start();
		break;
	case STATE::airborne_end:
		airborne_end();
		break;
	case STATE::airborne_up:
		airborne_up();
		break;
	case STATE::hit:
		hit();
		break;
	case STATE::knock_start:
		knock_start();
		break;
	case STATE::knock_end:
		knock_end();
		break;
	case STATE::knock_end_loop:
		knock_end_loop();
		break;
	case STATE::knock_end_hit:
		knock_end_hit();
		break;
	case STATE::knock_up:
		knock_up();
		break;
	case STATE::knockdown_start:
		knockdown_start();
		break;
	case STATE::knockdown_end:
		knockdown_end();
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
	case STATE::skill_1400:
		skill_1400();
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
	case STATE::skill_100200:
		skill_100200();
		break;
	case STATE::skill_200100:
		skill_200100();
		break;
	case STATE::skill_200200:
		skill_200200();
		break;
	case STATE::skill_200300:
		skill_200300();
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
	
	Update_GroupEffectWorldPos(Get_Owner()->Get_Transform()->Get_WorldMatrix());

	if (m_iPreFrame != m_iCurFrame)
		m_iPreFrame = m_iCurFrame;
}

void Kyle_FSM::State_Init()
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
		case STATE::airborne_start:
			airborne_start_Init();
			break;
		case STATE::airborne_end:
			airborne_end_Init();
			break;
		case STATE::airborne_up:
			airborne_up_Init();
			break;
		case STATE::hit:
			hit_Init();
			break;
		case STATE::knock_start:
			knock_start_Init();
			break;
		case STATE::knock_end:
			knock_end_Init();
			break;
		case STATE::knock_end_loop:
			knock_end_loop_Init();
			break;
		case STATE::knock_end_hit:
			knock_end_hit_Init();
			break;
		case STATE::knock_up:
			knock_up_Init();
			break;
		case STATE::knockdown_start:
			knockdown_start_Init();
			break;
		case STATE::knockdown_end:
			knockdown_end_Init();
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
		case STATE::skill_1400:
			skill_1400_Init();
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
		case STATE::skill_100200:
			skill_100200_Init();
			break;
		case STATE::skill_200100:
			skill_200100_Init();
			break;
		case STATE::skill_200200:
			skill_200200_Init();
			break;
		case STATE::skill_200300:
			skill_200300_Init();
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

void Kyle_FSM::Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget)
{
	//Calculate Damage 
	m_pOwner.lock()->Get_Hurt(fDamage);

	_float3 vMyPos = Get_Transform()->Get_State(Transform_State::POS).xyz();
	_float3 vOppositePos = pLookTarget->Get_Transform()->Get_State(Transform_State::POS).xyz();

	m_vHitDir = vOppositePos - vMyPos;
	m_vHitDir.y = 0.f;
	m_vHitDir.Normalize();

	Set_HitColor();

	if (skillname == NORMAL_ATTACK || skillname == NORMAL_SKILL)
	{
		if (!m_bSuperArmor)
		{
			if (m_eCurState == STATE::hit)
				Reset_Frame();
			else if (m_eCurState == STATE::knock_end_hit)
				Reset_Frame();
			else if (m_eCurState == STATE::knock_end_loop)
				m_eCurState = STATE::knock_end_hit;
			else
				m_eCurState = STATE::hit;

			CUR_SCENE->Get_MainCamera()->Get_Script<MainCameraScript>()->ShakeCamera(0.05f, 0.1f);


		}
	}
	else if (skillname == KNOCKBACK_ATTACK || skillname == KNOCKBACK_SKILL)
	{
		if (!m_bSuperArmor)
		{
			if (m_eCurState == STATE::knock_end_hit)
				Reset_Frame();
			else if (m_eCurState == STATE::knock_end_loop)
				m_eCurState = STATE::knock_end_hit;
			else
				m_eCurState = STATE::knock_start;

			CUR_SCENE->Get_MainCamera()->Get_Script<MainCameraScript>()->ShakeCamera(0.1f, 0.2f);

		}
	}
	else if (skillname == KNOCKDOWN_ATTACK || skillname == KNOCKDOWN_SKILL)
	{
		if (!m_bSuperArmor)
		{
			if (m_eCurState == STATE::knock_end_hit)
				Reset_Frame();
			else if (m_eCurState == STATE::knock_end_loop)
				m_eCurState = STATE::knock_end_hit;
			else
				m_eCurState = STATE::knockdown_start;

			CUR_SCENE->Get_MainCamera()->Get_Script<MainCameraScript>()->ShakeCamera(0.1f, 0.3f);

		}
	}
	else if (skillname == AIRBORNE_ATTACK || skillname == AIRBORNE_SKILL)
	{
		if (!m_bSuperArmor)
		{
			if (m_eCurState == STATE::knock_end_hit)
				Reset_Frame();
			else if (m_eCurState == STATE::knock_end_loop)
				m_eCurState = STATE::knock_end_hit;
			else
				m_eCurState = STATE::airborne_start;

			CUR_SCENE->Get_MainCamera()->Get_Script<MainCameraScript>()->ShakeCamera(0.05f, 0.3f);

		}
	}
	else
		CUR_SCENE->Get_MainCamera()->Get_Script<MainCameraScript>()->ShakeCamera(0.05f, 0.03f);

}

void Kyle_FSM::Set_State(_uint iIndex)
{
}

void Kyle_FSM::b_idle()
{
	_float3 vInputVector = Get_InputDirVector();

	if (!g_bIsCanMouseMove && !g_bCutScene)
	{
		if (KEYPUSH(KEY_TYPE::W) || KEYPUSH(KEY_TYPE::S) ||
			KEYPUSH(KEY_TYPE::A) || KEYPUSH(KEY_TYPE::D))
			m_eCurState = STATE::b_run_start;

		if (KEYTAP(KEY_TYPE::LBUTTON))
			m_eCurState = STATE::skill_1100;

		Use_Skill();
	}
}

void Kyle_FSM::b_idle_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_idle", 0.1f, true, 1.f);

	Get_Transform()->Set_Speed(m_fRunSpeed);
	m_tRunEndDelay.fAccTime = 0.f;

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = false;
}

void Kyle_FSM::b_run_start()
{
	Get_Transform()->Go_Straight();

	_float3 vInputVector = Get_InputDirVector();

	if (vInputVector == _float3(0.f))
	{
		m_tRunEndDelay.fAccTime += fDT;

		if (m_tRunEndDelay.fAccTime >= m_tRunEndDelay.fCoolTime)
			m_eCurState = STATE::b_run_end_l;
	}
	else
	{
		if (Is_AnimFinished())
			m_eCurState = STATE::b_run;

		Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);

		if (!g_bIsCanMouseMove && !g_bCutScene)
		{
			if (KEYTAP(KEY_TYPE::LBUTTON))
				m_eCurState = STATE::skill_1100;
			
			Use_Skill();
		}
	}
}

void Kyle_FSM::b_run_start_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_run_start", 0.1f, false, 1.5f);

	Get_Transform()->Set_Speed(m_fRunSpeed);
	m_tRunEndDelay.fAccTime = 0.f;

	AttackCollider_Off();
	
	m_bInvincible = false;
	m_bSuperArmor = false;
}

void Kyle_FSM::b_run()
{
	Get_Transform()->Go_Straight();

	_float3 vInputVector = Get_InputDirVector();

	
	if (vInputVector == _float3(0.f))
	{
		m_tRunEndDelay.fAccTime += fDT;

		if (m_tRunEndDelay.fAccTime >= m_tRunEndDelay.fCoolTime)
		{
			if (m_iCurFrame % 2 == 0)
				m_eCurState = STATE::b_run_end_r;
			else
				m_eCurState = STATE::b_run_end_l;
		}
	}
	else
		Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);

	if (g_bIsCanMouseMove || g_bCutScene)
	{
		if (m_iCurFrame % 2 == 0)
			m_eCurState = STATE::b_run_end_r;
		else
			m_eCurState = STATE::b_run_end_l;
	}

	if (!g_bIsCanMouseMove && !g_bCutScene)
	{
		if (KEYPUSH(KEY_TYPE::LSHIFT))
		{
			if (m_iCurFrame == 1)
				m_eCurState = STATE::b_sprint;
		}

		if (KEYTAP(KEY_TYPE::LBUTTON))
			m_eCurState = STATE::skill_1100;

		Use_Skill();
	}
}

void Kyle_FSM::b_run_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_run", 0.2f, true, 1.f);

	Get_Transform()->Set_Speed(m_fRunSpeed);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = false;
}

void Kyle_FSM::b_run_end_r()
{
	_float3 vInputVector = Get_InputDirVector();

	if (vInputVector != _float3(0.f))
		Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);

	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;

	if (!g_bIsCanMouseMove && !g_bCutScene)
	{
		if (KEYTAP(KEY_TYPE::LBUTTON))
			m_eCurState = STATE::skill_1100;

		Use_Skill();
	}
}

void Kyle_FSM::b_run_end_r_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_run_end_r", 0.1f, false, 1.5f);

	Get_Transform()->Set_Speed(m_fRunSpeed);
	m_tRunEndDelay.fAccTime = 0.f;

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = false;
}

void Kyle_FSM::b_run_end_l()
{
	_float3 vInputVector = Get_InputDirVector();

	if (vInputVector != _float3(0.f))
		Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);

	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;

	if (!g_bIsCanMouseMove && !g_bCutScene)
	{
		if (KEYTAP(KEY_TYPE::LBUTTON))
			m_eCurState = STATE::skill_1100;

		Use_Skill();
	}
}

void Kyle_FSM::b_run_end_l_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_run_end_l", 0.1f, false, 1.5f);

	Get_Transform()->Set_Speed(m_fRunSpeed);
	m_tRunEndDelay.fAccTime = 0.f;

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = false;
}

void Kyle_FSM::b_sprint()
{
	Get_Transform()->Go_Straight();

	_float3 vInputVector = Get_InputDirVector();

	if (vInputVector == _float3(0.f))
	{
		m_tRunEndDelay.fAccTime += fDT;

		if (m_tRunEndDelay.fAccTime >= m_tRunEndDelay.fCoolTime)
		{
			if (m_iCurFrame % 2 == 0)
				m_eCurState = STATE::b_run_end_r;
			else
				m_eCurState = STATE::b_run_end_l;
		}
	}
	else
		Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);

	if (g_bIsCanMouseMove || g_bCutScene)
	{
		if (m_iCurFrame % 2 == 0)
			m_eCurState = STATE::b_run_end_r;
		else
			m_eCurState = STATE::b_run_end_l;
	}

	if (!g_bIsCanMouseMove && !g_bCutScene)
	{
		if (!KEYPUSH(KEY_TYPE::LSHIFT))
		{
			if (m_iCurFrame < 1 || m_iCurFrame > 13)
				m_eCurState = STATE::b_run;
		}

		if (KEYTAP(KEY_TYPE::LBUTTON))
			m_eCurState = STATE::skill_1100;

		Use_Skill();
	}
}

void Kyle_FSM::b_sprint_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_sprint", 0.1f, true, 1.f);

	Get_Transform()->Set_Speed(m_fSprintSpeed);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = false;
}

void Kyle_FSM::b_walk()
{
}

void Kyle_FSM::b_walk_Init()
{
}

void Kyle_FSM::die()
{
}

void Kyle_FSM::die_Init()
{
}

void Kyle_FSM::airborne_start()
{
	m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

	Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

	if (Is_AnimFinished())
		m_eCurState = STATE::airborne_end;
}

void Kyle_FSM::airborne_start_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"airborne_start", 0.2f, false, 1.f);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = true;

	Get_CharacterController()->Add_Velocity(6.f);
}

void Kyle_FSM::airborne_end()
{
	if (Is_AnimFinished())
		m_eCurState = STATE::airborne_up;
}

void Kyle_FSM::airborne_end_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"airborne_end", 0.2f, false, 1.f);

	m_bInvincible = false;
	m_bSuperArmor = true;
}

void Kyle_FSM::airborne_up()
{
	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;
}

void Kyle_FSM::airborne_up_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"knock_Up", 0.1f, false, 1.f);

	m_bInvincible = false;
	m_bSuperArmor = true;
}

void Kyle_FSM::hit()
{
	m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

	Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;
}

void Kyle_FSM::hit_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"hit", 0.1f, false, 1.f);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = false;
}

void Kyle_FSM::knock_start()
{
	m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

	Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

	Get_Transform()->Go_Backward();

	if (Is_AnimFinished())
		m_eCurState = STATE::knock_end;
}

void Kyle_FSM::knock_start_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"knock_start", 0.1f, false, 1.f);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = true;

	Get_Transform()->Set_Speed(m_fKnockBackSpeed * 0.8f);
}

void Kyle_FSM::knock_end()
{
	if (m_iCurFrame < 16)
		Get_Transform()->Go_Backward();

	if (Is_AnimFinished())
		m_eCurState = STATE::knock_end_loop;
}

void Kyle_FSM::knock_end_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"knock_end", 0.2f, false, 1.f);

	m_bInvincible = false;
	m_bSuperArmor = true;

	Get_Transform()->Set_Speed(m_fKnockBackSpeed * 0.5f);
}

void Kyle_FSM::knock_end_loop()
{
	m_tKnockDownEndCoolTime.fAccTime += fDT;

	if (m_iCurFrame > Get_FinalFrame() / 2)
		m_eCurState = STATE::knock_up;
}

void Kyle_FSM::knock_end_loop_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"knock_end_loop", 0.2f, false, 1.f);

	m_bInvincible = false;
	m_bSuperArmor = false;
}

void Kyle_FSM::knock_end_hit()
{
	m_tKnockDownEndCoolTime.fAccTime += fDT;

	if (Is_AnimFinished())
	{
		if (m_tKnockDownEndCoolTime.fAccTime >= m_tKnockDownEndCoolTime.fCoolTime)
			m_eCurState = STATE::knock_up;
		else
			m_eCurState = STATE::knock_end_loop;
	}
}

void Kyle_FSM::knock_end_hit_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"knock_end_hit", 0.2f, false, 1.f);

	m_bInvincible = false;
	m_bSuperArmor = false;
}

void Kyle_FSM::knock_up()
{
	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;
}

void Kyle_FSM::knock_up_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"knock_Up", 0.2f, false, 1.f);

	m_bInvincible = false;
	m_bSuperArmor = true;

	m_tKnockDownEndCoolTime.fAccTime = 0.f;

	Get_Transform()->Set_Speed(m_fRunSpeed);
}

void Kyle_FSM::knockdown_start()
{
	m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

	Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

	Get_Transform()->Go_Backward();

	if (Is_AnimFinished())
		m_eCurState = STATE::knockdown_end;
}

void Kyle_FSM::knockdown_start_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"knockdown_start", 0.1f, false, 2.f);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = true;

	Get_Transform()->Set_Speed(m_fKnockDownSpeed);
}

void Kyle_FSM::knockdown_end()
{
	if (m_iCurFrame < 16)
		Get_Transform()->Go_Backward();

	if (Is_AnimFinished())
		m_eCurState = STATE::knock_up;
}

void Kyle_FSM::knockdown_end_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_CurrentAnim(L"knockdown_end", false, 1.5f);

	m_bInvincible = false;
	m_bSuperArmor = true;

	Get_Transform()->Set_Speed(m_fKnockDownSpeed * 0.5f);
}

void Kyle_FSM::skill_1100()
{
	if (Init_CurFrame(9))
		Add_And_Set_Effect(L"Kyle_1100");

	Update_GroupEffectWorldPos(Get_Owner()->Get_Transform()->Get_WorldMatrix());

	Look_DirToTarget();

	if (m_iCurFrame == 9)
	{
		Set_ColliderOption(WATER, L"Hit_Slash_Orange");
		AttackCollider_On(NORMAL_ATTACK, 10.f);
	}
	else if (m_iCurFrame == 19)
		AttackCollider_Off();

	if (!g_bIsCanMouseMove && !g_bCutScene)
	{
		if (Check_Combo(22, KEY_TYPE::LBUTTON))
			m_eCurState = STATE::skill_1200;

		//Using Skill
		Use_Skill();
	}
	
	if (Is_AnimFinished())
	{
		if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
			m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		
		m_eCurState = STATE::b_idle;
	}

}

void Kyle_FSM::skill_1100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_1100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

	if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Start_Attack_Button_Effect();

	m_bCanCombo = false;

	Set_DirToTargetOrInput(OBJ_MONSTER);

	m_bInvincible = false;
	m_bSuperArmor = false;
}

void Kyle_FSM::skill_1200()
{
	if (Init_CurFrame(9))
		Add_And_Set_Effect(L"Klye_1200");

	Update_GroupEffectWorldPos(Get_Owner()->Get_Transform()->Get_WorldMatrix());

	Look_DirToTarget();

	if (m_iCurFrame == 9)
	{
		Set_ColliderOption(WATER, L"Hit_Slash_Orange");
		AttackCollider_On(NORMAL_ATTACK ,15.f);
	}
	else if (m_iCurFrame == 20)
		AttackCollider_Off();

	if (!g_bIsCanMouseMove && !g_bCutScene)
	{
		if (Check_Combo(20, KEY_TYPE::LBUTTON))
			m_eCurState = STATE::skill_1300;

		//Using Skill
		Use_Skill();
	}

	if (Is_AnimFinished())
	{
		if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
			m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		
		m_bCanCombo = false;
		m_eCurState = STATE::b_idle;
	}
}

void Kyle_FSM::skill_1200_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_1200", 0.15f, false, m_fNormalAttack_AnimationSpeed);

	if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
	{
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Start_Attack_Button_Effect();
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(DEFAULT);
	}

	m_bCanCombo = false;

	Set_DirToTargetOrInput(OBJ_MONSTER);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = false;
}

void Kyle_FSM::skill_1300()
{
	if (Init_CurFrame(9))
		Add_And_Set_Effect(L"Klye_1300");

	Update_GroupEffectWorldPos(Get_Owner()->Get_Transform()->Get_WorldMatrix());

	Look_DirToTarget();

	if (m_iCurFrame == 8)
	{
		Set_ColliderOption(WATER, L"Hit_Slash_Orange");
		AttackCollider_On(NORMAL_ATTACK, 10.f);
	}
	else if (m_iCurFrame == 18)
		AttackCollider_Off();

	if (!g_bIsCanMouseMove && !g_bCutScene)
	{
		if (Check_Combo(24, KEY_TYPE::LBUTTON))
			m_eCurState = STATE::skill_1400;

		//Using Skill
		Use_Skill();
	}

	if (Is_AnimFinished())
	{
		if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
			m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		
		m_bCanCombo = false;
		m_eCurState = STATE::b_idle;
	}

}

void Kyle_FSM::skill_1300_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_1300", 0.15f, false, m_fNormalAttack_AnimationSpeed);

	if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
	{
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Start_Attack_Button_Effect();
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(DEFAULT);
	}

	m_bCanCombo = false;

	Set_DirToTargetOrInput(OBJ_MONSTER);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = false;
}

void Kyle_FSM::skill_1400()
{
	Look_DirToTarget();

	if (Init_CurFrame(11))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 1.5f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::LOOK) * 3.f;
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 2.3f, desc, AIRBORNE_ATTACK, 10.f, L"Hit_Slash_Orange");
	}

	if (Is_AnimFinished())
	{
		if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
			m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		
		m_bCanCombo = false;
		m_eCurState = STATE::b_idle;
	}

	if (!g_bIsCanMouseMove && !g_bCutScene)
	{
		//Using Skill
		Use_Skill();
	}
}

void Kyle_FSM::skill_1400_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_1400", 0.15f, false, m_fNormalAttack_AnimationSpeed);

	if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
	{
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Start_Attack_Button_Effect();
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(DEFAULT);
	}

	m_bCanCombo = false;

	Set_DirToTargetOrInput(OBJ_MONSTER);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = false;
}

void Kyle_FSM::skill_91100()
{
	Look_DirToTarget();

	_float3 vInputVector = Get_InputDirVector();

	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;

	if (m_iCurFrame >= 22)
	{
		if (vInputVector != _float3(0.f))
			m_eCurState = STATE::b_run;
	}
}

void Kyle_FSM::skill_91100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_91100", 0.15f, false, m_fEvade_AnimationSpeed);

	m_bCanCombo = false;

	m_vDirToTarget = _float3(0.f);
	m_vDirToTarget = Get_InputDirVector();

	AttackCollider_Off();

	m_bInvincible = true;
	m_bSuperArmor = false;
}

void Kyle_FSM::skill_93100()
{
	_float3 vInputVector = Get_InputDirVector();

	if (Is_AnimFinished())
		m_eCurState = STATE::b_idle;

	if (m_iCurFrame >= 22)
	{
		if (vInputVector != _float3(0.f))
			m_eCurState = STATE::b_run;
	}
}

void Kyle_FSM::skill_93100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_93100", 0.15f, false, m_fEvade_AnimationSpeed);

	m_bCanCombo = false;

	AttackCollider_Off();

	m_bInvincible = true;
	m_bSuperArmor = false;
}

void Kyle_FSM::skill_100100()
{
	Look_DirToTarget();

	if (m_iCurFrame >= 17)
	{
		if (m_iCurFrame == 17)
			m_vCamStopPos = m_pCamera.lock()->Get_Transform()->Get_State(Transform_State::POS);

		if (!m_pCamera.expired())
		{
			_float4 vDir = m_vCamStopPos - m_vCenterBonePos;
			vDir.Normalize();

			m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
			m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
			m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.35f, vDir.xyz(), 6.f);
		}
	}

	if (Init_CurFrame(5))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 1.5f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::UP);
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 2.f, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Orange");
	}

	if (!g_bIsCanMouseMove && !g_bCutScene)
	{
		if (Check_Combo(25, KEY_TYPE::KEY_1))
			m_eCurState = STATE::skill_100200;

		Use_Dash();
	}

	if (Is_AnimFinished())
	{
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		m_eCurState = STATE::b_idle;
	}

	Calculate_CamBoneMatrix();

}

void Kyle_FSM::skill_100100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_100100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

	m_bCanCombo = false;

	Set_DirToTargetOrInput(OBJ_MONSTER);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = true;
}

void Kyle_FSM::skill_100200()
{
	Look_DirToTarget();
	
	if (m_iCurFrame >= 4)
	{
		if (!m_pCamera.expired())
		{
			_float4 vDir = m_vCamStopPos - m_vCenterBonePos;
			vDir.Normalize();

			m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(2.f);
			m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
			m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.5f, vDir.xyz(), 5.f);
		}
	}

	if (Init_CurFrame(12))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 0.5f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS);

		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 2.f, desc, AIRBORNE_ATTACK, 10.f, L"Hit_Slash_Orange");
	}

	if (Is_AnimFinished())
	{
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		m_eCurState = STATE::b_idle;
		
		if (!m_pCamera.expired())
			m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedTime(0.f);
	}

	Calculate_CamBoneMatrix();

	if (!g_bIsCanMouseMove && !g_bCutScene)
		Use_Dash();
}

void Kyle_FSM::skill_100200_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_100200", 0.15f, false, m_fSkillAttack_AnimationSpeed);
	m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(SKILL1);

	m_bCanCombo = false;

	Set_DirToTargetOrInput(OBJ_MONSTER);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = true;

	Calculate_CamBoneMatrix();
}

void Kyle_FSM::skill_200100()
{
	Look_DirToTarget();

	if (m_iCurFrame == 11)
	{
		Set_ColliderOption(FIRE, L"Hit_Slash_Orange");
		AttackCollider_On(NORMAL_ATTACK, 10.f);
	}
	else if (m_iCurFrame == 23)
		AttackCollider_Off();
	
	if (!g_bIsCanMouseMove && !g_bCutScene)
	{
		if (Check_Combo(16, KEY_TYPE::KEY_2))
			m_eCurState = STATE::skill_200200;

		Use_Dash();
	}

	if (Is_AnimFinished())
	{
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		m_eCurState = STATE::b_idle;
	}


}

void Kyle_FSM::skill_200100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_200100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

	m_bCanCombo = false;

	Set_DirToTargetOrInput(OBJ_MONSTER);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = true;
}

void Kyle_FSM::skill_200200()
{
	if (m_iCurFrame == 13)
	{
		Set_ColliderOption(FIRE, L"Hit_Slash_Orange");

		AttackCollider_On(NORMAL_ATTACK, 10.f);
	}
	else if (m_iCurFrame == 18)
	{
		Set_ColliderOption(FIRE, L"Hit_Slash_Orange");
		AttackCollider_Off();
	}
	else if (m_iCurFrame == 21)
	{
		Set_ColliderOption(FIRE, L"Hit_Slash_Orange");
		AttackCollider_On(NORMAL_ATTACK, 10.f);
	}
	else if (m_iCurFrame == 28)
		AttackCollider_Off();

	Look_DirToTarget();

	if (!g_bIsCanMouseMove && !g_bCutScene)
	{
		if (Check_Combo(28, KEY_TYPE::KEY_2))
			m_eCurState = STATE::skill_200300;

		Use_Dash();
	}

	if (Is_AnimFinished())
	{
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		m_eCurState = STATE::b_idle;
	}

}

void Kyle_FSM::skill_200200_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_200200", 0.05f, false, m_fSkillAttack_AnimationSpeed);
	m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(SKILL2);

	m_bCanCombo = false;

	Set_DirToTargetOrInput(OBJ_MONSTER);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = true;
}

void Kyle_FSM::skill_200300()
{
	if (Init_CurFrame(9))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 1.5f;
		desc.fLimitDistance = 0.f;
		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::LOOK) * 5.5f;
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 3.f, desc, AIRBORNE_ATTACK, 10.f, L"Hit_Slash_Orange");
	}
	
	Look_DirToTarget();

	if (Is_AnimFinished())
	{
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		m_eCurState = STATE::b_idle;
	}

	if (!g_bIsCanMouseMove && !g_bCutScene)
		Use_Dash();
}

void Kyle_FSM::skill_200300_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_200300", 0.05f, false, m_fSkillAttack_AnimationSpeed);
	m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(SKILL2);

	m_bCanCombo = false;

	Set_DirToTargetOrInput(OBJ_MONSTER);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = true;
}

void Kyle_FSM::skill_300100()
{
	Look_DirToTarget();

	if (m_iCurFrame == 2)
	{
		Set_ColliderOption(FIRE, L"Hit_Slash_Orange");
		AttackCollider_On(NORMAL_ATTACK, 10.f);
	}
	else if (m_iCurFrame == 5)
		AttackCollider_Off();

	if (Init_CurFrame(10))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 0.5f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::UP);

		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 2.5f, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Orange");
	}
	else if (Init_CurFrame(28))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 0.5f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::LOOK) * 4.f;

		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 3.f, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Orange");

	}
	else if (Init_CurFrame(36))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 1.f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::LOOK) * 4.f;

		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 4.f, desc, AIRBORNE_ATTACK, 10.f, L"Hit_Slash_Orange");

	}


	if (Is_AnimFinished())
	{
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		m_eCurState = STATE::b_idle;
	}

	if (!g_bIsCanMouseMove && !g_bCutScene)
		Use_Dash();
}

void Kyle_FSM::skill_300100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_300100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

	m_bCanCombo = false;

	Set_DirToTargetOrInput(OBJ_MONSTER);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = true;
}

void Kyle_FSM::skill_502100()
{
	if (m_iCurFrame == 17)
	{
		if (!m_pCamera.expired())
		{
			_float4 vDestinationPos = (Get_Transform()->Get_State(Transform_State::POS)) + (Get_Transform()->Get_State(Transform_State::LOOK) * -5.f) + _float3::Up * 4.f;
			_float4 vDir = vDestinationPos - (Get_Transform()->Get_State(Transform_State::POS));
			vDir.Normalize();

			m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
			m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
			m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(4.5f, vDir.xyz(), 10.f);
		}
	}
	else if (m_iCurFrame == 90)
	{
		m_vCamStopPos = m_vCenterBonePos;
	}
	else if (m_iCurFrame > 90 && m_iCurFrame <= 152)
	{
		m_vCamStopPos.y += fDT * 2.f;
		m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCamStopPos.xyz());
	}
	else if (m_iCurFrame > 152)
	{
		if (m_vCamStopPos.y >= 0.f)
			m_vCamStopPos.y -= fDT * 3.f;
		else
			m_vCamStopPos.y = 0.f;

		m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCamStopPos.xyz());
	}
	

	Calculate_CamBoneMatrix();

	if (Init_CurFrame(55))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 1.f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::LOOK) * 2.f +
			Get_Transform()->Get_State(Transform_State::UP);
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 1.f, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Orange");

	}
	else if (Init_CurFrame(78))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 1.f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::LOOK) * 5.f +
			Get_Transform()->Get_State(Transform_State::UP);
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 2.f, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Orange");
	}
	else if (Init_CurFrame(106))
	{
		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::LOOK) * 5.f +
			Get_Transform()->Get_State(Transform_State::UP);

		_float4 vSkillDir = (Get_Transform()->Get_State(Transform_State::POS) - vSkillPos);
		vSkillDir.y = 0.f;

		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = vSkillDir.Normalize();
		desc.fMoveSpeed = 20.f;
		desc.fLifeTime = 1.f;
		desc.fLimitDistance = 4.f;

		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 2.f, desc, NORMAL_SKILL, 10.f, L"Hit_Slash_Orange");
	}

	Look_DirToTarget();

	if (Is_AnimFinished())
	{
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		m_eCurState = STATE::b_idle;
	}

}

void Kyle_FSM::skill_502100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_502100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

	m_bCanCombo = false;

	Set_DirToTargetOrInput(OBJ_MONSTER);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = true;
}

void Kyle_FSM::skill_500100()
{
	if (m_iCurFrame >= 1)
	{
		if (!m_pCamera.expired())
		{
			_float4 vDestinationPos = (Get_Transform()->Get_State(Transform_State::POS)) + 
									   m_vSkillCamRight +
									  (Get_Transform()->Get_State(Transform_State::LOOK) * -3.f) 
										+ _float3::Up * 6.f;
			_float4 vDir = vDestinationPos - m_vCenterBonePos; 

			vDir.Normalize();
			
			m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
			m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
			m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.5f, vDir.xyz(), 8.f);
		}
	}
	
	Calculate_CamBoneMatrix();

	if (Init_CurFrame(11))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 0.5f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::UP);
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 4.f, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Orange");

	}
	else if (Init_CurFrame(31))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 0.5f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::UP);
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 4.f, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Orange");

	}
	else if (Init_CurFrame(47))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 0.5f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::UP);
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 4.f, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Orange");

	}
	else if (Init_CurFrame(87))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 0.5f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::LOOK) * 5.f;
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 4.f, desc, AIRBORNE_SKILL, 10.f, L"Hit_Slash_Orange");

	}

	if (m_iCurFrame > 11)
		m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(Get_Transform()->Get_State(Transform_State::POS).xyz());

	if (m_iCurFrame == 164)
	{
		m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(0.2f);
	}

	Look_DirToTarget();

	if (Is_AnimFinished())
	{
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
		m_eCurState = STATE::b_idle;
	}

	if (!g_bIsCanMouseMove && !g_bCutScene)
		Use_Dash();
}

void Kyle_FSM::skill_500100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_500100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

	m_bCanCombo = false;

	Set_DirToTargetOrInput(OBJ_MONSTER);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = true;

	Calculate_CamBoneMatrix();

	//For.Skill_CutScene
	Cal_SkillCamDirection(3.f);
}

void Kyle_FSM::Use_Skill()
{
	if (KEYTAP(KEY_TYPE::KEY_1) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL1))
		m_eCurState = STATE::skill_100100;
	else if (KEYTAP(KEY_TYPE::KEY_2) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL2))
		m_eCurState = STATE::skill_200100;
	else if (KEYTAP(KEY_TYPE::KEY_3) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL3))
		m_eCurState = STATE::skill_300100;
	else if (KEYTAP(KEY_TYPE::KEY_4) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL4))
		m_eCurState = STATE::skill_502100;
	else if (KEYTAP(KEY_TYPE::KEY_5) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL5))
		m_eCurState = STATE::skill_500100;
	else 
		Use_Dash();
}

void Kyle_FSM::Use_Dash()
{
	if (KEYTAP(KEY_TYPE::SPACE))
	{
		if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(EVADE))
		{
			m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

			_float3 vInputVector = Get_InputDirVector();
			if (vInputVector != _float3(0.f))
				m_eCurState = STATE::skill_91100;
			else
				m_eCurState = STATE::skill_93100;
		}
	}
}
