#include "pch.h"
#include "MainCameraScript.h"
#include "Dellons_FSM.h"
#include "ModelAnimator.h"
#include "MotionTrailRenderer.h"
#include "Debug_CreateMotionTrail.h"
#include "SphereCollider.h"
#include "AttackColliderInfoScript.h"
#include "Model.h"
#include "Camera.h"
#include "DellonsWraith_FSM.h"
#include "CoolTimeCheckScript.h"
#include "CharacterController.h"


Dellons_FSM::Dellons_FSM()
{
}

Dellons_FSM::~Dellons_FSM()
{
}

HRESULT Dellons_FSM::Init()
{
    if (!m_bInitialize)
    {
        auto animator = Get_Owner()->Get_Animator();
        if (animator)
        {
            animator->Set_CurrentAnim(L"b_idle", true, 1.f);
            m_eCurState = STATE::b_idle;
        }

        m_pCamera = CUR_SCENE->Get_MainCamera();

        m_iCenterBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_CP");
        m_iCamBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_Cam");
        m_iSkillCamBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_SkillCam");
        m_fDetectRange = 5.f;

        m_bEntryTeam = true;

        m_bInitialize = true;
    }
	m_fNormalAttack_AnimationSpeed = 1.2f;
	m_fSkillAttack_AnimationSpeed = 1.0f;
	m_fEvade_AnimationSpeed = 1.5f;
    return S_OK;
}

void Dellons_FSM::Tick()
{
    State_Tick();

    if (!m_pAttackCollider.expired())
    {
        //m_pAttack transform set forward
		m_pAttackCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 1.5f + _float3::Up);
    }


}

void Dellons_FSM::State_Tick()
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
    case STATE::skill_300100:
        skill_300100();
        break;
    case STATE::skill_400100:
        skill_400100();
        break;
    case STATE::skill_501100:
        skill_501100();
        break;
    }

    Update_GroupEffectWorldPos(Get_Owner()->Get_Transform()->Get_WorldMatrix());

    if (m_iPreFrame != m_iCurFrame)
        m_iPreFrame = m_iCurFrame;
}

void Dellons_FSM::State_Init()
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
        case STATE::skill_300100:
            skill_300100_Init();
            break;
        case STATE::skill_400100:
            skill_400100_Init();
            break;
        case STATE::skill_501100:
            skill_501100_Init();
            break;
        }
        m_ePreState = m_eCurState;
    }
}

void Dellons_FSM::Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget)
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

void Dellons_FSM::Set_State(_uint iIndex)
{
}

void Dellons_FSM::b_idle()
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

void Dellons_FSM::b_idle_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    if (m_ePreState != STATE::skill_400100)
        animator->Set_NextTweenAnim(L"b_idle", 0.1f, true, 1.f);
    else
        animator->Set_NextTweenAnim(L"b_idle", 0.3f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Dellons_FSM::b_run_start()
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

void Dellons_FSM::b_run_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_start", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Dellons_FSM::b_run()
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
            if ((m_iCurFrame == 1))
                m_eCurState = STATE::b_sprint;

        }

        if (KEYTAP(KEY_TYPE::LBUTTON))
            m_eCurState = STATE::skill_1100;

        Use_Skill();
    }
}

void Dellons_FSM::b_run_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Dellons_FSM::b_run_end_r()
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

void Dellons_FSM::b_run_end_r_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_end_r", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Dellons_FSM::b_run_end_l()
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

void Dellons_FSM::b_run_end_l_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_end_l", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Dellons_FSM::b_sprint()
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

void Dellons_FSM::b_sprint_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_sprint", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fSprintSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Dellons_FSM::b_walk()
{
}

void Dellons_FSM::b_walk_Init()
{
}

void Dellons_FSM::die()
{
}

void Dellons_FSM::die_Init()
{
}

void Dellons_FSM::airborne_start()
{
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::airborne_end;
}

void Dellons_FSM::airborne_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"airborne_start", 0.2f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_CharacterController()->Add_Velocity(6.f);
}

void Dellons_FSM::airborne_end()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::airborne_up;
}

