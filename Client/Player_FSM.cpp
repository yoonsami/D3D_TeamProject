#include "pch.h"
#include "MainCameraScript.h"
#include "Player_FSM.h"
#include "ModelAnimator.h"
#include "MotionTrailRenderer.h"
#include "Debug_CreateMotionTrail.h"
#include "SphereCollider.h"
#include "AttackColliderInfoScript.h"
#include "Model.h"
#include "CoolTimeCheckScript.h"
#include "ModelRenderer.h"
#include "DistortionRenderer.h"
#include <Utils.h>
#include "CharacterController.h"
#include "Friend_FSM.h"
#include "WeaponScript.h"

Player_FSM::Player_FSM()
{
}

Player_FSM::~Player_FSM()
{
}

HRESULT Player_FSM::Init()
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
        m_iSkillBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"B_nose");

        m_pCamera = CUR_SCENE->Get_MainCamera();

        m_fDetectRange = 5.f;
	    m_fRunSpeed = 6.f;
	    m_fSprintSpeed = 8.f;

        m_bEntryTeam = true;

        m_bInitialize = true;  
    }


	m_fNormalAttack_AnimationSpeed = 1.5f;
	m_fSkillAttack_AnimationSpeed = 1.0f;
	m_fEvade_AnimationSpeed = 1.5f;

    return S_OK;
}

void Player_FSM::Tick()
{
    State_Tick();

    if (!m_pAttackCollider.expired())
    {
        //m_pAttack transform set forward
		m_pAttackCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 1.5f + _float3::Up);
    }

    if (KEYTAP(KEY_TYPE::DOWN))
    {
        shared_ptr<GameObject> obj = make_shared<GameObject>();
        obj->GetOrAddTransform()->Set_State(Transform_State::POS, Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::RIGHT) * 3.f);
        obj->GetOrAddTransform()->LookAt(Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 3.f);

        obj->Add_Component(make_shared<Friend_FSM>(HERO::SHANE));

        obj->Set_DrawShadow(true);
        obj->Set_VelocityMap(true);
        obj->Set_Name(L"Friend");
        obj->Get_FSM()->Init();

        EVENTMGR.Create_Object(obj);
    }
}

void Player_FSM::State_Tick()
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
	case STATE::skill_100200:
        skill_100200();
		break;
	case STATE::skill_100300:
        skill_100300();
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
	case STATE::skill_300200:
		skill_300200();
		break;
    }

    Update_GroupEffectWorldPos(Get_Owner()->Get_Transform()->Get_WorldMatrix());

    if (m_iPreFrame != m_iCurFrame)
        m_iPreFrame = m_iCurFrame;
}

void Player_FSM::State_Init()
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
		case STATE::skill_100300:
			skill_100300_Init();
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
		case STATE::skill_300200:
			skill_300200_Init();
			break;
        }
        m_ePreState = m_eCurState;
    }
}

void Player_FSM::Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget)
{
    //Calculate Damage 
    //m_pOwner.lock()->Get_Hurt(fDamage);
    m_pOwner.lock()->Get_Hurt(1.f);
	
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

void Player_FSM::Set_State(_uint iIndex)
{
}

void Player_FSM::b_idle()
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

void Player_FSM::b_idle_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_idle", 0.1f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Player_FSM::b_run_start()
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

void Player_FSM::b_run_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_start", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();
    
    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Player_FSM::b_run()
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

void Player_FSM::b_run_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run", 0.2f, true, 1.2f);

    Get_Transform()->Set_Speed(m_fRunSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Player_FSM::b_run_end_r()
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

void Player_FSM::b_run_end_r_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_end_r", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Player_FSM::b_run_end_l()
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

void Player_FSM::b_run_end_l_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_end_l", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Player_FSM::b_sprint()
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

void Player_FSM::b_sprint_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_sprint", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fSprintSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Player_FSM::b_walk()
{
}

void Player_FSM::b_walk_Init()
{
}

void Player_FSM::die()
{
}

void Player_FSM::die_Init()
{
}

void Player_FSM::airborne_start()
{
    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::airborne_end;
}

void Player_FSM::airborne_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"airborne_start", 0.2f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_CharacterController()->Add_Velocity(6.f);
}

void Player_FSM::airborne_end()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::airborne_up;
}

