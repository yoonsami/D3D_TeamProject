#include "pch.h"
#include "Boss_Mir_FSM.h"
#include "Boss_Giant_Mir_FSM.h"

#include "ModelAnimator.h"
#include "SphereCollider.h"
#include "OBBBoxCollider.h"
#include "AttackColliderInfoScript.h"
#include "ObjectDissolveCreate.h"
#include "Model.h"
#include "CharacterController.h"
#include "CounterMotionTrailScript.h"
#include "MainCameraScript.h"
#include "UiDamageCreate.h"
#include "DragonBall_FSM.h"
#include "UIBossHpBar.h"
#include "SimpleMath.h"
#include "ModelAnimation.h"
#include "Camera.h"
#include "GroupEffect.h"
#include "TimerScript.h"
#include "RigidBody.h"

/* Effect Script */
#include "Mir_13100_Fireball.h"

Boss_Mir_FSM::Boss_Mir_FSM()
{
}

Boss_Mir_FSM::~Boss_Mir_FSM()
{
}

HRESULT Boss_Mir_FSM::Init()
{
    if (!m_bInitialize)
    {
        auto animator = Get_Owner()->Get_Animator();
        if (animator)
        {
            animator->Set_CurrentAnim(L"3D_ui_start", false, 1.f);
            m_eCurState = STATE::First_Meet;
        }

        Add_Boss_Mir_Collider();

        m_iHeadBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_Head");
        m_iMouseBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Bone057");
        m_iTailBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Bone040");
        m_iTopBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_Top");

        HeadBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iHeadBoneIndex) *
            _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

        m_vHeadCamPos = m_vHeadBonePos + (Get_Transform()->Get_State(Transform_State::LOOK) * 10.f);

        m_pCamera = CUR_SCENE->Get_MainCamera();

        m_fRunSpeed = 4.f;
        m_fKnockDownSpeed = 4.f;

        m_fDetectRange = 28.f;

        if (!m_pTarget.expired())
        {
            _float3 vLook = m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS).xyz();
            vLook.y = 0.f;
            Get_Transform()->Set_LookDir(vLook);
        }
        
        Setting_DragonBall();

        m_vSetPlayerPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 20.f;

        m_fNormalAttack_AnimationSpeed = 1.5f;

        m_bInitialize = true;
    }
    {
        shared_ptr<GameObject> obj = make_shared<GameObject>();
        obj->GetOrAddTransform()->Set_WorldMat(Get_Transform()->Get_WorldMatrix());
		{
			auto controller = make_shared<CharacterController>();
			obj->Add_Component(controller);

			auto& desc = controller->Get_CapsuleControllerDesc();
			desc.radius = 2.f;
			desc.height = 5.f;
			desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;

			controller->Create_Controller();
			controller->Get_Actor()->setStepOffset(0.1f);

		}
        m_pSubController[0] = obj;
        EVENTMGR.Create_Object(obj);
    }
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->GetOrAddTransform()->Set_WorldMat(Get_Transform()->Get_WorldMatrix());
		{
			auto controller = make_shared<CharacterController>();
			obj->Add_Component(controller);

			auto& desc = controller->Get_CapsuleControllerDesc();
			desc.radius = 2.f;
			desc.height = 5.f;
			desc.climbingMode = PxCapsuleClimbingMode::eCONSTRAINED;

			controller->Create_Controller();
			controller->Get_Actor()->setStepOffset(0.1f);

		}
		m_pSubController[1] = obj;
		EVENTMGR.Create_Object(obj);
	}

    return S_OK;
}

void Boss_Mir_FSM::Tick()
{
    DeadCheck();

    State_Tick();
    if (!m_pSubController[0].expired())
    {
        _float4 newPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 3.f;
        if (m_pSubController[0].lock()->Get_CharacterController())
            m_pSubController[0].lock()->Get_CharacterController()->Get_Actor()->setFootPosition({ newPos.x,newPos.y,newPos.z });
        m_pSubController[0].lock()->Get_Transform()->Set_State(Transform_State::POS, newPos);
    }
	if (!m_pSubController[1].expired())
	{
		_float4 newPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 6.f;
		if (m_pSubController[1].lock()->Get_CharacterController())
			m_pSubController[1].lock()->Get_CharacterController()->Get_Actor()->setFootPosition({ newPos.x,newPos.y,newPos.z });
		m_pSubController[1].lock()->Get_Transform()->Set_State(Transform_State::POS, newPos);
	}
    if (!m_pAttackCollider.expired())
    {
        //m_pAttack transform set forward
        m_pAttackCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 6.f + _float3::Up);
    }

    if (!m_pTailCollider.expired())
    {
        //Head_Bone_Pos
        //(AnimationModel BonePos)
        TailBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iTailBoneIndex) *
            _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

        _float4 vBonePos = _float4{ TailBoneMatrix.Translation().x, TailBoneMatrix.Translation().y, TailBoneMatrix.Translation().z , 1.f };
        m_pTailCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, vBonePos);
    }
    
    //For. Debugging
    if (KEYTAP(KEY_TYPE::P))
    {
        m_bCheckPhaseChange[0] = true;
        m_bCheckPhaseChange[1] = true;
        m_bPhaseChange[0] = true;
        m_bPhaseChange[1] = true;
        m_bSummonMeteor = false;
        m_tMeteorCoolTime.fAccTime = 0.f;

        _uint iDragonBallIndex = 4;

        for (_uint i = 0; i < 3; i++)
        {
            wstring DragonBallName = L"Anim_P_R02_DecoBall_00_Idle-1"; // TODO 14,15,16
            DragonBallName += to_wstring(iDragonBallIndex);

            if (CUR_SCENE->Get_GameObject(DragonBallName))
            {
                EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(DragonBallName));
            }

            iDragonBallIndex++;
        }

        if (m_bPhaseOneEmissive)
        {
            for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
                material->Get_MaterialDesc().emissive = Color(0.f, 0.f, 0.f, 1.f);

            m_bPhaseOneEmissive = false;
            m_iCrashCnt = 0;
            m_bInvincible = false;
        }
        m_eCurPhase = PHASE::PHASE2;
        m_pOwner.lock()->Set_Hp(0.f);
    }

}

