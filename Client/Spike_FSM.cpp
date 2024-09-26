#include "pch.h"
#include "MainCameraScript.h"
#include "Spike_FSM.h"
#include "ModelAnimator.h"
#include "MotionTrailRenderer.h"
#include "Debug_CreateMotionTrail.h"
#include "SphereCollider.h"
#include "AttackColliderInfoScript.h"
#include "Model.h"
#include "CoolTimeCheckScript.h"
#include "UiSkillGauge.h"
#include "GroupEffect.h"
#include "CharacterController.h"

Spike_FSM::Spike_FSM()
{
}

Spike_FSM::~Spike_FSM()
{
}

HRESULT Spike_FSM::Init()
{
    if (!m_bInitialize)
    {
        auto animator = Get_Owner()->Get_Animator();
        if (animator)
        {
            animator->Set_CurrentAnim(L"b_idle", true, 1.f);
            m_eCurState = STATE::b_idle;
        }

        m_iDummy_CP_BoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_CP");
        m_iCenterBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_Center");
        m_iCamBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_Cam");
        m_iSkillCamBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_SkillCam");


        m_pCamera = CUR_SCENE->Get_MainCamera();
        m_fDetectRange = 5.f;

        m_bEntryTeam = true;

        m_bInitialize = true;
    }

	m_fNormalAttack_AnimationSpeed = 1.5f;
	m_fSkillAttack_AnimationSpeed = 1.0f;
	m_fEvade_AnimationSpeed = 1.5f;
    return S_OK;
}

void Spike_FSM::Tick()
{
    State_Tick();

    if (!m_pAttackCollider.expired())
    {
        //m_pAttack transform set forward
		m_pAttackCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 1.5f + _float3::Up);
    }

}

void Spike_FSM::State_Tick()
{
    State_Init();

    m_iCurFrame = Get_CurFrame();
    Recovery_Color();
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
    case STATE::skill_100300:
        skill_100300();
        break;
    case STATE::skill_200100:
        skill_200100();
        break;
    case STATE::skill_200100_l:
        skill_200100_l();
        break;
    case STATE::skill_200200:
        skill_200200();
        break;
    case STATE::skill_200300:
        skill_200300();
        break;
    case STATE::skill_200400:
        skill_200400();
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

void Spike_FSM::State_Init()
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
        case STATE::skill_100300:
            skill_100300_Init();
            break;
        case STATE::skill_200100:
            skill_200100_Init();
            break;
        case STATE::skill_200100_l:
            skill_200100_l_Init();
            break;
        case STATE::skill_200200:
            skill_200200_Init();
            break;
        case STATE::skill_200300:
            skill_200300_Init();
            break;
        case STATE::skill_200400:
            skill_200400_Init();
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

void Spike_FSM::Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget)
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

void Spike_FSM::Set_State(_uint iIndex)
{
}

void Spike_FSM::b_idle()
{
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

void Spike_FSM::b_idle_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_idle", 0.1f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;

    m_fWheelWindRange = 2.f;
    m_fWheelWindSpeed = 6.f;

    m_fChargingRatio = 0.f;
}

void Spike_FSM::b_run_start()
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


        if (KEYTAP(KEY_TYPE::LBUTTON))
            m_eCurState = STATE::skill_1100;

        Use_Skill();
    }
}

void Spike_FSM::b_run_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_start", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Spike_FSM::b_run()
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

void Spike_FSM::b_run_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Spike_FSM::b_run_end_r()
{
    _float3 vInputVector = Get_InputDirVector();

    if (vInputVector != _float3(0.f))
        Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;


    if (KEYTAP(KEY_TYPE::LBUTTON))
        m_eCurState = STATE::skill_1100;

    Use_Skill();
}

void Spike_FSM::b_run_end_r_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_end_r", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Spike_FSM::b_run_end_l()
{
    _float3 vInputVector = Get_InputDirVector();

    if (vInputVector != _float3(0.f))
        Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;


    if (KEYTAP(KEY_TYPE::LBUTTON))
        m_eCurState = STATE::skill_1100;

    Use_Skill();
}

void Spike_FSM::b_run_end_l_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_end_l", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Spike_FSM::b_sprint()
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

