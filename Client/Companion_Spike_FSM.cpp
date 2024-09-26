#include "pch.h"
#include "MainCameraScript.h"
#include "Companion_Spike_FSM.h"
#include "ModelAnimator.h"
#include "MotionTrailRenderer.h"
#include "Debug_CreateMotionTrail.h"
#include "SphereCollider.h"
#include "AttackColliderInfoScript.h"
#include "Model.h"
#include "UiSkillGauge.h"
#include "GroupEffect.h"
#include "CharacterController.h"
#include "MathUtils.h"

Companion_Spike_FSM::Companion_Spike_FSM()
{
}

Companion_Spike_FSM::~Companion_Spike_FSM()
{
}

HRESULT Companion_Spike_FSM::Init()
{
    if (!m_bInitialize)
    {
        auto animator = Get_Owner()->Get_Animator();
        if (animator)
        {
            animator->Set_CurrentAnim(L"b_idle", true, 1.f);
            m_eCurState = STATE::n_idle;
        }

        shared_ptr<GameObject> attackCollider = make_shared<GameObject>();
        attackCollider->GetOrAddTransform();
        attackCollider->Add_Component(make_shared<SphereCollider>(1.5f));
        attackCollider->Get_Collider()->Set_CollisionGroup(Player_Attack);

        m_pAttackCollider = attackCollider;

        EVENTMGR.Create_Object(m_pAttackCollider.lock());
        m_pAttackCollider.lock()->Get_Collider()->Set_Activate(false);

        m_pAttackCollider.lock()->Add_Component(make_shared<AttackColliderInfoScript>());
        m_pAttackCollider.lock()->Set_Name(L"Companion_Spike_AttackCollider");
        m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_ColliderOwner(Get_Owner());

        m_pWeapon = CUR_SCENE->Get_GameObject(L"Companion_Weapon_Spike");

        m_fDetectRange = 30.f;

        m_bInitialize = true;
    }
    
    m_tRunEndDelay.fCoolTime = 0.5f;

    m_fNormalAttack_AnimationSpeed = 1.5f;
    m_fSkillAttack_AnimationSpeed = 1.0f;
    m_fEvade_AnimationSpeed = 1.5f;

    return S_OK;
}

void Companion_Spike_FSM::Tick()
{
    DeadCheck();

    State_Tick();

    if (!m_pAttackCollider.expired())
    {
        //m_pAttack transform set forward
        m_pAttackCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 1.5f + _float3::Up);
    }

}

