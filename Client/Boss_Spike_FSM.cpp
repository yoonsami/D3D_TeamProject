#include "pch.h"
#include "MainCameraScript.h"
#include "Boss_Spike_FSM.h"
#include "ModelAnimator.h"
#include "MotionTrailRenderer.h"
#include "ModelRenderer.h"
#include "Debug_CreateMotionTrail.h"
#include "SphereCollider.h"
#include "AttackColliderInfoScript.h"
#include "Model.h"
#include "Camera.h"
#include "Boss_DellonsWraith_FSM.h"
#include "MathUtils.h"
#include "CounterMotionTrailScript.h"
#include "UiDamageCreate.h"
#include "UIBossHpBar.h"
#include "ObjectDissolve.h"
#include "CharacterController.h"

Boss_Spike_FSM::Boss_Spike_FSM()
{
}

Boss_Spike_FSM::~Boss_Spike_FSM()
{
    if (!m_pTarget.expired())
    {
        if (m_DieCamPlayerWorld != XMMatrixIdentity())
        {
            m_pTarget.lock()->Get_CharacterController()->Get_Actor()->setFootPosition({ m_DieCamPlayerWorld.Translation().x,  m_DieCamPlayerWorld.Translation().y, m_DieCamPlayerWorld.Translation().z });
            m_pTarget.lock()->Get_Animator()->Set_RenderState(true);
        }
    }

    if (!m_pCamera.expired())
    {
        m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedTime(0.f);
        g_bCutScene = false;
    }
}

HRESULT Boss_Spike_FSM::Init()
{
    if (!m_bInitialize)
    {
        auto animator = Get_Owner()->Get_Animator();
        if (animator)
        {
            animator->Set_CurrentAnim(L"SQ_Appear_01", true, 1.f);
            m_eCurState = STATE::SQ_Appear_01;
        }
        shared_ptr<GameObject> attackCollider = make_shared<GameObject>();
        attackCollider->GetOrAddTransform();
        attackCollider->Add_Component(make_shared<SphereCollider>(1.5f));
        attackCollider->Get_Collider()->Set_CollisionGroup(Monster_Attack);

        m_pAttackCollider = attackCollider;

        EVENTMGR.Create_Object(m_pAttackCollider.lock());
        m_pAttackCollider.lock()->Get_Collider()->Set_Activate(false);

        m_pAttackCollider.lock()->Add_Component(make_shared<AttackColliderInfoScript>());
        m_pAttackCollider.lock()->Set_Name(L"Boss_Spike_AttackCollider");
        m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_ColliderOwner(Get_Owner());

        m_pWeapon = CUR_SCENE->Get_GameObject(L"Weapon_Boss_Spike");

        m_pCamera = CUR_SCENE->Get_MainCamera();

        m_iCenterBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_Center");
        m_iCamBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_Cam");
        m_iSkillCamBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_SkillCam");
        m_iLipBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"B_Lip_Up");
   

        m_fDetectRange = 36.f;
        m_fRunSpeed = 6.f;

        if (!m_pTarget.expired())
            Get_Transform()->LookAt(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS));

        m_iChairBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Bip001-Pelvis");
        m_ChairBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iChairBoneIndex) *
            _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();
        m_vChairBonePos = _float4{ m_ChairBoneMatrix.Translation().x, m_ChairBoneMatrix.Translation().y, m_ChairBoneMatrix.Translation().z , 1.f };

        m_vFirstPos = Get_Transform()->Get_State(Transform_State::POS);

        m_fNormalAttack_AnimationSpeed = 1.f;
        
        m_bInitialize = true;
    }

    return S_OK;
}

void Boss_Spike_FSM::Tick()
{
    DeadCheck();

    State_Tick();

    if (!m_pAttackCollider.expired())
    {
        //m_pAttack transform set forward
        m_pAttackCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 2.f + _float3::Up);
    }

}

void Boss_Spike_FSM::State_Tick()
{
    State_Init();

    m_iCurFrame = Get_CurFrame();
    Recovery_Color();
    switch (m_eCurState)
    {
    case STATE::SQ_Appear_01:
        SQ_Appear_01();
        break;
    case STATE::SQ_Appear_02:
        SQ_Appear_02();
        break;
    case STATE::SQ_Appear_03:
        SQ_Appear_03();
        break;
    case STATE::Spawn:
        Spawn();
        break;
    case STATE::b_idle:
        b_idle();
        break;
    case STATE::b_run:
        b_run();
        break;
    case STATE::gaze_b:
        gaze_b();
        break;
    case STATE::gaze_f:
        gaze_f();
        break;
    case STATE::gaze_l:
        gaze_l();
        break;
    case STATE::gaze_r:
        gaze_r();
        break;
    case STATE::SQ_Die:
        SQ_Die();
        break;
    case STATE::groggy_start:
        groggy_start();
        break;
    case STATE::groggy_loop:
        groggy_loop();
        break;
    case STATE::groggy_end:
        groggy_end();
        break;
    case STATE::hit:
        hit();
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
    case STATE::skill_9100:
        skill_9100();
        break;
    case STATE::skill_9200:
        skill_9200();
        break;
    case STATE::skill_9300:
        skill_9300();
        break;
    case STATE::skill_9400:
        skill_9400();
        break;
    case STATE::skill_2100:
        skill_2100();
        break;
    case STATE::skill_2200:
        skill_2200();
        break;
    case STATE::skill_3100:
        skill_3100();
        break;
    case STATE::skill_3200:
        skill_3200();
        break;
    case STATE::skill_6100:
        skill_6100();
        break;
    case STATE::skill_7100:
        skill_7100();
        break;
    case STATE::skill_8100:
        skill_8100();
        break;
    case STATE::skill_100000:
        skill_100000();
        break;
    case STATE::skill_100100:
        skill_100100();
        break;
    case STATE::skill_201100:
        skill_201100();
        break;
    case STATE::skill_201200:
        skill_201200();
        break;
    }

    Update_GroupEffectWorldPos(Get_Owner()->Get_Transform()->Get_WorldMatrix());

    if (m_iPreFrame != m_iCurFrame)
        m_iPreFrame = m_iCurFrame;
}