void Spike_FSM::b_sprint_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_sprint", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fSprintSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Spike_FSM::b_walk()
{
}

void Spike_FSM::b_walk_Init()
{
}

void Spike_FSM::die()
{
}

void Spike_FSM::die_Init()
{
}

void Spike_FSM::airborne_start()
{
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::airborne_end;
}

void Spike_FSM::airborne_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"airborne_start", 0.2f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_CharacterController()->Add_Velocity(6.f);
}

void Spike_FSM::airborne_end()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::airborne_up;
}

void Spike_FSM::airborne_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"airborne_end", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Spike_FSM::airborne_up()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Spike_FSM::airborne_up_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_Up", 0.1f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Spike_FSM::hit()
{
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Spike_FSM::hit_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"hit", 0.1f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Spike_FSM::knock_start()
{
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_end;
}

void Spike_FSM::knock_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_start", 0.1f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockBackSpeed * 0.8f);
}

void Spike_FSM::knock_end()
{
    if (m_iCurFrame < 16)
        Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_end_loop;
}

void Spike_FSM::knock_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockBackSpeed * 0.5f);
}

void Spike_FSM::knock_end_loop()
{
    m_tKnockDownEndCoolTime.fAccTime += fDT;

    if (m_iCurFrame > Get_FinalFrame() / 2)
        m_eCurState = STATE::knock_up;
}

void Spike_FSM::knock_end_loop_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end_loop", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Spike_FSM::knock_end_hit()
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

void Spike_FSM::knock_end_hit_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end_hit", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Spike_FSM::knock_up()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Spike_FSM::knock_up_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_Up", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    m_tKnockDownEndCoolTime.fAccTime = 0.f;

    Get_Transform()->Set_Speed(m_fRunSpeed);
}

void Spike_FSM::knockdown_start()
{
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knockdown_end;
}

void Spike_FSM::knockdown_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knockdown_start", 0.1f, false, 2.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockDownSpeed);
}

void Spike_FSM::knockdown_end()
{
    if (m_iCurFrame < 16)
        Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_up;
}

void Spike_FSM::knockdown_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_CurrentAnim(L"knockdown_end", false, 1.5f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockDownSpeed * 0.5f);
}

