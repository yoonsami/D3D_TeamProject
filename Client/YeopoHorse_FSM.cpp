#include "pch.h"
#include "MainCameraScript.h"
#include "YeopoHorse_FSM.h"
#include "ModelAnimator.h"
#include "SphereCollider.h"
#include "AttackColliderInfoScript.h"
#include "Model.h"



YeopoHorse_FSM::YeopoHorse_FSM()
{
}

YeopoHorse_FSM::~YeopoHorse_FSM()
{
    if (!m_pAttackCollider.expired())
        EVENTMGR.Delete_Object(m_pAttackCollider.lock());
}

HRESULT YeopoHorse_FSM::Init()
{
    if (!m_bInitialize)
    {
        auto animator = Get_Owner()->Get_Animator();
        if (animator)
        {
            animator->Set_CurrentAnim(L"n_idle", true, 1.f);
            m_eCurState = STATE::n_idle;
        }
        shared_ptr<GameObject> attackCollider = make_shared<GameObject>();
        attackCollider->GetOrAddTransform();
        attackCollider->Add_Component(make_shared<SphereCollider>(1.f));
        attackCollider->Get_Collider()->Set_CollisionGroup(Player_Attack);

        m_pAttackCollider = attackCollider;

        EVENTMGR.Create_Object(m_pAttackCollider.lock());
        m_pAttackCollider.lock()->Get_Collider()->Set_Activate(false);
        
        m_pAttackCollider.lock()->Add_Component(make_shared<AttackColliderInfoScript>());
        m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_ColliderOwner(Get_Owner());

        m_pAttackCollider.lock()->Set_Name(L"Vehicle_AttackCollider");
   
        m_iSkillBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Bip001-Head");
        m_pOwner.lock()->Set_DrawShadow(true);



        m_bInitialize = true;
    }


    return S_OK;
}

void YeopoHorse_FSM::Tick()
{
    if (!m_pAttackCollider.expired())
    {
        //Head_Bone_Pos
        //(AnimationModel BonePos)
        matBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iSkillBoneIndex) *
            _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

        _float4 vBonePos = _float4{ matBoneMatrix.Translation().x, matBoneMatrix.Translation().y, matBoneMatrix.Translation().z , 1.f };
        m_pAttackCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, vBonePos);
    }
    State_Tick();

}

void YeopoHorse_FSM::State_Tick()
{
    State_Init();

    m_iCurFrame = Get_CurFrame();

    switch (m_eCurState)
    {
    case STATE::n_idle:
        n_idle();
        break;
    case STATE::skill_400100_fx:
        skill_400100_fx();
        break;
    case STATE::skill_902100_fx:
        skill_902100_fx();
        break;
    case STATE::SQ_RideHorse_Idle:
        SQ_RideHorse_Idle();
        break;
    case STATE::SQ_RideHorse_Run:
        SQ_RideHorse_Run();
        break;
    case STATE::SQ_RideHorse_Stop:
        SQ_RideHorse_Stop();
        break;
    case STATE::SQ_RideHorse_End:
        SQ_RideHorse_End();
        break;
    }

    Update_GroupEffectWorldPos(Get_Owner()->Get_Transform()->Get_WorldMatrix());

    if (m_iPreFrame != m_iCurFrame)
        m_iPreFrame = m_iCurFrame;
}

void YeopoHorse_FSM::State_Init()
{
    if (m_eCurState != m_ePreState)
    {
        switch (m_eCurState)
        {
        case STATE::n_idle:
            n_idle_Init();
            break;
        case STATE::skill_400100_fx:
            skill_400100_fx_Init();
            break;
        case STATE::skill_902100_fx:
            skill_902100_fx_Init();
            break;
        case STATE::SQ_RideHorse_Idle:
            SQ_RideHorse_Idle_Init();
            break;
        case STATE::SQ_RideHorse_Run:
            SQ_RideHorse_Run_Init();
            break;
        case STATE::SQ_RideHorse_Stop:
            SQ_RideHorse_Stop_Init();
            break;
        case STATE::SQ_RideHorse_End:
            SQ_RideHorse_End_Init();
            break;
        }
        m_ePreState = m_eCurState;
    }
}

void YeopoHorse_FSM::Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget)
{
    
}

void YeopoHorse_FSM::Set_State(_uint iIndex)
{
    m_eCurState = (STATE)iIndex;
}

void YeopoHorse_FSM::n_idle()
{
}

void YeopoHorse_FSM::n_idle_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"n_idle", 0.1f, true, 1.f);
}

void YeopoHorse_FSM::skill_400100_fx()
{
    int a = 10;


    //skillcam look  Dummycam (SKILLCAM->DUMMYCAM)

    if (Is_AnimFinished())
    {
        EVENTMGR.Delete_Object(m_pAttackCollider.lock());
        EVENTMGR.Delete_Object(m_pOwner.lock());
    }
}

void YeopoHorse_FSM::skill_400100_fx_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_400100_fx", 0.15f, false, 1.f);

    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());

    AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
}

void YeopoHorse_FSM::skill_902100_fx()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::n_idle;
}

void YeopoHorse_FSM::skill_902100_fx_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"skill_902100_fx", 0.15f, false, 1.f);
}

void YeopoHorse_FSM::SQ_RideHorse_Idle()
{
    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());
}

void YeopoHorse_FSM::SQ_RideHorse_Idle_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"SQ_RideHorse_Idle", 0.15f, true, 1.f);

    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());

    AttackCollider_Off();
}

void YeopoHorse_FSM::SQ_RideHorse_Run()
{
    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());
}

void YeopoHorse_FSM::SQ_RideHorse_Run_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"SQ_RideHorse_Run", 0.1f, true, 1.f);

    AttackCollider_On(KNOCKDOWN_ATTACK, 10.f);
}

void YeopoHorse_FSM::SQ_RideHorse_Stop()
{
    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());
}

void YeopoHorse_FSM::SQ_RideHorse_Stop_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"SQ_RideHorse_Stop", 0.1f, false, 1.f);

    AttackCollider_Off();
}

void YeopoHorse_FSM::SQ_RideHorse_End()
{
    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());

    if (Is_AnimFinished())
    {
        EVENTMGR.Delete_Object(m_pAttackCollider.lock());
        EVENTMGR.Delete_Object(m_pOwner.lock());
    }
}

void YeopoHorse_FSM::SQ_RideHorse_End_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"SQ_RideHorse_Idle", 0.15f, false, 1.f);

}
