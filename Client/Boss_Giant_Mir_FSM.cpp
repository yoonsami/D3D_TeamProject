#include "pch.h"
#include "Boss_Giant_Mir_FSM.h"
#include "ModelAnimator.h"
#include "SphereCollider.h"
#include "AttackColliderInfoScript.h"
#include "Model.h"
#include "CharacterController.h"
#include "CounterMotionTrailScript.h"
#include "MainCameraScript.h"
#include "UiDamageCreate.h"
#include "DragonBall_FSM.h"
#include "DragonBallMonster_FSM.h"
#include "UIBossHpBar.h"
#include "SimpleMath.h"
#include "DestroyBuilding_FSM.h"
#include "Boss_Giant_Mir_Parts_FSM.h"
#include "FSM.h"
#include "ObjectDissolveCreate.h"
#include "RigidBody.h"
#include "FloorSkill_Script.h"
#include "OBBBoxCollider.h"
#include "ModelRenderer.h"
#include "TimerScript.h"

HRESULT Boss_Giant_Mir_FSM::Init()
{
    if (!m_bInitialize)
    {
        auto animator = Get_Owner()->Get_Animator();
        if (animator)
        {
            animator->Set_CurrentAnim(L"SQ_Spawn", false, 1.f);
            m_eCurState = STATE::SQ_Spawn;
        }

        if (!m_pTarget.expired())
        {
            _float3 vLookPos = _float3{ -0.25f, 0.f, -57.f };
            Get_Transform()->Set_LookDir(vLookPos);
        }


        // 0, 2, 2
        _float4 vPos = Get_Transform()->Get_State(Transform_State::POS) +
                       Get_Transform()->Get_State(Transform_State::LOOK) * -2.f +
                       Get_Transform()->Get_State(Transform_State::UP) * 2.f;

        Get_Transform()->Set_State(Transform_State::POS, vPos);

        m_vFirstPos = vPos;

        if (!m_pTarget.expired())
        {
            m_vSetPlayerPos = m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS);
        }

        m_iHeadBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Bone067");
        m_iMouseBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Bone062");
        m_iTailBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"SOXNFB_Bone045");
        m_iStomachBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"SOXNFB_Bone006");
        m_iLfootBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Bip001-L-Foot");
  
        HeadBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iHeadBoneIndex) *
            _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

        StomachBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iStomachBoneIndex) *
            _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

        LfootBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iLfootBoneIndex) *
            _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();


        m_vHeadCamPos = m_vHeadBonePos + (Get_Transform()->Get_State(Transform_State::LOOK) * 30.f);

        m_pCamera = CUR_SCENE->Get_MainCamera();

        m_fNormalAttack_AnimationSpeed = 1.5f;

        m_fRunSpeed = 4.f;
        m_fKnockDownSpeed = 4.f;

        if (!m_pOwner.expired())
            m_pOwner.lock()->Set_MaxHp(300.f);


        Create_Giant_Mir_Collider();

        m_bInitialize = true;
    }

    return S_OK;
}

void Boss_Giant_Mir_FSM::Tick()
{
    DeadCheck();

    State_Tick();

    Update_Collider();
}

void Boss_Giant_Mir_FSM::State_Tick()
{
    State_Init();

    m_iCurFrame = Get_CurFrame();

    if (m_bSummonMeteor)
        Create_Meteor();

    switch (m_eCurState)
    {
    case STATE::SQ_Spawn:
        SQ_Spawn();
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
    case STATE::SQ_Leave_Groggy_Start:
        SQ_Leave_Groggy_Start();
        break;
    case STATE::SQ_Leave_Groggy_End:
        SQ_Leave_Groggy_End();
        break;
    case STATE::SQ_Leave:
        SQ_Leave();
        break;
    case STATE::b_idle:
        b_idle();
        break;
    case STATE::skill_1100:
        skill_1100();
        break;
    case STATE::skill_1200:
        skill_1200();
        break;
    case STATE::skill_2100:
        skill_2100();
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
    case STATE::skill_200000:
        skill_200000();
        break;
    case STATE::skill_200100:
        skill_200100();
        break;

    }


    if (m_iPreFrame != m_iCurFrame)
        m_iPreFrame = m_iCurFrame;
}

void Boss_Giant_Mir_FSM::State_Init()
{
    if (m_eCurState != m_ePreState)
    {
        switch (m_eCurState)
        {
        case STATE::SQ_Spawn:
            SQ_Spawn_Init();
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
        case STATE::SQ_Leave_Groggy_Start:
            SQ_Leave_Groggy_Start_Init();
            break;
        case STATE::SQ_Leave_Groggy_End:
            SQ_Leave_Groggy_End_Init();
            break;
        case STATE::SQ_Leave:
            SQ_Leave_Init();
            break;
        case STATE::b_idle:
            b_idle_Init();
            break;
        case STATE::skill_1100:
            skill_1100_Init();
            break;
        case STATE::skill_1200:
            skill_1200_Init();
            break;
        case STATE::skill_2100:
            skill_2100_Init();
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
        case STATE::skill_200000:
            skill_200000_Init();
            break;
        case STATE::skill_200100:
            skill_200100_Init();
            break;
        }
        m_ePreState = m_eCurState;
    }
}

void Boss_Giant_Mir_FSM::Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget)
{
    //Calculate Damage 
    m_pOwner.lock()->Get_Hurt(fDamage);

    CUR_SCENE->Get_UI(L"UI_Damage_Controller")->Get_Script<UiDamageCreate>()->Create_Damage_Font(Get_Owner(), fDamage);

    _float3 vMyPos = Get_Transform()->Get_State(Transform_State::POS).xyz();
    _float3 vOppositePos = pLookTarget->Get_Transform()->Get_State(Transform_State::POS).xyz();

    m_vHitDir = vOppositePos - vMyPos;
    m_vHitDir.y = 0.f;
    m_vHitDir.Normalize();

    if (skillname == NORMAL_ATTACK || skillname == NORMAL_SKILL)
    {
        
    }
    else if (skillname == KNOCKBACK_ATTACK || skillname == KNOCKBACK_SKILL)
    {
        
    }
    else if (skillname == KNOCKDOWN_ATTACK || skillname == KNOCKDOWN_SKILL)
    {
        
    }
    else if (skillname == AIRBORNE_ATTACK || skillname == AIRBORNE_SKILL)
    {
        
    }
}

void Boss_Giant_Mir_FSM::Set_State(_uint iIndex)
{
    m_eCurState = (STATE)iIndex;
}