void Spike_FSM::skill_1100()
{
    if (Init_CurFrame(11))
    {
        CAMERA_SHAKE(0.1f, 0.1f)
        Add_And_Set_Effect(L"Spike_1100");
        Set_ColliderOption(WATER, L"Hit_Slash_Blue");
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    }
    else if (m_iCurFrame == 17)
    {

        AttackCollider_Off();
    }

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

void Spike_FSM::skill_1100_Init()
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

void Spike_FSM::skill_1200()
{
    if (Init_CurFrame(0))
	{
		CAMERA_SHAKE(0.1f, 0.1f)
        Add_And_Set_Effect(L"Spike_1200");
        Set_ColliderOption(WATER, L"Hit_Slash_Blue");
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    }
    else if (m_iCurFrame == 14)
    {
        AttackCollider_Off();
    }

    Look_DirToTarget();

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        if (Check_Combo(33, KEY_TYPE::LBUTTON))
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

void Spike_FSM::skill_1200_Init()
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

void Spike_FSM::skill_1300()
{
    if (Init_CurFrame(15))
	{
		CAMERA_SHAKE(0.1f, 0.1f)
        Add_And_Set_Effect(L"Spike_1300");
        Set_ColliderOption(WATER, L"Hit_Slash_Blue");
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    }
    else if (m_iCurFrame == 18)
    {
        AttackCollider_Off();
    }
    Look_DirToTarget();

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        if (Check_Combo(35, KEY_TYPE::LBUTTON))
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

void Spike_FSM::skill_1300_Init()
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

void Spike_FSM::skill_1400()
{
    if (Init_CurFrame(7))
    {
        Add_And_Set_Effect(L"Spike_1400_1");
    }
    if (Init_CurFrame(11))
    {
        Add_GroupEffectOwner(L"Spike_1400_2", _float3(0.f, 0.f, 1.f), false);
        AttackCollider_On(KNOCKDOWN_ATTACK, 10.f);
        Set_ColliderOption(WATER, L"Hit_Slash_Blue");
        
        //CAMERA_SHAKE(0.3f, 0.4f);
    }
    else if (m_iCurFrame == 15)
    {
		AttackCollider_Off();
    }

    if(Init_CurFrame(13))
        CAMERA_SHAKE(0.3f, 0.4f);

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

void Spike_FSM::skill_1400_Init()
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
    Set_ColliderOption(WATER, L"");
}

void Spike_FSM::skill_91100()
{
    _float3 vInputVector = Get_InputDirVector();

    Look_DirToTarget();

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Spike_FSM::skill_91100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_91100", 0.15f, false, m_fEvade_AnimationSpeed);

    CUR_SCENE->Get_UI(L"UI_Skill_Use_Gauge")->Get_Script<UiSkillGauge>()->Change_Render(false);

    m_bCanCombo = false;

    m_vDirToTarget = _float3(0.f);
    m_vDirToTarget = Get_InputDirVector();

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Spike_FSM::skill_93100()
{
    _float3 vInputVector = Get_InputDirVector();

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Spike_FSM::skill_93100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_93100", 0.15f, false, m_fEvade_AnimationSpeed);

    CUR_SCENE->Get_UI(L"UI_Skill_Use_Gauge")->Get_Script<UiSkillGauge>()->Change_Render(false);

    m_bCanCombo = false;

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Spike_FSM::skill_100100()
{
    if (Init_CurFrame(19))
        Add_And_Set_Effect(L"Spike_100100_RunWind");
   
    if (m_iCurFrame >= 19)
    {   
       m_fEffectCreateTimer[0] += fDT;

        if (m_fEffectCreateTimer[0] >= 0.12f)
        {
            m_fEffectCreateTimer[0] = 0.f;
            Add_Effect(L"Spike_100100_IceFlower");
        }

        Get_Transform()->Go_Straight();
    }

    if (m_iCurFrame >= 16)
    {
        m_fAssaultColliderTimer += fDT;

        if (m_fAssaultColliderTimer >= 0.2f)
        {
            m_bAssaultColliderOn = true;
            m_fAssaultColliderTimer = 0.f;
            AttackCollider_On(NORMAL_ATTACK, 10.f);
            Set_ColliderOption(WATER, L"Hit_Slash_Blue");
            CAMERA_SHAKE(0.1f, 0.02f);
        }
        else
        {
            if (m_bAssaultColliderOn)
            {
                AttackCollider_Off();
                m_bAssaultColliderOn = false;
            }
        }

    }

    _float3 vInputVector = Get_InputDirVector();

    if (vInputVector != _float3(0.f))
        Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        //THIS LOGIC IS PERFECT -> DON'T TOUCH COMBO
        if (m_iCurFrame > 20)
        {
            if (KEYTAP(KEY_TYPE::KEY_1))
                m_bCanCombo = true;
        }

        if (m_bCanCombo)
        {
            m_eCurState = STATE::skill_100300;
        }
        
        Use_Dash();
    }

    if (Is_AnimFinished())
    {
        m_eCurState = STATE::skill_100300;
    }
}

void Spike_FSM::skill_100100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    Get_Transform()->Set_Speed(m_fRunSpeed * 1.2f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    m_fAssaultColliderTimer = 0.2f;
}

void Spike_FSM::skill_100300()
{
    if (Init_CurFrame(20))
    {
        Add_GroupEffectOwner(L"Spike_100100_IceAttack",_float3(0,0,1), false);
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
        Set_ColliderOption(WATER, L"Hit_Slash_Blue");
        CAMERA_SHAKE(0.2f, 0.2f);
    }
    if(Init_CurFrame(32))
        CAMERA_SHAKE(0.2f, 0.2f);
    
    if (Init_CurFrame(37))
        AttackCollider_Off();

    if (Is_AnimFinished())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::b_idle;
    }

    if (!g_bIsCanMouseMove && !g_bCutScene)
        Use_Dash();
}

void Spike_FSM::skill_100300_Init()
{
    FreeLoopMembers();

    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100300", 0.15f, false, m_fSkillAttack_AnimationSpeed);
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(SKILL1);

    m_bCanCombo = false;

    AttackCollider_Off();

    Get_Transform()->Set_Speed(m_fRunSpeed * 1.f);

    if (!m_pCamera.expired())
        m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedTime(0.f);
    

    m_bInvincible = false;
    m_bSuperArmor = true;

}

void Spike_FSM::skill_200100()
{
    if (Init_CurFrame(3))
	{
		Set_ColliderOption(WATER, L"Hit_Slash_Blue");
        Add_And_Set_Effect(L"Spike_200100L_1");
        CAMERA_SHAKE(0.2f, 0.3f)
    }

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        if (KEYAWAY(KEY_TYPE::KEY_2))
            m_eCurState = STATE::skill_200200;

        Use_Dash();
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::skill_200100_l;

}

void Spike_FSM::skill_200100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    Add_Effect(L"Spike_200100");

    m_bCanCombo = false;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Spike_FSM::skill_200100_l()
{
    CAMERA_SHAKE(0.1f, 0.02f)
    if (Init_CurFrame(0))
        Add_And_Set_Effect(L"Spike_200100L_floor");

  /*  m_fEffectCreateTimer[0] += fDT;
    m_fEffectCreateTimer[1] += fDT;
    if (m_fEffectCreateTimer[0] >= 0.4f)
    {
        m_fEffectCreateTimer[0] = 0.f;
        Add_Effect(L"Spike_200100L_Wind1");
    }
    if (m_fEffectCreateTimer[1] >= 0.1f)
    {
        m_fEffectCreateTimer[1] = 0.f;
        Add_Effect(L"Spike_200100L_Wind2");
    }*/

    _float3 vInputVector = Get_InputDirVector();

    m_fChargingRatio = _float(m_iCurFrame) / _float(Get_FinalFrame());

    CUR_SCENE->Get_UI(L"UI_Skill_Use_Gauge")->Get_Script<UiSkillGauge>()->Change_Ratio(m_fChargingRatio);

    if (KEYAWAY(KEY_TYPE::KEY_2))
    {
        if (m_fChargingRatio < 0.33f)
            m_eCurState = STATE::skill_200200;
        else if (m_fChargingRatio <= 0.66f)
            m_eCurState = STATE::skill_200300;
        else
            m_eCurState = STATE::skill_200400;

        FreeLoopMembers();
    }

    m_fWheelWindRange += fDT * 0.5f;
    m_fWheelWindSpeed += fDT;

    if (Is_AnimFinished())
    {
        m_eCurState = STATE::skill_200400;

        FreeLoopMembers();
    }

    if (!g_bIsCanMouseMove && !g_bCutScene)
        Use_Dash();
}

void Spike_FSM::skill_200100_l_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_CurrentAnim(L"skill_200100_l", false, m_fSkillAttack_AnimationSpeed);

	Add_And_Set_Effect(L"Spike_200100L_LoopWind");

    m_bCanCombo = false;

    m_vDirToTarget = _float3(0.f);
    m_vDirToTarget = Get_InputDirVector();

    CUR_SCENE->Get_UI(L"UI_Skill_Use_Gauge")->Get_Script<UiSkillGauge>()->Change_Render(true, SkillInfo::CHARGING);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Spike_FSM::skill_200200()
{

    m_fEffectCreateTimer[0] += fDT;
    
    if (Init_CurFrame(0))
        FreeLoopMembers();
    if (Init_CurFrame(5))
        Add_And_Set_Effect(L"Spike_200200_Slash");

    m_tWheelWindCoolTime.fAccTime += fDT;

    if (m_iCurFrame >= 5 && m_iCurFrame < 33)
    {
        if (m_fEffectCreateTimer[0] >= 0.1f)
        {
            m_fEffectCreateTimer[0] = 0.f;
            Add_Effect(L"Spike_200200_Particle1");
        }

        if (m_tWheelWindCoolTime.fAccTime >= m_tWheelWindCoolTime.fCoolTime)
        {
            m_tWheelWindCoolTime.fAccTime = 0.f;

            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 0.f;
            desc.fLifeTime = 0.3f;
            desc.fLimitDistance = 0.f;
            CAMERA_SHAKE(0.1f, 0.2f)
            Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", Get_Transform()->Get_State(Transform_State::POS) + _float3::Up, m_fWheelWindRange, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Blue");
        }
    }
    
    _float3 vInputVector = Get_InputDirVector();

    if (vInputVector != _float3(0.f))
        Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);

    if (Is_AnimFinished())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::b_idle;
    }

    if (!g_bIsCanMouseMove && !g_bCutScene)
        Use_Dash();
}