void Boss_Mir_FSM::State_Tick()
{
    State_Init();

    m_iCurFrame = Get_CurFrame();

    if (m_bSummonMeteor)
        Create_Meteor();

    switch (m_eCurState)
    {
    case STATE::First_Meet:
        First_Meet();
        break;
    case STATE::sq_Intro:
        sq_Intro();
        break;
    case STATE::sq_Intro2:
        sq_Intro2();
        break;
    case STATE::b_idle:
        b_idle();
        break;
    case STATE::turn_l:
        turn_l();
        break;
    case STATE::turn_r:
        turn_r();
        break;
    case STATE::die:
        die();
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
    case STATE::SQ_Flee:
        SQ_Flee();
        break;
    case STATE::skill_Assault:
        skill_Assault();
        break;
    case STATE::skill_Return:
        skill_Return();
        break;
    case STATE::skill_Restart_Phase1:
        skill_Restart_Phase1();
        break;
    case STATE::skill_Restart_Phase1_Intro:
        skill_Restart_Phase1_Intro();
        break;
    case STATE::SQ_SBRin_Roar:
        SQ_SBRin_Roar();
        break;
    case STATE::skill_1100:
        skill_1100();
        break;
    case STATE::skill_2100:
        skill_2100();
        break;
    case STATE::skill_3100:
        skill_3100();
        break;
    case STATE::skill_4100:
        skill_4100();
        break;
    case STATE::skill_5100:
        skill_5100();
        break;
    case STATE::skill_9100:
        skill_9100();
        break;
    case STATE::skill_11100:
        skill_11100();
        break;
    case STATE::skill_12100:
        skill_12100();
        break;
    case STATE::skill_13100:
        skill_13100();
        break;
    case STATE::skill_14100:    
        skill_14100();
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

void Boss_Mir_FSM::State_Init()
{
    if (m_eCurState != m_ePreState)
    {
        switch (m_eCurState)
        {
        case STATE::First_Meet:
            First_Meet_Init();
            break;
        case STATE::sq_Intro:
            sq_Intro_Init();
            break;
        case STATE::sq_Intro2:
            sq_Intro2_Init();
            break;
        case STATE::b_idle:
            b_idle_Init();
            break;
        case STATE::turn_l:
            turn_l_Init();
            break;
        case STATE::turn_r:
            turn_r_Init();
            break;
        case STATE::die:
            die_Init();
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
        case STATE::SQ_Flee:
            SQ_Flee_Init();
            break;
        case STATE::skill_Assault:
            skill_Assault_Init();
            break;
        case STATE::skill_Return:
            skill_Return_Init();
            break;
        case STATE::skill_Restart_Phase1:
            skill_Restart_Phase1_Init();
            break;
        case STATE::skill_Restart_Phase1_Intro:
            skill_Restart_Phase1_Intro_Init();
            break;
        case STATE::SQ_SBRin_Roar:
            SQ_SBRin_Roar_Init();
            break;
        case STATE::skill_1100:
            skill_1100_Init();
            break;
        case STATE::skill_2100:
            skill_2100_Init();
            break;
        case STATE::skill_3100:
            skill_3100_Init();
            break;
        case STATE::skill_4100:
            skill_4100_Init();
            break;
        case STATE::skill_5100:
            skill_5100_Init();
            break;
        case STATE::skill_9100:
            skill_9100_Init();
            break;
        case STATE::skill_11100:
            skill_11100_Init();
            break;
        case STATE::skill_12100:
            skill_12100_Init();
            break;
        case STATE::skill_13100:
            skill_13100_Init();
            break;
        case STATE::skill_14100:
            skill_14100_Init();
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

void Boss_Mir_FSM::Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget)
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
        if (m_bCounter)
        {
            if (CounterAttackCheck(90.f))
            {
                Create_CounterMotionTrail();
                m_eCurState = STATE::groggy_start;
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
    }

}

void Boss_Mir_FSM::Set_State(_uint iIndex)
{
    m_eCurState = (STATE)iIndex;
}

void Boss_Mir_FSM::First_Meet()
{
    Calculate_IntroHeadCam();

    if (Target_In_DetectRange())
        m_bDetected = true;

    //THIS CAMERA MOVING IS MASTERPIECE -> NEVER DON'T TOUCH
    if (m_bDetected)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimState(false);
        
        g_bCutScene = true;

        if (m_iCurFrame >= 1 && m_iCurFrame < 90)
        {
            if (!m_pCamera.expired())
            {
                m_fIntroCamDistance = CamDistanceLerp(20.f, 10.f, m_fCamRatio);
                
                m_fCamRatio += fDT;

                if (m_fCamRatio >= 1.f)
                    m_fCamRatio = 1.f;

                m_vHeadCamDir = m_pCamera.lock()->Get_Transform()->Get_State(Transform_State::POS) - m_vHeadBonePos.xyz();
                m_vHeadCamDir.Normalize();

                m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(0.8f);
                m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vHeadBonePos.xyz());
                m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(1.f, m_vHeadCamDir, m_fIntroCamDistance);
                
            }
        }
        else if (m_iCurFrame >= 90)
        {
            if (!m_pCamera.expired())
            {
                m_pCamera.lock()->Get_Transform()->Set_State(Transform_State::POS, m_vHeadCamPos);

                m_vHeadCamDir = m_vHeadBonePos.xyz() - m_vHeadCamPos.xyz();
                m_vHeadCamDir.Normalize();
                
                m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(0.5f);
                m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vHeadBonePos.xyz());
                m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(1.f, m_vHeadCamDir * -1.f, 10.f);
            }
        }
    }


    if (Is_AnimFinished())
        m_eCurState = STATE::sq_Intro2;
}

void Boss_Mir_FSM::First_Meet_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"3D_ui_start", 0.1f, false, 1.f);

    m_bInvincible = true;

    animator->Set_AnimState(true);

    m_fCamRatio = 0.f;

    Calculate_IntroHeadCam();
}

void Boss_Mir_FSM::sq_Intro()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::sq_Intro2;
}

void Boss_Mir_FSM::sq_Intro_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"sq_Intro", 0.1f, false, 1.5f);

    m_bInvincible = true;
}

void Boss_Mir_FSM::sq_Intro2()
{
    Calculate_IntroHeadCam();
 
    if (m_iCurFrame > 5)
    {
        if (!m_pCamera.expired())
        {
            m_vHeadCamDir = m_vHeadBonePos.xyz() - m_vHeadCamPos.xyz();
            m_vHeadCamDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(0.5f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vHeadBonePos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(1.f, m_vHeadCamDir * -1.f, 10.f);

        }
    }
    if (m_iCurFrame > 28 && m_iCurFrame < 56)
    {
        m_fStateTimer += fDT;
        CAMERA_SHAKE(0.05f, 0.1f);
		auto animator = Get_Owner()->Get_Animator();

		auto& tweenDesc = animator->Get_TweenDesc();

		const auto& animation = animator->Get_Model()->Get_AnimationByIndex(tweenDesc.curr.animIndex);

		_float timePerFrame = 1 / (animation->frameRate * animation->speed);
        _float time = 28.f * timePerFrame;

        CUR_SCENE->g_bAberrationOn = true;
        CUR_SCENE->g_fAberrationPower = -500.f * (m_fStateTimer) * (m_fStateTimer - time);

        CUR_SCENE->g_RadialBlurData.g_bRadialBlurOn = true;
        CUR_SCENE->g_RadialBlurData.g_fNormalRadius = 0.f;
        CUR_SCENE->g_RadialBlurData.g_fRadialBlurStrength = -0.7f * (m_fStateTimer) * (m_fStateTimer - time);

        const _float4x4& matView = CUR_SCENE->Get_MainCamera()->Get_Camera()->Get_ViewMat();
        const _float4x4& matProj = CUR_SCENE->Get_MainCamera()->Get_Camera()->Get_ProjMat();

        _float3 vCenterPos = _float3::Transform(m_vHeadBonePos.xyz(), matView * matProj);
        CUR_SCENE->g_RadialBlurData.g_vCenterPos = { vCenterPos.x,vCenterPos.y };

    }
    if (m_iCurFrame > 56)
    {
		CUR_SCENE->g_bAberrationOn = false;
		CUR_SCENE->g_fAberrationPower = 0.f;
        CUR_SCENE->g_RadialBlurData.g_bRadialBlurOn = false;
        CUR_SCENE->g_RadialBlurData.g_fRadialBlurStrength = 0.f;
    }

    if (Is_AnimFinished())
    {
        g_bCutScene = false;
        m_eCurState = STATE::b_idle;

        if (!m_pCamera.expired())
        {
            m_vPhaseChangePos = m_pCamera.lock()->Get_Transform()->Get_State(Transform_State::POS);
        }
    }
}

void Boss_Mir_FSM::sq_Intro2_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"sq_Intro2", 0.1f, false, 1.f);

    m_bInvincible = true;

    Calculate_IntroHeadCam();

    m_FirstWorldMat = Get_Transform()->Get_WorldMatrix();
    m_fStateTimer = 0.f;
}

void Boss_Mir_FSM::b_idle()
{
    m_tAttackCoolTime.fAccTime += fDT;
   
    if (m_tAttackCoolTime.fAccTime >= m_tAttackCoolTime.fCoolTime)
    {
        //Phase1 Assault and Return
        if (m_eCurPhase == PHASE::PHASE1)
        {
            CalCulate_PlayerDir();

            if (!m_bTurnMotion)
                m_eCurState = STATE::skill_Assault;
            else
            {
                if (m_iPhaseOne_TurnCnt == 0)
                {
                    m_iPhaseOne_TurnCnt++;

                    if (m_eAttackDir == DIR::FORWARD_LEFT)
                    {
                        m_eCurState = STATE::turn_l;
                    }
                    else if (m_eAttackDir == DIR::BACKWARD_LEFT)
                    {
                        m_eCurState = STATE::turn_l;
                        m_fTurnSpeed = XM_PI * 1.2f;
                    }
                    else if (m_eAttackDir == DIR::FORWARD_RIGHT)
                    {
                        m_eCurState = STATE::turn_r;
                    }
                    else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
                    {
                        m_eCurState = STATE::turn_r;
                        m_fTurnSpeed = XM_PI * 1.2f;
                    }
                }
                else
                {
                    m_eCurState = STATE::skill_Assault;
                }
            }
        }
        else
        {
            Set_AttackPattern();            
        }
    }

    if (m_eCurPhase == PHASE::PHASE2)
    {
        if ((m_bCheckPhaseChange[0] && !m_bPhaseChange[0]) || (m_bCheckPhaseChange[1] && !m_bPhaseChange[1]))
        {
            m_iPreAttack = 100;
            m_eCurState = STATE::skill_Restart_Phase1;
        }
    }

    DeadSetting();
}

void Boss_Mir_FSM::b_idle_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"b_idle", 0.3f, true, 1.f);

    Get_Transform()->Set_Speed(m_fRunSpeed);

    m_vTurnVector = _float3(0.f);

    m_tAttackCoolTime.fAccTime = 0.f;
    m_tBreathCoolTime.fAccTime = 0.f;

    AttackCollider_Off();
    TailAttackCollider_Off();
    Check_PhaseChange();

    if (m_eCurPhase == PHASE::PHASE1)
    {
        m_bSummonMeteor = true;

        if (!m_bPhaseOneEmissive)
        {
            for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
            {
                material->Get_MaterialDesc().emissive = Color(1.f, 0.1f, 0.1f, 1.f);
            }

            m_bPhaseOneEmissive = true;
     
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
    }
    else
    {
        m_bInvincible = false;
    }
}

void Boss_Mir_FSM::turn_l()
{
    if (m_iCurFrame > 9 && m_iCurFrame < 28)
    {
        if (!m_pTarget.expired())
            Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);
    }

    if (Is_AnimFinished())
    {
        m_fTurnSpeed = XM_PI * 0.5f;
        m_eCurState = STATE::b_idle;
    }

    DeadSetting();
}