void Dellons_FSM::airborne_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"airborne_end", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Dellons_FSM::airborne_up()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Dellons_FSM::airborne_up_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_Up", 0.1f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Dellons_FSM::hit()
{
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Dellons_FSM::hit_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"hit", 0.1f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Dellons_FSM::knock_start()
{
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_end;
}

void Dellons_FSM::knock_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_start", 0.1f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockBackSpeed * 0.8f);
}

void Dellons_FSM::knock_end()
{
    if (m_iCurFrame < 16)
        Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_end_loop;
}

void Dellons_FSM::knock_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockBackSpeed * 0.5f);
}

void Dellons_FSM::knock_end_loop()
{
    m_tKnockDownEndCoolTime.fAccTime += fDT;

    if (m_iCurFrame > Get_FinalFrame() / 2)
        m_eCurState = STATE::knock_up;
}

void Dellons_FSM::knock_end_loop_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end_loop", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Dellons_FSM::knock_end_hit()
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

void Dellons_FSM::knock_end_hit_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end_hit", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Dellons_FSM::knock_up()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Dellons_FSM::knock_up_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_Up", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    m_tKnockDownEndCoolTime.fAccTime = 0.f;

    Get_Transform()->Set_Speed(m_fRunSpeed);
}

void Dellons_FSM::knockdown_start()
{
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knockdown_end;
}

void Dellons_FSM::knockdown_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knockdown_start", 0.1f, false, 2.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockDownSpeed);
}

void Dellons_FSM::knockdown_end()
{
    if (m_iCurFrame < 16)
        Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_up;
}

void Dellons_FSM::knockdown_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_CurrentAnim(L"knockdown_end", false, 1.5f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockDownSpeed * 0.5f);
}

void Dellons_FSM::skill_1100()
{
    if (m_iCurFrame == 9)
	{
		Set_ColliderOption(DARK, L"Hit_Slash_Dark");
		AttackCollider_On(NORMAL_ATTACK, 10.f);

    }
    else if (m_iCurFrame == 19)
        AttackCollider_Off();


    Look_DirToTarget();

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        if (Check_Combo(24, KEY_TYPE::LBUTTON))
            m_eCurState = STATE::skill_1200;

        Use_Skill();
    }

    if (Is_AnimFinished())
    {
        if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
            m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        
        m_eCurState = STATE::b_idle;
    }


}

void Dellons_FSM::skill_1100_Init()
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

void Dellons_FSM::skill_1200()
{
    if (m_iCurFrame == 8)
    {
        Set_ColliderOption(DARK, L"Hit_Slash_Dark");
        AttackCollider_On(NORMAL_ATTACK,10.f);
    }
    else if (m_iCurFrame == 18)
        AttackCollider_Off();

    Look_DirToTarget();

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        if (Check_Combo(20, KEY_TYPE::LBUTTON))
            m_eCurState = STATE::skill_1300;

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

void Dellons_FSM::skill_1200_Init()
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

void Dellons_FSM::skill_1300()
{
    if (m_iCurFrame == 8)
    {
        Set_ColliderOption(DARK, L"Hit_Slash_Dark");
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    }
    else if (m_iCurFrame == 33)
        AttackCollider_Off();

    Look_DirToTarget();

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        if (Check_Combo(18, KEY_TYPE::LBUTTON))
            m_eCurState = STATE::skill_1400;

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

void Dellons_FSM::skill_1300_Init()
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

void Dellons_FSM::skill_1400()
{
    if (m_iCurFrame == 8)
    {
        Set_ColliderOption(DARK, L"Hit_Slash_Dark");
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    }
    else if (m_iCurFrame == 14)
        AttackCollider_Off();
    else if (m_iCurFrame == 16)
    {
        Set_ColliderOption(DARK, L"Hit_Slash_Dark");
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    }
    else if (m_iCurFrame == 24)
        AttackCollider_Off();


    Look_DirToTarget();

    if (Is_AnimFinished())
    {
        if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
            m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        
        m_bCanCombo = false;
        m_eCurState = STATE::b_idle;
    }

    if (!g_bIsCanMouseMove && !g_bCutScene)
        Use_Skill();
}

void Dellons_FSM::skill_1400_Init()
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

void Dellons_FSM::skill_91100()
{
    _float3 vInputVector = Get_InputDirVector();

    Look_DirToTarget();

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;

    if (m_iCurFrame >= 27)
    {
        if (vInputVector != _float3(0.f))
            m_eCurState = STATE::b_run;
    }
}

void Dellons_FSM::skill_91100_Init()
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

void Dellons_FSM::skill_93100()
{
    _float3 vInputVector = Get_InputDirVector();

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;

    if (m_iCurFrame >= 18)
    {
        if (vInputVector != _float3(0.f))
            m_eCurState = STATE::b_run;
    }
}

void Dellons_FSM::skill_93100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_93100", 0.15f, false, m_fEvade_AnimationSpeed);

    m_bCanCombo = false;

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Dellons_FSM::skill_100100()
{
    if (Init_CurFrame(12))
    {
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 0.5f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::LOOK) * 2.f +
			_float3::Up;

		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 1.5f, desc, KNOCKBACK_ATTACK, 5.f, L"Hit_Slash_Dark");
    }
    

    Look_DirToTarget();

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        if (Check_Combo(26, KEY_TYPE::KEY_1))
            m_eCurState = STATE::skill_100200;

        Use_Dash();
    }

    if (Is_AnimFinished())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::b_idle;
    }


}