void Spike_FSM::skill_200200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200200", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    CUR_SCENE->Get_UI(L"UI_Skill_Use_Gauge")->Get_Script<UiSkillGauge>()->Change_Render(false);

    m_bCanCombo = false;

    m_vDirToTarget = _float3(0.f);
    m_vDirToTarget = Get_InputDirVector();

    m_fChargingRatio = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fRunSpeed + m_fWheelWindSpeed);
}

void Spike_FSM::skill_200300()
{
    m_fEffectCreateTimer[0] += fDT;
 
    if (Init_CurFrame(0))
        FreeLoopMembers();
    if (Init_CurFrame(5))
        Add_And_Set_Effect(L"Spike_200300_Slash");

    m_tWheelWindCoolTime.fAccTime += fDT;

    if (m_iCurFrame >= 5 && m_iCurFrame < 52)
    {
        if (m_fEffectCreateTimer[0] >= 0.1f)
        {
            m_fEffectCreateTimer[0] = 0.f;
            Add_Effect(L"Spike_200200_Particle1");
        }

        if (m_tWheelWindCoolTime.fAccTime >= m_tWheelWindCoolTime.fCoolTime)
        {
            m_tWheelWindCoolTime.fAccTime = 0.f;

            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 0.f;
            desc.fLifeTime = 0.3f;
            desc.fLimitDistance = 0.f;
            CAMERA_SHAKE(0.1f, 0.2f)
            Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", Get_Transform()->Get_State(Transform_State::POS) + _float3::Up, m_fWheelWindRange, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Blue");
        }
    }

    _float3 vInputVector = Get_InputDirVector();

    if (vInputVector != _float3(0.f))
        Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);

    if (Is_AnimFinished())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::b_idle;
    }

    if (!g_bIsCanMouseMove && !g_bCutScene)
        Use_Dash();
}