void Companion_Spike_FSM::State_Tick()
{
    Detect_Target();
    
    Calculate_EvadeCool();

    State_Init();

    m_iCurFrame = Get_CurFrame();
    
    Recovery_Color();
    
    switch (m_eCurState)
    {
    case STATE::talk_01:
        talk_01();
        break;
    case STATE::n_idle:
        n_idle();
        break;
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
    case STATE::stun:
        stun();
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

void Companion_Spike_FSM::State_Init()
{
    if (m_eCurState != m_ePreState)
    {
        switch (m_eCurState)
        {
        case STATE::talk_01:
            talk_01_Init();
            break;
        case STATE::n_idle:
            n_idle_Init();
            break;
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
        case STATE::stun:
            stun_Init();
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

void Companion_Spike_FSM::Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget)
{
    if (!m_bSuperArmor)
    {
        if (m_bCanEvade)
        {
            if (rand() % 4 == 0)
                m_bEvade = true;
        }
        else
            m_bEvade = false;
    }
    else
        m_bEvade = false;


    if (!m_bEvade)
    {
        //Calculate Damage 
        m_pOwner.lock()->Get_Hurt(fDamage);
    }

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
            if (!m_bEvade)
            {
                if (m_eCurState == STATE::hit)
                    Reset_Frame();
                else if (m_eCurState == STATE::knock_end_hit)
                    Reset_Frame();
                else if (m_eCurState == STATE::knock_end_loop)
                    m_eCurState = STATE::knock_end_hit;
                else
                    m_eCurState = STATE::hit;
            }
            else
            {
                if (rand() % 3 == 0)
                    m_eCurState = STATE::skill_93100;
                else
                    m_eCurState = STATE::skill_91100;
            }
        }
    }
    else if (skillname == KNOCKBACK_ATTACK || skillname == KNOCKBACK_SKILL)
    {
        if (!m_bSuperArmor)
        {
            if (!m_bEvade)
            {
                if (m_eCurState == STATE::knock_end_hit)
                    Reset_Frame();
                else if (m_eCurState == STATE::knock_end_loop)
                    m_eCurState = STATE::knock_end_hit;
                else
                    m_eCurState = STATE::knock_start;
            }
            else
            {
                if (rand() % 3 == 0)
                    m_eCurState = STATE::skill_93100;
                else
                    m_eCurState = STATE::skill_91100;
            }
        }
    }
    else if (skillname == KNOCKDOWN_ATTACK || skillname == KNOCKDOWN_SKILL)
    {
        if (!m_bSuperArmor)
        {
            if (!m_bEvade)
            {
                if (m_eCurState == STATE::knock_end_hit)
                    Reset_Frame();
                else if (m_eCurState == STATE::knock_end_loop)
                    m_eCurState = STATE::knock_end_hit;
                else
                    m_eCurState = STATE::knockdown_start;
            }
            else
            {
                if (rand() % 3 == 0)
                    m_eCurState = STATE::skill_93100;
                else
                    m_eCurState = STATE::skill_91100;
            }
        }
    }
    else if (skillname == AIRBORNE_ATTACK || skillname == AIRBORNE_SKILL)
    {
        if (!m_bSuperArmor)
        {
            if (!m_bEvade)
            {
                if (m_eCurState == STATE::knock_end_hit)
                    Reset_Frame();
                else if (m_eCurState == STATE::knock_end_loop)
                    m_eCurState = STATE::knock_end_hit;
                else
                    m_eCurState = STATE::airborne_start;
            }
            else
            {
                if (rand() % 3 == 0)
                    m_eCurState = STATE::skill_93100;
                else
                    m_eCurState = STATE::skill_91100;
            }
        }
    }
}

void Companion_Spike_FSM::Set_State(_uint iIndex)
{
}

void Companion_Spike_FSM::talk_01()
{
    if (!m_pTarget.expired())
        Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);

    if (KEYTAP(KEY_TYPE::P)) //For. Debugging
    {
        m_bEntryTeam = true;
        m_eCurState = STATE::b_idle;
    }
}

void Companion_Spike_FSM::talk_01_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();
    
    //TODO talk_01
    animator->Set_NextTweenAnim(L"talk_01", 0.1f, true, 1.f);
}

void Companion_Spike_FSM::n_idle()
{
    if (Can_Interact())
    {
        if (KEYTAP(KEY_TYPE::P)) //For. Debugging
        {
            m_eCurState = STATE::talk_01;
        }
    }
    else
    {

    }
}

void Companion_Spike_FSM::n_idle_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_idle", 0.1f, true, 1.f);

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Companion_Spike_FSM::b_idle()
{
    //Monster Follow (Battle)
    if (!m_pLookingTarget.expired())
    {
        //Monster Follow
        Soft_Turn_ToTarget(m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);

        m_tAttackCoolTime.fAccTime += fDT;
        m_tSkillCoolTime.fAccTime += fDT;

        if (!m_bSetAttack)
        {
            if (m_tAttackCoolTime.fAccTime >= m_tAttackCoolTime.fCoolTime)
                Set_AttackSkill();
        }
        else
        {
            if (LookingTarget_In_AttackRange())
                m_eCurState = m_eCurSkillState;
            else
                m_eCurState = STATE::b_run_start;
        }
    }
    else  //Player Follow (Not Battle)
    {
        //Player Follow
        if (!m_pTarget.expired())
            Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);

        if (Go_Follow())
        {
            m_tFollowCheckTime.fAccTime += fDT;

            if (m_tFollowCheckTime.fAccTime >= m_tFollowCheckTime.fCoolTime)
            {
                m_tFollowCheckTime.fAccTime = 0.f;
                m_eCurState = STATE::b_run_start;
            }
        }
        else
        {
            m_tFollowCheckTime.fAccTime = 0.f;
        }
    }

    StunSetting();
}