void Boss_Giant_Mir_FSM::SQ_Spawn()
{
    Calculate_IntroHeadCam();

    if (m_iCurFrame < 70)
    {
        if (Init_CurFrame(56))
        {
            if (!m_pOwner.expired())
                m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(1.5f);
        }

        if (!m_pCamera.expired())
        {
            m_fIntroCamDistance = CamDistanceLerp(15.f, 7.f, m_fCamRatio);
            
            m_fCamRatio += fDT * 0.5f;
            
            if (m_fCamRatio >= 1.f)
                m_fCamRatio = 1.f;
            
            _float3 vLookPos = Get_Transform()->Get_State(Transform_State::POS).xyz() +
                               Get_Transform()->Get_State(Transform_State::UP) * 15.f;
            
            _float3 vDir = m_vIntroCamPos - vLookPos;
            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(5.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(vLookPos);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(1.f, vDir, m_fIntroCamDistance);
        }
    }
    else 
    {
        if (Init_CurFrame(70))
        {
            if (!m_pCamera.expired())
                m_vCamStopPos = m_pCamera.lock()->Get_Transform()->Get_State(Transform_State::POS);
        }
        else if (Init_CurFrame(235))
        {
            Destroy_MapObject();
        }

        if (!m_pCamera.expired())
        {
            m_pCamera.lock()->Get_Transform()->Set_State(Transform_State::POS, m_vCamStopPos);
            m_vHeadCamDir = m_vCamStopPos - m_vHeadBonePos.xyz();
            m_vHeadCamDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vHeadBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedTime(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedDir(m_vHeadCamDir);
        }
    }

    if (Is_AnimFinished())
    {
        g_bCutScene = false;
        m_eCurState = STATE::b_idle;

        //Add_BossHp UI
        if (!m_pOwner.expired())
        {
            if (!m_pOwner.lock()->Get_Script<UIBossHpBar>())
            {
                auto pScript = make_shared<UIBossHpBar>(BOSS::MIR);
                m_pOwner.lock()->Add_Component(pScript);
                pScript->Init();
            }
        }

        for (size_t i = 0; i < m_vecDestroyObject.size(); i++)
        {
            EVENTMGR.Delete_Object(m_vecDestroyObject[i].lock());
        }
    }
}

void Boss_Giant_Mir_FSM::SQ_Spawn_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"SQ_Spawn", 0.3f, false, 1.f);

    Calculate_IntroHeadCam();
    
    m_fCamRatio = 0.f;
    g_bCutScene = true;
    
    if (!m_pCamera.expired())
        m_pCamera.lock()->Get_Transform()->Set_State(Transform_State::POS, m_vIntroCamPos);
}

void Boss_Giant_Mir_FSM::groggy_start()
{
    if (Init_CurFrame(26))
        Set_RigidBodyActivate(false);
    else if (Init_CurFrame(80))
        Set_RigidBodyActivate(true);


    if (m_iCurFrame >= 58)
    {
        m_vFirstPos.y -= fDT;

        if (m_vFirstPos.y <= 0.f)
            m_vFirstPos.y = 0.f;

        _float4 vPos = m_vFirstPos;

        Get_Transform()->Set_State(Transform_State::POS, vPos);
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::groggy_loop;

    DeadSetting();
}

void Boss_Giant_Mir_FSM::groggy_start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"groggy_start", 0.4f, false, 1.f);

    AttackCollider_Off();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_tBreathCoolTime.fAccTime = 0.f;
    m_iCurMeteorCnt = 0;
    m_iPreAttack = 100;

    m_vFirstPos.y = 2.f;

    Set_Invincible(false);

    m_bDragonBall = false; 
    m_bSummonMeteor = false;

    Set_RigidBodyActivate(true);
}

void Boss_Giant_Mir_FSM::groggy_loop()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::groggy_end;

    DeadSetting();
}

void Boss_Giant_Mir_FSM::groggy_loop_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"groggy_loop", 0.1f, false, 0.5f);
}

void Boss_Giant_Mir_FSM::groggy_end()
{
    if (Init_CurFrame(38))
        Set_RigidBodyActivate(false);
    else if (Init_CurFrame(83))
        Set_RigidBodyActivate(true);


    if (m_iCurFrame >= 15)
    {
        m_vFirstPos.y += fDT;

        if (m_vFirstPos.y >= 2.f)
            m_vFirstPos.y = 2.f;
        _float4 vPos = m_vFirstPos;

        Get_Transform()->Set_State(Transform_State::POS, vPos);
    }

    if (Get_FinalFrame() - 10 < m_iCurFrame)
        m_eCurState = STATE::b_idle;

    DeadSetting();
}

void Boss_Giant_Mir_FSM::groggy_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"groggy_end", 0.1f, false, 1.f);
}

void Boss_Giant_Mir_FSM::SQ_Leave_Groggy_Start()
{
    Calculate_LeaveHeadCam();

    if (m_iCurFrame >= 58)
    {
        m_vFirstPos.y -= fDT;

        if (m_vFirstPos.y <= 0.f)
            m_vFirstPos.y = 0.f;

        _float4 vPos = m_vFirstPos;

        Get_Transform()->Set_State(Transform_State::POS, vPos);
    }

    if (!m_pCamera.expired())
    {
        m_fIntroCamDistance = CamDistanceLerp(8.f, 15.f, m_fCamRatio);

        m_fCamRatio += fDT * 0.5f;

        if (m_fCamRatio >= 1.f)
            m_fCamRatio = 1.f;

        _float3 vLookPos = Get_Transform()->Get_State(Transform_State::POS).xyz() +
            Get_Transform()->Get_State(Transform_State::UP) * 10.f;

        _float3 vDir = m_vLeaveCamPos - vLookPos;
        vDir.Normalize();

        m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(5.f);
        m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(vLookPos);
        m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(1.f, vDir, m_fIntroCamDistance);
    }

    if (Is_AnimFinished())
    {
        m_eCurState = STATE::SQ_Leave_Groggy_End;
    }
}

void Boss_Giant_Mir_FSM::SQ_Leave_Groggy_Start_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_CurrentAnim(L"groggy_start", false, 1.f);

    m_tAttackCoolTime.fAccTime = 0.f;
    m_tBreathCoolTime.fAccTime = 0.f;

    if (!m_pTailCollider.expired())
        EVENTMGR.Delete_Object(m_pTailCollider.lock());

    if (!m_pStomachCollider.expired())
        EVENTMGR.Delete_Object(m_pStomachCollider.lock());

    if (!m_pLfootCollider.expired())
        EVENTMGR.Delete_Object(m_pLfootCollider.lock());

    if (!m_pFootRigidBody.expired())
        EVENTMGR.Delete_Object(m_pFootRigidBody.lock());

    Get_Transform()->Set_State(Transform_State::POS, _float4{0.f, 1.5f, 0.f, 1.f});

    Calculate_LeaveHeadCam();
    
    if (!m_pCamera.expired())
        m_pCamera.lock()->Get_Transform()->Set_State(Transform_State::POS, m_vLeaveCamPos);

    m_fCamRatio = 0.f;

    g_bCutScene = true;
    
    m_bSummonMeteor = false;

    Set_Invincible(true);

    m_vFirstPos.y = 2.f;

    if (!m_pTarget.expired())
    {
        m_pTarget.lock()->Get_Animator()->Set_RenderState(false);
        
        if (m_pTarget.lock()->Get_FSM()->Get_Weapon())
            m_pTarget.lock()->Get_FSM()->Get_Weapon()->Get_ModelRenderer()->Set_RenderState(false);
    }

    if (!m_pOwner.expired())
    {
        if (m_pOwner.lock()->Get_Script<UIBossHpBar>())
            m_pOwner.lock()->Get_Script<UIBossHpBar>()->Remove_HpBar();
    }
}