void Spike_FSM::skill_200300_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200300", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    CUR_SCENE->Get_UI(L"UI_Skill_Use_Gauge")->Get_Script<UiSkillGauge>()->Change_Render(false);

    m_bCanCombo = false;

    m_fChargingRatio = 0.f;

    m_vDirToTarget = _float3(0.f);
    m_vDirToTarget = Get_InputDirVector();

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fRunSpeed + m_fWheelWindSpeed);
}

void Spike_FSM::skill_200400()
{
    m_fEffectCreateTimer[0] += fDT;

    if (Init_CurFrame(0))
        FreeLoopMembers();
    if (Init_CurFrame(3))
        Add_And_Set_Effect(L"Spike_200400_Slash");
    if (Init_CurFrame(62))
        Add_GroupEffectOwner(L"Spike_200400_Crack", _float3(0.f, 0.f, 2.5f), false);
    
    if (m_iCurFrame < 44)
    {
		_float3 vInputVector = Get_InputDirVector();

		if (vInputVector != _float3(0.f))
			Soft_Turn_ToInputDir(vInputVector, XM_PI * 5.f);
    }

    m_tWheelWindCoolTime.fAccTime += fDT;

    if (m_iCurFrame >= 5 && m_iCurFrame < 44)
    {
        if (m_fEffectCreateTimer[0] >= 0.1f)
        {
            m_fEffectCreateTimer[0] = 0.f;
            Add_Effect(L"Spike_200200_Particle1");
        }

        if (m_tWheelWindCoolTime.fAccTime >= m_tWheelWindCoolTime.fCoolTime)
        {
            m_tWheelWindCoolTime.fAccTime = 0.f;

            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 0.f;
            desc.fLifeTime = 0.3f;
            desc.fLimitDistance = 0.f;
            CAMERA_SHAKE(0.1f, 0.2f)
            Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", Get_Transform()->Get_State(Transform_State::POS) + _float3::Up, m_fWheelWindRange, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Blue");
        }
    }

    if (Init_CurFrame(65))
    {
		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 2.f;

		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 0.5f;
		desc.fLimitDistance = 0.f;
        CAMERA_SHAKE(0.2f, 0.5f)
        Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 3.f, desc, AIRBORNE_ATTACK, 10.f, L"Hit_Slash_Blue");
    }

    if (Is_AnimFinished())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::b_idle;
    }

    if (!g_bIsCanMouseMove && !g_bCutScene)
        Use_Dash();
}