void Boss_Spike_FSM::State_Init()
{
    if (m_eCurState != m_ePreState)
    {
        switch (m_eCurState)
        {
        case STATE::SQ_Appear_01:
            SQ_Appear_01_Init();
            break;
        case STATE::SQ_Appear_02:
            SQ_Appear_02_Init();
            break;
        case STATE::SQ_Appear_03:
            SQ_Appear_03_Init();
            break;
        case STATE::Spawn:
            Spawn_Init();
            break;
        case STATE::b_idle:
            b_idle_Init();
            break;
        case STATE::b_run:
            b_run_Init();
            break;
        case STATE::gaze_b:
            gaze_b_Init();
            break;
        case STATE::gaze_f:
            gaze_f_Init();
            break;
        case STATE::gaze_l:
            gaze_l_Init();
            break;
        case STATE::gaze_r:
            gaze_r_Init();
            break;
        case STATE::SQ_Die:
            SQ_Die_Init();
            break;
        case STATE::groggy_start:
            groggy_start_Init();
            break;
        case STATE::groggy_loop:
            groggy_loop_Init();
            break;
        case STATE::groggy_end:
            groggy_end_Init();
            break;
        case STATE::hit:
            hit_Init();
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
        case STATE::skill_9100:
            skill_9100_Init();
            break;
        case STATE::skill_9200:
            skill_9200_Init();
            break;
        case STATE::skill_9300:
            skill_9300_Init();
            break;
        case STATE::skill_9400:
            skill_9400_Init();
            break;
        case STATE::skill_2100:
            skill_2100_Init();
            break;
        case STATE::skill_2200:
            skill_2200_Init();
            break;
        case STATE::skill_3100:
            skill_3100_Init();
            break;
        case STATE::skill_3200:
            skill_3200_Init();
            break;
        case STATE::skill_6100:
            skill_6100_Init();
            break;
        case STATE::skill_7100:
            skill_7100_Init();
            break;
        case STATE::skill_8100:
            skill_8100_Init();
            break;
        case STATE::skill_100000:
            skill_100000_Init();
            break;
        case STATE::skill_100100:
            skill_100100_Init();
            break;
        case STATE::skill_201100:
            skill_201100_Init();
            break;
        case STATE::skill_201200:
            skill_201200_Init();
            break;
        }
        m_ePreState = m_eCurState;
    }
}

void Boss_Spike_FSM::Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget)
{
    m_pOwner.lock()->Get_Hurt(fDamage);
    CUR_SCENE->Get_UI(L"UI_Damage_Controller")->Get_Script<UiDamageCreate>()->Create_Damage_Font(Get_Owner(), fDamage);


    _float3 vMyPos = Get_Transform()->Get_State(Transform_State::POS).xyz();
    _float3 vOppositePos = pLookTarget->Get_Transform()->Get_State(Transform_State::POS).xyz();

    m_vHitDir = vOppositePos - vMyPos;
    m_vHitDir.y = 0.f;
    m_vHitDir.Normalize();
	Set_HitColor();

    if (skillname == NORMAL_ATTACK || skillname == NORMAL_SKILL)
    {
        if (m_bCounter)
        {
            if (CounterAttackCheck(90.f))
            {
                Create_CounterMotionTrail();
                m_eCurState = STATE::groggy_start;
            }
        }

        if (m_bGroggyPattern)
        {
            if (m_pOwner.lock()->Get_GroggyGauge(10.f))
            {
                m_bGroggyPattern = false;
                m_eCurState = STATE::groggy_start;

                Create_CounterMotionTrail();

                if (!m_pCamera.expired())
                    m_pCamera.lock()->Get_Script<MainCameraScript>()->ShakeCamera(0.f, 0.f);   
            }
        }
    }
    else if (skillname == KNOCKBACK_ATTACK || skillname == KNOCKBACK_SKILL)
    {
        if (m_bCounter)
        {
            if (CounterAttackCheck(90.f))
            {
                Create_CounterMotionTrail();
                m_eCurState = STATE::groggy_start;
            }
        }

        if (m_bGroggyPattern)
        {
            if (m_pOwner.lock()->Get_GroggyGauge(15.f))
            {
                m_bGroggyPattern = false;
                m_eCurState = STATE::groggy_start;

                Create_CounterMotionTrail();

                if (!m_pCamera.expired())
                    m_pCamera.lock()->Get_Script<MainCameraScript>()->ShakeCamera(0.f, 0.f);
            }
        }
    }
    else if (skillname == KNOCKDOWN_ATTACK || skillname == KNOCKDOWN_SKILL)
    {
        if (m_bCounter)
        {
            if (CounterAttackCheck(90.f))
            {
                Create_CounterMotionTrail();
                m_eCurState = STATE::groggy_start;
            }
        }

        if (m_bGroggyPattern)
        {
            if (m_pOwner.lock()->Get_GroggyGauge(20.f))
            {
                m_bGroggyPattern = false;
                m_eCurState = STATE::groggy_start;

                Create_CounterMotionTrail();

                if (!m_pCamera.expired())
                    m_pCamera.lock()->Get_Script<MainCameraScript>()->ShakeCamera(0.f, 0.f);
            }
        }
    }
    else if (skillname == AIRBORNE_ATTACK || skillname == AIRBORNE_SKILL)
    {
        if (m_bCounter)
        {
            if (CounterAttackCheck(90.f))
            {
                Create_CounterMotionTrail();
                m_eCurState = STATE::groggy_start;
            }
        }

        if (m_bGroggyPattern)
        {
            if (m_pOwner.lock()->Get_GroggyGauge(20.f))
            {
                m_bGroggyPattern = false;
                m_eCurState = STATE::groggy_start;

                Create_CounterMotionTrail();

                if (!m_pCamera.expired())
                    m_pCamera.lock()->Get_Script<MainCameraScript>()->ShakeCamera(0.f, 0.f);
            }
        }
    }
}

void Boss_Spike_FSM::Set_State(_uint iIndex)
{
}