void Companion_Spike_FSM::b_idle_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_idle", 0.1f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);

    m_tRunEndDelay.fAccTime = 0.f;
    m_tRunEndDelay.fCoolTime = 0.5f;
    m_tSprintCoolTime.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;

    m_fWheelWindRange = 2.f;
    m_fWheelWindSpeed = 6.f;

    m_fChargingRatio = 0.f;
}

void Companion_Spike_FSM::b_run_start()
{
    if (!m_pLookingTarget.expired())
    {
        Soft_Turn_ToTarget(m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);

        m_tAttackCoolTime.fAccTime += fDT;
        m_tSkillCoolTime.fAccTime += fDT;

        if (!m_bSetAttack)
        {
            if (m_tAttackCoolTime.fAccTime >= m_tAttackCoolTime.fCoolTime)
                Set_AttackSkill();
        }
        else
        {
            if (LookingTarget_In_AttackRange())
                m_eCurState = m_eCurSkillState;
        }
    }
    else
    {
        if (!m_pTarget.expired())
            Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);
    }

    Get_Transform()->Go_Straight();

    if (Is_AnimFinished())
        m_eCurState = STATE::b_run;

    StunSetting();
}

void Companion_Spike_FSM::b_run_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_start", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Spike_FSM::b_run()
{
    if (!m_pLookingTarget.expired())
    {
        Soft_Turn_ToTarget(m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);

        Get_Transform()->Go_Straight();

        m_tSprintCoolTime.fAccTime += fDT;

        if (m_tSprintCoolTime.fAccTime >= m_tSprintCoolTime.fCoolTime)
            m_bSprint = true;

        if (m_bSprint)
        {
            if (m_iCurFrame == 1)
            {
                m_bSprint = false;
                m_eCurState = STATE::b_sprint;
            }
        }

        m_tAttackCoolTime.fAccTime += fDT;
        m_tSkillCoolTime.fAccTime += fDT;

        if (!m_bSetAttack)
        {
            if (m_tAttackCoolTime.fAccTime >= m_tAttackCoolTime.fCoolTime)
                Set_AttackSkill();
        }
        else
        {
            if (LookingTarget_In_AttackRange())
                m_eCurState = m_eCurSkillState;
        }
    }
    else
    {
        if (!m_pTarget.expired())
            Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);
    
        Get_Transform()->Go_Straight();

        m_tSprintCoolTime.fAccTime += fDT;

        if (!Go_Follow())
        {
            m_tRunEndDelay.fAccTime += fDT;

            if (m_tRunEndDelay.fAccTime >= m_tRunEndDelay.fCoolTime)
            {
                m_tRunEndDelay.fAccTime = 0.f;

                if (m_iCurFrame % 2 == 0)
                    m_eCurState = STATE::b_run_end_r;
                else
                    m_eCurState = STATE::b_run_end_l;
            }
        }
        else
        {
            if (m_tSprintCoolTime.fAccTime >= m_tSprintCoolTime.fCoolTime)
                m_bSprint = true;

            if (m_bSprint)
            {
                if (m_iCurFrame == 1)
                {
                    m_bSprint = false;
                    m_eCurState = STATE::b_sprint;
                }
            }
        }
    }

    StunSetting();
}

