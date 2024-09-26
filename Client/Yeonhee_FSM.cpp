#include "pch.h"
#include "MainCameraScript.h"
#include "Yeonhee_FSM.h"
#include "ModelAnimator.h"
#include "ModelAnimation.h"
#include "MotionTrailRenderer.h"
#include "Debug_CreateMotionTrail.h"
#include "SphereCollider.h"
#include "AttackColliderInfoScript.h"
#include "Model.h"
#include "CoolTimeCheckScript.h"
#include "SpearAce_Clone_FSM.h"
#include "ModelRenderer.h"
#include "UiSkillGauge.h"
#include "CharacterController.h"


Yeonhee_FSM::Yeonhee_FSM()
{
}

Yeonhee_FSM::~Yeonhee_FSM()
{
}

HRESULT Yeonhee_FSM::Init()
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
        m_iCamBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_Cam");
        m_iSkillCamBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_SkillCam");
        
        m_iHeadBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Bip001-Head");

        m_pCamera = CUR_SCENE->Get_MainCamera();

        m_fDetectRange = 15.f;

        m_bEntryTeam = true;

        m_bInitialize = true;
    }

	m_fNormalAttack_AnimationSpeed = 1.2f;
	m_fSkillAttack_AnimationSpeed = 1.0f;
	m_fEvade_AnimationSpeed = 1.5f;

    return S_OK;
}

void Yeonhee_FSM::Tick()
{
    State_Tick();

    if (!m_pAttackCollider.expired())
    {
        //m_pAttack transform set forward
		m_pAttackCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 1.5f + _float3::Up);
    }

  
}

void Yeonhee_FSM::State_Tick()
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
    case STATE::skill_91100:
        skill_91100();
        break;
    case STATE::skill_93100:
        skill_93100();
        break;
    case STATE::skill_100100:
        skill_100100();
        break;
	case STATE::skill_100100_e:
        skill_100100_e();
		break;
    case STATE::skill_200100:
        skill_200100();
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

    for (auto& iter : m_vGroupEffect)
    {
        if (!iter.expired())
            iter.lock()->Get_Transform()->Set_WorldMat(Get_Transform()->Get_WorldMatrix());
    }

    if (m_iPreFrame != m_iCurFrame)
        m_iPreFrame = m_iCurFrame;
}

void Yeonhee_FSM::State_Init()
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
        case STATE::skill_91100:
            skill_91100_Init();
            break;
        case STATE::skill_93100:
            skill_93100_Init();
            break;
        case STATE::skill_100100:
            skill_100100_Init();
            break;
		case STATE::skill_100100_e:
			skill_100100_e_Init();
			break;
        case STATE::skill_200100:
            skill_200100_Init();
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

void Yeonhee_FSM::Get_Hit(const wstring& skillname, _float fDamage,  shared_ptr<GameObject> pLookTarget)
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

void Yeonhee_FSM::Set_State(_uint iIndex)
{
}

void Yeonhee_FSM::b_idle()
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

void Yeonhee_FSM::b_idle_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_idle", 0.1f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;


    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Yeonhee_FSM::b_run_start()
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

void Yeonhee_FSM::b_run_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_start", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();
    
    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Yeonhee_FSM::b_run()
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

void Yeonhee_FSM::b_run_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Yeonhee_FSM::b_run_end_r()
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

void Yeonhee_FSM::b_run_end_r_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_end_r", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Yeonhee_FSM::b_run_end_l()
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

void Yeonhee_FSM::b_run_end_l_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_end_l", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Yeonhee_FSM::b_sprint()
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

void Yeonhee_FSM::b_sprint_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_sprint", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fSprintSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Yeonhee_FSM::b_walk()
{
}

void Yeonhee_FSM::b_walk_Init()
{
}

void Yeonhee_FSM::die()
{
}

void Yeonhee_FSM::die_Init()
{
}

void Yeonhee_FSM::airborne_start()
{
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::airborne_end;
}

void Yeonhee_FSM::airborne_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"airborne_start", 0.2f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_CharacterController()->Add_Velocity(6.f);
}

void Yeonhee_FSM::airborne_end()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::airborne_up;
}

