#include "pch.h"
#include "Friend_FSM.h"
#include "ModelAnimator.h"
#include "ObjectDissolve.h"
#include "ModelRenderer.h"
#include "WeaponScript.h"
#include "Model.h"
#include <SphereCollider.h>
#include "AttackColliderInfoScript.h"
#include "DellonsWraith_FSM.h"
#include "MainCameraScript.h"

Friend_FSM::Friend_FSM(HERO eType)
    : m_eType(eType)
{
}

HRESULT Friend_FSM::Init()
{
	shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
	{
		shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
		shared_ptr<Model> model = RESOURCES.Get<Model>(GET_DATA(m_eType).ModelTag);
		animator->Set_Model(model);
		Get_Owner()->Add_Component(animator);
	}
	_float3 vPos = Get_Transform()->Get_State(Transform_State::POS).xyz();
	m_fBottomHeight = vPos.y;

	Get_Transform()->Set_State(Transform_State::POS, _float4(vPos, 1.f) + _float3::Up * 5.f);

    if(!GET_DATA(m_eType).WeaponTag.empty())
    {
        shared_ptr<GameObject> weapon = make_shared<GameObject>();
        weapon->GetOrAddTransform();
        shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
        shared_ptr<Model> model = RESOURCES.Get<Model>(GET_DATA(m_eType).WeaponTag);
        renderer->Set_Model(model);
        weapon->Add_Component(renderer);

		WeaponScript::WEAPONDESC desc;
		desc.strBoneName = L"Bip001-Prop1";
		desc.matPivot = _float4x4::CreateRotationX(-XM_PI / 2.f) * _float4x4::CreateRotationZ(XM_PI);
		desc.pWeaponOwner = Get_Owner();
        weapon->Add_Component(make_shared<WeaponScript>(desc));
        weapon->Set_DrawShadow(true);
        weapon->Set_VelocityMap(true);
        weapon->Set_Name(L"Friend_Weapon");
        weapon->Init();
        EVENTMGR.Create_Object(weapon);
        Set_Weapon(weapon);
    }

	shared_ptr<GameObject> attackCollider = make_shared<GameObject>();
	attackCollider->GetOrAddTransform();
	attackCollider->Add_Component(make_shared<SphereCollider>(1.f));
	attackCollider->Get_Collider()->Set_CollisionGroup(Player_Attack);

	Set_AttackCollider(attackCollider);

	EVENTMGR.Create_Object(attackCollider);
	attackCollider->Get_Collider()->Set_Activate(false);

	attackCollider->Add_Component(make_shared<AttackColliderInfoScript>());
	attackCollider->Set_Name(L"Friend_AttackCollider");
	attackCollider->Get_Script<AttackColliderInfoScript>()->Set_ColliderOwner(Get_Owner());

    m_eCurState = STATE::fall_loop;
	{
		shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();
		auto animation = Get_Owner()->Get_Model()->Get_AnimationByName(L"fall_loop");
		if (animation)
			animator->Set_CurrentAnim(L"fall_loop", true, 1.f);
		else
		{
			animation = Get_Owner()->Get_Model()->Get_AnimationByName(L"fall");
			if (animation)
				animator->Set_CurrentAnim(L"fall", true, 1.f);
		}
	}

    return S_OK;
}

void Friend_FSM::Tick()
{
    State_Tick();
	if (!m_pAttackCollider.expired())
	{
		//m_pAttack transform set forward
		m_pAttackCollider.lock()->Get_Transform()->Set_State(Transform_State::POS, Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 1.5f + _float3::Up);
	}
}

void Friend_FSM::Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget)
{
}

void Friend_FSM::State_Tick()
{
    State_Init();
    m_iCurFrame = Get_CurFrame();
    switch (m_eCurState)
    {
    case Friend_FSM::STATE::fall_loop:
        fall_loop();
        break;
    case Friend_FSM::STATE::fall_end:
        fall_end();
        break;
    case Friend_FSM::STATE::ATTACK:
        ATTACK();
        break;
    case Friend_FSM::STATE::EXIT:
        EXIT();
        break;
    }
}

void Friend_FSM::State_Init()
{
    if (m_eCurState != m_ePreState)
    {
		switch (m_eCurState)
		{
		case Friend_FSM::STATE::fall_loop:
            fall_loop_Init();
            break;
        case Friend_FSM::STATE::fall_end:
            fall_end_Init();
            break;
        case Friend_FSM::STATE::ATTACK:
            ATTACK_Init();
            break;
        case Friend_FSM::STATE::EXIT:
            EXIT_Init();
            break;
        }
        m_ePreState = m_eCurState;
    }

	if (m_iPreFrame != m_iCurFrame)
		m_iPreFrame = m_iCurFrame;
}

void Friend_FSM::Set_State(_uint iIndex)
{
}

void Friend_FSM::Summon_Wraith()
{
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
	ObjWraith->Set_Name(L"Wraith");

	EVENTMGR.Create_Object(ObjWraith);

	m_pDellonsWraith = ObjWraith;
}