void Boss_Spike_FSM::SQ_Appear_01()
{
    if (Target_In_DetectRange())
        Get_Owner()->Get_Animator()->Set_AnimState(false);

    if (m_iCurFrame >= 10 && m_iCurFrame <= 75)
    {
        g_bCutScene = true;

        if (!m_pCamera.expired())
        {
            _float4 vDir = (m_vSkillCamBonePos +
                Get_Transform()->Get_State(Transform_State::LOOK) + 
              Get_Transform()->Get_State(Transform_State::RIGHT) * 1.f)
                - m_vCenterBonePos;

            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(5.f, vDir.xyz(), 5.f);
        }

        if (m_iCurFrame == 75)
            Get_Owner()->Get_Animator()->Set_AnimationSpeed(4.f);
    }

    Calculate_CamBoneMatrix();

    if (Is_AnimFinished())
    {
        m_vPlayerBodyPos = m_vCenterBonePos;
        m_eCurState = STATE::SQ_Appear_02;
    }
}

void Boss_Spike_FSM::SQ_Appear_01_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"SQ_Appear_01", 0.1f, false, 1.f);

    animator->Set_AnimState(true);

    Get_Transform()->Set_Speed(m_fRunSpeed);
    Get_Transform()->Set_State(Transform_State::POS, m_vFirstPos);
    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
    m_bSetPattern = false;

    Create_BossSpikeChair();

    Calculate_CamBoneMatrix();
}

void Boss_Spike_FSM::SQ_Appear_02()
{
    m_vPlayerBodyPos += Get_Transform()->Get_State(Transform_State::LOOK) * fDT * m_fRunSpeed * 4.f;
    Get_Transform()->Set_State(Transform_State::POS, m_vPlayerBodyPos);

    if (m_iCurFrame <= 10)
    {
        if (!m_pCamera.expired())
        {
            _float4 vDir = m_vPlayerBodyPos - m_vChairPos;
        
            vDir.Normalize();
        
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(5.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vPlayerBodyPos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.35f, vDir.xyz(), 10.f);
        }

        if (m_iCurFrame == 10)
            m_vCamStopPos = m_vPlayerBodyPos;
    }
    else
    {
        if (!m_pCamera.expired())
        {
            _float4 vDir = m_vCamStopPos - m_vChairPos;

            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(5.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCamStopPos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.35f, vDir.xyz(), 7.f);
        }
    }

    Calculate_CamBoneMatrix();


    if (Is_AnimFinished())
        m_eCurState = STATE::SQ_Appear_03;
}

void Boss_Spike_FSM::SQ_Appear_02_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"SQ_Appear_02", 0.1f, false, 1.f);

    //EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Boss_Spike_Chair"));

    Calculate_CamBoneMatrix();

    m_vPlayerBodyPos.y = m_vPlayerBodyPos.y / 2.f;
    Get_Transform()->Set_State(Transform_State::POS, m_vPlayerBodyPos);
    m_vChairPos = m_vFirstPos;
    m_vChairPos.y = m_vPlayerBodyPos.y - 0.5f;
    m_vChairPos = m_vChairPos + Get_Transform()->Get_State(Transform_State::RIGHT) * 2.f;

    if (!m_pCamera.expired())
    {
        m_pCamera.lock()->Get_Transform()->Set_State(Transform_State::POS,
            m_vPlayerBodyPos + Get_Transform()->Get_State(Transform_State::LOOK) * 5.f);
    }
}

void Boss_Spike_FSM::SQ_Appear_03()
{
    if (!m_pCamera.expired())
    {
        _float4 vDir = m_vCamStopPos - m_vCenterBonePos;

        vDir.Normalize();

        m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
        m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
        m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.35f, vDir.xyz(), 4.f);
    }

    Calculate_CamBoneMatrix();


    if (Is_AnimFinished())
        m_eCurState = STATE::Spawn;
}

void Boss_Spike_FSM::SQ_Appear_03_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"SQ_Appear_03", 0.1f, false, 1.f);

    _float4 vPos = m_vCamStopPos;
    vPos.y = 0.f;

    Get_Transform()->Set_State(Transform_State::POS, vPos);

    Calculate_CamBoneMatrix();

    if (!m_pCamera.expired())
    {
        m_pCamera.lock()->Get_Transform()->Set_State(Transform_State::POS,
            Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 15.f + _float4{ 0.f,1.5f,0.f,0.f });

        m_vCamStopPos = m_pCamera.lock()->Get_Transform()->Get_State(Transform_State::POS);
    }
}

void Boss_Spike_FSM::Spawn()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Boss_Spike_FSM::Spawn_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"Spawn", 0.3f, false, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);

    AttackCollider_Off();

    //Add_BossHp UI
    if (!m_pOwner.expired())
    {
        auto pScript = make_shared<UIBossHpBar>(BOSS::SPIKE);
        m_pOwner.lock()->Add_Component(pScript);
        pScript->Init();
    }

    m_DieCamWorld = Get_Transform()->Get_WorldMatrix();

    g_bCutScene = false;
}

void Boss_Spike_FSM::b_idle()
{
    if (!m_pTarget.expired())
        Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);

    m_tAttackCoolTime.fAccTime += fDT;
    m_tGroggyPatternTimer.fAccTime += fDT;

    if (!m_bSetPattern)
    {
        if (m_tAttackCoolTime.fAccTime >= m_tAttackCoolTime.fCoolTime)
            Set_AttackSkill();
    }
    else
    {
        if (Target_In_AttackRange())
            m_eCurState = m_ePatternState;
        else
            m_eCurState = STATE::b_run;
    }

    DeadSetting();
}

void Boss_Spike_FSM::b_idle_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_idle", 0.3f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}



void Boss_Spike_FSM::b_run()
{
    if (!m_pTarget.expired())
        Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);

    Get_Transform()->Go_Straight();

    m_tChaseCoolTime.fAccTime += fDT;
    m_tGroggyPatternTimer.fAccTime += fDT * 0.5f;

    if (Target_In_AttackRange())
    {
        m_eCurState = m_ePatternState;
    }
    else
    {
        if (m_tChaseCoolTime.fAccTime >= m_tChaseCoolTime.fCoolTime)
            m_bChaseSkill = true;

        if (m_bChaseSkill)
        {
            m_bChaseSkill = false;
            m_eCurState = STATE::skill_9100;
        }
    }

    DeadSetting();
}

