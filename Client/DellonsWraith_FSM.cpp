#include "pch.h"
#include "MainCameraScript.h"
#include "DellonsWraith_FSM.h"
#include "ModelAnimator.h"
#include "SphereCollider.h"
#include "AttackColliderInfoScript.h"
#include "Model.h"



DellonsWraith_FSM::DellonsWraith_FSM()
{
}

DellonsWraith_FSM::~DellonsWraith_FSM()
{
}

HRESULT DellonsWraith_FSM::Init()
{
    if (!m_bInitialize)
    {
        auto animator = Get_Owner()->Get_Animator();
        if (animator)
        {
            animator->Set_CurrentAnim(L"FX_DellonsWraith_skill_30010", true, 1.f);
            m_eCurState = STATE::FX_DellonsWraith_skill_30010;
        }

   
        shared_ptr<GameObject> attackCollider = make_shared<GameObject>();
        attackCollider->GetOrAddTransform();
        attackCollider->Add_Component(make_shared<SphereCollider>(2.5f));
        
        m_pAttackCollider = attackCollider;

        EVENTMGR.Create_Object(m_pAttackCollider.lock());
        m_pAttackCollider.lock()->Get_Collider()->Set_Activate(false);
        m_pAttackCollider.lock()->Get_Collider()->Set_CollisionGroup(Player_Attack);

        m_pAttackCollider.lock()->Add_Component(make_shared<AttackColliderInfoScript>());
        m_pAttackCollider.lock()->Get_Script<AttackColliderInfoScript>()->Set_ColliderOwner(Get_Owner());

        m_pAttackCollider.lock()->Set_Name(L"Wraith_AttackCollider");

        m_iSkillBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Bip001-R-Hand");



        m_bInitialize = true;
    }

    return S_OK;
}

void DellonsWraith_FSM::Tick()
{
    State_Tick();

    if (!m_pAttackCollider.expired())
    {
        //(AnimationModel BonePos)
        matBoneMatrix = m_pOwner.lock()->Get_Animator()->Get_CurAnimTransform(m_iSkillBoneIndex) *
            _float4x4::CreateRotationX(XMConvertToRadians(-90.f)) * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI) * m_pOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

        _float4 vBonePos = _float4{ matBoneMatrix.Translation().x, matBoneMatrix.Translation().y, matBoneMatrix.Translation().z , 1.f };
        m_pAttackCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, vBonePos);
    }
}

void DellonsWraith_FSM::State_Tick()
{
    State_Init();

    m_iCurFrame = Get_CurFrame();

    switch (m_eCurState)
    {
    case STATE::FX_DellonsWraith_skill_30010:
        FX_DellonsWraith_skill_30010();
        break;
    case STATE::FX_DellonsWraith_skill_400100:
        FX_DellonsWraith_skill_400100();
        break;
    case STATE::FX_DellonsWraith_skill_601100:
        FX_DellonsWraith_skill_601100();
        break;
    case STATE::FX_DellonsWraith_skill_903100:
        FX_DellonsWraith_skill_903100();
        break;
    case STATE::FX_DellonsWraith01_Special_01:
        FX_DellonsWraith01_Special_01();
        break;
    case STATE::FX_SQ_Dellons_QuestEnd_2:
        FX_SQ_Dellons_QuestEnd_2();
        break;
    case STATE::FX_Mn_Dellons_skill_5100:
        FX_Mn_Dellons_skill_5100();
        break;
    case STATE::FX_Mn_Dellons_skill_500200:
        FX_Mn_Dellons_skill_500200();
        break;
    case STATE::FX_Mn_Dellons_skill_5200:
        FX_Mn_Dellons_skill_5200();
        break;
    case STATE::FX_Mn_Dellons_skill_6100:
        FX_Mn_Dellons_skill_6100();
        break;
    }

    Update_GroupEffectWorldPos(Get_Owner()->Get_Transform()->Get_WorldMatrix());

    if (m_iPreFrame != m_iCurFrame)
        m_iPreFrame = m_iCurFrame;
}

void DellonsWraith_FSM::State_Init()
{
    if (m_eCurState != m_ePreState)
    {
        switch (m_eCurState)
        {
        case STATE::FX_DellonsWraith_skill_30010:
            FX_DellonsWraith_skill_30010_Init();
            break;
        case STATE::FX_DellonsWraith_skill_400100:
            FX_DellonsWraith_skill_400100_Init();
            break;
        case STATE::FX_DellonsWraith_skill_601100:
            FX_DellonsWraith_skill_601100_Init();
            break;
        case STATE::FX_DellonsWraith_skill_903100:
            FX_DellonsWraith_skill_903100_Init();
            break;
        case STATE::FX_DellonsWraith01_Special_01:
            FX_DellonsWraith01_Special_01_Init();
            break;
        case STATE::FX_SQ_Dellons_QuestEnd_2:
            FX_SQ_Dellons_QuestEnd_2_Init();
            break;
        case STATE::FX_Mn_Dellons_skill_5100:
            FX_Mn_Dellons_skill_5100_Init();
            break;
        case STATE::FX_Mn_Dellons_skill_500200:
            FX_Mn_Dellons_skill_500200_Init();
            break;
        case STATE::FX_Mn_Dellons_skill_5200:
            FX_Mn_Dellons_skill_5200_Init();
            break;
        case STATE::FX_Mn_Dellons_skill_6100:
            FX_Mn_Dellons_skill_6100_Init();
            break;
        }
        m_ePreState = m_eCurState;
    }
}