void Boss_Giant_Mir_FSM::SQ_Leave_Groggy_End()
{
    Calculate_LeaveHeadCam();

    if (m_iCurFrame >= 15)
    {
        m_vFirstPos.y += fDT;

        if (m_vFirstPos.y >= 2.f)
            m_vFirstPos.y = 2.f;
        _float4 vPos = m_vFirstPos;

        Get_Transform()->Set_State(Transform_State::POS, vPos);
    }

    if (!m_pCamera.expired())
    {
        _float3 vLookPos = Get_Transform()->Get_State(Transform_State::POS).xyz() +
            Get_Transform()->Get_State(Transform_State::UP) * 10.f;

        _float3 vDir = m_vLeaveCamPos - vLookPos;
        vDir.Normalize();

        m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(5.f);
        m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(vLookPos);
        m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(1.f, vDir, m_fIntroCamDistance);
    }



    if (Is_AnimFinished())
        m_eCurState = STATE::SQ_Leave;

}

void Boss_Giant_Mir_FSM::SQ_Leave_Groggy_End_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_CurrentAnim(L"groggy_end", false, 1.f);

    Calculate_LeaveHeadCam();

    if (!m_pCamera.expired())
    {
        _float3 vLookPos = Get_Transform()->Get_State(Transform_State::POS).xyz() +
            Get_Transform()->Get_State(Transform_State::UP) * 10.f;

        _float3 vDir = m_vLeaveCamPos - vLookPos;
        vDir.Normalize();

        m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(5.f);
        m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(vLookPos);
        m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(1.f, vDir, m_fIntroCamDistance);
    }

}

void Boss_Giant_Mir_FSM::SQ_Leave()
{
    Calculate_LeaveHeadCam();

    if (!m_pCamera.expired())
    {
        if (m_iCurFrame < 50)
        {
            m_fIntroCamDistance = CamDistanceLerp(15.f, 90.f, m_fCamRatio);

            m_fCamRatio += fDT * 0.5f;

            if (m_fCamRatio >= 1.f)
                m_fCamRatio = 1.f;

            _float3 vLookPos = Get_Transform()->Get_State(Transform_State::POS).xyz() +
                Get_Transform()->Get_State(Transform_State::UP) * 10.f;

            _float3 vDir = m_vLeaveCamPos - vLookPos;
            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(5.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(vLookPos);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(1.f, vDir, m_fIntroCamDistance);
        }
        else if (m_iCurFrame >= 50 && m_iCurFrame < 180)
        {
            if (Init_CurFrame(50))
            {
                m_vCamStopPos = m_pCamera.lock()->Get_Transform()->Get_State(Transform_State::POS);
                
                m_vLeaveCamLookPos = Get_Transform()->Get_State(Transform_State::POS).xyz() +
                    Get_Transform()->Get_State(Transform_State::UP) * 10.f;
            }

            m_vLeaveCamLookPos.y += fDT * 25.f;

            //m_pCamera.lock()->Get_Transform()->Set_State(Transform_State::POS, m_vCamStopPos);
            _float3 vDir = m_vCamStopPos - m_vLeaveCamLookPos.xyz();
            vDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(2.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vLeaveCamLookPos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedTime(0.2f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedDir(vDir);
        }
        else
        {
            if (Init_CurFrame(180))
            {
                if (!m_pTarget.expired())
                {
                    m_pTarget.lock()->Get_CharacterController()->Get_Actor()->setFootPosition({ m_vSetPlayerPos.x,  m_vSetPlayerPos.y, m_vSetPlayerPos.z });

                    m_pTarget.lock()->Get_Animator()->Set_RenderState(true);

                    if (m_pTarget.lock()->Get_FSM()->Get_Weapon())
                        m_pTarget.lock()->Get_FSM()->Get_Weapon()->Get_ModelRenderer()->Set_RenderState(true);
                }
                
                _float4 vCamPos = m_vSetPlayerPos +
                    m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::UP) * 5.f +
                    m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::LOOK) * 5.f;

                m_pCamera.lock()->Get_Transform()->Set_State(Transform_State::POS, vCamPos);
                m_pCamera.lock()->Get_Transform()->LookAt(m_vSetPlayerPos);
                g_bCutScene = false;
            }
        }
    }
   
    if (Is_AnimFinished())
    {
        g_bCutScene = false;

        if (!m_pOwner.expired())
            EVENTMGR.Delete_Object(m_pOwner.lock());

        if (!m_pTailCollider.expired())
            EVENTMGR.Delete_Object(m_pTailCollider.lock());

        if (!m_pStomachCollider.expired())
            EVENTMGR.Delete_Object(m_pStomachCollider.lock());

        if (!m_pLfootCollider.expired())
            EVENTMGR.Delete_Object(m_pLfootCollider.lock());

        if (!m_pFootRigidBody.expired())
            EVENTMGR.Delete_Object(m_pFootRigidBody.lock());
    }
}

void Boss_Giant_Mir_FSM::SQ_Leave_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_CurrentAnim(L"SQ_Leave", false, 1.f);
   
    m_fCamRatio = 0.f;
    m_tAttackCoolTime.fAccTime = 0.f;

    g_bCutScene = true;
    Calculate_LeaveHeadCam();
}

void Boss_Giant_Mir_FSM::b_idle()
{
    m_tAttackCoolTime.fAccTime += fDT;

    if (m_tAttackCoolTime.fAccTime >= m_tAttackCoolTime.fCoolTime)
       Set_AttackPattern();

    DeadSetting();
}

void Boss_Giant_Mir_FSM::b_idle_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_idle", 0.6f, true, 1.f);
    
    m_tAttackCoolTime.fAccTime = 0.f;
    m_tBreathCoolTime.fAccTime = 0.f;

    if (m_ePreState == STATE::groggy_end)
        Set_Invincible(true);
    
    //Add_BossHp UI
    if (!m_pOwner.expired())
    {
        if (!m_pOwner.lock()->Get_Script<UIBossHpBar>())
        {
            auto pScript = make_shared<UIBossHpBar>(BOSS::MIR);
            m_pOwner.lock()->Add_Component(pScript);
            pScript->Init();
        }
    }   
}

void Boss_Giant_Mir_FSM::skill_1100()
{
    if (Init_CurFrame(119))
        Add_And_Set_Effect(L"GiantMir_1100");

    _float4x4 MouseBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iMouseBoneIndex)
        * _float4x4::CreateRotationX(XMConvertToRadians(-90.f))
        * _float4x4::CreateScale(0.01f)
        * _float4x4::CreateRotationY(XM_PI)
        * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();
    _float4 vBonePos = _float4{ MouseBoneMatrix.Translation().x, MouseBoneMatrix.Translation().y, MouseBoneMatrix.Translation().z , 1.f };
    _float4 vDir = Get_Transform()->Get_State(Transform_State::LOOK) +
        Get_Transform()->Get_State(Transform_State::RIGHT) * -1.f;
    vDir.Normalize();
    _float3 vLook = _float3(vDir);
    _float3 vRight = vLook.Cross(_float3::Up);
    _float4x4 matFinal = _float4x4(_float4(vRight, 0.f), _float4(0.f, 1.f, 0.f, 0.f), _float4(vLook, 0.f), vBonePos);

    Update_GroupEffectWorldPos(matFinal);

    if (Init_CurFrame(51))
        Set_RigidBodyActivate(false);
    else if (Init_CurFrame(192))
        Set_RigidBodyActivate(true);


    m_tBreathCoolTime.fAccTime += fDT;

    if (m_iCurFrame >= 126 && m_iCurFrame <= 163)
    {
        if (m_tBreathCoolTime.fAccTime >= m_tBreathCoolTime.fCoolTime)
        {
            m_tBreathCoolTime.fAccTime = 0.f;

            MouseBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iMouseBoneIndex) *
                _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

            _float4 vBonePos = _float4{ MouseBoneMatrix.Translation().x, MouseBoneMatrix.Translation().y, MouseBoneMatrix.Translation().z , 1.f };

            vBonePos = vBonePos + Get_Transform()->Get_State(Transform_State::LOOK);
            vBonePos.y = 0.f;

            _float4 vDir = Get_Transform()->Get_State(Transform_State::LOOK) +
                           Get_Transform()->Get_State(Transform_State::RIGHT) * -1.f;
            vDir.Normalize();

            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = vDir;
            desc.fMoveSpeed = 20.f;
            desc.fLifeTime = 1.5f;
            desc.fLimitDistance = 50.f;

            _float4 vSkillPos = vBonePos;

            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 6.f, desc, KNOCKBACK_ATTACK, 10.f);
        }
    }

    if (m_iCurFrame >= Get_FinalFrame() - 5 )
        m_eCurState = STATE::b_idle;    
    

}