void Friend_FSM::Set_WraithState(_uint iAnimindex)
{
	if (!m_pDellonsWraith.expired())
		m_pDellonsWraith.lock()->Get_FSM()->Set_State(iAnimindex);
}

void Friend_FSM::fall_loop()
{
    _float3 vPos = Get_Transform()->Get_State(Transform_State::POS).xyz();
    m_fStateAcc -= 9.8f * fDT;
    vPos += _float3(0.f, m_fStateAcc, 0.f) * fDT;


    if (vPos.y < m_fBottomHeight)
    {
        vPos.y = m_fBottomHeight;
        m_eCurState = STATE::fall_end;
    }

    Get_Transform()->Set_State(Transform_State::POS, _float4(vPos, 1.F));
}

void Friend_FSM::fall_loop_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();
    auto animation = Get_Owner()->Get_Model()->Get_AnimationByName(L"fall_loop");
    if(animation)
	    animator->Set_NextTweenAnim(L"fall_loop", 0.15f, true, 1.f);
    else
    {
        animation = Get_Owner()->Get_Model()->Get_AnimationByName(L"fall");
        if(animation)
            animator->Set_NextTweenAnim(L"fall", 0.15f, true, 1.f);
    }
    m_fStateAcc = 0.f;
}

void Friend_FSM::fall_end()
{
    if (Is_AnimFinished())
        m_eCurState = STATE::ATTACK;
}

void Friend_FSM::fall_end_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_CurrentAnim(L"fall_end",  false, 2.f);
	m_fStateAcc = 0.f;
}

void Friend_FSM::ATTACK()
{
	if (!m_pLookingTarget.expired())
	{
		Look_DirToTarget();
	}

    switch (m_eType)
    {
    case HERO::PLAYER:
        break;
    case HERO::ACE3:
    {
		
		if (m_iCurFrame == 15)
			AttackCollider_On(KNOCKBACK_ATTACK, 10.f);
		else if (m_iCurFrame == 33)
			AttackCollider_Off();
    }
        break;
    case HERO::KYLE:
    {

		if (Init_CurFrame(11))
		{
			FORWARDMOVINGSKILLDESC desc;
			desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
			desc.fMoveSpeed = 0.f;
			desc.fLifeTime = 0.5f;
			desc.fLimitDistance = 0.f;

			_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
				Get_Transform()->Get_State(Transform_State::UP);
			Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 4.f, desc, NORMAL_ATTACK, 10.f);

		}
		else if (Init_CurFrame(31))
		{
			FORWARDMOVINGSKILLDESC desc;
			desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
			desc.fMoveSpeed = 0.f;
			desc.fLifeTime = 0.5f;
			desc.fLimitDistance = 0.f;

			_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
				Get_Transform()->Get_State(Transform_State::UP);
			Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 4.f, desc, NORMAL_ATTACK, 10.f);

		}
		else if (Init_CurFrame(47))
		{
			FORWARDMOVINGSKILLDESC desc;
			desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
			desc.fMoveSpeed = 0.f;
			desc.fLifeTime = 0.5f;
			desc.fLimitDistance = 0.f;

			_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
				Get_Transform()->Get_State(Transform_State::UP);
			Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 4.f, desc, NORMAL_ATTACK, 10.f);

		}
		else if (Init_CurFrame(87))
		{
			FORWARDMOVINGSKILLDESC desc;
			desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
			desc.fMoveSpeed = 0.f;
			desc.fLifeTime = 0.5f;
			desc.fLimitDistance = 0.f;

			_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) +
				Get_Transform()->Get_State(Transform_State::LOOK) * 5.f;
			Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 4.f, desc, AIRBORNE_SKILL, 10.f);
		}
    }
        break;
    case HERO::YEOPO:
    {
		if (m_iCurFrame == 12)
			AttackCollider_On(NORMAL_ATTACK, 10.f);
		else if (m_iCurFrame == 16)
			AttackCollider_Off();
		else if (m_iCurFrame == 17)
			AttackCollider_On(NORMAL_ATTACK, 10.f);
		else if (m_iCurFrame == 19)
			AttackCollider_Off();
		else if (m_iCurFrame == 21)
			AttackCollider_On(NORMAL_ATTACK, 10.f);
		else if (m_iCurFrame == 23)
			AttackCollider_Off();
		else if (m_iCurFrame == 25)
			AttackCollider_On(NORMAL_ATTACK, 10.f);
		else if (m_iCurFrame == 28)
			AttackCollider_Off();
		else if (m_iCurFrame == 30)
			AttackCollider_On(NORMAL_ATTACK, 10.f);
		else if (m_iCurFrame == 35)
			AttackCollider_Off();
		else if (m_iCurFrame == 52)
			AttackCollider_On(KNOCKDOWN_ATTACK, 10.f);
		else if (m_iCurFrame == 55)
			AttackCollider_Off();
    }
        break;
    case HERO::DELLONS:
	{
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
			Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 1.f, desc, NORMAL_ATTACK, 10.f);

		}
		else if (Init_CurFrame(99))
		{
			FORWARDMOVINGSKILLDESC desc;
			desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
			desc.fMoveSpeed = 20.f;
			desc.fLifeTime = 1.f;
			desc.fLimitDistance = 5.f;

			_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * -0.5f + _float3::Up;
			Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 2.f, desc, KNOCKDOWN_SKILL, 10.f);
		}
	}
        break;
    case HERO::SPIKE:
	{
		if (Init_CurFrame(1))
			Add_And_Set_Effect(L"Spike_300100_Jump");
		if (Init_CurFrame(30))
			Add_And_Set_Effect(L"Spike_300100");
		if (Init_CurFrame(30))
		{
			CAMERA_SHAKE(0.4f, 0.5f);
			FORWARDMOVINGSKILLDESC desc;
			desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
			desc.fMoveSpeed = 0.f;
			desc.fLifeTime = 1.f;
			desc.fLimitDistance = 0.f;

			Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", Get_Transform()->Get_State(Transform_State::POS), 3.f, desc, AIRBORNE_ATTACK, 10.f);

		}
	}
        break;
    case HERO::YEONHEE:
	{
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

				Create_InstallationSkillCollider(Player_Skill, L"InstallationSkillCollider", vSkillPos, 2.f, desc);
			}
		}
	}
        break;
    case HERO::SHANE:
	{
		if (Init_CurFrame(56))
		{
			FORWARDMOVINGSKILLDESC desc;
			desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
			desc.fMoveSpeed = 0.f;
			desc.fLifeTime = 0.5f;
			desc.fLimitDistance = 0.f;

			_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 2.f + _float3::Up;
			Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 2.5f, desc, NORMAL_SKILL, 10.f);
		}
		else if (Init_CurFrame(63))
		{
			FORWARDMOVINGSKILLDESC desc;
			desc.vSkillDir = Get_Transform()->Get_State(Transform_State::LOOK);
			desc.fMoveSpeed = 0.f;
			desc.fLifeTime = 0.5f;
			desc.fLimitDistance = 0.f;

			_float4 vSkillPos = Get_Transform()->Get_State(Transform_State::POS) + Get_Transform()->Get_State(Transform_State::LOOK) * 2.f + _float3::Up;
			Create_ForwardMovingSkillCollider(Player_Skill, L"Player_SkillCollider", vSkillPos, 2.5f, desc, KNOCKBACK_SKILL, 10.f);
		}
	}
        break;
    case HERO::MAX:
        break;
    default:
        break;
    }

    if (Is_AnimFinished())
        m_eCurState = STATE::EXIT;
}