void Boss_Mir_FSM::turn_l_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"turn_l", 0.2f, false, 1.f);
    
    m_bTurnMotion = false;

    if (m_eCurPhase == PHASE::PHASE1)
        m_bSummonMeteor = true;
}

void Boss_Mir_FSM::turn_r()
{
    if (m_iCurFrame > 9 && m_iCurFrame < 28)
    {
        if (!m_pTarget.expired())
            Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), m_fTurnSpeed);
    }

    if (Is_AnimFinished())
    {
        m_fTurnSpeed = XM_PI * 0.5f;
        m_eCurState = STATE::b_idle;
    }

    DeadSetting();
}

void Boss_Mir_FSM::turn_r_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"turn_r", 0.2f, false, 1.f);

    m_bTurnMotion = false;

    
    if (m_eCurPhase == PHASE::PHASE1)
        m_bSummonMeteor = true;
}

void Boss_Mir_FSM::die()
{
}

void Boss_Mir_FSM::die_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"SQ_Dead", 0.1f, false, 1.f);

    AttackCollider_Off();
    TailAttackCollider_Off();
}

void Boss_Mir_FSM::groggy_start()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::groggy_loop;
}

void Boss_Mir_FSM::groggy_start_Init()
{
    KillAllEffect();

    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"groggy_start", 0.1f, false, 1.f);

    AttackCollider_Off();
    TailAttackCollider_Off();

    m_tAttackCoolTime.fAccTime = 0.f;
    m_tBreathCoolTime.fAccTime = 0.f;
    m_bCounter = false;
    m_bSummonMeteor = false;
    m_tMeteorCoolTime.fAccTime = 0.f;
    m_iCurMeteorCnt = 0;

    if (m_eCurPhase == PHASE::PHASE1)
    {
        m_iCrashCnt++;

        //In Phase1 Crash3 -> Phase 2 Start
        if (m_iCrashCnt == 3)
        {
            m_eCurPhase = PHASE::PHASE2;
        }

        if (m_eCurPhase == PHASE::PHASE2)
        {
            if (m_bPhaseOneEmissive)
            {
                for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
                    material->Get_MaterialDesc().emissive = Color(0.f, 0.f, 0.f, 1.f);

                m_bPhaseOneEmissive = false;
                m_iCrashCnt = 0;
                m_bInvincible = false;
            }

        }
    }
}

void Boss_Mir_FSM::groggy_loop()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::groggy_end;
}

void Boss_Mir_FSM::groggy_loop_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"groggy_loop", 0.1f, false, 1.f);
}

void Boss_Mir_FSM::groggy_end()
{
    if (Is_AnimFinished())
    {
        CalCulate_PlayerDir();

        if (!m_bTurnMotion)
            m_eCurState = STATE::b_idle;
        else
        {
            if (m_eAttackDir == DIR::FORWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
            }
            else if (m_eAttackDir == DIR::BACKWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
                m_fTurnSpeed = XM_PI * 1.f;
            }
            else if (m_eAttackDir == DIR::FORWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
            }
            else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
                m_fTurnSpeed = XM_PI * 1.f;
            }
        }
    }
}

void Boss_Mir_FSM::groggy_end_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"groggy_end", 0.1f, false, 1.f);
}

void Boss_Mir_FSM::SQ_Flee()
{
    Calculate_PhaseChangeHeadCam();

    if (m_iCurFrame < 80)
    {
        if (!m_pCamera.expired())
        {
            _float3 vLookPos = m_vTopBonePos + (_float3::Up * 6.f);

            if (vLookPos.y <= m_vHeadBonePos.y)
                vLookPos.y = m_vHeadBonePos.y;

            m_vHeadCamDir = vLookPos - m_vCamStopPos.xyz();
            m_vHeadCamDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(2.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(vLookPos);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(1.f, m_vHeadCamDir * -1.f, 6.f);
        
            if (m_iCurFrame == 79)
                m_vFleeCamPos = vLookPos;
        }
    }
    else
    {
        if (!m_pCamera.expired())
        {
            m_vHeadCamDir = m_vFleeCamPos - m_vCamStopPos.xyz();
            m_vHeadCamDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(2.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vFleeCamPos.xyz());
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(2.f, m_vHeadCamDir * -1.f, 6.f);
        }
    }
    
    if (Is_AnimFinished())
    {
        if (!m_pOwner.expired())
            EVENTMGR.Delete_Object(m_pOwner.lock());
        
        if (!m_pAttackCollider.expired())
            EVENTMGR.Delete_Object(m_pAttackCollider.lock());
        
        if (!m_pTailCollider.expired())
            EVENTMGR.Delete_Object(m_pTailCollider.lock());

        if (!m_pSubController[0].expired())
            EVENTMGR.Delete_Object(m_pSubController[0].lock());

        if (!m_pSubController[1].expired())
            EVENTMGR.Delete_Object(m_pSubController[1].lock());

        g_bCutScene = false;

        if (!m_pTarget.expired())
        {
            m_pTarget.lock()->Get_CharacterController()->Get_Actor()->setFootPosition({ m_vSetPlayerPos.x,  m_vSetPlayerPos.y, m_vSetPlayerPos.z });
        }


        Load_Giant_Boss_Mir();
    }

}

void Boss_Mir_FSM::SQ_Flee_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"SQ_Flee", 0.1f, false, 1.f);

    m_bInvincible = true;

    //Setting Spawn Position For CutScene
    Get_CharacterController()->Get_Actor()->setFootPosition({ m_FirstWorldMat.Translation().x,  m_FirstWorldMat.Translation().y, m_FirstWorldMat.Translation().z });
    Get_Transform()->Set_State(Transform_State::POS, _float4(m_FirstWorldMat.Translation(), 1.f));
    Get_Transform()->Set_LookDir(m_FirstWorldMat.Backward());

	m_vCamStopPos = _float4(m_FirstWorldMat.Translation() +
		                    m_FirstWorldMat.Backward() * 15.f +
		                    m_FirstWorldMat.Right() * -5.f +
		                    m_FirstWorldMat.Up() * 12.f, 1.f);

	if (!m_pCamera.expired())
	{
		m_pCamera.lock()->Get_Transform()->Set_State(Transform_State::POS, m_vCamStopPos);
	}

    AttackCollider_Off();
    TailAttackCollider_Off();

    Calculate_PhaseChangeHeadCam();

    if (!m_pOwner.expired())
    {
        if (m_pOwner.lock()->Get_Script<UIBossHpBar>())
            m_pOwner.lock()->Get_Script<UIBossHpBar>()->Remove_HpBar();
    }

    g_bCutScene = true;
    m_bSummonMeteor = false;
    m_tMeteorCoolTime.fAccTime = 0.f;
    m_iCurMeteorCnt = 0;
}

void Boss_Mir_FSM::skill_Assault()
{
    if (m_iCurFrame <= 30)
    {
        if (!m_pTarget.expired())
            Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), XM_PI * 2.f);
    }

    if (Init_CurFrame(75))
    {
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    }
    else if (Init_CurFrame(92))
    {
        AttackCollider_Off();
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::skill_Return;
}

void Boss_Mir_FSM::skill_Assault_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();
    
    animator->Set_NextTweenAnim(L"skill_2100", 0.1f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;

    m_iPhaseOne_TurnCnt = 0;
    m_bSummonMeteor = true;
}

void Boss_Mir_FSM::skill_Return()
{
    if (Is_AnimFinished())
    {
        CalCulate_PlayerDir();

        if (!m_bTurnMotion)
            m_eCurState = STATE::b_idle;
        else
        {
            if (m_eAttackDir == DIR::FORWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
            }
            else if (m_eAttackDir == DIR::BACKWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
                m_fTurnSpeed = XM_PI * 1.f;
            }
            else if (m_eAttackDir == DIR::FORWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
            }
            else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
                m_fTurnSpeed = XM_PI * 1.f;
            }
        }
    }
}

void Boss_Mir_FSM::skill_Return_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_5100", 0.1f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;

    m_bSummonMeteor = true;
}

