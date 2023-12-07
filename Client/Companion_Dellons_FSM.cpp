#include "pch.h"
#include "MainCameraScript.h"
#include "Companion_Dellons_FSM.h"
#include "ModelAnimator.h"
#include "MotionTrailRenderer.h"
#include "Debug_CreateMotionTrail.h"
#include "SphereCollider.h"
#include "AttackColliderInfoScript.h"
#include "Model.h"
#include "Camera.h"
#include "DellonsWraith_FSM.h"
#include "CharacterController.h"
#include "MathUtils.h"


Companion_Dellons_FSM::Companion_Dellons_FSM()
{
}

Companion_Dellons_FSM::~Companion_Dellons_FSM()
{
}

HRESULT Companion_Dellons_FSM::Init()
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
        attackCollider->Add_Component(make_shared<SphereCollider>(1.f));
        attackCollider->Get_Collider()->Set_CollisionGroup(Companion_Attack);

        m_pAttackCollider = attackCollider;

        EVENTMGR.Create_Object(m_pAttackCollider.lock());
        m_pAttackCollider.lock()->Get_Collider()->Set_Activate(false);

        m_pAttackCollider.lock()->Add_Component(make_shared<AttackColliderInfoScript>());
        m_pAttackCollider.lock()->Set_Name(L"Companion_Dellons_AttackCollider");
        m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_ColliderOwner(Get_Owner());

        m_pWeapon = CUR_SCENE->Get_GameObject(L"Companion_Weapon_Dellons");

        m_fDetectRange = 10.f;

        m_bInitialize = true;
    }

    m_tRunEndDelay.fCoolTime = 0.5f;

    m_fNormalAttack_AnimationSpeed = 1.2f;
    m_fSkillAttack_AnimationSpeed = 1.0f;
    m_fEvade_AnimationSpeed = 1.5f;
    return S_OK;
}

void Companion_Dellons_FSM::Tick()
{
    Detect_Target();

    State_Tick();

    if (!m_pAttackCollider.expired())
    {
        //m_pAttack transform set forward
        m_pAttackCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 1.5f + _float3::Up);
    }
}

void Companion_Dellons_FSM::State_Tick()
{
    Detect_Target();

    State_Init();

    Recovery_Color();

    m_iCurFrame = Get_CurFrame();

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

void Companion_Dellons_FSM::State_Init()
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

void Companion_Dellons_FSM::OnCollision(shared_ptr<BaseCollider> pCollider, _float fGap)
{
}

void Companion_Dellons_FSM::OnCollisionEnter(shared_ptr<BaseCollider> pCollider, _float fGap)
{
    if (pCollider->Get_Owner() == nullptr)
        return;

    if (!pCollider->Get_Owner()->Get_Script<AttackColliderInfoScript>())
        return;


    if (!m_bInvincible)
    {
        wstring strSkillName = pCollider->Get_Owner()->Get_Script<AttackColliderInfoScript>()->Get_SkillName();
        _float fAttackDamage = pCollider->Get_Owner()->Get_Script<AttackColliderInfoScript>()->Get_AttackDamage();

        shared_ptr<GameObject> targetToLook = nullptr;
        // skillName�� _Skill �����̸�
        if (strSkillName.find(L"_Skill") != wstring::npos)
        {
            targetToLook = pCollider->Get_Owner(); // Collider owner�� �Ѱ��ش�
        }
        else // �ƴϸ�
            targetToLook = pCollider->Get_Owner()->Get_Script<AttackColliderInfoScript>()->Get_ColliderOwner(); // Collider�� ���� ��ü�� �Ѱ��ش�

        if (targetToLook == nullptr)
            return;

        Get_Hit(strSkillName, fAttackDamage, targetToLook);
    }
}

void Companion_Dellons_FSM::OnCollisionExit(shared_ptr<BaseCollider> pCollider, _float fGap)
{
}

void Companion_Dellons_FSM::Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget)
{
    if (!m_bSuperArmor)
    {
        if (rand() % 4 == 0)
            m_bEvade = true;
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

void Companion_Dellons_FSM::AttackCollider_On(const wstring& skillname, _float fAttackDamage)
{
    if (!m_pAttackCollider.expired())
    {
        m_pAttackCollider.lock()->Get_Collider()->Set_Activate(true);
        m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_SkillName(skillname);
        m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_AttackDamage(fAttackDamage);
    }
}

void Companion_Dellons_FSM::AttackCollider_Off()
{
    if (!m_pAttackCollider.expired())
    {
        m_pAttackCollider.lock()->Get_Collider()->Set_Activate(false);
        m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_SkillName(L"");
        m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_AttackDamage(0.f);
    }
}

void Companion_Dellons_FSM::Set_State(_uint iIndex)
{
}

void Companion_Dellons_FSM::talk_01()
{
    if (!m_pTarget.expired())
        Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);

    if (KEYTAP(KEY_TYPE::P)) //For. Debugging
    {
        m_bIsFollow = true;
        m_eCurState = STATE::b_idle;
    }
}

void Companion_Dellons_FSM::talk_01_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    //TODO talk_01
    animator->Set_NextTweenAnim(L"talk_01", 0.1f, true, 1.f);
}