void Boss_Giant_Mir_FSM::skill_1100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
    m_tBreathCoolTime.fAccTime = 0.f;
}

void Boss_Giant_Mir_FSM::skill_1200()
{
    if (Init_CurFrame(182))
    {
        Add_GroupEffectOwner(L"GiantMir_1200_floor", _float3(20.f, -2.f, -5.f), false);
    }

    if (Init_CurFrame(131))
        Set_RigidBodyActivate(false);
    else if (Init_CurFrame(182))
        Set_RigidBodyActivate(true);

    if (Init_CurFrame(191))
        TailAttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    else if (Init_CurFrame(194))
        TailAttackCollider_Off();
    else if (Init_CurFrame(244))
    {
        if (!m_pTarget.expired())
        {
            INSTALLATIONSKILLDESC desc;
            desc.fAttackTickTime = 3.f; //JANGPAN TIME  

            desc.iLimitAttackCnt = 1;
            desc.strAttackType = KNOCKDOWN_SKILL;
            desc.strLastAttackType = KNOCKDOWN_SKILL;
            desc.bFirstAttack = false;
            desc.fAttackDamage = 5.f;
            desc.fLastAttackDamage = 5.f;

            _float4 vSkillPos = m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) + _float4{ 0.001f, 0.f,0.f,0.f };

            Create_InstallationSkillCollider(Monster_Skill,L"Boss_Giant_Mir_InstallationSkillCollider",vSkillPos, 3.f, desc);
        }
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;

}

void Boss_Giant_Mir_FSM::skill_1200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1200", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
    m_tBreathCoolTime.fAccTime = 0.f;
}

void Boss_Giant_Mir_FSM::skill_2100()
{
    if (Init_CurFrame(179))
        Add_And_Set_Effect(L"GiantMir_1100");

    _float4x4 MouseBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iMouseBoneIndex)
        * _float4x4::CreateRotationX(XMConvertToRadians(-90.f))
        * _float4x4::CreateScale(0.01f)
        * _float4x4::CreateRotationY(XM_PI)
        * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();
    _float4 vBonePos = _float4{ MouseBoneMatrix.Translation().x, MouseBoneMatrix.Translation().y, MouseBoneMatrix.Translation().z , 1.f };
    _float4 vDir = Get_Transform()->Get_State(Transform_State::LOOK);
    vDir.Normalize();
    vBonePos += vDir * 1.f;
    _float3 vLook = _float3(vDir);
    _float3 vRight = vLook.Cross(_float3::Up);
    _float4x4 matFinal = _float4x4(_float4(vRight, 0.f), _float4(0.f, 1.f, 0.f, 0.f), _float4(vLook, 0.f), vBonePos);

    Update_GroupEffectWorldPos(matFinal);

    m_tBreathCoolTime.fAccTime += fDT;

    if (Init_CurFrame(19))
        Set_RigidBodyActivate(false);
    else if (Init_CurFrame(330))
        Set_RigidBodyActivate(true);



    if (m_iCurFrame >= 184 && m_iCurFrame <= 260)
    {
        if (m_tBreathCoolTime.fAccTime >= m_tBreathCoolTime.fCoolTime)
        {
            m_tBreathCoolTime.fAccTime = 0.f;

            MouseBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iMouseBoneIndex) *
                _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

            _float4 vBonePos = _float4{ MouseBoneMatrix.Translation().x, MouseBoneMatrix.Translation().y, MouseBoneMatrix.Translation().z , 1.f };

            vBonePos = vBonePos + Get_Transform()->Get_State(Transform_State::LOOK);
            vBonePos.y = 0.f;

            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 20.f;
            desc.fLifeTime = 1.f;
            desc.fLimitDistance = 10.f;

            _float4 vSkillPos = vBonePos;

            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 6.f, desc, NORMAL_ATTACK, 10.f);
        }
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Boss_Giant_Mir_FSM::skill_2100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_2100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
    m_tBreathCoolTime.fAccTime = 0.f;
}

void Boss_Giant_Mir_FSM::skill_7100()
{
    if (Init_CurFrame(52))
        Set_RigidBodyActivate(false);
    else if (Init_CurFrame(195))
        Set_RigidBodyActivate(true);

    if (Init_CurFrame(142))
        Create_DragonBall();

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Boss_Giant_Mir_FSM::skill_7100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_7100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
    m_tBreathCoolTime.fAccTime = 0.f;

    m_bDragonBall = true;

    Set_Invincible(true);
}

void Boss_Giant_Mir_FSM::skill_8100()
{
    if (Init_CurFrame(52))
        Set_RigidBodyActivate(false);
    else if (Init_CurFrame(195))
        Set_RigidBodyActivate(true);

    if (Init_CurFrame(142))
    {
        m_bSummonMeteor = true;
        m_iLimitMeteorCnt = rand() % 3 + 3;
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Boss_Giant_Mir_FSM::skill_8100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_8100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
    m_tBreathCoolTime.fAccTime = 0.f;
    m_iCurMeteorCnt = 0;
}

void Boss_Giant_Mir_FSM::skill_100000()
{
    if (Init_CurFrame(38))
        Set_RigidBodyActivate(false);

    if (Is_AnimFinished())
        m_eCurState = STATE::skill_100100;
}

void Boss_Giant_Mir_FSM::skill_100000_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100000", 0.15f, false, 2.f);

    m_tAttackCoolTime.fAccTime = 0.f;
    m_tBreathCoolTime.fAccTime = 0.f;
}