void Spike_FSM::skill_200400_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    CUR_SCENE->Get_UI(L"UI_Skill_Use_Gauge")->Get_Script<UiSkillGauge>()->Change_Render(false);

    animator->Set_NextTweenAnim(L"skill_200400", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    m_fChargingRatio = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fRunSpeed + m_fWheelWindSpeed);
}

void Spike_FSM::skill_300100()
{
    if (Init_CurFrame(1))
        Add_And_Set_Effect(L"Spike_300100_Jump");
    if (Init_CurFrame(30))
        Add_And_Set_Effect(L"Spike_300100");

    if (m_iCurFrame <= 50)
    {
        if (!m_pCamera.expired())
        {
            _float4 vDir = m_vSkillCamBonePos - m_vCenterBonePos;
            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.35f, vDir.xyz(), 12.f);
        }
    }
    else
    {
        if (!m_pCamera.expired())
        {
            _float4 vDir = m_vSkillCamBonePos - m_vCenterBonePos;
            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.35f, vDir.xyz(), 6.f);
        }
    }

    Calculate_CamBoneMatrix();

    if (Init_CurFrame(30))
	{
        _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
                            Get_Transform()->Get_State(Transform_State::LOOK) * 2.f;

		CAMERA_SHAKE(0.4f, 0.5f);
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 1.f;
		desc.fLimitDistance = 0.f;

		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 2.5f, desc, AIRBORNE_ATTACK, 10.f, L"Hit_Slash_Blue");

    }


    Look_DirToTarget();

    if (Is_AnimFinished())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::b_idle;
    }
}

void Spike_FSM::skill_300100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_300100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Spike_FSM::skill_400100()
{
    if (Init_CurFrame(52))
    {
        Add_And_Set_Effect(L"Spike_400100_1");
        
    }
    
    if (Init_CurFrame(52) || Init_CurFrame(77) || Init_CurFrame(97))
        CAMERA_SHAKE(0.2f, 0.3f);

    if (Init_CurFrame(150))
    {
        FreeLoopMembers();
        Add_And_Set_Effect(L"Spike_400100_2");
        Add_And_Set_Effect(L"Spike_400100_3");
        CAMERA_SHAKE(0.4f, 0.5f);
    }

	static _float fAngle = 0.f;
	static _float3 vUp = _float3(0.f,1.f,0.f);
    if (m_iCurFrame == 17)
    {
        if (!m_pCamera.expired())
        {

            _float4 vDestinationPos = (Get_Transform()->Get_State(Transform_State::POS)) + (Get_Transform()->Get_State(Transform_State::LOOK) * -5.f) + _float3::Up * 5.f;
            _float4 vDir = vDestinationPos - (Get_Transform()->Get_State(Transform_State::POS));
            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(2.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz() + _float3::Up);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(7.f, vDir.xyz(), 10.f);
        }
    }
    else if (m_iCurFrame >= 26)
    {
        if (m_iCurFrame == 26)
        {
            _float4x4 matWorld = Get_Transform()->Get_WorldMatrix();
            vUp = matWorld.Backward() + matWorld.Up() * 3.f;
            vUp.Normalize();
            fAngle = 0.f;
            m_vCamStopPos = m_vCenterBonePos + _float3::Up;
            
        }

        fAngle = 0.75f *fDT;
        m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCamStopPos.xyz());
        _float3 vDir = m_pCamera.lock()->Get_Script<MainCameraScript>()->Get_FixedDir();

        vDir = _float3::TransformNormal(vDir, _float4x4::CreateFromAxisAngle(vUp, fAngle));
        m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedDir(vDir);
    }


    Calculate_CamBoneMatrix();

    if (Init_CurFrame(52))
    {
        _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + 
                            Get_Transform()->Get_State(Transform_State::RIGHT) * 2.f +
                            Get_Transform()->Get_State(Transform_State::LOOK) * -1.f;
        
        FORWARDMOVINGSKILLDESC desc;
        desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
        desc.fMoveSpeed = 0.f;
        desc.fLifeTime = 1.f;
        desc.fLimitDistance = 0.f;

        Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 3.f, desc, AIRBORNE_ATTACK, 10.f, L"Hit_Slash_Blue");
    }
    else if (Init_CurFrame(77))
    {
        _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
                            Get_Transform()->Get_State(Transform_State::RIGHT) * -3.f +
                            Get_Transform()->Get_State(Transform_State::LOOK);

        FORWARDMOVINGSKILLDESC desc;
        desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
        desc.fMoveSpeed = 0.f;
        desc.fLifeTime = 1.f;
        desc.fLimitDistance = 0.f;

        Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 3.f, desc, AIRBORNE_ATTACK, 10.f, L"Hit_Slash_Blue");
    }
    else if (Init_CurFrame(97))
    {
        _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
                            Get_Transform()->Get_State(Transform_State::RIGHT) * -1.f +
                            Get_Transform()->Get_State(Transform_State::LOOK) * -4.f;

        FORWARDMOVINGSKILLDESC desc;
        desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
        desc.fMoveSpeed = 0.f;
        desc.fLifeTime = 1.f;
        desc.fLimitDistance = 0.f;

        Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 3.f, desc, AIRBORNE_ATTACK, 10.f, L"Hit_Slash_Blue");
    }
    else if (Init_CurFrame(150))
    {
        _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
                            Get_Transform()->Get_State(Transform_State::LOOK) * 2.f;

        FORWARDMOVINGSKILLDESC desc;
        desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
        desc.fMoveSpeed = 0.f;
        desc.fLifeTime = 1.f;
        desc.fLimitDistance = 0.f;

        Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 3.f, desc, AIRBORNE_ATTACK, 10.f, L"Hit_Slash_Blue");
    }

    if (Is_AnimFinished())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::b_idle;
    }
}