void Companion_Dellons_FSM::n_idle()
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

void Companion_Dellons_FSM::n_idle_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_idle", 0.1f, true, 1.f);

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::b_idle()
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
}

void Companion_Dellons_FSM::b_idle_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    if (m_ePreState != STATE::skill_400100)
        animator->Set_NextTweenAnim(L"b_idle", 0.1f, true, 1.f);
    else
        animator->Set_NextTweenAnim(L"b_idle", 0.3f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
  
    m_tRunEndDelay.fAccTime = 0.f;
    m_tRunEndDelay.fCoolTime = 0.5f;
    m_tSprintCoolTime.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::b_run_start()
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
}

void Companion_Dellons_FSM::b_run_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_start", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::b_run()
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
}

void Companion_Dellons_FSM::b_run_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::b_run_end_r()
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
}

void Companion_Dellons_FSM::b_run_end_r_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_end_r", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::b_run_end_l()
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
}

void Companion_Dellons_FSM::b_run_end_l_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run_end_l", 0.1f, false, 1.5f);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    m_tRunEndDelay.fAccTime = 0.f;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::b_sprint()
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
}

void Companion_Dellons_FSM::b_sprint_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_sprint", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fSprintSpeed);

    m_tRunEndDelay.fCoolTime = 0.3f;
    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::b_walk()
{
}

void Companion_Dellons_FSM::b_walk_Init()
{
}

void Companion_Dellons_FSM::die()
{
}

void Companion_Dellons_FSM::die_Init()
{
}

void Companion_Dellons_FSM::stun()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Companion_Dellons_FSM::stun_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"stun", 0.2f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = true;
}

void Companion_Dellons_FSM::airborne_start()
{
    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::airborne_end;
}

void Companion_Dellons_FSM::airborne_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"airborne_start", 0.2f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_CharacterController()->Add_Velocity(6.f);
}

void Companion_Dellons_FSM::airborne_end()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::airborne_up;
}

void Companion_Dellons_FSM::airborne_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"airborne_end", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Companion_Dellons_FSM::airborne_up()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Companion_Dellons_FSM::airborne_up_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_Up", 0.1f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Companion_Dellons_FSM::hit()
{
    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Companion_Dellons_FSM::hit_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"hit", 0.1f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::knock_start()
{
    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_end;
}

void Companion_Dellons_FSM::knock_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_start", 0.1f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockBackSpeed * 0.8f);
}

void Companion_Dellons_FSM::knock_end()
{
    if (m_iCurFrame < 16)
        Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_end_loop;
}

void Companion_Dellons_FSM::knock_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockBackSpeed * 0.5f);
}

void Companion_Dellons_FSM::knock_end_loop()
{
    m_tKnockDownEndCoolTime.fAccTime += fDT;

    if (m_iCurFrame > Get_FinalFrame() / 2)
        m_eCurState = STATE::knock_up;
}

void Companion_Dellons_FSM::knock_end_loop_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end_loop", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::knock_end_hit()
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

void Companion_Dellons_FSM::knock_end_hit_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_end_hit", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::knock_up()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Companion_Dellons_FSM::knock_up_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knock_Up", 0.2f, false, 1.f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    m_tKnockDownEndCoolTime.fAccTime = 0.f;

    Get_Transform()->Set_Speed(m_fRunSpeed);
}

void Companion_Dellons_FSM::knockdown_start()
{
    Soft_Turn_ToInputDir(m_vHitDir, XM_PI * 5.f);

    Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knockdown_end;
}

void Companion_Dellons_FSM::knockdown_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"knockdown_start", 0.1f, false, 2.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockDownSpeed);
}