void Boss_Giant_Mir_FSM::skill_100100()
{
    if (Init_CurFrame(24))
        Add_And_Set_Effect(L"GiantMir_2100");

    _float4x4 MouseBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iMouseBoneIndex)
        * _float4x4::CreateRotationX(XMConvertToRadians(-90.f))
        * _float4x4::CreateScale(0.01f)
        * _float4x4::CreateRotationY(XM_PI)
        * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();
    _float4 vBonePos = _float4{ MouseBoneMatrix.Translation().x, MouseBoneMatrix.Translation().y, MouseBoneMatrix.Translation().z , 1.f };
    _float4 vDir = Get_Transform()->Get_State(Transform_State::LOOK);
    vDir.Normalize();
    vBonePos += vDir * 1.f;
    _float3 vLook = _float3(vDir);
    _float3 vRight = vLook.Cross(_float3::Up);
    _float4x4 matFinal = _float4x4(_float4(vRight, 0.f), _float4(0.f, 1.f, 0.f, 0.f), _float4(vLook, 0.f), vBonePos);

    Update_GroupEffectWorldPos(matFinal);

    if (Init_CurFrame(179))
        Set_RigidBodyActivate(true);


    m_tBreathCoolTime.fAccTime += fDT;

    if (m_iCurFrame >= 24 && m_iCurFrame <= 98)
    {
        if (m_tBreathCoolTime.fAccTime >= m_tBreathCoolTime.fCoolTime)
        {
            m_tBreathCoolTime.fAccTime = 0.f;

            MouseBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iMouseBoneIndex) *
                _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

            _float4 vBonePos = _float4{ MouseBoneMatrix.Translation().x, MouseBoneMatrix.Translation().y, MouseBoneMatrix.Translation().z , 1.f };

            vBonePos = vBonePos + Get_Transform()->Get_State(Transform_State::LOOK);
            vBonePos.y = 0.f;

            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 20.f;
            desc.fLifeTime = 2.f;
            desc.fLimitDistance = 50.f;

            _float4 vSkillPos = vBonePos;

            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 5.f, desc, KNOCKBACK_ATTACK, 10.f);
        }
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Boss_Giant_Mir_FSM::skill_100100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
    m_tBreathCoolTime.fAccTime = 0.f;
}

void Boss_Giant_Mir_FSM::skill_200000()
{
    if (Init_CurFrame(10))
        Set_RigidBodyActivate(false);


    if (Is_AnimFinished())
        m_eCurState = STATE::skill_200100;
}

void Boss_Giant_Mir_FSM::skill_200000_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200000", 0.15f, false, 2.f);

    m_tAttackCoolTime.fAccTime = 0.f;
    m_tBreathCoolTime.fAccTime = 0.f;
}

void Boss_Giant_Mir_FSM::skill_200100()
{
    if (Init_CurFrame(47))
        Add_And_Set_Effect(L"GiantMir_2100");

    _float4x4 MouseBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iMouseBoneIndex)
        * _float4x4::CreateRotationX(XMConvertToRadians(-90.f))
        * _float4x4::CreateScale(0.01f)
        * _float4x4::CreateRotationY(XM_PI)
        * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();
    _float4 vBonePos = _float4{ MouseBoneMatrix.Translation().x, MouseBoneMatrix.Translation().y, MouseBoneMatrix.Translation().z , 1.f };
    _float4 vDir = Get_Transform()->Get_State(Transform_State::LOOK);
    vDir.Normalize();
    vBonePos += vDir * 1.f;
    _float3 vLook = _float3(vDir);
    _float3 vRight = vLook.Cross(_float3::Up);
    _float4x4 matFinal = _float4x4(_float4(vRight, 0.f), _float4(0.f, 1.f, 0.f, 0.f), _float4(vLook, 0.f), vBonePos);

    Update_GroupEffectWorldPos(matFinal);

    if (Init_CurFrame(201))
        Set_RigidBodyActivate(true);

    m_tBreathCoolTime.fAccTime += fDT;

    if (m_iCurFrame >= 47 && m_iCurFrame <= 117)
    {
        if (m_tBreathCoolTime.fAccTime >= m_tBreathCoolTime.fCoolTime)
        {
            m_tBreathCoolTime.fAccTime = 0.f;

            MouseBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iMouseBoneIndex) *
                _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

            _float4 vBonePos = _float4{ MouseBoneMatrix.Translation().x, MouseBoneMatrix.Translation().y, MouseBoneMatrix.Translation().z , 1.f };

            vBonePos = vBonePos + Get_Transform()->Get_State(Transform_State::LOOK);
            vBonePos.y = 0.f;

            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 20.f;
            desc.fLifeTime = 2.f;
            desc.fLimitDistance = 50.f;

            _float4 vSkillPos = vBonePos;

            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 5.f, desc, KNOCKBACK_ATTACK, 10.f);
        }
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::b_idle;
}

void Boss_Giant_Mir_FSM::skill_200100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
    m_tBreathCoolTime.fAccTime = 0.f;
}

void Boss_Giant_Mir_FSM::Create_Meteor()
{
    m_tMeteorCoolTime.fAccTime += fDT;

    if (m_tMeteorCoolTime.fAccTime >= m_tMeteorCoolTime.fCoolTime)
    {
        if (!m_pTarget.expired())
        {
            _float4 vPlayerPos = m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS);

            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = _float3{ 0.f,-1.f,0.f };
            desc.fMoveSpeed = 10.f;
            desc.fLifeTime = 1.f;
            desc.fLimitDistance = 20.f;

            for (_uint i = 0; i < 6; i++)
            {
                _float fOffSetX = ((rand() * 2 / _float(RAND_MAX) - 1) * (rand() % 10 + 5));
                _float fOffSetZ = ((rand() * 2 / _float(RAND_MAX) - 1) * (rand() % 10 + 5));

                _float4 vSkillPos = vPlayerPos + _float4{ fOffSetX, 13.5f, fOffSetZ, 0.f };

                Add_GroupEffectOwner(L"Mir_Meteor_Meteor", _float3(vSkillPos.x, vPlayerPos.y, vSkillPos.z), true);
                Add_GroupEffectOwner(L"Mir_Meteor_Floor", _float3(vSkillPos.x, vPlayerPos.y, vSkillPos.z), true);
                Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 1.f, desc, KNOCKDOWN_SKILL, 10.f);
            }

            m_tMeteorCoolTime.fAccTime = 0.f;
            m_iCurMeteorCnt++;
        }
    }

    if (m_iCurMeteorCnt >= m_iLimitMeteorCnt)
    {
        m_iCurMeteorCnt = 0;
        m_bSummonMeteor = false;
    }
}

void Boss_Giant_Mir_FSM::Create_Giant_Mir_Collider()
{
    shared_ptr<GameObject> tailCollider = make_shared<GameObject>();
    tailCollider->GetOrAddTransform();
    tailCollider->Add_Component(make_shared<SphereCollider>(14.f));
    tailCollider->Get_Collider()->Set_CollisionGroup(Monster_Attack);

    EVENTMGR.Create_Object(tailCollider);
    tailCollider->Get_Collider()->Set_Activate(false);

    tailCollider->Add_Component(make_shared<AttackColliderInfoScript>());
    tailCollider->Set_Name(L"Boss_Giant_Mir_TailCollider");
    tailCollider->Get_Script<AttackColliderInfoScript>()->Set_ColliderOwner(m_pOwner.lock());

    m_pTailCollider = tailCollider;

    shared_ptr<GameObject> StomachCollider = make_shared<GameObject>();
    StomachCollider->GetOrAddTransform();
    StomachCollider->Add_Component(make_shared<SphereCollider>(12.f));
    StomachCollider->Get_Collider()->Set_CollisionGroup(Monster_Body);
    StomachCollider->Add_Component(make_shared<Boss_Giant_Mir_Parts_FSM>());
    EVENTMGR.Create_Object(StomachCollider);
    StomachCollider->Get_Collider()->Set_Activate(true);

    StomachCollider->Set_Name(L"Boss_Giant_Mir_StomachCollider");
    StomachCollider->Get_FSM()->Set_Target(m_pOwner.lock());

    m_pStomachCollider = StomachCollider;

    shared_ptr<GameObject> LfootCollider = make_shared<GameObject>();
    LfootCollider->GetOrAddTransform();
    LfootCollider->Add_Component(make_shared<SphereCollider>(6.f));
    LfootCollider->Get_Collider()->Set_CollisionGroup(Monster_Body);
    LfootCollider->Add_Component(make_shared<Boss_Giant_Mir_Parts_FSM>());
    EVENTMGR.Create_Object(LfootCollider);
    LfootCollider->Get_Collider()->Set_Activate(true);

    LfootCollider->Set_Name(L"Boss_Giant_Mir_LfootCollider");
    LfootCollider->Get_FSM()->Set_Target(m_pOwner.lock());

    m_pLfootCollider = LfootCollider;

    shared_ptr<GameObject> rigidBodyObj = make_shared<GameObject>();
    rigidBodyObj->GetOrAddTransform()->Set_State(Transform_State::POS,{10.7f,3.f,28.5f,1.f});
	{
		_float3 vObjPos = rigidBodyObj->Get_Transform()->Get_State(Transform_State::POS).xyz();
		auto rigidBody = make_shared<RigidBody>();
        rigidBodyObj->Add_Component(rigidBody);
		rigidBody->Create_CapsuleRigidBody(vObjPos, 6.f, 6.f);
	}
    EVENTMGR.Create_Object(rigidBodyObj);
    m_pFootRigidBody = rigidBodyObj;

}