void Yeonhee_FSM::airborne_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"airborne_end", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Yeonhee_FSM::airborne_up()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Yeonhee_FSM::airborne_up_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_Up", 0.1f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Yeonhee_FSM::hit()
{
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Yeonhee_FSM::hit_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"hit", 0.1f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Yeonhee_FSM::knock_start()
{
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_end;
}

void Yeonhee_FSM::knock_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_start", 0.1f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockBackSpeed * 0.8f);
}

void Yeonhee_FSM::knock_end()
{
    if (m_iCurFrame < 16)
        Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_end_loop;
}

void Yeonhee_FSM::knock_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockBackSpeed * 0.5f);
}

void Yeonhee_FSM::knock_end_loop()
{
    m_tKnockDownEndCoolTime.fAccTime += fDT;
    
    if (m_iCurFrame > Get_FinalFrame() / 2)
        m_eCurState = STATE::knock_up;
}

void Yeonhee_FSM::knock_end_loop_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end_loop", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Yeonhee_FSM::knock_end_hit()
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

void Yeonhee_FSM::knock_end_hit_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end_hit", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Yeonhee_FSM::knock_up()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Yeonhee_FSM::knock_up_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_Up", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    m_tKnockDownEndCoolTime.fAccTime = 0.f;

    Get_Transform()->Set_Speed(m_fRunSpeed);
}

void Yeonhee_FSM::knockdown_start()
{
    m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();

    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knockdown_end;
}

void Yeonhee_FSM::knockdown_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knockdown_start", 0.1f, false, 2.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockDownSpeed);
}

void Yeonhee_FSM::knockdown_end()
{
    if (m_iCurFrame < 16)
        Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_up;
}

void Yeonhee_FSM::knockdown_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_CurrentAnim(L"knockdown_end", false, 1.5f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockDownSpeed * 0.5f);
}

void Yeonhee_FSM::skill_1100()
{
    Set_DirToTargetOrInput(OBJ_MONSTER);

    Look_DirToTarget();

    if (Init_CurFrame(12))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = -Get_Transform()->Get_State(Transform_State::UP);
		desc.fMoveSpeed = 15.f;
		desc.fLifeTime = 0.2f;
		desc.fLimitDistance = 3.f;

		// Find Monster
		_float4 vSkillPos;
		if (!m_pTarget.expired())
		{
			vSkillPos = m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) + _float3::Up * 5.f;
		}
		else
		{
			if (!m_pLookingTarget.expired())
				vSkillPos = m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) + _float3::Up * 5.f;
			else
				vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 5 + _float3::Up * 5.f;
		}
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 1.f, desc, KNOCKBACK_ATTACK, 10.f);
	}

    _float3 vInputVector = Get_InputDirVector();
    Get_Transform()->Go_Dir(vInputVector* m_fRunSpeed * 0.3f * fDT);

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        if (Check_Combo(22, KEY_TYPE::LBUTTON))
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

void Yeonhee_FSM::skill_1100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_bCanCombo = false;

    if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Start_Attack_Button_Effect();

    m_bInvincible = false;
    m_bSuperArmor = false;
}
	

void Yeonhee_FSM::skill_1200()
{
    Set_DirToTargetOrInput(OBJ_MONSTER);

	Look_DirToTarget();

    if(Init_CurFrame(17))
    {
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = -Get_Transform()->Get_State(Transform_State::UP);
		desc.fMoveSpeed = 15.f;
		desc.fLifeTime = 0.2f;
		desc.fLimitDistance = 3.f;

		_float4 vSkillPos;
		if (!m_pTarget.expired())
		{
			vSkillPos = m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) + _float3::Up * 5.f;
		}
		else
		{
			if (!m_pLookingTarget.expired())
				vSkillPos = m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) + _float3::Up * 5.f;
			else
				vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 5 + _float3::Up * 5.f;
		}
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 1.f, desc, KNOCKBACK_ATTACK, 10.f);

    }

	_float3 vInputVector = Get_InputDirVector();
	Get_Transform()->Go_Dir(vInputVector * m_fRunSpeed * 0.3f * fDT);

    if (!g_bIsCanMouseMove && !g_bCutScene)
    {
        if (Check_Combo(23, KEY_TYPE::LBUTTON))
            m_eCurState = STATE::skill_1300;

        Use_Skill();
    }

    if (Is_AnimFinished())
    {
        if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
            m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		
        m_eCurState = STATE::b_idle;
    }

}