void Companion_Spike_FSM::b_run_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Spike_FSM::b_run_end_r()
{
    if (!m_pLookingTarget.expired())
    {
        Soft_Turn_ToTarget(m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);

        m_tAttackCoolTime.fAccTime += fDT;
        m_tSkillCoolTime.fAccTime += fDT;

        if (!m_bSetAttack)
        {
            if (m_tAttackCoolTime.fAccTime >= m_tAttackCoolTime.fCoolTime)
                Set_AttackSkill();
        }
        else
        {
            if (LookingTarget_In_AttackRange())
                m_eCurState = m_eCurSkillState;
        }
    }
    else
    {
        if (!m_pTarget.expired())
            Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;

    StunSetting();
}

void Companion_Spike_FSM::b_run_end_r_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_end_r", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Spike_FSM::b_run_end_l()
{
    if (!m_pLookingTarget.expired())
    {
        Soft_Turn_ToTarget(m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);

        m_tAttackCoolTime.fAccTime += fDT;
        m_tSkillCoolTime.fAccTime += fDT;

        if (!m_bSetAttack)
        {
            if (m_tAttackCoolTime.fAccTime >= m_tAttackCoolTime.fCoolTime)
                Set_AttackSkill();
        }
        else
        {
            if (LookingTarget_In_AttackRange())
                m_eCurState = m_eCurSkillState;
        }
    }
    else
    {
        if (!m_pTarget.expired())
            Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;

    StunSetting();
}

void Companion_Spike_FSM::b_run_end_l_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_end_l", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Spike_FSM::b_sprint()
{
    if (!m_pLookingTarget.expired())
    {
        Soft_Turn_ToTarget(m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);

        Get_Transform()->Go_Straight();

        m_tAttackCoolTime.fAccTime += fDT;
        m_tSkillCoolTime.fAccTime += fDT;

        if (!m_bSetAttack)
        {
            if (m_tAttackCoolTime.fAccTime >= m_tAttackCoolTime.fCoolTime)
                Set_AttackSkill();
        }
        else
        {
            if (LookingTarget_In_AttackRange())
                m_eCurState = m_eCurSkillState;
        }
    }
    else
    {
        if (!m_pTarget.expired())
            Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);

        Get_Transform()->Go_Straight();

        if (!Go_Follow())
        {
            m_tRunEndDelay.fAccTime += fDT;

            if (m_tRunEndDelay.fAccTime >= m_tRunEndDelay.fCoolTime)
            {
                m_tRunEndDelay.fAccTime = 0.f;
                m_tRunEndDelay.fCoolTime = 0.5f;
                if (m_iCurFrame % 2 == 0)
                    m_eCurState = STATE::b_run_end_r;
                else
                    m_eCurState = STATE::b_run_end_l;
            }
        }
    }

    StunSetting();
}

void Companion_Spike_FSM::b_sprint_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_sprint", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fSprintSpeed);

    m_tRunEndDelay.fCoolTime = 0.3f;
    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Spike_FSM::b_walk()
{
}

void Companion_Spike_FSM::b_walk_Init()
{
}

void Companion_Spike_FSM::die()
{
}

void Companion_Spike_FSM::die_Init()
{
}

void Companion_Spike_FSM::stun()
{
    if (Is_AnimFinished())
    {
        m_pOwner.lock()->Set_Hp(m_pOwner.lock()->Get_MaxHp());
        m_eCurState = STATE::b_idle;
    }
}

void Companion_Spike_FSM::stun_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"stun", 0.2f, false, 0.2f);

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = true;

    FreeLoopMembers();
}

void Companion_Spike_FSM::airborne_start()
{
    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::airborne_end;
}

void Companion_Spike_FSM::airborne_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"airborne_start", 0.2f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_CharacterController()->Add_Velocity(6.f);
}

void Companion_Spike_FSM::airborne_end()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::airborne_up;
}