void Dellons_FSM::skill_100100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Dellons_FSM::skill_100200()
{
    if (Init_CurFrame(15))
    {
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 0.5f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
			Get_Transform()->Get_State(Transform_State::LOOK) * 2.f +
			_float3::Up;
        Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 1.5f, desc, AIRBORNE_ATTACK, 10.f, L"Hit_Slash_Dark");
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

void Dellons_FSM::skill_100200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100200", 0.15f, false, m_fSkillAttack_AnimationSpeed);
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(SKILL1);

    m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Dellons_FSM::skill_200100()
{
    if (m_iCurFrame == 7)
    {
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
        Set_ColliderOption(DARK, L"Hit_Slash_Dark");
    }
    else if (m_iCurFrame == 12)
        AttackCollider_Off();

    Look_DirToTarget();

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        if (Check_Combo(20, KEY_TYPE::KEY_2))
            m_eCurState = STATE::skill_200200;

        Use_Dash();
    }

    if (Is_AnimFinished())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::b_idle;
    }


}

void Dellons_FSM::skill_200100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Dellons_FSM::skill_200200()
{
    if (Init_CurFrame(7))
    {
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 1.f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 3.f + _float3::Up;
        Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 2.f, desc, KNOCKBACK_SKILL, 10.f, L"Hit_Slash_Dark");
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

void Dellons_FSM::skill_200200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200200", 0.15f, false, m_fSkillAttack_AnimationSpeed);
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(SKILL2);

    m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Dellons_FSM::skill_300100()
{
    Look_DirToTarget();
    if (m_iCurFrame >= 10)
    {
        if (!m_pCamera.expired())
        {
            _float4 vDestinationPos = (Get_Transform()->Get_State(Transform_State::POS)) +
                m_vSkillCamRight +
                (Get_Transform()->Get_State(Transform_State::LOOK) * -3.f)
                + _float3::Up * 6.f;
            _float4 vDir = vDestinationPos - m_vSkillCamBonePos;

            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.5f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vSkillCamBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.3f, vDir.xyz(), 13.f);
        }
    }

    Calculate_CamBoneMatrix();

    if (Init_CurFrame(10))
    {
		Summon_Wraith();
		Set_WraithState((_uint)DellonsWraith_FSM::STATE::FX_DellonsWraith_skill_30010);
    }

    if (Is_AnimFinished())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::b_idle;
    }

}

void Dellons_FSM::skill_300100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_300100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = true;

    Calculate_CamBoneMatrix();
    Cal_SkillCamDirection(3.f);
}