void DellonsWraith_FSM::Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget)
{

}

void DellonsWraith_FSM::Set_State(_uint iIndex)
{
    m_eCurState = (STATE)iIndex;
}

void DellonsWraith_FSM::FX_DellonsWraith_skill_30010()
{
    if (m_iCurFrame == 42)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 50)
        AttackCollider_Off();
    else if (m_iCurFrame == 54)
    {
        if (m_iPreFrame != m_iCurFrame)
        {
            FORWARDMOVINGSKILLDESC desc;
            desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
            desc.fMoveSpeed = 0.f;
            desc.fLifeTime = 0.5f;
            desc.fLimitDistance = 0.f;

            
            _float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
                Get_Transform()->Get_State(Transform_State::RIGHT) * -4.f +
                Get_Transform()->Get_State(Transform_State::LOOK) * 3.f +
                _float3::Up;

            Create_ForwardMovingSkillCollider(Player_Skill, L"Wraith_SkillCollider", vSkillPos, 1.5f, desc, KNOCKBACK_ATTACK, 10.f);
           
            vSkillPos = vSkillPos + Get_Transform()->Get_State(Transform_State::LOOK) +
                                    Get_Transform()->Get_State(Transform_State::RIGHT) * 2.f;

            Create_ForwardMovingSkillCollider(Player_Skill, L"Wraith_SkillCollider", vSkillPos, 1.5f, desc, KNOCKBACK_ATTACK, 10.f);
            
            vSkillPos = vSkillPos + Get_Transform()->Get_State(Transform_State::LOOK) +
                                    Get_Transform()->Get_State(Transform_State::RIGHT) * 2.f;

            Create_ForwardMovingSkillCollider(Player_Skill, L"Wraith_SkillCollider", vSkillPos, 1.5f, desc, KNOCKBACK_ATTACK, 10.f);

            vSkillPos = vSkillPos - Get_Transform()->Get_State(Transform_State::LOOK) +
                                    Get_Transform()->Get_State(Transform_State::RIGHT) * 2.f;

            Create_ForwardMovingSkillCollider(Player_Skill, L"Wraith_SkillCollider", vSkillPos, 1.5f, desc, KNOCKBACK_ATTACK, 10.f);

            vSkillPos = vSkillPos - Get_Transform()->Get_State(Transform_State::LOOK) +
                                    Get_Transform()->Get_State(Transform_State::RIGHT) * 2.f;

            Create_ForwardMovingSkillCollider(Player_Skill, L"Wraith_SkillCollider", vSkillPos, 1.5f, desc, KNOCKBACK_ATTACK, 10.f);
        }
    }

    if (Is_AnimFinished())
    {
        EVENTMGR.Delete_Object(m_pAttackCollider.lock());
        EVENTMGR.Delete_Object(m_pOwner.lock());
    }
}

void DellonsWraith_FSM::FX_DellonsWraith_skill_30010_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"FX_DellonsWraith_skill_30010", 0.15f, false, 1.f);

    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());
}

void DellonsWraith_FSM::FX_DellonsWraith_skill_400100()
{
    if (m_iCurFrame == 12)
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    else if (m_iCurFrame == 20)
        AttackCollider_Off();


    if (Is_AnimFinished())
    {
        EVENTMGR.Delete_Object(m_pAttackCollider.lock());
        EVENTMGR.Delete_Object(m_pOwner.lock());
    }
}

void DellonsWraith_FSM::FX_DellonsWraith_skill_400100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"FX_DellonsWraith_skill_400100", 0.15f, false, 1.5f);

    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());
}

void DellonsWraith_FSM::FX_DellonsWraith_skill_601100()
{
    if (m_iCurFrame == 126)
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    else if (m_iCurFrame == 140)
        AttackCollider_Off();


    if (Is_AnimFinished())
    {
        EVENTMGR.Delete_Object(m_pAttackCollider.lock());
        EVENTMGR.Delete_Object(m_pOwner.lock());
    }
}

void DellonsWraith_FSM::FX_DellonsWraith_skill_601100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"FX_DellonsWraith_skill_601100", 0.15f, false, 1.f);

    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());
}