void Boss_Giant_Mir_FSM::Create_DragonBall()
{
    Add_GroupEffectOwner(L"GiantMir_7100", _float3(0.f, -2.3f, 0.5f), false);
    
    _uint iDragonBallIndex = 4;

    shared_ptr<GameObject> ObjDragonBall = make_shared<GameObject>();

    ObjDragonBall->Add_Component(make_shared<Transform>());

    ObjDragonBall->Get_Transform()->Set_State(Transform_State::POS, _float4(0.f,1.5f,0.f,1.f));
    {
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

        shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
        {
            shared_ptr<Model> model = RESOURCES.Get<Model>(L"Anim_P_R02_DecoBall_00_Idle");
            animator->Set_Model(model);
        }

        ObjDragonBall->Add_Component(animator);

        ObjDragonBall->Add_Component(make_shared<SphereCollider>(2.f)); //SphereCollider
        ObjDragonBall->Get_Collider()->Set_CollisionGroup(Monster_Body);
        ObjDragonBall->Get_Collider()->Set_Activate(true);

        wstring DragonBallName = L"Boss_Giant_Mir_DragonBall";
        ObjDragonBall->Set_Name(DragonBallName);
        ObjDragonBall->Set_DrawShadow(true);
        ObjDragonBall->Set_ObjectGroup(OBJ_MONSTER);
        ObjDragonBall->Add_Component(make_shared<DragonBallMonster_FSM>());
        ObjDragonBall->Get_FSM()->Set_Target(m_pOwner.lock());
        ObjDragonBall->Get_FSM()->Init();
        EVENTMGR.Create_Object(ObjDragonBall);

        //Add ObjectDissolveCreate
        ObjDragonBall->Add_Component(make_shared<ObjectDissolveCreate>(1.f));
        ObjDragonBall->Get_Script<ObjectDissolveCreate>()->Init();
    }
}

void Boss_Giant_Mir_FSM::Set_AttackPattern()
{
    _uint iRan = rand() % 6;

    while (true)
    {
        if (iRan == m_iPreAttack)
            iRan = rand() % 6;
        else
            break;
    }

    if (iRan == 0)
    {
        m_eCurState = STATE::skill_1100;
        m_iPreAttack = 0;
    }
    else if (iRan == 1)
    {
        m_eCurState = STATE::skill_1200;
        m_iPreAttack = 1;
    }
    else if (iRan == 2)
    {
        m_eCurState = STATE::skill_2100;
        m_iPreAttack = 2;
    }
    else if (iRan == 3)
    {
        m_eCurState = STATE::skill_8100;
        m_iPreAttack = 3;
    }
    else if (iRan == 4)
    {
        m_eCurState = STATE::skill_100000;
        m_iPreAttack = 4;
    }
    else if (iRan == 5)
    {
        m_eCurState = STATE::skill_200000;
        m_iPreAttack = 5;
    }

    if (!m_bDragonBall)
        m_eCurState = STATE::skill_7100;

    m_tAttackCoolTime.fCoolTime = _float(rand() % 2) + 1.5f;
}

void Boss_Giant_Mir_FSM::Setting_DragonBall()
{
    _uint iDragonBallIndex = 4;
    for (_uint i = 0; i < 3; i++)
    {
        wstring DragonBallName = L"Anim_P_R02_DecoBall_00_Idle-1";
        DragonBallName += to_wstring(iDragonBallIndex);
        auto DragonBall = CUR_SCENE->Get_GameObject(DragonBallName);

        if (DragonBall == nullptr)
            continue;

        DragonBall->Add_Component(make_shared<SphereCollider>(2.f)); //SphereCollider
        DragonBall->Get_Collider()->Set_CollisionGroup(MAPObject);
        DragonBall->Get_Collider()->Set_Activate(true);

        DragonBall->Set_DrawShadow(true);
        DragonBall->Set_ObjectGroup(OBJ_MAPOBJECT);
        DragonBall->Add_Component(make_shared<DragonBall_FSM>());
        DragonBall->Get_FSM()->Set_Target(m_pOwner.lock());

        iDragonBallIndex++;
    }
}