void Yeonhee_FSM::skill_1200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1200", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Start_Attack_Button_Effect();
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(DEFAULT);
    }

    m_bCanCombo = false;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Yeonhee_FSM::skill_1300()
{
    Set_DirToTargetOrInput(OBJ_MONSTER);

    Look_DirToTarget();

	if (Init_CurFrame(20))
	{
		FORWARDMOVINGSKILLDESC desc;
		desc.vSkillDir = -Get_Transform()->Get_State(Transform_State::UP);
		desc.fMoveSpeed = 15.f;
		desc.fLifeTime = 0.2f;
		desc.fLimitDistance = 3.f;

		// Find Monster
		_float4 vSkillPos;
		if (!m_pTarget.expired())
		{
			vSkillPos = m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) + _float3::Up * 5.f;
		}
		else
		{
			if (!m_pLookingTarget.expired())
				vSkillPos = m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) + _float3::Up * 5.f;
			else
				vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 5 + _float3::Up * 5.f;
		}
		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 1.f, desc, KNOCKBACK_ATTACK, 10.f);
	}
	
	_float3 vInputVector = Get_InputDirVector();
	Get_Transform()->Go_Dir(vInputVector * m_fRunSpeed * 0.3f * fDT);

    if (Is_AnimFinished())
    {
        if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
            m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		
        m_eCurState = STATE::b_idle;
    }

    if (!g_bIsCanMouseMove && !g_bCutScene)
	    Use_Skill();
}

void Yeonhee_FSM::skill_1300_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1300", 0.15f, false, m_fNormalAttack_AnimationSpeed);
    
    if (m_pOwner.lock()->Get_Script<CoolTimeCheckScript>())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Start_Attack_Button_Effect();
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Next_Combo(DEFAULT);
    }

    m_bCanCombo = false;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;

}

void Yeonhee_FSM::skill_91100()
{
    _float3 vInputVector = Get_InputDirVector();

    Look_DirToTarget();

    if (m_iCurFrame == 7)
        Get_Owner()->Get_Animator()->Set_RenderState(false);
    else if (m_iCurFrame == 16)
        Get_Owner()->Get_Animator()->Set_RenderState(true);

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Yeonhee_FSM::skill_91100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_91100", 0.15f, false, m_fEvade_AnimationSpeed);

    CUR_SCENE->Get_UI(L"UI_Skill_Use_Gauge")->Get_Script<UiSkillGauge>()->Change_Render(false);

    m_bCanCombo = false;

    m_vDirToTarget = Get_InputDirVector();

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Yeonhee_FSM::skill_93100()
{
    _float3 vInputVector = Get_InputDirVector();
    
    if (m_iCurFrame == 8)
        Get_Owner()->Get_Animator()->Set_RenderState(false);
    else if (m_iCurFrame == 16)
        Get_Owner()->Get_Animator()->Set_RenderState(true);

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Yeonhee_FSM::skill_93100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_93100", 0.15f, false, m_fEvade_AnimationSpeed);

    CUR_SCENE->Get_UI(L"UI_Skill_Use_Gauge")->Get_Script<UiSkillGauge>()->Change_Render(false);

    m_bCanCombo = false;
    
    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Yeonhee_FSM::skill_100100()
{
    Look_DirToTarget();

    if (m_iCurFrame > 20)
    {
        if (m_iPreFrame != m_iCurFrame)
        {
            m_fCurrentHoldingSkillTime -= m_fTimePerFrame;
            
            _float fHoldingRatio = 1.f - (m_fCurrentHoldingSkillTime / m_fMaxHoldingSkillTime);
         
            CUR_SCENE->Get_UI(L"UI_Skill_Use_Gauge")->Get_Script<UiSkillGauge>()->Change_Ratio(fHoldingRatio);
        }

        m_fKeyPushTimer += fDT;
        if (m_fKeyPushTimer >= 0.3f)
        {
            m_fKeyPushTimer = 0.f;
			FORWARDMOVINGSKILLDESC desc;
			desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
			desc.fMoveSpeed = 15.f;
			desc.fLifeTime = 0.2f;
			desc.fLimitDistance = 15.f;

			_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) + _float3::Up;
			Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 1.f, desc, KNOCKBACK_ATTACK, 10.f);
        }
    }
	
    if (Is_AnimFinished() || !(KEYPUSH(KEY_TYPE::KEY_1)))
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::skill_100100_e;
    }

    if (!g_bIsCanMouseMove && !g_bCutScene)
        Use_Dash();
}