void Boss_Spike_FSM::b_run_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_run", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Boss_Spike_FSM::gaze_b()
{
    if (!m_pTarget.expired())
        Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), XM_PI * 5.f);

    m_tAttackCoolTime.fAccTime += fDT;
    m_tGroggyPatternTimer.fAccTime += fDT;

    Get_Transform()->Go_Backward();

    if (!m_bSetPattern)
    {
        if (m_tAttackCoolTime.fAccTime >= m_tAttackCoolTime.fCoolTime)
            Set_AttackSkill();
    }
    else
    {
        if (Target_In_AttackRange())
            m_eCurState = m_ePatternState;
        else
            m_eCurState = STATE::b_run;
    }

    DeadSetting();
}

void Boss_Spike_FSM::gaze_b_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"gaze_b", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed / 2.f);

    m_bSuperArmor = false;

    m_tChaseCoolTime.fAccTime = 0.f;
}

void Boss_Spike_FSM::gaze_f()
{
    if (!m_pTarget.expired())
        Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), XM_PI * 5.f);

    m_tAttackCoolTime.fAccTime += fDT;
    m_tGroggyPatternTimer.fAccTime += fDT;

    Get_Transform()->Go_Straight();

    if (!m_bSetPattern)
    {
        if (m_tAttackCoolTime.fAccTime >= m_tAttackCoolTime.fCoolTime)
            Set_AttackSkill();
    }
    else
    {
        if (Target_In_AttackRange())
            m_eCurState = m_ePatternState;
        else
            m_eCurState = STATE::b_run;
    }

    DeadSetting();
}

void Boss_Spike_FSM::gaze_f_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"gaze_f", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed / 2.f);

    m_bSuperArmor = false;

    m_tChaseCoolTime.fAccTime = 0.f;
}

void Boss_Spike_FSM::gaze_l()
{
    if (!m_pTarget.expired())
        Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), XM_PI * 5.f);

    m_tAttackCoolTime.fAccTime += fDT;
    m_tGroggyPatternTimer.fAccTime += fDT;

    Get_Transform()->Go_Left();

    if (!m_bSetPattern)
    {
        if (m_tAttackCoolTime.fAccTime >= m_tAttackCoolTime.fCoolTime)
            Set_AttackSkill();
    }
    else
    {
        if (Target_In_AttackRange())
            m_eCurState = m_ePatternState;
        else
            m_eCurState = STATE::b_run;
    }

    DeadSetting();
}

void Boss_Spike_FSM::gaze_l_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"gaze_l", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed / 2.f);

    m_bSuperArmor = false;

    m_tChaseCoolTime.fAccTime = 0.f;
}

void Boss_Spike_FSM::gaze_r()
{
    if (!m_pTarget.expired())
        Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), XM_PI * 5.f);

    m_tAttackCoolTime.fAccTime += fDT;
    m_tGroggyPatternTimer.fAccTime += fDT;

    Get_Transform()->Go_Right();

    if (!m_bSetPattern)
    {
        if (m_tAttackCoolTime.fAccTime >= m_tAttackCoolTime.fCoolTime)
            Set_AttackSkill();
    }
    else
    {
        if (Target_In_AttackRange())
            m_eCurState = m_ePatternState;
        else
            m_eCurState = STATE::b_run;
    }

    DeadSetting();
}

void Boss_Spike_FSM::gaze_r_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"gaze_r", 0.2f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed / 2.f);

    m_bSuperArmor = false;

    m_tChaseCoolTime.fAccTime = 0.f;
}


void Boss_Spike_FSM::SQ_Die()
{
    if (m_iCurFrame <= 4)
    {
        if (!m_pCamera.expired())
        {
            _float4 vDir = m_vSkillCamBonePos - m_vCenterBonePos;

            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.2f, vDir.xyz(), 4.f);

            if (m_iCurFrame == 4)
                m_vDieCamStopPos = m_vCenterBonePos;
        }
    }
    else if (m_iCurFrame > 4 && m_iCurFrame < 100)
    {
        if (!m_pCamera.expired())
        {
            _float4 vDir = m_vSkillCamBonePos - m_vDieCamStopPos;

            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vDieCamStopPos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.2f, vDir.xyz(), 4.f);
        }
    }
    else
    {    
        if (!m_pCamera.expired())
        {
            Calculate_LipBoneMatrix();

            m_fDieCamDistance = CamDistanceLerp(4.f, 1.f, m_fCamRatio);

            m_fCamRatio += fDT * 0.5f;

            if (m_fCamRatio >= 1.f)
                m_fCamRatio = 1.f;

            _float4 vDir = m_vSkillCamBonePos - m_vLipBonePos;

            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vLipBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.2f, vDir.xyz(), m_fDieCamDistance);
        }
    }

    Calculate_CamBoneMatrix();


    if (Is_AnimFinished())
    {
        auto script = make_shared<ObjectDissolve>(1.f);
        Get_Owner()->Add_Component(script);
        script->Init();

        if (!m_pAttackCollider.expired())
            EVENTMGR.Delete_Object(m_pAttackCollider.lock());

        if (!m_pWeapon.expired())
        {
            auto script = make_shared<ObjectDissolve>(1.f);
            m_pWeapon.lock()->Add_Component(script);
            script->Init();
        }
    }
}

void Boss_Spike_FSM::SQ_Die_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"SQ_Die", 0.2f, false, 1.f);

    m_bInvincible = true;

    m_fCamRatio = 0.f;

    {
        //Setting Spawn Position For CutScene
        Get_CharacterController()->Get_Actor()->setFootPosition({ m_DieCamWorld.Translation().x,  m_DieCamWorld.Translation().y, m_DieCamWorld.Translation().z });
        Get_Transform()->Set_LookDir(m_DieCamWorld.Backward());
    }


    if (!m_pTarget.expired())
    {
        //Save PlayerWorld 
        m_DieCamPlayerWorld = m_pTarget.lock()->Get_Transform()->Get_WorldMatrix();
        
        //Player Render False For CutScene
        m_pTarget.lock()->Get_Animator()->Set_RenderState(false);
        
        //Player Go to America For CutScene 
        m_pTarget.lock()->Get_Transform()->Set_State(Transform_State::POS, _float4{ 0.f,1000.f,0.f,1.f });
    }

    Calculate_CamBoneMatrix();

    if (!m_pCamera.expired())
    {
        m_pCamera.lock()->Get_Transform()->Set_State(Transform_State::POS,
            Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 3.f + _float4{ 0.f,3.f,0.f,0.f });
        
        m_vCamStopPos = m_pCamera.lock()->Get_Transform()->Get_State(Transform_State::POS);

        g_bCutScene = true;
    }

    if (!m_pOwner.expired())
    {
        if (m_pOwner.lock()->Get_Script<UIBossHpBar>())
            m_pOwner.lock()->Get_Script<UIBossHpBar>()->Remove_HpBar();
    }
}