void Boss_Giant_Mir_FSM::Destroy_MapObject()
{
    if (CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_MiddleHouse_03_AHJ-49"))
    {
        _float4 vPos = CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_MiddleHouse_03_AHJ-49")->Get_Transform()->Get_State(Transform_State::POS);
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_MiddleHouse_03_AHJ-49"));
        
        shared_ptr<GameObject> DestroyBuilding = make_shared<GameObject>();
        DestroyBuilding->GetOrAddTransform()->Set_State(Transform_State::POS, vPos);
        DestroyBuilding->Get_Transform()->Scaled(_float3(2.f, 2.f,2.f));
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

        shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
        {
            shared_ptr<Model> model = RESOURCES.Get<Model>(L"Anim_Mir_Intro_v02_BuildCenter");
            animator->Set_Model(model);
        }

        DestroyBuilding->Add_Component(animator);

        DestroyBuilding->Add_Component(make_shared<DestroyBuilding_FSM>());
        DestroyBuilding->Get_FSM()->Init();
        EVENTMGR.Create_Object(DestroyBuilding);

        m_vecDestroyObject.push_back(DestroyBuilding);
    }

    if (CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_MiddleHouse_02_AHJ-50"))
    {
        _float4 vPos = CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_MiddleHouse_02_AHJ-50")->Get_Transform()->Get_State(Transform_State::POS);
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_MiddleHouse_02_AHJ-50"));

        shared_ptr<GameObject> DestroyBuilding = make_shared<GameObject>();
        DestroyBuilding->GetOrAddTransform()->Set_State(Transform_State::POS, vPos);
        DestroyBuilding->Get_Transform()->Scaled(_float3(2.f, 2.f, 2.f));
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

        shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
        {
            shared_ptr<Model> model = RESOURCES.Get<Model>(L"Anim_Mir_Intro_v02_BuildCenter");
            animator->Set_Model(model);
        }

        DestroyBuilding->Add_Component(animator);

        DestroyBuilding->Add_Component(make_shared<DestroyBuilding_FSM>());
        DestroyBuilding->Get_FSM()->Init();
        EVENTMGR.Create_Object(DestroyBuilding);

        m_vecDestroyObject.push_back(DestroyBuilding);
    }

    if (CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_MiddleHouse_01_AHJ-51"))
    {
        _float4 vPos = CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_MiddleHouse_01_AHJ-51")->Get_Transform()->Get_State(Transform_State::POS);
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_MiddleHouse_01_AHJ-51"));

        shared_ptr<GameObject> DestroyBuilding = make_shared<GameObject>();
        DestroyBuilding->GetOrAddTransform()->Set_State(Transform_State::POS, vPos);
        DestroyBuilding->Get_Transform()->Scaled(_float3(2.f, 2.f, 2.f));
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

        shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
        {
            shared_ptr<Model> model = RESOURCES.Get<Model>(L"Anim_Mir_Intro_v02_BuildCenter");
            animator->Set_Model(model);
        }

        DestroyBuilding->Add_Component(animator);

        DestroyBuilding->Add_Component(make_shared<DestroyBuilding_FSM>());
        DestroyBuilding->Get_FSM()->Init();
        EVENTMGR.Create_Object(DestroyBuilding);

        m_vecDestroyObject.push_back(DestroyBuilding);
    }

    if (CUR_SCENE->Get_GameObject(L"F01_P_BackGCircle_02_KEK-52"))
    {
        _float4 vPos = CUR_SCENE->Get_GameObject(L"F01_P_BackGCircle_02_KEK-52")->Get_Transform()->Get_State(Transform_State::POS);
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"F01_P_BackGCircle_02_KEK-52"));

        shared_ptr<GameObject> DestroyBuilding = make_shared<GameObject>();
        DestroyBuilding->GetOrAddTransform()->Set_State(Transform_State::POS, vPos);
        DestroyBuilding->Get_Transform()->Scaled(_float3(2.f, 2.f, 2.f));
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

        shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
        {
            shared_ptr<Model> model = RESOURCES.Get<Model>(L"Anim_Mir_Intro_v02_BuildCenter");
            animator->Set_Model(model);
        }

        DestroyBuilding->Add_Component(animator);

        DestroyBuilding->Add_Component(make_shared<DestroyBuilding_FSM>());
        DestroyBuilding->Get_FSM()->Init();
        EVENTMGR.Create_Object(DestroyBuilding);

        m_vecDestroyObject.push_back(DestroyBuilding);
    }

    if (CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_Circle_02_AHJ-53"))
    {
        _float4 vPos = CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_Circle_02_AHJ-53")->Get_Transform()->Get_State(Transform_State::POS);
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_Circle_02_AHJ-53"));

        shared_ptr<GameObject> DestroyBuilding = make_shared<GameObject>();
        DestroyBuilding->GetOrAddTransform()->Set_State(Transform_State::POS, vPos);
        DestroyBuilding->Get_Transform()->Scaled(_float3(2.f, 2.f, 2.f));
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

        shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
        {
            shared_ptr<Model> model = RESOURCES.Get<Model>(L"Anim_Mir_Intro_v02_BuildCenter");
            animator->Set_Model(model);
        }

        DestroyBuilding->Add_Component(animator);

        DestroyBuilding->Add_Component(make_shared<DestroyBuilding_FSM>());
        DestroyBuilding->Get_FSM()->Init();
        EVENTMGR.Create_Object(DestroyBuilding);

        m_vecDestroyObject.push_back(DestroyBuilding);
    }

    if (CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_Circle_01_AHJ-54"))
    {
        _float4 vPos = CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_Circle_01_AHJ-54")->Get_Transform()->Get_State(Transform_State::POS);
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Mir_Mirdragon_P_Circle_01_AHJ-54"));

        shared_ptr<GameObject> DestroyBuilding = make_shared<GameObject>();
        DestroyBuilding->GetOrAddTransform()->Set_State(Transform_State::POS, vPos);
        DestroyBuilding->Get_Transform()->Scaled(_float3(2.f, 2.f, 2.f));
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

        shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
        {
            shared_ptr<Model> model = RESOURCES.Get<Model>(L"Anim_Mir_Intro_v02_BuildCenter");
            animator->Set_Model(model);
        }

        DestroyBuilding->Add_Component(animator);

        DestroyBuilding->Add_Component(make_shared<DestroyBuilding_FSM>());
        DestroyBuilding->Get_FSM()->Init();
        EVENTMGR.Create_Object(DestroyBuilding);

        m_vecDestroyObject.push_back(DestroyBuilding);
    }

    if (CUR_SCENE->Get_GameObject(L"Mir_R02_Dragon_001_01-55"))
    {
        _float4 vPos = CUR_SCENE->Get_GameObject(L"Mir_R02_Dragon_001_01-55")->Get_Transform()->Get_State(Transform_State::POS);
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Mir_R02_Dragon_001_01-55"));

        shared_ptr<GameObject> DestroyBuilding = make_shared<GameObject>();
        DestroyBuilding->GetOrAddTransform()->Set_State(Transform_State::POS, vPos);
        DestroyBuilding->Get_Transform()->Scaled(_float3(2.f, 2.f, 2.f));
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

        shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
        {
            shared_ptr<Model> model = RESOURCES.Get<Model>(L"Anim_Mir_Intro_v02_BuildCenter");
            animator->Set_Model(model);
        }

        DestroyBuilding->Add_Component(animator);

        DestroyBuilding->Add_Component(make_shared<DestroyBuilding_FSM>());
        DestroyBuilding->Get_FSM()->Init();
        EVENTMGR.Create_Object(DestroyBuilding);

        m_vecDestroyObject.push_back(DestroyBuilding);
    }

    if (CUR_SCENE->Get_GameObject(L"R02_Dragon_002_01-67"))
    {
         _float4 vPos = CUR_SCENE->Get_GameObject(L"R02_Dragon_002_01-67")->Get_Transform()->Get_State(Transform_State::POS);
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"R02_Dragon_002_01-67"));

        shared_ptr<GameObject> DestroyBuilding = make_shared<GameObject>();
        DestroyBuilding->GetOrAddTransform()->Set_State(Transform_State::POS, vPos);
        DestroyBuilding->Get_Transform()->Scaled(_float3(2.f, 2.f, 2.f));
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

        shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
        {
            shared_ptr<Model> model = RESOURCES.Get<Model>(L"Anim_Mir_Intro_v02_BuildCenter");
            animator->Set_Model(model);
        }

        DestroyBuilding->Add_Component(animator);

        DestroyBuilding->Add_Component(make_shared<DestroyBuilding_FSM>());
        DestroyBuilding->Get_FSM()->Init();
        EVENTMGR.Create_Object(DestroyBuilding);

        m_vecDestroyObject.push_back(DestroyBuilding);
    }


    //Just Destroy
    if (CUR_SCENE->Get_GameObject(L"Mir_R01_Floor_01_Circle_03_ASB-4"))
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Mir_R01_Floor_01_Circle_03_ASB-4"));
    
    if (CUR_SCENE->Get_GameObject(L"Mir_Dragon_Cloud_01_AHJ-41"))
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Mir_Dragon_Cloud_01_AHJ-41"));

    if (CUR_SCENE->Get_GameObject(L"Mir_Dragon_Cloud_02_AHJ-42"))
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Mir_Dragon_Cloud_02_AHJ-42"));

    if (CUR_SCENE->Get_GameObject(L"Mir_Dragon_Cloud_03_AHJ-43"))
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Mir_Dragon_Cloud_03_AHJ-43"));

    if (CUR_SCENE->Get_GameObject(L"Mir_Dragon_Cloud_01_AHJ-45"))
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Mir_Dragon_Cloud_01_AHJ-45"));

    if (CUR_SCENE->Get_GameObject(L"Mir_Dragon_Cloud_01_AHJ-46"))
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Mir_Dragon_Cloud_01_AHJ-46"));

    if (CUR_SCENE->Get_GameObject(L"Mir_Dragon_Cloud_03_AHJ-47"))
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Mir_Dragon_Cloud_03_AHJ-47"));

    if (CUR_SCENE->Get_GameObject(L"Mir_Dragon_Cloud_03_AHJ-48"))
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Mir_Dragon_Cloud_03_AHJ-48"));

    if (CUR_SCENE->Get_GameObject(L"Anim_R02_P_lamp_01-8"))
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Anim_R02_P_lamp_01-8"));

    if (CUR_SCENE->Get_GameObject(L"Anim_R02_P_lamp_01-9"))
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Anim_R02_P_lamp_01-9"));

    if (CUR_SCENE->Get_GameObject(L"Anim_R02_P_lamp_01-11"))
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Anim_R02_P_lamp_01-11"));

    if (CUR_SCENE->Get_GameObject(L"Anim_R02_P_lamp_01-12"))
        EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Anim_R02_P_lamp_01-12"));




}

void Boss_Giant_Mir_FSM::Update_Collider()
{
    if (!m_pAttackCollider.expired())
    {
        //m_pAttack transform set forward
        m_pAttackCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 6.f + _float3::Up);
    }

    if (!m_pTailCollider.expired())
    {
        //(AnimationModel BonePos)
        TailBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iTailBoneIndex) *
            _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

        _float4 vBonePos = _float4{ TailBoneMatrix.Translation().x, TailBoneMatrix.Translation().y, TailBoneMatrix.Translation().z , 1.f };
        m_pTailCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, vBonePos);
    }

    if (!m_pStomachCollider.expired())
    {
        StomachBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iStomachBoneIndex) *
            _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

        _float4 vBonePos = _float4{ StomachBoneMatrix.Translation().x, StomachBoneMatrix.Translation().y, StomachBoneMatrix.Translation().z , 1.f };
        vBonePos = vBonePos + 
                   Get_Transform()->Get_State(Transform_State::RIGHT) * 2.f + // 4
                   Get_Transform()->Get_State(Transform_State::LOOK) * 1.f; //2
        m_pStomachCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, vBonePos);
    }

    if (!m_pLfootCollider.expired())
    {
        LfootBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iLfootBoneIndex) *
            _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

        _float4 vBonePos = _float4{ LfootBoneMatrix.Translation().x, LfootBoneMatrix.Translation().y, LfootBoneMatrix.Translation().z , 1.f };
        vBonePos = vBonePos +
                   Get_Transform()->Get_State(Transform_State::RIGHT) * 2.f +
                   Get_Transform()->Get_State(Transform_State::LOOK) * 2.f; 
        
        m_pLfootCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, vBonePos);
    }
}