void Player_FSM::airborne_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"airborne_end", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Player_FSM::airborne_up()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Player_FSM::airborne_up_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_Up", 0.1f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Player_FSM::hit()
{
    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Player_FSM::hit_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"hit", 0.1f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Player_FSM::knock_start()
{
    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_end;
}

void Player_FSM::knock_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_start", 0.1f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockBackSpeed * 0.8f);
}

void Player_FSM::knock_end()
{
    if (m_iCurFrame < 16)
        Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_end_loop;
}

void Player_FSM::knock_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockBackSpeed * 0.5f);
}

void Player_FSM::knock_end_loop()
{
    m_tKnockDownEndCoolTime.fAccTime += fDT;
    
    if (m_iCurFrame > Get_FinalFrame() / 2)
        m_eCurState = STATE::knock_up;
}

void Player_FSM::knock_end_loop_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end_loop", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Player_FSM::knock_end_hit()
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

void Player_FSM::knock_end_hit_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end_hit", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Player_FSM::knock_up()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Player_FSM::knock_up_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_Up", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    m_tKnockDownEndCoolTime.fAccTime = 0.f;

    Get_Transform()->Set_Speed(m_fRunSpeed);
}

void Player_FSM::knockdown_start()
{
    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knockdown_end;
}

void Player_FSM::knockdown_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knockdown_start", 0.1f, false, 2.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockDownSpeed);
}

void Player_FSM::knockdown_end()
{
    if (m_iCurFrame < 16)
        Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_up;
}

void Player_FSM::knockdown_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_CurrentAnim(L"knockdown_end", false, 1.5f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockDownSpeed * 0.5f);
}

void Player_FSM::skill_1100()
{
    if(Init_CurFrame(9))
        Add_Effect(L"Teo_1100");

    if (m_iCurFrame == 9)
    {
        AttackCollider_On(NORMAL_ATTACK, _float(rand() % 10 + 1));
        Set_ColliderOption(DARK, L"Hit_Slash_Dark");
    }
    else if (m_iCurFrame == 13)
        AttackCollider_Off();

    Look_DirToTarget();

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        if(Check_Combo(24, KEY_TYPE::LBUTTON))
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

void Player_FSM::skill_1100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    Set_DirToTargetOrInput(OBJ_MONSTER);

    if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Start_Attack_Button_Effect();

    AttackCollider_Off();

    m_bCanCombo = false;
    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Player_FSM::skill_1200()
{
	if (Init_CurFrame(4))
		Add_Effect(L"Teo_1200");

    if (m_iCurFrame == 4)
    {
        Set_ColliderOption(DARK, L"Hit_Slash_Dark");
        AttackCollider_On(NORMAL_ATTACK, _float(rand() % 10 + 1));
    }

    else if (m_iCurFrame > 8)
        AttackCollider_Off();
    
    Look_DirToTarget();
    
    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        if(Check_Combo(13,KEY_TYPE::LBUTTON))
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

void Player_FSM::skill_1200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1200", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    Set_DirToTargetOrInput(OBJ_MONSTER);

    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Start_Attack_Button_Effect();
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(DEFAULT);

    AttackCollider_Off();

    m_bCanCombo = false;
    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Player_FSM::skill_1300()
{
    if (m_iCurFrame == 12)
	{
		Set_ColliderOption(DARK, L"Hit_Slash_Dark");
        AttackCollider_On(NORMAL_ATTACK, _float(rand() % 10 + 1));

        if (m_iPreFrame != m_iCurFrame)
            Add_Effect(L"Teo_1300");
    }
    else if (m_iCurFrame == 14)
        AttackCollider_Off();

    Look_DirToTarget();

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        if (Check_Combo(27, KEY_TYPE::LBUTTON))
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

void Player_FSM::skill_1300_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1300", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_bCanCombo = false;

    if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Start_Attack_Button_Effect();
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(DEFAULT);
    }

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Player_FSM::skill_1400()
{
    if (m_iCurFrame == 16)
    {
        Set_ColliderOption(DARK, L"Hit_Slash_Dark");

		AttackCollider_On(NORMAL_ATTACK, _float(rand() % 10 + 1));
    }
	else if (m_iCurFrame == 20)
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
    {
	    Use_Skill();
    }
}

void Player_FSM::skill_1400_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_1400", 0.15f, false, m_fNormalAttack_AnimationSpeed);

	m_bCanCombo = false;

    if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Start_Attack_Button_Effect();
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(DEFAULT);
    }

    Set_DirToTargetOrInput(OBJ_MONSTER);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = false;
}