void Companion_Dellons_FSM::knockdown_end()
{
    if (m_iCurFrame < 16)
        Get_Transform()->Go_Backward();

    if (Is_AnimFinished())
        m_eCurState = STATE::knock_up;
}

void Companion_Dellons_FSM::knockdown_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_CurrentAnim(L"knockdown_end", false, 1.5f);

    m_bInvincible = false;
    m_bSuperArmor = true;

    Get_Transform()->Set_Speed(m_fKnockDownSpeed * 0.5f);
}

void Companion_Dellons_FSM::skill_1100()
{
    if (m_iCurFrame < 9)
        Look_DirToTarget();

    if (m_iCurFrame == 9)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 19)
        AttackCollider_Off();

    if (m_iCurFrame >= 24)
        m_eCurState = STATE::skill_1200;
}

void Companion_Dellons_FSM::skill_1100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_bInvincible = false;
    m_bSuperArmor = false;
    m_bSetAttack = false;
}

void Companion_Dellons_FSM::skill_1200()
{
    if (m_iCurFrame < 8)
        Look_DirToTarget();


    if (m_iCurFrame == 8)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 18)
        AttackCollider_Off();

    if (m_iCurFrame >= 20)
        m_eCurState = STATE::skill_1300;
}

void Companion_Dellons_FSM::skill_1200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1200", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::skill_1300()
{
    if (m_iCurFrame < 8)
        Look_DirToTarget();


    if (m_iCurFrame == 8)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 33)
        AttackCollider_Off();

    if (m_iCurFrame >= 18)
        m_eCurState = STATE::skill_1400;
}

void Companion_Dellons_FSM::skill_1300_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1300", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::skill_1400()
{
    if (m_iCurFrame < 8)
        Look_DirToTarget();

    if (m_iCurFrame == 8)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 14)
        AttackCollider_Off();
    else if (m_iCurFrame == 16)
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    else if (m_iCurFrame == 24)
        AttackCollider_Off();


    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;    
}

void Companion_Dellons_FSM::skill_1400_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1400", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::skill_91100()
{
    if (m_vEvadeVector != _float3(0.f))
        Soft_Turn_ToInputDir(m_vEvadeVector, m_fTurnSpeed);


    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Companion_Dellons_FSM::skill_91100_Init()
{
    m_vEvadeVector = MathUtils::Get_RandomVector(_float3{ -1.f,0.f,-1.f }, _float3{ 1.f,0.f,1.f });
    m_vEvadeVector.Normalize();

    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_91100", 0.15f, false, m_fEvade_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::skill_93100()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Companion_Dellons_FSM::skill_93100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_93100", 0.15f, false, m_fEvade_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Companion_Dellons_FSM::skill_100100()
{
    if (m_iCurFrame < 12)
        Look_DirToTarget();

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

        Create_ForwardMovingSkillCollider(vSkillPos, 1.5f, desc, KNOCKBACK_ATTACK, 5.f);
    }

    if (m_iCurFrame >= 26)
        m_eCurState = STATE::skill_100200;
}

void Companion_Dellons_FSM::skill_100100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
    m_bSetAttack = false;
}

void Companion_Dellons_FSM::skill_100200()
{
    if (m_iCurFrame < 15)
        Look_DirToTarget();

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
        Create_ForwardMovingSkillCollider(vSkillPos, 1.5f, desc, AIRBORNE_ATTACK, 10.f);
    }

  
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Companion_Dellons_FSM::skill_100200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100200", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Companion_Dellons_FSM::skill_200100()
{
    if (m_iCurFrame < 7)
        Look_DirToTarget();

    if (m_iCurFrame == 7)
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    else if (m_iCurFrame == 12)
        AttackCollider_Off();

    
    if (m_iCurFrame >= 20)
        m_eCurState = STATE::skill_200200;

}