void Companion_Spike_FSM::airborne_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"airborne_end", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Companion_Spike_FSM::airborne_up()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Companion_Spike_FSM::airborne_up_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_Up", 0.1f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Companion_Spike_FSM::hit()
{
    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Companion_Spike_FSM::hit_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"hit", 0.1f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Spike_FSM::knock_start()
{
    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_end;
}

void Companion_Spike_FSM::knock_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_start", 0.1f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockBackSpeed * 0.8f);
}

void Companion_Spike_FSM::knock_end()
{
    if (m_iCurFrame < 16)
        Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_end_loop;
}

void Companion_Spike_FSM::knock_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockBackSpeed * 0.5f);
}

void Companion_Spike_FSM::knock_end_loop()
{
    m_tKnockDownEndCoolTime.fAccTime += fDT;

    if (m_iCurFrame > Get_FinalFrame() / 2)
        m_eCurState = STATE::knock_up;
}

void Companion_Spike_FSM::knock_end_loop_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end_loop", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Spike_FSM::knock_end_hit()
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

void Companion_Spike_FSM::knock_end_hit_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end_hit", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Spike_FSM::knock_up()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Companion_Spike_FSM::knock_up_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_Up", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    m_tKnockDownEndCoolTime.fAccTime = 0.f;

    Get_Transform()->Set_Speed(m_fRunSpeed);
}

void Companion_Spike_FSM::knockdown_start()
{
    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knockdown_end;
}

void Companion_Spike_FSM::knockdown_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knockdown_start", 0.1f, false, 2.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockDownSpeed);
}

void Companion_Spike_FSM::knockdown_end()
{
    if (m_iCurFrame < 16)
        Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_up;
}

void Companion_Spike_FSM::knockdown_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_CurrentAnim(L"knockdown_end", false, 1.5f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockDownSpeed * 0.5f);
}

void Companion_Spike_FSM::skill_1100()
{
    if (m_iCurFrame < 11)
        Look_DirToTarget();

    if (Init_CurFrame(11))
    {
        Add_And_Set_Effect(L"Spike_1100");
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    }
    else if (m_iCurFrame == 17)
        AttackCollider_Off();

    if (m_iCurFrame >= 24)
        m_eCurState = STATE::skill_1200;

    StunSetting();
}

void Companion_Spike_FSM::skill_1100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_bInvincible = false;
    m_bSuperArmor = false;
    m_bSetAttack = false;
}

void Companion_Spike_FSM::skill_1200()
{
    if (Init_CurFrame(0))
    {
        Add_And_Set_Effect(L"Spike_1200");
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    }
    else if (m_iCurFrame == 14)
        AttackCollider_Off();

    if (m_iCurFrame >= 33)
        m_eCurState = STATE::skill_1300;

    StunSetting();
}

void Companion_Spike_FSM::skill_1200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1200", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Spike_FSM::skill_1300()
{
    if (m_iCurFrame < 15)
        Look_DirToTarget();

    if (Init_CurFrame(15))
    {
        Add_And_Set_Effect(L"Spike_1300");
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    }
    else if (m_iCurFrame == 18)
        AttackCollider_Off();

    if (m_iCurFrame >= 35)
        m_eCurState = STATE::skill_1400;

    StunSetting();
}

void Companion_Spike_FSM::skill_1300_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1300", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Spike_FSM::skill_1400()
{
    if (Init_CurFrame(7))
    {
        Add_And_Set_Effect(L"Spike_1400_1");
    }
    if (Init_CurFrame(11))
    {
        Add_GroupEffectOwner(L"Spike_1400_2", _float3(0.f, 0.f, 1.f), false);
        AttackCollider_On(KNOCKDOWN_ATTACK, 10.f);

    }
    else if (m_iCurFrame == 15)
        AttackCollider_Off();

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;

    StunSetting();
}

void Companion_Spike_FSM::skill_1400_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1400", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Spike_FSM::skill_91100()
{
    if (m_vEvadeVector != _float3(0.f))
        Soft_Turn_ToInputDir(m_vEvadeVector, m_fTurnSpeed);

    if (Is_AnimFinished())
    {
        m_bCanEvade = false;
        m_eCurState = STATE::b_idle;
    }
}