void Player_FSM::skill_91100()
{
	if (m_vDirToTarget != _float3(0.f))
		Soft_Turn_ToInputDir(m_vDirToTarget, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Player_FSM::skill_91100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_91100", 0.15f, false, m_fEvade_AnimationSpeed);

    m_bCanCombo = false;

    AttackCollider_Off();
    m_vDirToTarget = Get_InputDirVector();
    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Player_FSM::skill_93100()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Player_FSM::skill_93100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_93100", 0.15f, false, m_fEvade_AnimationSpeed);

    m_bCanCombo = false;
    
    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Player_FSM::skill_100100()
{
    Look_DirToTarget();

    if(Init_CurFrame(38))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 50.f;
		desc.fLifeTime = 0.5f;
		desc.fLimitDistance = 25.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 1.f + _float3::Up;
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider",vSkillPos, 1.f, desc, NORMAL_SKILL, 10.f, L"Hit_Slash_Dark");
	}
    
    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        if(Check_Combo(40,KEY_TYPE::KEY_1))
            m_eCurState = STATE::skill_100200;

        Use_Dash();
    }

    if (Is_AnimFinished())
    {
        if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
            m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        
        m_eCurState = STATE::b_idle;
    }

   
}

void Player_FSM::skill_100100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Player_FSM::skill_100200()
{
    Look_DirToTarget();

	if (Init_CurFrame(8))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 0.5f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS);
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 5.f, desc, KNOCKBACK_ATTACK, 10.f, L"Hit_Slash_Dark");

	}
    else if (Init_CurFrame(26))
    {
        vector<shared_ptr<GameObject>> targetMonster;
        for (auto& obj : CUR_SCENE->Get_Objects())
        {
            if(obj->Get_ObjectGroup()!= OBJ_MONSTER)
                continue;

			if (!obj->Get_FSM())
				continue;

            _float3 vObjPos = obj->Get_Transform()->Get_State(Transform_State::POS).xyz();
            _float3 vMyPos = Get_Transform()->Get_State(Transform_State::POS).xyz();

            _float3 vDir = vObjPos - vMyPos;
            vDir.y = 0.f;

            if(vDir.LengthSquared() > PLAYER_SKILL1_DIST * PLAYER_SKILL1_DIST)
                continue;

            vDir.Normalize();

            _float3 vLook = Get_Transform()->Get_State(Transform_State::LOOK).xyz();
            vLook.Normalize();

            if (vDir.Dot(vLook) > cosf(PLAYER_SKILL1_ANGLE * 0.5f))
                obj->Get_FSM()->Get_Hit(KNOCKDOWN_ATTACK, 10.f,Get_Owner());
        }
    }

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
    	if (Check_Combo(27, KEY_TYPE::KEY_1))
    		m_eCurState = STATE::skill_100300;
	    
        Use_Dash();
    }
	
	if (Is_AnimFinished())
	{
        if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
		    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		
        m_eCurState = STATE::b_idle;
	}
}