void Boss_Giant_Mir_FSM::DeadSetting()
{
    if (m_bIsDead)
    {
        Set_Invincible(true);
        m_eCurState = STATE::SQ_Leave_Groggy_Start;
    }
}

void Boss_Giant_Mir_FSM::Set_Invincible(_bool bFlag)
{
    m_bInvincible = bFlag;

    if (!m_pStomachCollider.expired())
    {
        if (m_pStomachCollider.lock()->Get_FSM())
            m_pStomachCollider.lock()->Get_FSM()->Set_Invincible(bFlag);
    }

    if (!m_pLfootCollider.expired())
    {
        if (m_pLfootCollider.lock()->Get_FSM())
            m_pLfootCollider.lock()->Get_FSM()->Set_Invincible(bFlag);
    }
}

void Boss_Giant_Mir_FSM::Set_RigidBodyActivate(_bool flag)
{
    if (m_pFootRigidBody.expired())
        return;

    if (!m_pFootRigidBody.lock()->Get_RigidBody() || !m_pFootRigidBody.lock()->Get_RigidBody()->Get_RigidBody())
        return;


    m_pFootRigidBody.lock()->Get_RigidBody()->AddOrRemoveRigidBody_FromScene(flag);
}

void Boss_Giant_Mir_FSM::Calculate_IntroHeadCam()
{
    HeadBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iHeadBoneIndex) *
        _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

    m_vHeadBonePos = _float4{ HeadBoneMatrix.Translation().x, HeadBoneMatrix.Translation().y, HeadBoneMatrix.Translation().z , 1.f };
    m_vHeadCamPos = m_vHeadBonePos + 
                    (Get_Transform()->Get_State(Transform_State::RIGHT) * -20.f) +
                    (Get_Transform()->Get_State(Transform_State::UP) * 20.f) +
                    (Get_Transform()->Get_State(Transform_State::LOOK) * 30.f) ;

    m_vIntroCamPos = (Get_Transform()->Get_State(Transform_State::POS) +
                      Get_Transform()->Get_State(Transform_State::LOOK) * 5.f +
                      Get_Transform()->Get_State(Transform_State::UP) * 15.f);
}

void Boss_Giant_Mir_FSM::Calculate_LeaveHeadCam()
{
    HeadBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iHeadBoneIndex) *
        _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

    m_vHeadBonePos = _float4{ HeadBoneMatrix.Translation().x, HeadBoneMatrix.Translation().y, HeadBoneMatrix.Translation().z , 1.f };
    m_vHeadCamPos = m_vHeadBonePos +
        (Get_Transform()->Get_State(Transform_State::RIGHT) * -20.f) +
        (Get_Transform()->Get_State(Transform_State::UP) * 20.f) +
        (Get_Transform()->Get_State(Transform_State::LOOK) * 30.f);

    StomachBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iStomachBoneIndex) *
        _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

    m_vStomachBonePos = _float4{ HeadBoneMatrix.Translation().x, HeadBoneMatrix.Translation().y, HeadBoneMatrix.Translation().z , 1.f };

    m_vLeaveCamPos = (Get_Transform()->Get_State(Transform_State::POS) +
        Get_Transform()->Get_State(Transform_State::LOOK) * 5.f +
        Get_Transform()->Get_State(Transform_State::UP) * 10.f);
}

void Boss_Giant_Mir_FSM::TailAttackCollider_On(const wstring& skillname, _float fAttackDamage)
{
    m_pTailCollider.lock()->Get_Collider()->Set_Activate(true);
    m_pTailCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_SkillName(skillname);
    m_pTailCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_AttackDamage(fAttackDamage);
}

void Boss_Giant_Mir_FSM::TailAttackCollider_Off()
{
    m_pTailCollider.lock()->Get_Collider()->Set_Activate(false);
    m_pTailCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_SkillName(L"");
    m_pTailCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_AttackDamage(0.f);
}

_float Boss_Giant_Mir_FSM::CamDistanceLerp(_float fStart, _float fEnd, _float fRatio)
{
    return fStart * (1.f - fRatio) + fEnd * fRatio;
}