void Companion_Spike_FSM::skill_91100_Init()
{
    m_vEvadeVector = MathUtils::Get_RandomVector(_float3{ -1.f,0.f,-1.f }, _float3{ 1.f,0.f,1.f });
    m_vEvadeVector.Normalize();

    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_91100", 0.15f, false, m_fEvade_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Companion_Spike_FSM::skill_93100()
{
    if (Is_AnimFinished())
    {
        m_bCanEvade = false;
        m_eCurState = STATE::b_idle;
    }
}

void Companion_Spike_FSM::skill_93100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_93100", 0.15f, false, m_fEvade_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Companion_Spike_FSM::skill_100100()
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

    Look_DirToTarget();

    if (Is_AnimFinished())
    {
        m_eCurState = STATE::skill_100300;
    }

    StunSetting();
}

void Companion_Spike_FSM::skill_100100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100100", 0.15f, false, m_fSkillAttack_AnimationSpeed);


    Get_Transform()->Set_Speed(m_fRunSpeed * 1.2f);

    AttackCollider_Off();
    m_bSetAttack = false;
    m_bInvincible = false;
    m_bSuperArmor = true;

    m_fAssaultColliderTimer = 0.2f;
}

void Companion_Spike_FSM::skill_100300()
{
    if (m_iCurFrame < 20)
        Look_DirToTarget();

    if (Init_CurFrame(20))
    {
        Add_GroupEffectOwner(L"Spike_100100_IceAttack", _float3(0, 0, 1), false);
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    }

    if (Init_CurFrame(37))
        AttackCollider_Off();

    
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;

    StunSetting();
}

void Companion_Spike_FSM::skill_100300_Init()
{
    FreeLoopMembers();

    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100300", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    AttackCollider_Off();

    Get_Transform()->Set_Speed(m_fRunSpeed * 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Companion_Spike_FSM::skill_200100()
{
    if (Init_CurFrame(3))
    {
        Add_And_Set_Effect(L"Spike_200100L_1");
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::skill_200100_l;

    StunSetting();
}

void Companion_Spike_FSM::skill_200100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    Add_Effect(L"Spike_200100");
    
    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
    m_bSetAttack = false;
}

void Companion_Spike_FSM::skill_200100_l()
{   
    if (Init_CurFrame(0))
        Add_And_Set_Effect(L"Spike_200100L_floor");

    m_fWheelWindRange += fDT * 0.5f;
    m_fWheelWindSpeed += fDT;

    m_fChargingRatio = _float(m_iCurFrame) / _float(Get_FinalFrame());

    if (m_iWheelWindType == 0)
    {
        if (m_fChargingRatio >= 0.33f)
            m_eCurState = STATE::skill_200200;

        FreeLoopMembers();
    }
    else  if (m_iWheelWindType == 1)
    {
        if (m_fChargingRatio >= 0.66f)
            m_eCurState = STATE::skill_200300;

        FreeLoopMembers();
    }
 
    if (Is_AnimFinished())
    {
        m_eCurState = STATE::skill_200400;

        FreeLoopMembers();
    }

    StunSetting();
}

void Companion_Spike_FSM::skill_200100_l_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_CurrentAnim(L"skill_200100_l", false, m_fSkillAttack_AnimationSpeed);

    Add_And_Set_Effect(L"Spike_200100L_LoopWind");

    AttackCollider_Off();

    m_iWheelWindType = rand() % 3;

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Companion_Spike_FSM::skill_200200()
{
    m_fEffectCreateTimer[0] += fDT;

    if (Init_CurFrame(0))
        FreeLoopMembers();
    if (Init_CurFrame(5))
        Add_And_Set_Effect(L"Spike_200200_Slash");

    m_tWheelWindCoolTime.fAccTime += fDT;

    Look_DirToTarget();

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
            Create_ForwardMovingSkillCollider(Player_Skill, L"Companion_Spike_SkillCollider", Get_Transform()->Get_State(Transform_State::POS) + _float3::Up, m_fWheelWindRange, desc, NORMAL_ATTACK, 10.f);
        }
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;

    StunSetting();
}

void Companion_Spike_FSM::skill_200200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200200", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fRunSpeed + m_fWheelWindSpeed);
}