void Yeonhee_FSM::skill_100100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    CUR_SCENE->Get_UI(L"UI_Skill_Use_Gauge")->Get_Script<UiSkillGauge>()->Change_Render(true, HOLDING);
    CUR_SCENE->Get_UI(L"UI_Skill_Use_Gauge")->Get_Script<UiSkillGauge>()->Change_Ratio(0.f);
    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
    m_fKeyPushTimer = 0.f;

    m_fTimePerFrame = 1.f / (Get_Owner()->Get_Model()->Get_AnimationByName(L"skill_100100")->frameRate *
                            Get_Owner()->Get_Model()->Get_AnimationByName(L"skill_100100")->speed);

    m_fMaxHoldingSkillTime = m_fTimePerFrame * 76.f;
    m_fCurrentHoldingSkillTime = m_fMaxHoldingSkillTime;
}

void Yeonhee_FSM::skill_100100_e()
{
	if (Is_AnimFinished())
	{
		m_eCurState = STATE::b_idle;
	}
}

void Yeonhee_FSM::skill_100100_e_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_100100_e", 0.15f, false, m_fSkillAttack_AnimationSpeed);

	m_bCanCombo = false;

    CUR_SCENE->Get_UI(L"UI_Skill_Use_Gauge")->Get_Script<UiSkillGauge>()->Change_Render(false);


	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = true;
}

void Yeonhee_FSM::skill_200100()
{
    Set_DirToTarget();

    Look_DirToTarget();

    if (Init_CurFrame(24))
    {
		INSTALLATIONSKILLDESC desc;
		desc.fAttackTickTime = 0.5f;
		desc.iLimitAttackCnt = 10;
		desc.strAttackType = NORMAL_SKILL;
		desc.strLastAttackType = KNOCKDOWN_SKILL;
		desc.fAttackDamage = 5.f;
		desc.fLastAttackDamage = 12.f;

		_float4 vSkillPos;
		if (!m_pLookingTarget.expired())
			vSkillPos = m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) + _float3::Up;

		else
			vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 5.f + _float3::Up;

		Create_InstallationSkillCollider(Player_Skill, L"Player_InstallationSkillCollider", vSkillPos, 3.f, desc);
    }

	_float3 vInputVector = Get_InputDirVector();
	vInputVector.Normalize();
	Get_Transform()->Go_Dir(vInputVector * m_fRunSpeed * 0.3f * fDT);


    if (Is_AnimFinished())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::b_idle;
    }

    if (!g_bIsCanMouseMove && !g_bCutScene)
        Use_Dash();
}

void Yeonhee_FSM::skill_200100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    //animator->Set_NextTweenAnim(L"skill_200100", 0.15f, false, m_fSkillAttack_AnimationSpeed);
    animator->Set_CurrentAnim(L"skill_200100", false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    AttackCollider_Off();

    Set_DirToTargetOrInput(OBJ_MONSTER);

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Yeonhee_FSM::skill_300100()
{
    Set_DirToTarget();

    Look_DirToTarget();

	if (Init_CurFrame(24))
	{
		FORWARDMOVINGSKILLDESC desc;

		_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::UP) * 10.f - Get_Transform()->Get_State(Transform_State::LOOK) * 2.f;

		_float4 vTargetPos;
		if (!m_pLookingTarget.expired())
			vTargetPos = m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS);
		else
			vTargetPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 10.f;

		desc.vSkillDir = vTargetPos - vSkillPos;
		desc.fMoveSpeed = 30.f;
		desc.fLifeTime = 1.f;
		desc.fLimitDistance = 30.f;

		Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 3.f, desc, KNOCKBACK_ATTACK, 10.f);
	}

	_float3 vInputVector = Get_InputDirVector();
	Get_Transform()->Go_Dir(vInputVector * m_fRunSpeed * 0.3f * fDT);

    if (Is_AnimFinished())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::b_idle;
    }
}

void Yeonhee_FSM::skill_300100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_300100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Set_DirToTargetOrInput(OBJ_MONSTER);
}