void Spike_FSM::skill_400100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_400100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;

    Calculate_CamBoneMatrix();
}

void Spike_FSM::skill_501100()
{
    if (Init_CurFrame(20))
    {
        
        Add_And_Set_Effect(L"Spike_500100_1");
        Add_GroupEffectOwner(L"Spike_500100_Floor", _float3(0.f, 0.f, 0.5f), false);
    }
    if(Init_CurFrame(22))
        CAMERA_SHAKE(0.1f, 0.4f)

    if (Init_CurFrame(34))
    {
        Add_GroupEffectOwner(L"Spike_500100_Floor2", _float3(0.f, 0.f, 2.f), false);
		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 3.f;

		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 1.f;
		desc.fLimitDistance = 0.f;

		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 3.f, desc, KNOCKDOWN_ATTACK, 10.f, L"Hit_Slash_Blue");
    }
    if (Init_CurFrame(36))
        CAMERA_SHAKE(0.2f, 0.4f)

    if (m_iCurFrame == 18)
    {
        Set_ColliderOption(WATER, L"Hit_Slash_Blue");
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    }
    else if (m_iCurFrame == 23)
        AttackCollider_Off();

    Look_DirToTarget();

    if (Is_AnimFinished())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::b_idle;
    }

    if (!g_bIsCanMouseMove && !g_bCutScene)
        Use_Dash();
}

void Spike_FSM::skill_501100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_501100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Spike_FSM::Use_Skill()
{
    if (KEYTAP(KEY_TYPE::KEY_1) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL1))
        m_eCurState = STATE::skill_100100;
    else if (KEYTAP(KEY_TYPE::KEY_2) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL2))
        m_eCurState = STATE::skill_200100;
    else if (KEYTAP(KEY_TYPE::KEY_3) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL3))
        m_eCurState = STATE::skill_300100;
    else if (KEYTAP(KEY_TYPE::KEY_4))
        m_eCurState = STATE::skill_400100;
    else if (KEYTAP(KEY_TYPE::KEY_5) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL5))
        m_eCurState = STATE::skill_501100;
    else
        Use_Dash();

}

void Spike_FSM::Use_Dash()
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