void Dellons_FSM::skill_400100()
{
    if (m_iCurFrame >= 13)
    {
        if (!m_pCamera.expired())
        {
            _float4 vDestinationPos = (Get_Transform()->Get_State(Transform_State::POS)) +
                m_vSkillCamRight +
                (Get_Transform()->Get_State(Transform_State::LOOK) * -3.f)
                + _float3::Up * 6.f;
            _float4 vDir = vDestinationPos - m_vSkillCamBonePos;

            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.5f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vSkillCamBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.3f, vDir.xyz(), 4.5f);
        }
    }

    Calculate_CamBoneMatrix();

    if (Init_CurFrame(20))
    {
		Summon_Wraith();
        Set_WraithState((_uint)DellonsWraith_FSM::STATE::FX_Mn_Dellons_skill_5100);
    }
    else if (Init_CurFrame(33) ||
        Init_CurFrame(40) ||
        Init_CurFrame(47) ||
        Init_CurFrame(60) ||
        Init_CurFrame(67) ||
        Init_CurFrame(72))
    {
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 20.f;
		desc.fLifeTime = 0.5f;
		desc.fLimitDistance = 3.5f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 2.f + _float3::Up;
        Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 1.f, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Dark");

    }
    else if (Init_CurFrame(99))
    {
        FORWARDMOVINGSKILLDESC desc;
        desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
        desc.fMoveSpeed = 20.f;
        desc.fLifeTime = 1.f;
        desc.fLimitDistance = 5.f;
    
        _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * -0.5f + _float3::Up;
        Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 2.f, desc, KNOCKDOWN_SKILL, 10.f, L"Hit_Slash_Dark");
    }



    Look_DirToTarget();

    if (m_iCurFrame >= 110)
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::b_idle;
    }

}

void Dellons_FSM::skill_400100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_400100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = true;

    Cal_SkillCamDirection(3.f);
}

void Dellons_FSM::skill_501100()
{
    if (Init_CurFrame(4))
    {
		Summon_Wraith();
		Set_WraithState((_uint)DellonsWraith_FSM::STATE::FX_Mn_Dellons_skill_500200);
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

void Dellons_FSM::skill_501100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_501100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Dellons_FSM::Use_Skill()
{
     if (KEYTAP(KEY_TYPE::KEY_1) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL1))
         m_eCurState = STATE::skill_100100;
     else if (KEYTAP(KEY_TYPE::KEY_2) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL2))
         m_eCurState = STATE::skill_200100;
     else if (KEYTAP(KEY_TYPE::KEY_3) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL3))
         m_eCurState = STATE::skill_300100;
     else if (KEYTAP(KEY_TYPE::KEY_4) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL4))
         m_eCurState = STATE::skill_400100;
     else if (KEYTAP(KEY_TYPE::KEY_5) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL5))
         m_eCurState = STATE::skill_501100;
     else 
         Use_Dash();
}

void Dellons_FSM::Use_Dash()
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

void Dellons_FSM::Summon_Wraith()
{
    //Add. Player's Wraith
    shared_ptr<GameObject> ObjWraith = make_shared<GameObject>();

    ObjWraith->Add_Component(make_shared<Transform>());
    {
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

        shared_ptr<ModelAnimator> renderer = make_shared<ModelAnimator>(shader);
        {
            shared_ptr<Model> model = RESOURCES.Get<Model>(L"Dellons_Wraith");
            renderer->Set_Model(model);
        }

        ObjWraith->Add_Component(renderer);

    }
    ObjWraith->Add_Component(make_shared<DellonsWraith_FSM>());
    ObjWraith->Get_FSM()->Set_Target(m_pOwner.lock());
    ObjWraith->Get_FSM()->Init();
    ObjWraith->Set_Name(L"Dellons_Wraith");

    EVENTMGR.Create_Object(ObjWraith);

    m_pDellonsWraith = ObjWraith;
}

void Dellons_FSM::Set_WraithState(_uint iAnimindex)
{
    if (!m_pDellonsWraith.expired())
        m_pDellonsWraith.lock()->Get_FSM()->Set_State(iAnimindex);
}