void DellonsWraith_FSM::FX_DellonsWraith_skill_903100()
{
    if (m_iCurFrame == 65)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 72)
        AttackCollider_Off();
    else if (m_iCurFrame == 77)
        AttackCollider_On(NORMAL_ATTACK, 10.f);
    else if (m_iCurFrame == 82)
        AttackCollider_Off();
    else if (m_iCurFrame == 109)
        AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
    else if (m_iCurFrame == 116)
        AttackCollider_Off();

    if (Is_AnimFinished())
    {
        EVENTMGR.Delete_Object(m_pAttackCollider.lock());
        EVENTMGR.Delete_Object(m_pOwner.lock());
    }
}

void DellonsWraith_FSM::FX_DellonsWraith_skill_903100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"FX_DellonsWraith_skill_903100", 0.15f, false, 1.f);

    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());
}

void DellonsWraith_FSM::FX_DellonsWraith01_Special_01()
{
    if (Is_AnimFinished())
    {
        EVENTMGR.Delete_Object(m_pAttackCollider.lock());
        EVENTMGR.Delete_Object(m_pOwner.lock());
    }
}

void DellonsWraith_FSM::FX_DellonsWraith01_Special_01_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"FX_DellonsWraith01_Special_01", 0.15f, false, 1.f);

    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());
}


void DellonsWraith_FSM::FX_SQ_Dellons_QuestEnd_2()
{
    if (m_iCurFrame == 85)
        AttackCollider_On(KNOCKDOWN_ATTACK, 10.f);
    else if (m_iCurFrame == 100)
        AttackCollider_Off();

    if (Is_AnimFinished())
    {
        EVENTMGR.Delete_Object(m_pAttackCollider.lock());
        EVENTMGR.Delete_Object(m_pOwner.lock());
    }
}

void DellonsWraith_FSM::FX_SQ_Dellons_QuestEnd_2_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"FX_SQ_Dellons_QuestEnd_2", 0.15f, false, 1.f);

    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());
}

void DellonsWraith_FSM::FX_Mn_Dellons_skill_500200()
{
    if (m_iCurFrame == 19)
        AttackCollider_On(KNOCKDOWN_ATTACK, 10.f);

    if (Is_AnimFinished())
    {
        EVENTMGR.Delete_Object(m_pAttackCollider.lock());
        EVENTMGR.Delete_Object(m_pOwner.lock());
    }
}

void DellonsWraith_FSM::FX_Mn_Dellons_skill_500200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"FX_Mn_Dellons_skill_500200", 0.15f, false, 1.f);

    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());
}

void DellonsWraith_FSM::FX_Mn_Dellons_skill_5100()
{
    if (m_iCurFrame == 94)
    {
        EVENTMGR.Delete_Object(m_pAttackCollider.lock());
        EVENTMGR.Delete_Object(m_pOwner.lock());
    }
    
    //if (m_iCurFrame == 80)
    //    AttackCollider_On(KNOCKDOWN_ATTACK, 10.f);
    //else if (m_iCurFrame == 94)
    //{
    //    CUR_SCENE->Remove_GameObject(m_pAttackCollider.lock());
    //    CUR_SCENE->Remove_GameObject(m_pOwner.lock());
    //}
}

void DellonsWraith_FSM::FX_Mn_Dellons_skill_5100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_CurrentAnim(L"FX_Mn_Dellons_skill_5100",  false, 1.f);

    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());
}


void DellonsWraith_FSM::FX_Mn_Dellons_skill_6100()
{
    if (m_iCurFrame == 59)
        AttackCollider_On(KNOCKDOWN_ATTACK, 10.f);
    else if (m_iCurFrame == 69)
        AttackCollider_Off();

    if (Is_AnimFinished())
    {
        EVENTMGR.Delete_Object(m_pAttackCollider.lock());
        EVENTMGR.Delete_Object(m_pOwner.lock());
    }
}

void DellonsWraith_FSM::FX_Mn_Dellons_skill_6100_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"FX_Mn_Dellons_skill_6100", 0.15f, false, 1.f);

    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());
}

void DellonsWraith_FSM::FX_Mn_Dellons_skill_5200()
{
    if (m_iCurFrame == 90)
        AttackCollider_On(KNOCKDOWN_ATTACK, 10.f);
    else if (m_iCurFrame == 114)
        AttackCollider_Off();

    if (Is_AnimFinished())
    {
        EVENTMGR.Delete_Object(m_pAttackCollider.lock());
        EVENTMGR.Delete_Object(m_pOwner.lock());
    }
}

void DellonsWraith_FSM::FX_Mn_Dellons_skill_5200_Init()
{
    shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    animator->Set_NextTweenAnim(L"FX_Mn_Dellons_skill_5200", 0.15f, false, 1.5f);

    if (!m_pTarget.expired())
        Get_Transform()->Set_WorldMat(m_pTarget.lock()->Get_Transform()->Get_WorldMatrix());
}