void Player_FSM::skill_100200_Init()
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

void Player_FSM::skill_100300()
{
    Look_DirToTarget();

    if (m_iCurFrame >= 10 && m_iCurFrame <= 23)
    {
        if (m_iCurFrame == 10)
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
    else if (m_iCurFrame >= 24 && m_iCurFrame < 31)
    {
        if (!m_pCamera.expired())
        {
            _float4 vDir = m_vCamStopPos - m_vCenterBonePos;
            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.35f, vDir.xyz(), 7.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_SmoothReturn();
        }
    }

    Calculate_CamBoneMatrix();

	if (Init_CurFrame(29))
	{
		for (auto& obj : CUR_SCENE->Get_Objects())
		{
			if (obj->Get_ObjectGroup() != OBJ_MONSTER)
				continue;

			if (!obj->Get_FSM())
				continue;

			_float3 vObjPos = obj->Get_Transform()->Get_State(Transform_State::POS).xyz();
			_float3 vMyPos = Get_Transform()->Get_State(Transform_State::POS).xyz();

			_float3 vDir = vObjPos - vMyPos;
			vDir.y = 0.f;

			if (vDir.LengthSquared() > PLAYER_SKILL1_DIST * PLAYER_SKILL1_DIST)
				continue;

			vDir.Normalize();

			_float3 vLook = Get_Transform()->Get_State(Transform_State::LOOK).xyz();
			vLook.Normalize();

			if (vDir.Dot(vLook) > cosf(PLAYER_SKILL1_ANGLE * 0.5f))
                obj->Get_FSM()->Get_Hit(KNOCKDOWN_ATTACK, 10.f ,Get_Owner());
		}
	}

	if (Is_AnimFinished())
	{
        if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
		    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		
        m_eCurState = STATE::b_idle;
	}

}

void Player_FSM::skill_100300_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_100300", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(SKILL1);

	m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = true;
}

void Player_FSM::skill_200100()
{
    if (m_iCurFrame >= 15)
    {
        if (m_iCurFrame == 15)
            m_vCamStopPos = m_pCamera.lock()->Get_Transform()->Get_State(Transform_State::POS);

        if (!m_pCamera.expired())
        {
            _float4 vDir = m_vCamStopPos - m_vCenterBonePos;
            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.35f, vDir.xyz(), 10.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_SmoothReturn();
        }
    }

    Calculate_CamBoneMatrix();

	if (m_iCurFrame >= 30)
	{
        m_fSkillCreateTimer += fDT;
        if (m_fSkillCreateTimer >= 0.3f)
        {
            m_fSkillCreateTimer = 0.f;
			FORWARDMOVINGSKILLDESC desc;
			desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
			desc.fMoveSpeed = 0.f;
			desc.fLifeTime = 0.3f;
			desc.fLimitDistance = 0.f;

			_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS);
			Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 5.f, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Dark");

			Get_Owner()->Get_Animator()->Set_RenderState(false);
			m_pWeapon.lock()->Get_ModelRenderer()->Set_RenderState(false);
        }
	}

    Look_DirToTarget();

    if (Is_AnimFinished())
    {
        if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
            m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        
        m_eCurState = STATE::skill_200200;
    }

}

void Player_FSM::skill_200100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = true;
}

void Player_FSM::skill_200200()
{
    Look_DirToTarget();

    if (m_iCurFrame >= 5)
	{
		shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();
        animator->Set_RenderState(true);
        m_pWeapon.lock()->Get_ModelRenderer()->Set_RenderState(true);
     }

    if (Init_CurFrame(22))
    {
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 1.f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 2.f + _float3::Up;
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 3.f, desc, KNOCKBACK_SKILL, 10.f, L"Hit_Slash_Dark");
    }

    if (Is_AnimFinished())
    {
        if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
            m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        
        m_eCurState = STATE::b_idle;
    }
}