void Companion_Spike_FSM::skill_200300()
{
    m_fEffectCreateTimer[0] += fDT;

    if (Init_CurFrame(0))
        FreeLoopMembers();
    if (Init_CurFrame(5))
        Add_And_Set_Effect(L"Spike_200300_Slash");

    m_tWheelWindCoolTime.fAccTime += fDT;

    Look_DirToTarget();

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
                Create_ForwardMovingSkillCollider(Player_Skill, L"Companion_Spike_SkillCollider", Get_Transform()->Get_State(Transform_State::POS) + _float3::Up, m_fWheelWindRange, desc, NORMAL_ATTACK, 10.f);
        }
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;

    StunSetting();
}

void Companion_Spike_FSM::skill_200300_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200300", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fRunSpeed + m_fWheelWindSpeed);
}

void Companion_Spike_FSM::skill_200400()
{
    m_fEffectCreateTimer[0] += fDT;

    if (Init_CurFrame(0))
        FreeLoopMembers();
    if (Init_CurFrame(3))
        Add_And_Set_Effect(L"Spike_200400_Slash");
    if (Init_CurFrame(62))
        Add_GroupEffectOwner(L"Spike_200400_Crack", _float3(0.f, 0.f, 2.5f), false);

    Look_DirToTarget();

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
            Create_ForwardMovingSkillCollider(Player_Skill, L"Companion_Spike_SkillCollider", Get_Transform()->Get_State(Transform_State::POS) + _float3::Up, m_fWheelWindRange, desc, NORMAL_ATTACK, 10.f);
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
        Create_ForwardMovingSkillCollider(Player_Skill, L"Companion_Spike_SkillCollider", vSkillPos, 3.f, desc, AIRBORNE_ATTACK, 10.f);
    }

    if (Is_AnimFinished())
    {
        m_eCurState = STATE::b_idle;
    }

    StunSetting();
}

void Companion_Spike_FSM::skill_200400_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();
    
    animator->Set_NextTweenAnim(L"skill_200400", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fRunSpeed + m_fWheelWindSpeed);
}

void Companion_Spike_FSM::skill_300100()
{
    if (Init_CurFrame(1))
        Add_And_Set_Effect(L"Spike_300100_Jump");
    if (Init_CurFrame(30))
        Add_And_Set_Effect(L"Spike_300100");

    if (Init_CurFrame(30))
    {
        _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
                            Get_Transform()->Get_State(Transform_State::LOOK) * 2.f;

        FORWARDMOVINGSKILLDESC desc;
        desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
        desc.fMoveSpeed = 0.f;
        desc.fLifeTime = 1.f;
        desc.fLimitDistance = 0.f;

        Create_ForwardMovingSkillCollider(Player_Skill, L"Companion_Spike_SkillCollider", vSkillPos, 2.5f, desc, AIRBORNE_ATTACK, 10.f);

    }

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;    
}

void Companion_Spike_FSM::skill_300100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_300100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
    m_bSetAttack = false;
}