void Boss_Spike_FSM::hit()
{
    Soft_Turn_ToInputDir(m_vHitDir, m_fTurnSpeed);

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Boss_Spike_FSM::hit_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"hit", 0.1f, false, 1.f);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Boss_Spike_FSM::groggy_start()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::groggy_loop;
}

void Boss_Spike_FSM::groggy_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"groggy_start", 0.1f, false, m_fGroggyStateAnimationSpeed);

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;

    m_tChaseCoolTime.fAccTime = 0.f;
    m_tSkillCoolTime.fAccTime = 0.f;
    m_bCounter = false;
}

void Boss_Spike_FSM::groggy_loop()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::groggy_end;
}

void Boss_Spike_FSM::groggy_loop_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"groggy_loop", 0.1f, false, 3.f);
}

void Boss_Spike_FSM::groggy_end()
{
    Set_Gaze();

    DeadSetting();
}

void Boss_Spike_FSM::groggy_end_Init()
{
    m_pOwner.lock()->Set_GroggyGauge(100.f);

    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"groggy_end", 0.1f, false, m_fGroggyStateAnimationSpeed);
}


void Boss_Spike_FSM::skill_1100()
{
    if (m_vTurnVector != _float3(0.f))
        Soft_Turn_ToInputDir(m_vTurnVector, m_fTurnSpeed);

    if (m_iCurFrame == 9)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 19)
        AttackCollider_Off();
    else if (m_iCurFrame == 29)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 38)
        AttackCollider_Off();
    else if (m_iCurFrame == 53)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 60)
        AttackCollider_Off();

    Set_Gaze();
}

void Boss_Spike_FSM::skill_1100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Boss_Spike_FSM::skill_1200()
{
    if (m_vTurnVector != _float3(0.f))
        Soft_Turn_ToInputDir(m_vTurnVector, m_fTurnSpeed);

    if (m_iCurFrame == 9)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 19)
        AttackCollider_Off();
    else if (m_iCurFrame == 29)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 38)
        AttackCollider_Off();
    else if (m_iCurFrame == 53)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 60)
        AttackCollider_Off();
    else if (m_iCurFrame == 70)
    {
        //Aim Target
        m_vTurnVector = Calculate_TargetTurnVector();

        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(0.5f);

        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.05f, 0.2f, 1.f, 1.f);
        }

        m_bCounter = true;
        m_fGroggyStateAnimationSpeed = 1.5f;
    }
    else if (m_iCurFrame == 80)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(1.5f);
        m_bCounter = false;
        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.f, 0.f, 0.f, 1.f);
        }
    }
    else if (m_iCurFrame == 100)
        AttackCollider_On(KNOCKDOWN_ATTACK, 10.f);
    else if (m_iCurFrame == 105)
        AttackCollider_Off();


    Set_Gaze();
}

void Boss_Spike_FSM::skill_1200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1200", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Boss_Spike_FSM::skill_1300()
{
    if (m_vTurnVector != _float3(0.f))
        Soft_Turn_ToInputDir(m_vTurnVector, m_fTurnSpeed);

    if (m_iCurFrame == 10)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 15)
        AttackCollider_Off();
    else if (m_iCurFrame == 25)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(0.5f);

        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.05f, 0.2f, 1.f, 1.f);
        }

        m_bCounter = true;
        m_fGroggyStateAnimationSpeed = 1.5f;
    }
    else if (m_iCurFrame == 39)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(1.5f);
        m_bCounter = false;
        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.f, 0.f, 0.f, 1.f);
        }
    }
    else if (m_iCurFrame == 41)
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    else if (m_iCurFrame == 45)
        AttackCollider_Off();

    


    Set_Gaze();
}

void Boss_Spike_FSM::skill_1300_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1300", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = true;
}

void Boss_Spike_FSM::skill_1400()
{
    if (m_vTurnVector != _float3(0.f))
        Soft_Turn_ToInputDir(m_vTurnVector, m_fTurnSpeed);

    if (m_iCurFrame == 17)
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    else if (m_iCurFrame == 25)
        AttackCollider_Off();
    
    Set_Gaze();
}

void Boss_Spike_FSM::skill_1400_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1400", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Boss_Spike_FSM::skill_9100()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_run;
}

void Boss_Spike_FSM::skill_9100_Init()
{
    m_vTurnVector = MathUtils::Get_RandomVector(_float3{ -1.f,0.f,-1.f }, _float3{ 1.f,0.f,1.f });
    m_vTurnVector.Normalize();

    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_9100", 0.15f, false, m_fEvade_AnimationSpeed);

    AttackCollider_Off();

    m_tChaseCoolTime.fAccTime = 0.f;

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Boss_Spike_FSM::skill_9200()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_run;
}

void Boss_Spike_FSM::skill_9200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_9200", 0.15f, false, 1.5f);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Boss_Spike_FSM::skill_9300()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_run;
}

void Boss_Spike_FSM::skill_9300_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_9300", 0.15f, false, 1.5f);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Boss_Spike_FSM::skill_9400()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_run;
}

void Boss_Spike_FSM::skill_9400_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_9400", 0.15f, false, 1.5f);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}


void Boss_Spike_FSM::skill_2100()
{
    if (m_vTurnVector != _float3(0.f))
        Soft_Turn_ToInputDir(m_vTurnVector, m_fTurnSpeed);

    if (m_iCurFrame == 4)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(0.3f);

        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.05f, 0.2f, 1.f, 1.f);
        }
        m_bCounter = true;
        m_fGroggyStateAnimationSpeed = 1.5f;
    }
    else if (m_iCurFrame == 15)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(1.f);
        m_bCounter = false;
        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.f, 0.f, 0.f, 1.f);
        }
    }
    else if (m_iCurFrame == 16)
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    else if (m_iCurFrame == 34)
        AttackCollider_Off();
    else if (m_iCurFrame == 52)
        AttackCollider_On(KNOCKDOWN_ATTACK, 10.f);
    else if (m_iCurFrame == 56)
        AttackCollider_Off();

    Set_Gaze();
}