void Friend_FSM::ATTACK_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

    switch (m_eType)
    {
    case HERO::PLAYER:
        break;
    case HERO::ACE3:
        animator->Set_NextTweenAnim(L"skill_500100", 0.15f, false, 1.f);
        break;
    case HERO::KYLE:
        animator->Set_NextTweenAnim(L"skill_500100", 0.15f, false, 1.f);
        break;
    case HERO::YEOPO:
        animator->Set_NextTweenAnim(L"skill_501100", 0.15f, false, 1.f);
        break;
    case HERO::DELLONS:
        animator->Set_NextTweenAnim(L"skill_400100", 0.15f, false, 1.f);
        break;
    case HERO::SPIKE:
        animator->Set_NextTweenAnim(L"skill_300100", 0.15f, false, 1.f);
        break;
	case HERO::YEONHEE:
		animator->Set_NextTweenAnim(L"skill_501100", 0.15f, false, 1.f);
		break;
	case HERO::SHANE:
		animator->Set_NextTweenAnim(L"skill_502100", 0.15f, false, 1.f);
		break;
    case HERO::MAX:
        break;
    default:
        break;
    }
    m_pLookingTarget = Find_TargetInFrustum(OBJ_MONSTER);
    if (!m_pLookingTarget.expired())
    {
        m_vDirToTarget = (m_pLookingTarget.lock()->Get_Transform()->Get_State(Transform_State::POS) - Get_Transform()->Get_State(Transform_State::POS)).xyz();
    }
}

void Friend_FSM::EXIT()
{
}

void Friend_FSM::EXIT_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"b_idle", 0.15f, true, 1.f);

	{
		auto script = make_shared<ObjectDissolve>(1.f);
		Get_Owner()->Add_Component(script);
		script->Init();
	}
    if(!m_pWeapon.expired())
    {
		auto script = make_shared<ObjectDissolve>(1.f);
        m_pWeapon.lock()->Add_Component(script);
		script->Init();
    }

	if(!m_pAttackCollider.expired())
		EVENTMGR.Delete_Object(m_pAttackCollider.lock());

	if (!m_pDellonsWraith.expired())
		EVENTMGR.Delete_Object(m_pDellonsWraith.lock());
   
}