void Boss_Mir_FSM::skill_Restart_Phase1()
{
    Calculate_PhaseChangeHeadCam();

    if (m_iCurFrame < 10)
    {
        if (!m_pCamera.expired())
        {
            _float3 vLookPos = m_vTopBonePos + (_float3::Up * -1.f);
            m_vHeadCamDir = vLookPos - m_vHeadCamPos.xyz();
            m_vHeadCamDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(2.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(vLookPos);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(1.f, m_vHeadCamDir * -1.f, 12.f);
        }
    }
    else if (m_iCurFrame >= 10 && m_iCurFrame < 100)
    {
        if (Init_CurFrame(10))
            m_vCamStopPos = m_vHeadCamPos;
    
        if (!m_pCamera.expired())
        {
            _float3 vLookPos = m_vTopBonePos + (_float3::Up * -1.f);
            m_vHeadCamDir = vLookPos - m_vCamStopPos.xyz();
            m_vHeadCamDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(1.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(vLookPos);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(1.f, m_vHeadCamDir * -1.f, 12.f);
        }
    }
    else
    {
        if (Init_CurFrame(100))
        {
            //Setting Spawn Position For CutScene
            Get_CharacterController()->Get_Actor()->setFootPosition({ m_FirstWorldMat.Translation().x,  m_FirstWorldMat.Translation().y, m_FirstWorldMat.Translation().z });
            Get_Transform()->Set_State(Transform_State::POS, _float4(m_FirstWorldMat.Translation(),1.f));
            Get_Transform()->Set_LookDir(m_FirstWorldMat.Backward());

            m_vCamStopPos = m_FirstWorldMat.Translation() + m_FirstWorldMat.Backward() * 10.f + Get_Transform()->Get_State(Transform_State::UP) * 5.f;

            if (!m_pCamera.expired())
            {
                m_pCamera.lock()->Get_Transform()->Set_State(Transform_State::POS, m_vPhaseChangePos);
                _float3 vDir = m_vPhaseChangePos.xyz() - (m_vTopBonePos.xyz() + (_float3::Up * -1.f));
                vDir.Normalize();
                m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_Offset(vDir);
            }

            Create_DragonBall();
        }
        
        if (!m_pCamera.expired())   
        {   
            _float3 vLookPos = Get_Transform()->Get_State(Transform_State::POS) + 
                               Get_Transform()->Get_State(Transform_State::LOOK) * 8.f +
                               (_float3::Up * 5.f);
            
            if (m_vHeadBonePos.y < vLookPos.y)
                vLookPos.y = m_vHeadBonePos.y;

            m_vHeadCamDir = vLookPos - m_vCamStopPos.xyz();
            m_vHeadCamDir.Normalize();

            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(2.f);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(vLookPos);
            m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(1.f, m_vHeadCamDir * -1.f, 12.f);
        }
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::skill_Restart_Phase1_Intro;
}

void Boss_Mir_FSM::skill_Restart_Phase1_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_9100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;

    m_fCamRatio = 0.f;

    m_bInvincible = true;

    Calculate_PhaseChangeHeadCam();

    if (m_bCheckPhaseChange[0])
        m_bPhaseChange[0] = true;
    
    if (m_bCheckPhaseChange[1])
        m_bPhaseChange[1] = true;
    
    m_bInvincible = true;
    g_bCutScene = true;
    
    m_bSummonMeteor = false;
    m_tMeteorCoolTime.fAccTime = 0.f;
    m_iCurMeteorCnt = 0;
}

void Boss_Mir_FSM::skill_Restart_Phase1_Intro()
{
    Calculate_IntroHeadCam();

    if (!m_pCamera.expired())
    {
        m_vHeadCamDir = m_vHeadBonePos.xyz() - m_vHeadCamPos.xyz();
        m_vHeadCamDir.Normalize();

        m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FollowSpeed(0.5f);
        m_pCamera.lock()->Get_Script<MainCameraScript>()->Set_FixedLookTarget(m_vHeadBonePos.xyz());
        m_pCamera.lock()->Get_Script<MainCameraScript>()->Fix_Camera(1.f, m_vHeadCamDir * -1.f, 10.f);
    }
    
	if (m_iCurFrame > 28 && m_iCurFrame < 56)
	{
		m_fStateTimer += fDT;
		CAMERA_SHAKE(0.05f, 0.1f);
		auto animator = Get_Owner()->Get_Animator();

		auto& tweenDesc = animator->Get_TweenDesc();

		const auto& animation = animator->Get_Model()->Get_AnimationByIndex(tweenDesc.curr.animIndex);

		_float timePerFrame = 1 / (animation->frameRate * animation->speed);
		_float time = 28.f * timePerFrame;

		CUR_SCENE->g_bAberrationOn = true;
		CUR_SCENE->g_fAberrationPower = -500.f * (m_fStateTimer) * (m_fStateTimer - time);

		CUR_SCENE->g_RadialBlurData.g_bRadialBlurOn = true;
		CUR_SCENE->g_RadialBlurData.g_fNormalRadius = 0.f;
		CUR_SCENE->g_RadialBlurData.g_fRadialBlurStrength = -0.7f * (m_fStateTimer) * (m_fStateTimer - time);

		const _float4x4& matView = CUR_SCENE->Get_MainCamera()->Get_Camera()->Get_ViewMat();
		const _float4x4& matProj = CUR_SCENE->Get_MainCamera()->Get_Camera()->Get_ProjMat();

		_float3 vCenterPos = _float3::Transform(m_vHeadBonePos.xyz(), matView * matProj);
		CUR_SCENE->g_RadialBlurData.g_vCenterPos = { vCenterPos.x,vCenterPos.y };

	}
	if (m_iCurFrame > 56)
	{
		CUR_SCENE->g_bAberrationOn = false;
		CUR_SCENE->g_fAberrationPower = 0.f;
		CUR_SCENE->g_RadialBlurData.g_bRadialBlurOn = false;
		CUR_SCENE->g_RadialBlurData.g_fRadialBlurStrength = 0.f;
	}

    if (Is_AnimFinished())
    {
        g_bCutScene = false;
        m_eCurState = STATE::b_idle;
        m_eCurPhase = PHASE::PHASE1;
    }
}

void Boss_Mir_FSM::skill_Restart_Phase1_Intro_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"sq_Intro2", 0.1f, false, 1.f);

    m_bInvincible = true;

    Calculate_IntroHeadCam();
    m_fStateTimer = 0.f;
}

void Boss_Mir_FSM::SQ_SBRin_Roar()
{
    if (m_iCurFrame == 24 ||
        m_iCurFrame == 34 ||
        m_iCurFrame == 44 ||
        m_iCurFrame == 54 ||
        m_iCurFrame == 64 || 
        m_iCurFrame == 74 )
    {
        if (m_iPreFrame != m_iCurFrame)
        {
            MouseBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iMouseBoneIndex) *
                _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

            _float4 vBonePos = _float4{ MouseBoneMatrix.Translation().x, MouseBoneMatrix.Translation().y, MouseBoneMatrix.Translation().z , 1.f };

            vBonePos = vBonePos + Get_Transform()->Get_State(Transform_State::LOOK);

            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 20.f;
            desc.fLifeTime = 0.5f;
            desc.fLimitDistance = 10.f;

            _float4 vSkillPos = vBonePos;

            if (m_iCurFrame != 74)
                Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 3.f, desc, NORMAL_ATTACK, 10.f);
            else
                Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 3.f, desc, KNOCKBACK_ATTACK, 10.f);
        }
    }

    if (Is_AnimFinished())
    {
        _uint iRan = rand() % 2;

        CalCulate_PlayerDir();

        if (!m_bTurnMotion)
            m_eCurState = STATE::b_idle;
        else
        {
            if (m_eAttackDir == DIR::FORWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
            }
            else if (m_eAttackDir == DIR::BACKWARD_LEFT)
            {
                if (iRan == 0)
                    m_eCurState = STATE::skill_4100;
                else
                {
                    m_eCurState = STATE::turn_l;
                    m_fTurnSpeed = XM_PI * 0.7f;
                }
            }
            else if (m_eAttackDir == DIR::FORWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
            }
            else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
            {
                if (iRan == 0)
                    m_eCurState = STATE::skill_3100;
                else
                {
                    m_eCurState = STATE::turn_l;
                    m_fTurnSpeed = XM_PI * 0.7f;
                }
            }
        }
    }
}

void Boss_Mir_FSM::SQ_SBRin_Roar_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"SQ_SBRin_Roar", 0.1f, false, 1.f);

    m_tAttackCoolTime.fAccTime = 0.f;

    m_bTurnMotion = false;
}

void Boss_Mir_FSM::skill_1100()
{
    if (Init_CurFrame(0))
        Add_GroupEffectOwner(L"Mir_1100", _float3(0.f, 0.f, 2.f), false);

    if (m_iCurFrame == 46 ||
        m_iCurFrame == 56 ||
        m_iCurFrame == 66 ||
        m_iCurFrame == 76)
    {
        if (m_iPreFrame != m_iCurFrame)
        {
            MouseBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iMouseBoneIndex) *
                _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

            _float4 vBonePos = _float4{ MouseBoneMatrix.Translation().x, MouseBoneMatrix.Translation().y, MouseBoneMatrix.Translation().z , 1.f };

            vBonePos = vBonePos + Get_Transform()->Get_State(Transform_State::LOOK);

            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 20.f;
            desc.fLifeTime = 0.5f;
            desc.fLimitDistance = 10.f;

            _float4 vSkillPos = vBonePos;
            
            if (m_iCurFrame != 76)
                Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 3.f, desc, NORMAL_ATTACK, 10.f);
            else
                Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 3.f, desc, KNOCKBACK_ATTACK, 10.f);
        }
    }

    if (Is_AnimFinished())
    {
        _uint iRan = rand() % 2;
        
        CalCulate_PlayerDir();

        if (!m_bTurnMotion)
            m_eCurState = STATE::b_idle;
        else
        {
            if (m_eAttackDir == DIR::FORWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
            }
            else if (m_eAttackDir == DIR::BACKWARD_LEFT)
            {
                if (iRan == 0)
                    m_eCurState = STATE::skill_4100;
                else
                {
                    m_eCurState = STATE::turn_l;
                    m_fTurnSpeed = XM_PI * 0.7f;
                }
            }
            else if (m_eAttackDir == DIR::FORWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
            }
            else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
            {
                if (iRan == 0)
                    m_eCurState = STATE::skill_3100;
                else
                {
                    m_eCurState = STATE::turn_l;
                    m_fTurnSpeed = XM_PI * 0.7f;
                }
            }
        }
    }
    
}