void Companion_Dellons_FSM::skill_200100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    m_bSetAttack = false;
    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Companion_Dellons_FSM::skill_200200()
{
    if (m_iCurFrame < 7)
        Look_DirToTarget();

    if (Init_CurFrame(7))
    {
        FORWARDMOVINGSKILLDESC desc;
        desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
        desc.fMoveSpeed = 0.f;
        desc.fLifeTime = 1.f;
        desc.fLimitDistance = 0.f;

        _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 3.f + _float3::Up;
        Create_ForwardMovingSkillCollider(vSkillPos, 2.f, desc, KNOCKBACK_SKILL, 10.f);
    }


    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Companion_Dellons_FSM::skill_200200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200200", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Companion_Dellons_FSM::skill_300100()
{
    if (m_iCurFrame < 10)
        Look_DirToTarget();
   
    
    if (Init_CurFrame(10))
    {
        Summon_Wraith();
        Set_WraithState((_uint)DellonsWraith_FSM::STATE::FX_DellonsWraith_skill_30010);
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Companion_Dellons_FSM::skill_300100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_300100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bSetAttack = false;
    m_bInvincible = true;
    m_bSuperArmor = true;
}

void Companion_Dellons_FSM::skill_400100()
{
    if (m_iCurFrame < 20)
        Look_DirToTarget();

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
        Create_ForwardMovingSkillCollider(vSkillPos, 1.f, desc, NORMAL_ATTACK, 10.f);

    }
    else if (Init_CurFrame(99))
    {
        FORWARDMOVINGSKILLDESC desc;
        desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
        desc.fMoveSpeed = 20.f;
        desc.fLifeTime = 1.f;
        desc.fLimitDistance = 5.f;

        _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * -0.5f + _float3::Up;
        Create_ForwardMovingSkillCollider(vSkillPos, 2.f, desc, KNOCKDOWN_SKILL, 10.f);
    }

    if (m_iCurFrame >= 110)
        m_eCurState = STATE::b_idle;
}

void Companion_Dellons_FSM::skill_400100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_400100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bSetAttack = false;
    m_bInvincible = true;
    m_bSuperArmor = true;
}

void Companion_Dellons_FSM::skill_501100()
{
    if (m_iCurFrame < 4)
        Look_DirToTarget();

    if (Init_CurFrame(4))
    {
        Summon_Wraith();
        Set_WraithState((_uint)DellonsWraith_FSM::STATE::FX_Mn_Dellons_skill_500200);
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Companion_Dellons_FSM::skill_501100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_501100", 0.15f, false, m_fSkillAttack_AnimationSpeed);

    AttackCollider_Off();

    m_bSetAttack = false;
    m_bInvincible = false;
    m_bSuperArmor = true;
}


void Companion_Dellons_FSM::Create_ForwardMovingSkillCollider(const _float4& vPos, _float fSkillRange, FORWARDMOVINGSKILLDESC desc, const wstring& SkillType, _float fAttackDamage)
{
    shared_ptr<GameObject> SkillCollider = make_shared<GameObject>();

    m_pSkillCollider = SkillCollider;

    m_pSkillCollider.lock()->GetOrAddTransform();
    m_pSkillCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, vPos);

    auto pSphereCollider = make_shared<SphereCollider>(fSkillRange);
    pSphereCollider->Set_CenterPos(_float3{ vPos.x,vPos.y, vPos.z });
    m_pSkillCollider.lock()->Add_Component(pSphereCollider);

    m_pSkillCollider.lock()->Get_Collider()->Set_CollisionGroup(Companion_Skill);

    m_pSkillCollider.lock()->Add_Component(make_shared<AttackColliderInfoScript>());
    m_pSkillCollider.lock()->Get_Collider()->Set_Activate(true);
    m_pSkillCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_SkillName(SkillType);
    m_pSkillCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_AttackDamage(fAttackDamage);
    m_pSkillCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_ColliderOwner(m_pOwner.lock());
    m_pSkillCollider.lock()->Set_Name(L"Companion_Dellons_SkillCollider");
    m_pSkillCollider.lock()->Add_Component(make_shared<ForwardMovingSkillScript>(desc));
    m_pSkillCollider.lock()->Get_Script<ForwardMovingSkillScript>()->Init();

    EVENTMGR.Create_Object(m_pSkillCollider.lock());
}

void Companion_Dellons_FSM::Summon_Wraith()
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
    ObjWraith->Set_Name(L"Companion_Dellons_Wraith");

    EVENTMGR.Create_Object(ObjWraith);

    m_pDellonsWraith = ObjWraith;
}

void Companion_Dellons_FSM::Set_WraithState(_uint iAnimindex)
{
    if (!m_pDellonsWraith.expired())
        m_pDellonsWraith.lock()->Get_FSM()->Set_State(iAnimindex);
}

void Companion_Dellons_FSM::Set_AttackSkill()
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
            m_fAttackRange = 4.f;
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
            m_fAttackRange = 3.f;
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