void Boss_Spike_FSM::skill_2100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_2100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Boss_Spike_FSM::skill_2200()
{
    if (m_vTurnVector != _float3(0.f))
        Soft_Turn_ToInputDir(m_vTurnVector, m_fTurnSpeed);

    if (m_iCurFrame == 4)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(0.3f);

        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.05f, 0.2f, 1.f, 1.f);
        }
        m_bCounter = true;
        m_fGroggyStateAnimationSpeed = 1.5f;
    }
    else if (m_iCurFrame == 15)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(1.f);
        m_bCounter = false;
        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.f, 0.f, 0.f, 1.f);
        }
    }
    else if (m_iCurFrame == 16)
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    else if (m_iCurFrame == 46)
        AttackCollider_Off();
    else if (m_iCurFrame == 62)
        AttackCollider_On(KNOCKDOWN_ATTACK, 10.f);
    else if (m_iCurFrame == 68)
        AttackCollider_Off();
    else if (m_iCurFrame == 73)
        AttackCollider_On(KNOCKDOWN_ATTACK, 10.f);
    else if (m_iCurFrame == 82)
        AttackCollider_Off();

    Set_Gaze();
}

void Boss_Spike_FSM::skill_2200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_2200", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Boss_Spike_FSM::skill_3100()
{
    if (m_vTurnVector != _float3(0.f))
        Soft_Turn_ToInputDir(m_vTurnVector, m_fTurnSpeed);

    if (m_iCurFrame == 42)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(0.3f);
        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.05f, 0.2f, 1.f, 1.f);
        }

        m_bCounter = true;
        m_fGroggyStateAnimationSpeed = 1.5f;
    }
    else if (m_iCurFrame == 51)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(1.f);
        m_bCounter = false;
        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.f, 0.f, 0.f, 1.f);
        }
    }
    else if (m_iCurFrame == 53)
    {
        if (!m_pTarget.expired())
        {
            _float3 vPlayerPos = m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS).xyz();
            _float3 vMyPos = Get_Transform()->Get_State(Transform_State::POS).xyz();

            _float3 vDir = vPlayerPos - vMyPos;
            vDir.y = 0.f;

            if (vDir.LengthSquared() > 3.f * 3.f)
                return;

            vDir.Normalize();

            _float3 vLook = Get_Transform()->Get_State(Transform_State::LOOK).xyz();
            vLook.Normalize();

            if (vDir.Dot(vLook) > cosf(XM_PI / 3.f))
            {
                m_pTarget.lock()->Get_FSM()->Get_Hit(KNOCKDOWN_ATTACK, 30.f, Get_Owner());
            }
        }
    }

    Set_Gaze();
}

void Boss_Spike_FSM::skill_3100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_3100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Boss_Spike_FSM::skill_3200()
{
    if (m_vTurnVector != _float3(0.f))
        Soft_Turn_ToInputDir(m_vTurnVector, m_fTurnSpeed);

    m_tSkillCoolTime.fAccTime += fDT;

    if (m_iCurFrame == 55)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(0.3f);
        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.05f, 0.2f, 1.f, 1.f);
        }
    
        m_bCounter = true;
    }
    else if (m_iCurFrame == 69)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(1.f);
        m_bCounter = false;
        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.f, 0.f, 0.f, 1.f);
        }
    }
    else if (m_iCurFrame == 70)
        AttackCollider_On(KNOCKBACK_ATTACK,10.f);
    else if (m_iCurFrame == 74)
        AttackCollider_Off();
    else if (m_iCurFrame >= 133 && m_iCurFrame <= 156)
    {
        if (m_tSkillCoolTime.fAccTime >= m_tSkillCoolTime.fCoolTime)
        {
            m_tSkillCoolTime.fAccTime = 0.f;

            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 20.f;
            desc.fLifeTime = 0.5f;
            desc.fLimitDistance = 10.f;

            _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS);

            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Spike_SkillCollider", vSkillPos, 3.f, desc, KNOCKBACK_ATTACK, 10.f);
        }
    }

    Set_Gaze();
}

void Boss_Spike_FSM::skill_3200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_3200", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}


void Boss_Spike_FSM::skill_6100()
{
    if (m_iCurFrame < 33)
    {
        if (!m_pCamera.expired())
        {
            _float4 vDir = m_vCamStopPos - m_vCenterBonePos;

            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.2f, vDir.xyz(), 4.f);
        }
    }
    else if (m_iCurFrame >= 33 && m_iCurFrame < 108)
    {
        if (m_iCurFrame == 33)
            m_vLastPatternCamStopPos = m_vCenterBonePos;

        if (!m_pCamera.expired())
        {
            _float4 vDir = m_vCamStopPos - m_vLastPatternCamStopPos;

            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vLastPatternCamStopPos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.2f, vDir.xyz(), 4.f);
        }
    }
    else if (m_iCurFrame >= 108 && m_iCurFrame < 115)
    {
        if (!m_pCamera.expired())
        {
            _float4 vDir = m_vCamStopPos - m_vCenterBonePos;

            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.2f, vDir.xyz(), 4.f);
        }
    }
    else if (m_iCurFrame >= 115 && m_iCurFrame < 145)
    {
        if (m_iCurFrame == 115)
            m_vLastPatternCamStopPos = m_vCenterBonePos;

        if (!m_pCamera.expired())
        {
            _float4 vDir = m_vCamStopPos - m_vLastPatternCamStopPos;

            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vLastPatternCamStopPos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.2f, vDir.xyz(), 4.f);
        }
    }
    else
    {
        g_bCutScene = false;
    }

    Calculate_CamBoneMatrix();

    if (m_iCurFrame == 145)
        Get_Owner()->Get_Animator()->Set_AnimationSpeed(1.f);
    

    //160~380
    if (m_iCurFrame == 160 ||
        m_iCurFrame == 200 ||
        m_iCurFrame == 240 ||
        m_iCurFrame == 280 ||
        m_iCurFrame == 320 ||
        m_iCurFrame == 360 ||
        m_iCurFrame == 380)
    {
        if (m_iPreFrame != m_iCurFrame)
        {
            _float4 vMyPos = Get_Transform()->Get_State(Transform_State::POS);

            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = _float3{ 0.f,-1.f,0.f };
            desc.fMoveSpeed = 10.f;
            desc.fLifeTime = 1.f;
            desc.fLimitDistance = 20.f;

            for (_uint i = 0; i < 6; i++)
            {
                _float fOffSetX = ((rand() * 2 / _float(RAND_MAX) - 1) * (rand() % 10 + 3));
                _float fOffSetZ = ((rand() * 2 / _float(RAND_MAX) - 1) * (rand() % 10 + 3));

                _float4 vSkillPos = vMyPos + _float4{ fOffSetX, 10.f, fOffSetZ, 0.f };

                Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Spike_SkillCollider", vSkillPos, 1.5f, desc, AIRBORNE_ATTACK, 10.f);
            }
        }
    }

    Set_Gaze();
}