void Boss_Mir_FSM::skill_1100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_1100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;

}

void Boss_Mir_FSM::skill_2100()
{
    if (Init_CurFrame(55))
        Add_And_Set_Effect(L"Mir_2100");
    if (Init_CurFrame(93))
        Add_Effect(L"Mir_2100_End");

    
    if (m_iCurFrame == 55)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(m_fNormalAttack_AnimationSpeed / 4.f);

        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.05f, 0.2f, 1.f, 1.f);
        }

        m_bCounter = true;
    }
    else if (m_iCurFrame == 68)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(m_fNormalAttack_AnimationSpeed);
        m_bCounter = false;
		for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
		{
			material->Get_MaterialDesc().emissive = Color(0.f, 0.f, 0.f, 1.f);
		}
    }
    else if (m_iCurFrame == 70)
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    else if (m_iCurFrame == 93)
        AttackCollider_Off();

    if (Is_AnimFinished())
    {
        _float4 vPos = Get_Transform()->Get_State(Transform_State::POS);
        _uint iRan = rand() % 2;

        CalCulate_PlayerDir();

        if (iRan == 0)
        {

            if (!m_bTurnMotion)
                m_eCurState = STATE::b_idle;
            else
            {
                if (m_eAttackDir == DIR::FORWARD_LEFT)
                {
                    m_eCurState = STATE::turn_l;
                }
                else if (m_eAttackDir == DIR::BACKWARD_LEFT)
                {
                    m_eCurState = STATE::skill_4100;
                }
                else if (m_eAttackDir == DIR::FORWARD_RIGHT)
                {
                    m_eCurState = STATE::turn_r;
                }
                else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
                {
                    m_eCurState = STATE::skill_3100;
                }
            }
        }
        else
        {
            m_eCurState = STATE::skill_5100;
        }
    }
}

void Boss_Mir_FSM::skill_2100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_2100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;

    m_bCounter = false;
}

void Boss_Mir_FSM::skill_3100()
{
    if (Init_CurFrame(80))
        Add_Effect(L"Mir_3100");

    if (m_iCurFrame == 80)
        TailAttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    else if (m_iCurFrame == 98)
        TailAttackCollider_Off();

    if (Is_AnimFinished())
    {
        CalCulate_PlayerDir();

        if (!m_bTurnMotion)
            m_eCurState = STATE::b_idle;
        else
        {
            if (m_eAttackDir == DIR::FORWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
            }
            else if (m_eAttackDir == DIR::BACKWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
                m_fTurnSpeed = XM_PI * 0.7f;
            }
            else if (m_eAttackDir == DIR::FORWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
            }
            else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
                m_fTurnSpeed = XM_PI * 0.7f;
            }
        }
    }
   
}

void Boss_Mir_FSM::skill_3100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_3100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
}

void Boss_Mir_FSM::skill_4100()
{
    if (Init_CurFrame(86))
        Add_Effect(L"Mir_3100");

    if (m_iCurFrame == 86)
        TailAttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    else if (m_iCurFrame == 108)
        TailAttackCollider_Off();

    if (Is_AnimFinished())
    {
        CalCulate_PlayerDir();

        if (!m_bTurnMotion)
            m_eCurState = STATE::b_idle;
        else
        {
            if (m_eAttackDir == DIR::FORWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
            }
            else if (m_eAttackDir == DIR::BACKWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
                m_fTurnSpeed = XM_PI * 0.7f;
            }
            else if (m_eAttackDir == DIR::FORWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
            }
            else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
                m_fTurnSpeed = XM_PI * 0.7f;
            }
        }
    }
}

void Boss_Mir_FSM::skill_4100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_4100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
}

void Boss_Mir_FSM::skill_5100()
{
    if (Is_AnimFinished())
    {
        CalCulate_PlayerDir();
        
        if (!m_bTurnMotion)
            m_eCurState = STATE::b_idle;
        else
        {
            if (m_eAttackDir == DIR::FORWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
            }
            else if (m_eAttackDir == DIR::BACKWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
                m_fTurnSpeed = XM_PI * 1.f;
            }
            else if (m_eAttackDir == DIR::FORWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
            }
            else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
                m_fTurnSpeed = XM_PI * 1.f;
            }
        }
    }
}

void Boss_Mir_FSM::skill_5100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_5100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
}

void Boss_Mir_FSM::skill_9100()
{
    if (m_iCurFrame == 66 ||
        m_iCurFrame == 86 ||
        m_iCurFrame == 106)
    {
        if (m_iPreFrame != m_iCurFrame)
        {
            if (!m_pTarget.expired())
            {
                _float4 vPlayerPos = m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS);

                FORWARDMOVINGSKILLDESC desc;
                desc.vSkillDir = _float3{ 0.f,-1.f,0.f };
                desc.fMoveSpeed = 10.f;
                desc.fLifeTime = 1.f;
                desc.fLimitDistance = 20.f;

                for (_uint i = 0; i < 4; i++)
                {
                    _float fOffSetX = ((rand() * 2 / _float(RAND_MAX) - 1) * (rand() % 10 + 3));
                    _float fOffSetZ = ((rand() * 2 / _float(RAND_MAX) - 1) * (rand() % 10 + 3));

                    _float4 vSkillPos = vPlayerPos + _float4{ fOffSetX, 10.f, fOffSetZ, 0.f };

                    Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 1.f, desc, AIRBORNE_ATTACK, 10.f);
                }
            }
        }
    }

    if (Is_AnimFinished())
    {
        CalCulate_PlayerDir();

        if (!m_bTurnMotion)
            m_eCurState = STATE::b_idle;
        else
        {
            if (m_eAttackDir == DIR::FORWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
            }
            else if (m_eAttackDir == DIR::BACKWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
                m_fTurnSpeed = XM_PI * 0.7f;
            }
            else if (m_eAttackDir == DIR::FORWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
            }
            else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
                m_fTurnSpeed = XM_PI * 0.7f;
            }
        }
    }
}

void Boss_Mir_FSM::skill_9100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_9100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
}

void Boss_Mir_FSM::skill_11100()
{
    if (Init_CurFrame(4))
        Add_Effect(L"Mir_11100");

    if (m_iCurFrame == 67)
    {
        if (m_iPreFrame != m_iCurFrame)
        {
            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 0.f;
            desc.fLifeTime = 0.5f;
            desc.fLimitDistance = 10.f;

            _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS);

            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 8.f, desc, KNOCKDOWN_ATTACK, 10.f);
        }
    }


    if (Is_AnimFinished())
    {
        _uint iRan = rand() % 2;

        CalCulate_PlayerDir();

        if (!m_bTurnMotion)
            m_eCurState = STATE::b_idle;
        else
        {
            if (m_eAttackDir == DIR::FORWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
            }
            else if (m_eAttackDir == DIR::BACKWARD_LEFT)
            {
                if (iRan == 0)
                    m_eCurState = STATE::skill_4100;
                else
                {
                    m_eCurState = STATE::turn_l;
                    m_fTurnSpeed = XM_PI * 0.7f;
                }
            }
            else if (m_eAttackDir == DIR::FORWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
            }
            else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
            {
                if (iRan == 0)
                    m_eCurState = STATE::skill_3100;
                else
                {
                    m_eCurState = STATE::turn_l;
                    m_fTurnSpeed = XM_PI * 0.7f;
                }
            }
        }
    }
}

void Boss_Mir_FSM::skill_11100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_11100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
}