void Player_FSM::skill_200200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_CurrentAnim(L"skill_200200", false, m_fSkillAttack_AnimationSpeed);

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bCanCombo = false;
    m_bInvincible = true;
    m_bSuperArmor = true;
}

void Player_FSM::skill_300100()
{
    Look_DirToTarget();
    if (Init_CurFrame(14))
    {
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 1.f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS);
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 3.f, desc, KNOCKBACK_SKILL, 10.f, L"Hit_Slash_Dark");

    }

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
	    if (Check_Combo(44, KEY_TYPE::KEY_3))
            m_eCurState = STATE::skill_300200;
    }

  
    if (Is_AnimFinished())
    {
        if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
            m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        
        m_eCurState = STATE::b_idle;
    }
}

void Player_FSM::skill_300100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_300100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();
    
    m_bCanCombo = false;
    m_bInvincible = true;
    m_bSuperArmor = true;
}

void Player_FSM::skill_300200()
{
    Look_DirToTarget();

    if (m_iCurFrame < 91)
    {
        if (!m_pCamera.expired())
        {
            _float4 vDir = m_vCamStopPos - m_vCenterBonePos;
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
            _float4 vDir = m_vCamStopPos - m_vCenterBonePos;
            vDir.Normalize();
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.35f, vDir.xyz(), 4.5f);
        }
    }
    
    Calculate_CamBoneMatrix();

    if (m_iCurFrame >= 5 && m_iCurFrame < 57)
    {
        m_fSkillCreateTimer += fDT;
        if (m_fSkillCreateTimer >= 0.3f)
        {
            m_fSkillCreateTimer = 0.f;
            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 0.f;
            desc.fLifeTime = 0.3f;
            desc.fLimitDistance = 0.f;

            _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS);
            Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 5.f, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Dark");

        }
    }
    else if (m_iCurFrame >= 57 && m_iCurFrame < 75)
    {
        m_fSkillCreateTimer += fDT;
        if (m_fSkillCreateTimer >= 0.1f)
        {
            m_fSkillCreateTimer = 0.f;
            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 0.f;
            desc.fLifeTime = 0.1f;
            desc.fLimitDistance = 0.f;

            _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS);
            Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 5.f, desc, NORMAL_ATTACK, 10.f, L"Hit_Slash_Dark");

        }
    }
    else if (Init_CurFrame(101))
    {
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
		desc.fMoveSpeed = 0.f;
		desc.fLifeTime = 0.5f;
		desc.fLimitDistance = 0.f;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS);
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 3.f, desc, KNOCKBACK_SKILL, 10.f, L"Hit_Slash_Dark");

    }

    if (Is_AnimFinished())
    {
        if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
            m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        
        m_eCurState = STATE::b_idle;
    }

}

void Player_FSM::skill_300200_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_300200", 0.05f, false, m_fSkillAttack_AnimationSpeed);

	m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(SKILL3);

	AttackCollider_Off();

	m_bInvincible = true;
	m_bSuperArmor = true;

    m_vCamStopPos = m_pCamera.lock()->Get_Transform()->Get_State(Transform_State::POS);    

    Calculate_CamBoneMatrix();
}

void Player_FSM::Use_Skill()
{
    if (KEYTAP(KEY_TYPE::KEY_1) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL1))
    {
        m_eCurState = STATE::skill_100100;
    }
    else if (KEYTAP(KEY_TYPE::KEY_2) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL2))
    {
        m_eCurState = STATE::skill_200100;
    }
    else if (KEYTAP(KEY_TYPE::KEY_3) && m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->IsAvailable(SKILL3))
    {
        m_eCurState = STATE::skill_300100;
    }
    else
        Use_Dash();   
}

void Player_FSM::Use_Dash()
{
    if (!g_bIsCanMouseMove && !g_bCutScene)
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
}