void Boss_Spike_FSM::skill_6100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_6100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;

    Calculate_CamBoneMatrix();

    if (!m_pTarget.expired())
        Get_Transform()->LookAt(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS));

    if (!m_pCamera.expired())
    {
        m_pCamera.lock()->Get_Transform()->Set_State(Transform_State::POS,
            Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 3.f + _float4{ 0.f,2.f,0.f,0.f });
        
        m_vCamStopPos = m_pCamera.lock()->Get_Transform()->Get_State(Transform_State::POS);
        //m_pCamera.lock()->Get_Transform()->Set_State(Transform_State::POS, m_vSkillCamBonePos);    
    }
    
    m_bLastPattern = true;

    g_bCutScene = true;
}

void Boss_Spike_FSM::skill_7100()
{
    if (m_iCurFrame == 137)
    {
        if (m_iPreFrame != m_iCurFrame)
        {
            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = _float3(0.f);
            desc.fMoveSpeed = 0.f;
            desc.fLifeTime = 1.f;
            desc.fLimitDistance = 0.f;
            
            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Spike_SkillCollider", Get_Transform()->Get_State(Transform_State::POS), 5.f, desc, KNOCKDOWN_ATTACK, 10.f);
        }
    }

    Set_Gaze();
}

void Boss_Spike_FSM::skill_7100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_7100", 0.15f, false, m_fNormalAttack_AnimationSpeed * 1.3f);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Boss_Spike_FSM::skill_8100()
{
    if (m_iCurFrame == 88)
    {
        if (m_iPreFrame != m_iCurFrame)
        {
            _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS);

            FORWARDMOVINGSKILLDESC desc;
            desc.fMoveSpeed = 20.f;
            desc.fLifeTime = 1.5f;
            desc.fLimitDistance = 20.f;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);

            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Spike_SkillCollider", vSkillPos, 2.f, desc, KNOCKBACK_ATTACK, 10.f);

            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK) + Get_Transform()->Get_State(Transform_State::RIGHT) * -1.f;
            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Spike_SkillCollider", vSkillPos, 2.f, desc, KNOCKBACK_ATTACK, 10.f);

            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK) + Get_Transform()->Get_State(Transform_State::RIGHT) * 1.f;
            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Spike_SkillCollider", vSkillPos, 2.f, desc, KNOCKBACK_ATTACK, 10.f);
        }
    }

    Set_Gaze();
}

void Boss_Spike_FSM::skill_8100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_8100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Boss_Spike_FSM::skill_100000()
{
    if (m_vTurnVector != _float3(0.f))
        Soft_Turn_ToInputDir(m_vTurnVector, m_fTurnSpeed);


    if (m_iCurFrame <= 30)
    {
        if (!m_pCamera.expired())
        {
            _float4 vDir = m_pCamera.lock()->Get_Transform()->Get_State(Transform_State::POS) - m_vCenterBonePos;

            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vCenterBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(0.2f, vDir.xyz(), 4.f);
        }

        if (m_iCurFrame == 30)
        {
            m_bInvincible = false;
            m_bGroggyPattern = true;
            Get_Owner()->Get_Animator()->Set_AnimationSpeed(m_fGroggyPatternAnimationSpeed);

            for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
            {
                material->Get_MaterialDesc().emissive = Color(1.f, 0.1f, 0.1f, 1.f);
            }
        }
    }
    else
    {
        g_bCutScene = false;

        if (!m_pCamera.expired())
        {
            m_pCamera.lock()->Get_Script<MainCameraScript>()->ShakeCamera(0.1f, 0.07f);
        }
    }


    Calculate_CamBoneMatrix();

    if (Is_AnimFinished())
        m_eCurState = STATE::skill_100100;
}

void Boss_Spike_FSM::skill_100000_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100000", 0.15f, false, 1.f);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;

    if (!m_pTarget.expired())
        Get_Transform()->LookAt(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS));

    Calculate_CamBoneMatrix();

    if (!m_pCamera.expired())
    {
        m_pCamera.lock()->Get_Transform()->Set_State(Transform_State::POS,
            Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 3.f + _float4{ 0.f,2.f,0.f,0.f });

        g_bCutScene = true;
    }

}

void Boss_Spike_FSM::skill_100100()
{
    if (m_iCurFrame == 4)
    {
        m_pOwner.lock()->Set_GroggyGauge(100.f);

        m_bGroggyPattern = false;
        Get_Owner()->Get_Animator()->Set_AnimationSpeed(1.f);

        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.f, 0.0f, 0.0f, 1.f);
        }

        if (!m_pCamera.expired())
        {
            m_pCamera.lock()->Get_Script<MainCameraScript>()->ShakeCamera(0.f, 0.f);
        }
    }


    Calculate_CamBoneMatrix();


    if (m_iCurFrame == 85)
    {
        if (!m_pTarget.expired())
        {
            _float3 vPlayerPos = m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS).xyz();
            _float3 vMyPos = Get_Transform()->Get_State(Transform_State::POS).xyz();

            _float3 vDir = vPlayerPos - vMyPos;
            vDir.y = 0.f;

            if (vDir.LengthSquared() > 30.f * 30.f)
                return;

            vDir.Normalize();

            _float3 vLook = Get_Transform()->Get_State(Transform_State::LOOK).xyz();
            vLook.Normalize();

            if (vDir.Dot(vLook) > cosf(XM_PI))
            {
                m_pTarget.lock()->Get_FSM()->Get_Hit(KNOCKDOWN_ATTACK, 50.f, Get_Owner());
            }
        }
    }

    Set_Gaze();
}