void Boss_Mir_FSM::skill_12100()
{
    // For. Effect 
    if (m_iCurFrame >= 18 && m_iCurFrame <= 48)
    {
        for (_uint i = 0; i < 10; i++)
        {
            if (Init_CurFrame(28 + i*2))
            {
                Add_GroupEffectOwner(L"Mir_Lightning", _float3(0.f, 0.f, i * 3.f), false); // z+
                Add_GroupEffectOwner(L"Mir_Lightning", _float3(0.f, 0.f, i * -3.f), false); // z-
                Add_GroupEffectOwner(L"Mir_Lightning", _float3(i * 3.f, 0.f, 0.f), false); // x+
                Add_GroupEffectOwner(L"Mir_Lightning", _float3(i * -3.f, 0.f, 0.f), false); // x-
            }
        }
    }

    if (m_iCurFrame == 28 ||
        m_iCurFrame == 38 ||
        m_iCurFrame == 48 ||
        m_iCurFrame == 58)
    {
        if (m_iPreFrame != m_iCurFrame)
        {
            _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS);

            FORWARDMOVINGSKILLDESC desc;
            desc.fMoveSpeed = 20.f;
            desc.fLifeTime = 1.5f;
            desc.fLimitDistance = 20.f;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);

            wstring strAttackType = NORMAL_ATTACK;

            if (m_iCurFrame == 58)
                strAttackType = KNOCKBACK_ATTACK;

            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 2.f, desc, strAttackType, 10.f);

            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK) * -1.f;
            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 2.f, desc, strAttackType, 10.f);

            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::RIGHT) * -1.f;
            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 2.f, desc, strAttackType, 10.f);

            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::RIGHT) * 1.f;
            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 2.f, desc, strAttackType, 10.f);
        }
    }

    if (Is_AnimFinished())
    {
        _uint iRan = rand() % 2;

        CalCulate_PlayerDir();

        if (!m_bTurnMotion)
            m_eCurState = STATE::b_idle;
        else
        {
            if (m_eAttackDir == DIR::FORWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
            }
            else if (m_eAttackDir == DIR::BACKWARD_LEFT)
            {
                if (iRan == 0)
                    m_eCurState = STATE::skill_4100;
                else
                {
                    m_eCurState = STATE::turn_l;
                    m_fTurnSpeed = XM_PI * 0.7f;
                }
            }
            else if (m_eAttackDir == DIR::FORWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
            }
            else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
            {
                if (iRan == 0)
                    m_eCurState = STATE::skill_3100;
                else
                {
                    m_eCurState = STATE::turn_l;
                    m_fTurnSpeed = XM_PI * 0.7f;
                }
            }
        }
    }
}

void Boss_Mir_FSM::skill_12100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_12100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
}

void Boss_Mir_FSM::skill_13100()
{
    if (m_iCurFrame == 30)
    {
        if (m_iPreFrame != m_iCurFrame)
        {
            _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
                _float3::Up;

            FORWARDMOVINGSKILLDESC desc;
            desc.fMoveSpeed = 20.f;
            desc.fLifeTime = 1.5f;
            desc.fLimitDistance = 30.f;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK) +
                Get_Transform()->Get_State(Transform_State::RIGHT) * -1.f;

            // For. Collider 
            for (_uint i = 0; i < 3; i++)
            {
                Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 2.f, desc, KNOCKBACK_ATTACK, 10.f);

                desc.vSkillDir = desc.vSkillDir + Get_Transform()->Get_State(Transform_State::RIGHT);
            }

            // For. Effect 
            shared_ptr<Mir_13100_Fireball> pScript = make_shared<Mir_13100_Fireball>();
            Add_Effect(L"Mir_13100", pScript);
        }
    }

    if (Is_AnimFinished())
    {
        _uint iRan = rand() % 2;

        CalCulate_PlayerDir();

        if (!m_bTurnMotion)
            m_eCurState = STATE::b_idle;
        else
        {
            if (m_eAttackDir == DIR::FORWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
            }
            else if (m_eAttackDir == DIR::BACKWARD_LEFT)
            {
                if (iRan == 0)
                    m_eCurState = STATE::skill_4100;
                else
                {
                    m_eCurState = STATE::turn_l;
                    m_fTurnSpeed = XM_PI * 0.7f;
                }
            }
            else if (m_eAttackDir == DIR::FORWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
            }
            else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
            {
                if (iRan == 0)
                    m_eCurState = STATE::skill_3100;
                else
                {
                    m_eCurState = STATE::turn_l;
                    m_fTurnSpeed = XM_PI * 0.7f;
                }
            }
        }
    }
}

void Boss_Mir_FSM::skill_13100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_13100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
}

void Boss_Mir_FSM::skill_14100()
{
    if (m_iCurFrame == 25)
    {
        if (m_iPreFrame != m_iCurFrame)
        {
            _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS);

            FORWARDMOVINGSKILLDESC desc;
            desc.fMoveSpeed = 20.f;
            desc.fLifeTime = 1.5f;
            desc.fLimitDistance = 20.f;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);

            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 2.f, desc, KNOCKBACK_ATTACK, 10.f);

            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK) * -1.f;
            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 2.f, desc, KNOCKBACK_ATTACK, 10.f);

            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::RIGHT) * -1.f;
            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 2.f, desc, KNOCKBACK_ATTACK, 10.f);

            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::RIGHT) * 1.f;
            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 2.f, desc, KNOCKBACK_ATTACK, 10.f);

            // For. Effect 
            shared_ptr<Mir_13100_Fireball> pScript = make_shared<Mir_13100_Fireball>();
            Add_Effect(L"Mir_14100", pScript);
        }
    }
    else if (m_iCurFrame == 80)
    {
        if (m_iPreFrame != m_iCurFrame)
        {
            _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS);

            FORWARDMOVINGSKILLDESC desc;
            desc.fMoveSpeed = 20.f;
            desc.fLifeTime = 1.5f;
            desc.fLimitDistance = 20.f;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);

            wstring strAttackType = NORMAL_ATTACK;

            if (m_iCurFrame == 58)
                strAttackType = KNOCKBACK_ATTACK;

            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 2.f, desc, strAttackType, 10.f);

            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK) * -1.f;
            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 2.f, desc, strAttackType, 10.f);

            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::RIGHT) * -1.f;
            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 2.f, desc, strAttackType, 10.f);

            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::RIGHT) * 1.f;
            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 2.f, desc, strAttackType, 10.f);
        }
    }

    if (Is_AnimFinished())
    {
        _uint iRan = rand() % 2;

        CalCulate_PlayerDir();

        if (!m_bTurnMotion)
            m_eCurState = STATE::b_idle;
        else
        {
            if (m_eAttackDir == DIR::FORWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
            }
            else if (m_eAttackDir == DIR::BACKWARD_LEFT)
            {
                if (iRan == 0)
                    m_eCurState = STATE::skill_4100;
                else
                {
                    m_eCurState = STATE::turn_l;
                    m_fTurnSpeed = XM_PI * 0.7f;
                }
            }
            else if (m_eAttackDir == DIR::FORWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
            }
            else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
            {
                if (iRan == 0)
                    m_eCurState = STATE::skill_3100;
                else
                {
                    m_eCurState = STATE::turn_l;
                    m_fTurnSpeed = XM_PI * 0.7f;
                }
            }
        }
    }
}

void Boss_Mir_FSM::skill_14100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_14100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
}

void Boss_Mir_FSM::skill_100000()
{
    if (m_iCurFrame > 90)
    {
        if (!m_pTarget.expired())
            Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), XM_PI * 2.5f);
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::skill_100100;
}

void Boss_Mir_FSM::skill_100000_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100000", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
}

void Boss_Mir_FSM::skill_100100()
{
    for (_int i = 19; i < 85; i += 2)
    {
        MouseBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iMouseBoneIndex) *
            _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

        if (Init_CurFrame(i))
            Add_GroupEffectOwner(L"Mir_100100", _float3(MouseBoneMatrix.Translation().x, MouseBoneMatrix.Translation().y, MouseBoneMatrix.Translation().z), true);
    }

    if (m_iCurFrame == 1)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(m_fNormalAttack_AnimationSpeed / 4.f);

        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.05f, 0.2f, 1.f, 1.f);
        }

        m_bCounter = true;
    }
    else if (m_iCurFrame == 9)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(m_fNormalAttack_AnimationSpeed * 2.f);
        m_bCounter = false;
        
        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.f, 0.f, 0.f, 1.f);
        }
    }
    else if (m_iCurFrame == 19 ||
             m_iCurFrame == 29 ||
             m_iCurFrame == 39 ||
             m_iCurFrame == 49 ||
             m_iCurFrame == 59 ||
             m_iCurFrame == 69 ||
             m_iCurFrame == 79 ||
             m_iCurFrame == 84)
    {
        if (m_iPreFrame != m_iCurFrame)
        {
            MouseBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iMouseBoneIndex) *
                _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

            _float4 vBonePos = _float4{ MouseBoneMatrix.Translation().x, MouseBoneMatrix.Translation().y, MouseBoneMatrix.Translation().z , 1.f };

            vBonePos = vBonePos + Get_Transform()->Get_State(Transform_State::LOOK);

            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 20.f;
            desc.fLifeTime = 0.5f;
            desc.fLimitDistance = 10.f;

            _float4 vSkillPos = vBonePos;

            if (m_iCurFrame != 84)
                Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 3.f, desc, NORMAL_ATTACK, 10.f);
            else
                Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 3.f, desc, KNOCKDOWN_ATTACK, 10.f);
        }
    }
    else if (m_iCurFrame == 90)
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(m_fNormalAttack_AnimationSpeed);
    


    if (Is_AnimFinished())
    {
        CalCulate_PlayerDir();

        if (!m_bTurnMotion)
            m_eCurState = STATE::b_idle;
        else
        {
            if (m_eAttackDir == DIR::FORWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
            }
            else if (m_eAttackDir == DIR::BACKWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
                m_fTurnSpeed = XM_PI * 0.7f;
            }
            else if (m_eAttackDir == DIR::FORWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
            }
            else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
            {
                m_eCurState = STATE::turn_l;
                m_fTurnSpeed = XM_PI * 0.7f;   
            }
        }
    }
}

void Boss_Mir_FSM::skill_100100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_100100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
}