void Yeonhee_FSM::skill_400100()
{
    Set_DirToTarget();

    Look_DirToTarget();

	if (m_iCurFrame > 22 && m_iCurFrame < 72)
	{
		m_fKeyPushTimer += fDT;
		if (m_fKeyPushTimer >= 0.3f)
		{
			m_fKeyPushTimer = 0.f;
			FORWARDMOVINGSKILLDESC desc;
			desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
			desc.fMoveSpeed = 15.f;
			desc.fLifeTime = 0.2f;
			desc.fLimitDistance = 15.f;

			_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) + _float3::Up;
			Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 1.f, desc, KNOCKBACK_ATTACK, 10.f);

		}
	}

	if (Is_AnimFinished())
	{
		m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
		m_eCurState = STATE::b_idle;
	}

    if (!g_bIsCanMouseMove && !g_bCutScene)
	    Use_Dash();
}

void Yeonhee_FSM::skill_400100_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"skill_400100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

	m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

	AttackCollider_Off();

	m_bInvincible = false;
	m_bSuperArmor = true;
}

void Yeonhee_FSM::skill_501100()
{
	Look_DirToTarget();

    TIME.Set_TimeSlow(0.1f, 0.1f);

 

    if (m_iCurFrame < 97)
    {
        if (!m_pCamera.expired())
        {
            m_vHeadCamDir = m_vHeadBonePos.xyz() - m_vHeadCamPos.xyz();
            m_vHeadCamDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vHeadBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(1.f, m_vHeadCamDir * -1.f, 3.f);
        }
    }
    else
    {
        if (!m_pCamera.expired())
        {
            _float4 vDestinationPos = Get_Transform()->Get_State(Transform_State::POS) +
                (Get_Transform()->Get_State(Transform_State::LOOK) * -3.f)
                + _float3::Up * 3.f;
            _float4 vDir = vDestinationPos - m_vSkillCamBonePos;

            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(2.5f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vSkillCamBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.3f, vDir.xyz(), 7.f);
        }
    }

    Calculate_CamBoneMatrix();
    HeadBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iHeadBoneIndex) *
        _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

    if (Init_CurFrame(57))
    {
		_float4 vTargetPos;
		if (!m_pLookingTarget.expired())
			vTargetPos = m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS);
		else
			vTargetPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 10.f;

		INSTALLATIONSKILLDESC desc;
		desc.iLimitAttackCnt = 1;
		desc.strAttackType = KNOCKDOWN_SKILL;
		desc.strLastAttackType = KNOCKDOWN_SKILL;
		desc.bFirstAttack = false;
        desc.fAttackDamage = 5.f;
        desc.fLastAttackDamage = 5.f;

		_float fOffSetTime = 0.f;

		for (_uint i = 0; i < 5; i++)
		{
			desc.fAttackTickTime = 1.f + fOffSetTime;

			fOffSetTime += 0.3f;

			_float fOffSetX = ((rand() * 2 / _float(RAND_MAX) - 1));
			_float fOffSetZ = ((rand() * 2 / _float(RAND_MAX) - 1));

			_float4 vSkillPos = vTargetPos + _float4{ fOffSetX, 0.f, fOffSetZ, 0.f };

			Create_InstallationSkillCollider(Player_Skill, L"Player_InstallationSkillCollider", vSkillPos, 2.f, desc);
		}
	}

    if (Is_AnimFinished())
    {
        m_pOwner.lock()->Get_Script<CoolTimeCheckScript>()->Set_Skill_End();
        m_eCurState = STATE::b_idle;

        m_pOwner.lock()->Set_TimeSlowed(true);
    }

}

void Yeonhee_FSM::skill_501100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_501100", 0.15f, false, m_fSkillAttack_AnimationSpeed);//m_fSkillAttack_AnimationSpeed);

    m_bCanCombo = false;

    Set_DirToTargetOrInput(OBJ_MONSTER);

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = true;

    if (!m_pOwner.expired())
    {
        HeadBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iHeadBoneIndex) *
            _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

        m_vHeadBonePos = _float4{ HeadBoneMatrix.Translation().x, HeadBoneMatrix.Translation().y, HeadBoneMatrix.Translation().z , 1.f };
        m_vHeadCamPos = m_vHeadBonePos + (Get_Transform()->Get_State(Transform_State::LOOK) * 3.f) + _float4{ 0.f,-0.1f,0.f,0.f };
    
        m_pOwner.lock()->Set_TimeSlowed(false);
    }

    Calculate_CamBoneMatrix();


}

void Yeonhee_FSM::Use_Skill()
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

void Yeonhee_FSM::Use_Dash()
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