void Boss_Spike_FSM::skill_100100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100100", 0.15f, false, m_fGroggyPatternAnimationSpeed);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    Calculate_CamBoneMatrix();

    m_bInvincible = false;
    m_bSuperArmor = false;
}

void Boss_Spike_FSM::skill_201100()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::skill_201200;
}

void Boss_Spike_FSM::skill_201100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_201100", 0.15f, false, 1.f);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Boss_Spike_FSM::skill_201200()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Boss_Spike_FSM::skill_201200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_201200", 0.15f, false, 1.f);

    m_vTurnVector = Calculate_TargetTurnVector();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_bSetPattern = false;

    AttackCollider_Off();

    m_bInvincible = true;
    m_bSuperArmor = false;
}

void Boss_Spike_FSM::Calculate_LipBoneMatrix()
{
    m_LipBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iLipBoneIndex) *
        _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

    m_vLipBonePos = _float4{ m_LipBoneMatrix.Translation().x, m_LipBoneMatrix.Translation().y, m_LipBoneMatrix.Translation().z , 1.f };
}

void Boss_Spike_FSM::Set_AttackSkill()
{
    if (m_tGroggyPatternTimer.fAccTime < m_tGroggyPatternTimer.fCoolTime)
    {
        _uint iRan = rand() % 7;

        while (true)
        {
            if (iRan == m_iPreAttack)
                iRan = rand() % 7;
            else
                break;
        }

        if (iRan == 0)
        {
            m_fAttackRange = 2.f;
            m_ePatternState = STATE::skill_1100;
            m_iPreAttack = 0;
        }
        else if (iRan == 1)
        {
            m_fAttackRange = 2.f;

            if (rand() % 2 == 0)
                m_ePatternState = STATE::skill_1200;
            else
                m_ePatternState = STATE::skill_1300;
            
            m_iPreAttack = 1;
        }
        else if (iRan == 2)
        {
            m_fAttackRange = 5.f;
            m_ePatternState = STATE::skill_1400;
            m_iPreAttack = 2;
        }
        else if (iRan == 3)
        {
            m_fAttackRange = 5.f;

            if (rand() % 2 == 0)
                m_ePatternState = STATE::skill_2100;
            else
                m_ePatternState = STATE::skill_2200;

            m_iPreAttack = 3;
        }
        else if (iRan == 4)
        {
            m_fAttackRange = 5.f;

            if (rand() % 2 == 0)
                m_ePatternState = STATE::skill_3100;
            else
                m_ePatternState = STATE::skill_3200;
            
            m_iPreAttack = 4;
        }
        else if (iRan == 5)
        {
            m_fAttackRange = 5.f;
            m_ePatternState = STATE::skill_7100;
            m_iPreAttack = 5;
        }
        else if (iRan == 6)
        {
            m_fAttackRange = 5.f;
            m_ePatternState = STATE::skill_8100;
            m_iPreAttack = 6;
        }
    }
    else
    {
        m_tGroggyPatternTimer.fAccTime = 0.f;
        m_fAttackRange = 10.f;
        m_ePatternState = STATE::skill_100000;
    }

    m_bSetPattern = true;
}

void Boss_Spike_FSM::Create_BossSpikeChair()
{
    shared_ptr<GameObject> Chair = make_shared<GameObject>();
    Chair->Add_Component(make_shared<Transform>());
    Chair->Get_Transform()->Set_WorldMat(m_pOwner.lock()->Get_Transform()->Get_WorldMatrix());
    _float4 vChairPos = m_vChairBonePos + _float4{ 0.f,1.3f,0.f,0.f } + (Get_Transform()->Get_State(Transform_State::LOOK) * -0.2f);
    Chair->Get_Transform()->Set_State(Transform_State::POS, vChairPos);

    {
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

        shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
        {
            shared_ptr<Model> model = RESOURCES.Get<Model>(L"D_P_Spike_Chair_01");
            renderer->Set_Model(model);
        }
        Chair->Add_Component(renderer);
        Chair->Set_Name(L"Boss_Spike_Chair");
    }

    EVENTMGR.Create_Object(Chair);

}

void Boss_Spike_FSM::Set_Gaze()
{
    if (Is_AnimFinished())
    {
        _uint iRan = 0;

        if (Target_In_GazeCheckRange())
            iRan = rand() % 4;
        else
            iRan = rand() % 5;

        if (iRan == 0)
            m_eCurState = STATE::gaze_b;
        else if (iRan == 1)
            m_eCurState = STATE::gaze_l;
        else if (iRan == 2)
            m_eCurState = STATE::gaze_r;
        else if (iRan == 3)
            m_eCurState = STATE::b_idle;
        else if (iRan == 4)
            m_eCurState = STATE::gaze_f;
    
        if (m_pOwner.lock()->Get_HpRatio() <= 0.3f)
        {
            if (!m_bLastPattern)
                m_eCurState = STATE::skill_6100;
        }
    }
}

void Boss_Spike_FSM::DeadSetting()
{
    if (m_bIsDead)
    {
        if (!m_bLastPattern)
        {
            m_eCurState = STATE::skill_6100;
        }
        else
        {
            m_bInvincible = true;

            m_eCurState = STATE::SQ_Die;
        }
    }
}

void Boss_Spike_FSM::Create_CounterMotionTrail()
{
    for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
    {
        material->Get_MaterialDesc().emissive = Color(0.f, 0.f, 0.f, 1.f);
    }

    m_pOwner.lock()->Get_Script<CounterMotionTrailScript>()->Init();
}


_float3 Boss_Spike_FSM::Calculate_TargetTurnVector()
{
    if (m_pTarget.expired())
        return _float3(0.f);
    else
        return m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS).xyz() - m_pOwner.lock()->Get_Transform()->Get_State(Transform_State::POS).xyz();
}

_float Boss_Spike_FSM::CamDistanceLerp(_float fStart, _float fEnd, _float fRatio)
{
    return fStart * (1.f - fRatio) + fEnd * fRatio;
}