void Boss_Mir_FSM::skill_200000()
{
    if (Init_CurFrame(125))
        Add_GroupEffectOwner(L"Mir_200100_pizza", _float3(0.f, 0.f, 2.f), false);

    if (m_iCurFrame > 15)
    {
        if (!m_pTarget.expired())
            Soft_Turn_ToTarget(m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS), XM_PI * 2.f);
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::skill_200100;
}

void Boss_Mir_FSM::skill_200000_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200000", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
}

void Boss_Mir_FSM::skill_200100()
{
    if (Init_CurFrame(25))
        Add_GroupEffectOwner(L"Mir_200100", _float3(0.f, 0.f, 2.f), false);

    m_tBreathCoolTime.fAccTime += fDT;

    if (m_iCurFrame == 10)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(m_fNormalAttack_AnimationSpeed / 4.f);

        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.05f, 0.2f, 1.f, 1.f);
        }

        m_bCounter = true;
    }
    else if (m_iCurFrame == 20)
    {
        m_pOwner.lock()->Get_Animator()->Set_AnimationSpeed(m_fNormalAttack_AnimationSpeed / 2.f);
        m_bCounter = false;

        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
        {
            material->Get_MaterialDesc().emissive = Color(0.f, 0.f, 0.f, 1.f);
        }
    }
    else if (m_iCurFrame > 33 && m_iCurFrame < 100)
    {
        if (m_tBreathCoolTime.fAccTime >= m_tBreathCoolTime.fCoolTime)
        {
            m_tBreathCoolTime.fAccTime = 0.f;
        
            MouseBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iMouseBoneIndex) *
                _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();
        
            _float4 vBonePos = _float4{ MouseBoneMatrix.Translation().x, MouseBoneMatrix.Translation().y, MouseBoneMatrix.Translation().z , 1.f };
        
            vBonePos = vBonePos + Get_Transform()->Get_State(Transform_State::LOOK);
        
            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 20.f;
            desc.fLifeTime = 0.5f;
            desc.fLimitDistance = 10.f;
        
            _float4 vSkillPos = vBonePos;
        
            Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 3.f, desc, KNOCKBACK_ATTACK, 10.f);
        }
    }
    
    if (Is_AnimFinished())
    {
        CalCulate_PlayerDir();

        if (!m_bTurnMotion)
            m_eCurState = STATE::b_idle;
        else
        {
            if (m_eAttackDir == DIR::FORWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
            }
            else if (m_eAttackDir == DIR::BACKWARD_LEFT)
            {
                m_eCurState = STATE::turn_l;
                m_fTurnSpeed = XM_PI * 0.7f;   
            }
            else if (m_eAttackDir == DIR::FORWARD_RIGHT)
            {
                m_eCurState = STATE::turn_r;
            }
            else if (m_eAttackDir == DIR::BACKWARD_RIGHT)
            {
                m_eCurState = STATE::turn_l;
                m_fTurnSpeed = XM_PI * 0.7f;   
            }
        }
    }
}

void Boss_Mir_FSM::skill_200100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_200100", 0.15f, false, m_fNormalAttack_AnimationSpeed);

    m_tAttackCoolTime.fAccTime = 0.f;
}

Boss_Mir_FSM::DIR Boss_Mir_FSM::CalCulate_PlayerDir()
{
    _float4 vDir = _float4(0.f);
    vDir = m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) - Get_Transform()->Get_State(Transform_State::POS);
    vDir.y = 0.f; 
    vDir.Normalize();

    _float4 vDot = _float4(0.f);
    _float4 vCross = _float4(0.f);

    vDot = XMVector3Dot(Get_Transform()->Get_State(Transform_State::LOOK), vDir);
    vCross = XMVector3Cross(Get_Transform()->Get_State(Transform_State::LOOK), vDir);

    _float fStandardTurnDegree = 0.f;
    
    if (m_eCurPhase == PHASE::PHASE1)
        fStandardTurnDegree = 20.f;
    else
        fStandardTurnDegree = 20.f;
    

    if (XMVectorGetX(vDot) >= 0.f)
    {
        if (XMVectorGetY(vCross) < 0.f)
        {
            m_eAttackDir = DIR::FORWARD_LEFT;

            if (XMVectorGetX(vDot) < cosf(XMConvertToRadians(fStandardTurnDegree)))
                m_bTurnMotion = true;
        }
        else
        {
            m_eAttackDir = DIR::FORWARD_RIGHT;

            if (XMVectorGetX(vDot) < cosf(XMConvertToRadians(fStandardTurnDegree)))
                m_bTurnMotion = true;
        }
    }
    else
    {
        m_bTurnMotion = true;

        if (XMVectorGetY(vCross) < 0.f)
        {
            m_eAttackDir = DIR::BACKWARD_LEFT;
        }
        else
        {
            m_eAttackDir = DIR::BACKWARD_RIGHT;
        }
    }
    

    return m_eAttackDir;
}

void Boss_Mir_FSM::Add_Boss_Mir_Collider()
{
    shared_ptr<GameObject> attackCollider = make_shared<GameObject>();
    attackCollider->GetOrAddTransform();
    attackCollider->Add_Component(make_shared<SphereCollider>(3.f));
    attackCollider->Get_Collider()->Set_CollisionGroup(Monster_Attack);

    m_pAttackCollider = attackCollider;

    EVENTMGR.Create_Object(m_pAttackCollider.lock());
    m_pAttackCollider.lock()->Get_Collider()->Set_Activate(false);

    m_pAttackCollider.lock()->Add_Component(make_shared<AttackColliderInfoScript>());
    m_pAttackCollider.lock()->Set_Name(L"Boss_Mir_AttackCollider");
    m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_ColliderOwner(m_pOwner.lock());

    shared_ptr<GameObject> tailCollider = make_shared<GameObject>();
    tailCollider->GetOrAddTransform();
    tailCollider->Add_Component(make_shared<SphereCollider>(6.f));
    tailCollider->Get_Collider()->Set_CollisionGroup(Monster_Attack);

    EVENTMGR.Create_Object(tailCollider);
    tailCollider->Get_Collider()->Set_Activate(false);

    tailCollider->Add_Component(make_shared<AttackColliderInfoScript>());
    tailCollider->Set_Name(L"Boss_Mir_TailCollider");
    tailCollider->Get_Script<AttackColliderInfoScript>()->Set_ColliderOwner(m_pOwner.lock());

    m_pTailCollider = tailCollider;
}

void Boss_Mir_FSM::Create_CounterMotionTrail()
{
	for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
	{
		material->Get_MaterialDesc().emissive = Color(0.f, 0.f, 0.f, 1.f);
	}
    m_pOwner.lock()->Get_Script<CounterMotionTrailScript>()->Init();
}

void Boss_Mir_FSM::Create_Meteor()
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
            desc.fLifeTime = 1.5f;
            desc.fLimitDistance = 20.f;

            for (_uint i = 0; i < 6; i++)
            {
                _float fOffSetX = ((rand() * 2 / _float(RAND_MAX) - 1) * (rand() % 10 + 5));
                _float fOffSetZ = ((rand() * 2 / _float(RAND_MAX) - 1) * (rand() % 10 + 5));

                _float4 vSkillPos = vPlayerPos + _float4{ fOffSetX, 13.5f, fOffSetZ, 0.f };

           

                Add_GroupEffectOwner(L"Mir_Meteor_Meteor", _float3(vSkillPos.x, vPlayerPos.y, vSkillPos.z),true);
                Add_GroupEffectOwner(L"Mir_Meteor_Floor", _float3(vSkillPos.x, vPlayerPos.y, vSkillPos.z), true);
                Create_ForwardMovingSkillCollider(Monster_Skill, L"Boss_Mir_SkillCollider", vSkillPos, 1.f, desc, KNOCKDOWN_SKILL, 10.f);
            }
            {
                shared_ptr<GameObject> obj = make_shared<GameObject>();
                auto script = make_shared<TimerScript>(1.35f);
                script->Set_Function([]() {CAMERA_SHAKE(0.2f, 0.3f); });
                obj->Add_Component(script);
                EVENTMGR.Create_Object(obj);
            }
            m_iCurMeteorCnt++;
            m_tMeteorCoolTime.fAccTime = 0.f;
        }
    }

    if (m_eCurPhase == PHASE::PHASE2)
    {
        if (m_iCurMeteorCnt >= m_iLimitMeteorCnt)
        {
            m_iCurMeteorCnt = 0;
            m_bSummonMeteor = false;
        }
    }
}