void Companion_Spike_FSM::skill_400100()
{
    if (Init_CurFrame(52))
        Add_And_Set_Effect(L"Spike_400100_1");

    if (Init_CurFrame(150))
    {
        FreeLoopMembers();
        Add_And_Set_Effect(L"Spike_400100_2");
        Add_And_Set_Effect(L"Spike_400100_3");
    }

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

        Create_ForwardMovingSkillCollider(Player_Skill, L"Companion_Spike_SkillCollider", vSkillPos, 3.f, desc, AIRBORNE_ATTACK, 10.f);
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

        Create_ForwardMovingSkillCollider(Player_Skill, L"Companion_Spike_SkillCollider", vSkillPos, 3.f, desc, AIRBORNE_ATTACK, 10.f);
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

        Create_ForwardMovingSkillCollider(Player_Skill, L"Companion_Spike_SkillCollider", vSkillPos, 3.f, desc, AIRBORNE_ATTACK, 10.f);
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

        Create_ForwardMovingSkillCollider(Player_Skill, L"Companion_Spike_SkillCollider", vSkillPos, 3.f, desc, AIRBORNE_ATTACK, 10.f);
    }

    if (Is_AnimFinished())
    {
        m_eCurState = STATE::b_idle;
    }
}

void Companion_Spike_FSM::skill_400100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_400100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
    m_bSetAttack = false;
}

void Companion_Spike_FSM::skill_501100()
{
    if (m_iCurFrame < 20)
        Look_DirToTarget();

    if (Init_CurFrame(20))
    {
        Add_And_Set_Effect(L"Spike_500100_1");
        Add_GroupEffectOwner(L"Spike_500100_Floor", _float3(0.f, 0.f, 0.5f), false);
    }

    if (Init_CurFrame(34))
    {
        Add_GroupEffectOwner(L"Spike_500100_Floor2", _float3(0.f, 0.f, 2.f), false);
        _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 3.f;

        FORWARDMOVINGSKILLDESC desc;
        desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
        desc.fMoveSpeed = 0.f;
        desc.fLifeTime = 1.f;
        desc.fLimitDistance = 0.f;

        Create_ForwardMovingSkillCollider(Player_Skill, L"Companion_Spike_SkillCollider", vSkillPos, 3.f, desc, KNOCKDOWN_ATTACK, 10.f);
    }
    
    if (m_iCurFrame == 18)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 23)
        AttackCollider_Off();

    if (Is_AnimFinished())    
        m_eCurState = STATE::b_idle;

    StunSetting();
}

void Companion_Spike_FSM::skill_501100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_501100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
    m_bSetAttack = false;
}

void Companion_Spike_FSM::Set_AttackSkill()
{
    m_eCurSkillState = STATE::skill_1100;
    m_fAttackRange = 2.f;
    m_bSetAttack = true;
    m_tAttackCoolTime.fAccTime = 0.f;

    if (m_tSkillCoolTime.fAccTime >= m_tSkillCoolTime.fCoolTime)
    {
        m_tSkillCoolTime.fAccTime = 0.f;

        _uint iRan = rand() % 5;

        while (true)
        {
            if (iRan == m_iPreAttack)
                iRan = rand() % 5;
            else
                break;
        }

        if (iRan == 0)
        {
            m_fAttackRange = 2.f;
            m_eCurSkillState = STATE::skill_100100;
            m_iPreAttack = 0;
        }
        else if (iRan == 1)
        {
            m_fAttackRange = 2.f;
            m_eCurSkillState = STATE::skill_200100;
            m_iPreAttack = 1;
        }
        else if (iRan == 2)
        {
            m_fAttackRange = 5.f;
            m_eCurSkillState = STATE::skill_300100;
            m_iPreAttack = 2;
        }
        else if (iRan == 3)
        {
            m_fAttackRange = 5.f;
            m_eCurSkillState = STATE::skill_400100;
            m_iPreAttack = 3;
        }
        else if (iRan == 4)
        {
            m_fAttackRange = 5.f;
            m_eCurSkillState = STATE::skill_501100;
            m_iPreAttack = 4;
        }
    }

}

void Companion_Spike_FSM::StunSetting()
{
    if (m_bIsDead)
    {
        m_bInvincible = true;
        m_eCurState = STATE::stun;
    }
}