void Boss_Mir_FSM::Create_DragonBall()
{
    _uint iDragonBallIndex = 4;

    for (_uint i = 0; i < 3; i++)
    {
        shared_ptr<GameObject> ObjDragonBall = make_shared<GameObject>();

        ObjDragonBall->Add_Component(make_shared<Transform>());

        ObjDragonBall->Get_Transform()->Set_State(Transform_State::POS, m_vDragonBallPosArray[i]);
        {
            shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

            shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
            {
                shared_ptr<Model> model = RESOURCES.Get<Model>(L"Anim_P_R02_DecoBall_00_Idle");
                animator->Set_Model(model);
            }

            ObjDragonBall->Add_Component(animator);

            ObjDragonBall->Add_Component(make_shared<SphereCollider>(2.f)); //SphereCollider
            ObjDragonBall->Get_Collider()->Set_CollisionGroup(MAPObject);
            ObjDragonBall->Get_Collider()->Set_Activate(true);

            wstring DragonBallName = L"Anim_P_R02_DecoBall_00_Idle-1"; // TODO 14,15,16
            DragonBallName += to_wstring(iDragonBallIndex);
            iDragonBallIndex++;

            ObjDragonBall->Set_Name(DragonBallName);
            ObjDragonBall->Set_DrawShadow(true);
            ObjDragonBall->Set_ObjectGroup(OBJ_MAPOBJECT);
            ObjDragonBall->Add_Component(make_shared<DragonBall_FSM>());
            ObjDragonBall->Get_FSM()->Set_Target(m_pOwner.lock());



            EVENTMGR.Create_Object(ObjDragonBall);

            //Add ObjectDissolveCreate
            ObjDragonBall->Add_Component(make_shared<ObjectDissolveCreate>(1.f));
            ObjDragonBall->Get_Script<ObjectDissolveCreate>()->Init();
        }
       
        //PLEASE MAKE RIGIDBODY 
        //PLEASE DO. CAPTAIN
        {
			_float3 vObjPos = ObjDragonBall->Get_Transform()->Get_State(Transform_State::POS).xyz();
			auto rigidBody = make_shared<RigidBody>();
            ObjDragonBall->Add_Component(rigidBody);
			rigidBody->Create_CapsuleRigidBody(vObjPos, 2.f, 2.f);
        }

    }
}

void Boss_Mir_FSM::Set_AttackPattern()
{
    //m_eCurState = STATE::SQ_SBRin_Roar;

    _uint iRan = rand() % 10;
    
    while (true)
    {
        if (iRan == m_iPreAttack)
            iRan = rand() % 10;
        else
            break;
    }

    if (iRan == 0)
    {
        m_eCurState = STATE::SQ_SBRin_Roar;
        m_iPreAttack = 0;

        if (rand() % 2 == 0)
        {
            m_bSummonMeteor = true;
            m_iLimitMeteorCnt = rand() % 3 + 1;
            m_iCurMeteorCnt = 0;
        }
    }
    else if (iRan == 1)
    {
        m_eCurState = STATE::skill_1100;
        m_iPreAttack = 1;

        if (rand() % 2 == 0)
        {
            m_bSummonMeteor = true;
            m_iLimitMeteorCnt = rand() % 3 + 1;
            m_iCurMeteorCnt = 0;
        }
    }
    else if (iRan == 2)
    {
        m_eCurState = STATE::skill_2100;
        m_iPreAttack = 2;

        if (rand() % 2 == 0)
        {
            m_bSummonMeteor = true;
            m_iLimitMeteorCnt = rand() % 3 + 1;
            m_iCurMeteorCnt = 0;
        }
    }
    else if (iRan == 3)
    {
        m_eCurState = STATE::skill_9100;
        m_iPreAttack = 3;
    }
    else if (iRan == 4)
    {
        m_eCurState = STATE::skill_11100;
        m_iPreAttack = 4;

        if (rand() % 2 == 0)
        {
            m_bSummonMeteor = true;
            m_iLimitMeteorCnt = rand() % 3 + 1;
            m_iCurMeteorCnt = 0;
        }
    }
    else if (iRan == 5)
    {
        m_eCurState = STATE::skill_12100;
        m_iPreAttack = 5;

        if (rand() % 2 == 0)
        {
            m_bSummonMeteor = true;
            m_iLimitMeteorCnt = rand() % 3 + 1;
            m_iCurMeteorCnt = 0;
        }
    }
    else if (iRan == 6)
    {
        m_eCurState = STATE::skill_13100;
        m_iPreAttack = 6;

        if (rand() % 2 == 0)
        {
            m_bSummonMeteor = true;
            m_iLimitMeteorCnt = rand() % 3 + 1;
            m_iCurMeteorCnt = 0;
        }
    }
    else if (iRan == 7)
    {
        m_eCurState = STATE::skill_14100;
        m_iPreAttack = 7;

        if (rand() % 2 == 0)
        {
            m_bSummonMeteor = true;
            m_iLimitMeteorCnt = rand() % 3 + 1;
            m_iCurMeteorCnt = 0;
        }
    }
    else if (iRan == 8)
    {
        m_eCurState = STATE::skill_100000;
        m_iPreAttack = 8;

        if (rand() % 2 == 0)
        {
            m_bSummonMeteor = true;
            m_iLimitMeteorCnt = rand() % 3 + 1;
            m_iCurMeteorCnt = 0;
        }
    }
    else if (iRan == 9)
    {
        m_eCurState = STATE::skill_200000;
        m_iPreAttack = 9;

        if (rand() % 2 == 0)
        {
            m_bSummonMeteor = true;
            m_iLimitMeteorCnt = rand() % 3 + 1;
            m_iCurMeteorCnt = 0;
        }
    }

}

void Boss_Mir_FSM::Setting_DragonBall()
{
    _uint iDragonBallIndex = 4;

    for (_uint i = 0; i < 3; i++)
    {
        wstring DragonBallName = L"Anim_P_R02_DecoBall_00_Idle-1"; // TODO 14,15,16
        DragonBallName += to_wstring(iDragonBallIndex);
        auto DragonBall = CUR_SCENE->Get_GameObject(DragonBallName);
        
        m_vDragonBallPosArray[i] = DragonBall->Get_Transform()->Get_State(Transform_State::POS);

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

void Boss_Mir_FSM::Calculate_IntroHeadCam()
{
    HeadBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iHeadBoneIndex) *
        _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

    m_vHeadBonePos = _float4{ HeadBoneMatrix.Translation().x, HeadBoneMatrix.Translation().y, HeadBoneMatrix.Translation().z , 1.f };
    m_vHeadCamPos = m_vHeadBonePos + (Get_Transform()->Get_State(Transform_State::LOOK) * 10.f);
}

void Boss_Mir_FSM::Calculate_PhaseChangeHeadCam()
{
    HeadBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iHeadBoneIndex) *
        _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

    m_vHeadBonePos = _float4{ HeadBoneMatrix.Translation().x, HeadBoneMatrix.Translation().y, HeadBoneMatrix.Translation().z , 1.f };
    m_vHeadCamPos = m_vHeadBonePos + (Get_Transform()->Get_State(Transform_State::LOOK) * 10.f);

    TopBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iTopBoneIndex) *
        _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

    m_vTopBonePos = _float4{ TopBoneMatrix.Translation().x, TopBoneMatrix.Translation().y, TopBoneMatrix.Translation().z , 1.f };
}

void Boss_Mir_FSM::Check_PhaseChange()
{
    if (m_pOwner.lock()->Get_HpRatio() >= 0.33f && m_pOwner.lock()->Get_HpRatio() <= 0.66f)
    {
        if (!m_bCheckPhaseChange[0])
            m_bCheckPhaseChange[0] = true;
    }
    else if (m_pOwner.lock()->Get_HpRatio() <= 0.33f)
    {
        if (!m_bCheckPhaseChange[1])
            m_bCheckPhaseChange[1] = true;
    }
}

void Boss_Mir_FSM::TailAttackCollider_On(const wstring& skillname, _float fAttackDamage)
{
    m_pTailCollider.lock()->Get_Collider()->Set_Activate(true);
    m_pTailCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_SkillName(skillname);
    m_pTailCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_AttackDamage(fAttackDamage);
}

void Boss_Mir_FSM::TailAttackCollider_Off()
{
    m_pTailCollider.lock()->Get_Collider()->Set_Activate(false);
    m_pTailCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_SkillName(L"");
    m_pTailCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_AttackDamage(0.f);
}

void Boss_Mir_FSM::DeadSetting()
{
    if (m_bIsDead)
    {
        m_bInvincible = true;
        m_eCurState = STATE::SQ_Flee;
    }
}

void Boss_Mir_FSM::Load_Giant_Boss_Mir()
{
    // Add. Monster
    shared_ptr<GameObject> ObjMonster = make_shared<GameObject>();

    ObjMonster->Add_Component(make_shared<Transform>());

    ObjMonster->Get_Transform()->Set_State(Transform_State::POS, _float4(0.f, 0.f, 0.f, 1.f));
    {
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

        shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
        {
            shared_ptr<Model> model = RESOURCES.Get<Model>(L"Mir_03");
            animator->Set_Model(model);
        }

        ObjMonster->Add_Component(animator);
        ObjMonster->Add_Component(make_shared<Boss_Giant_Mir_FSM>());
        ObjMonster->Get_FSM()->Set_Target(m_pTarget.lock());
        ObjMonster->Get_FSM()->Init();
    }

    EVENTMGR.Create_Object(ObjMonster);
}


_float Boss_Mir_FSM::CamDistanceLerp(_float fStart, _float fEnd, _float fRatio)
{
    return fStart * (1.f - fRatio) + fEnd * fRatio;
}