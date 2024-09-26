#include "pch.h"
#include "Model.h"
#include "Gacha_FSM.h"
#include "ModelAnimator.h"
#include "WeaponScript.h"
#include "ModelRenderer.h"
#include "Model.h"

Gacha_FSM::Gacha_FSM(const Gacha_FSM_Desc& desc)
    : m_Desc(desc)
{
}

HRESULT Gacha_FSM::Init()
{
	auto animator = Get_Owner()->Get_Animator();
	if (animator)
	{
		animator->Set_CurrentAnim(m_Desc.strAnimTag, true, 1.f);
		m_eCurState = STATE::SQ_SpecialHero;
	}
	m_iCenterBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_Center");
	m_iDummy_CP_BoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_CP");
	m_iCamBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_Cam");
	m_iSkillCamBoneIndex = m_pOwner.lock()->Get_Model()->Get_BoneIndexByName(L"Dummy_SkillCam");
	Calculate_CamBoneMatrix();

	

	return S_OK;
}

void Gacha_FSM::Tick()
{
	State_Tick();
}

void Gacha_FSM::State_Tick()
{
	State_Init();

	switch (m_eCurState)
	{
	case Gacha_FSM::STATE::SQ_SpecialHero:
		SQ_SpecialHero();
		break;
	case Gacha_FSM::STATE::NONE:
		break;

	}
	Calculate_CamBoneMatrix();
}

void Gacha_FSM::State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case Gacha_FSM::STATE::SQ_SpecialHero:
			SQ_SpecialHero_Init();
			break;
		case Gacha_FSM::STATE::NONE:
			break;
		default:
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void Gacha_FSM::SQ_SpecialHero()
{
	CUR_SCENE->Get_MainCamera()->Get_Transform()->Set_State(Transform_State::POS, m_vSkillCamBonePos);
	CUR_SCENE->Get_MainCamera()->Get_Transform()->LookAt(m_vCenterBonePos - Get_Transform()->Get_State(Transform_State::RIGHT) * 0.5f + _float3::Up * 0.5f );
	if (Get_CurFrame() > m_Desc.iAnimStopFrame)
	{
		m_fAnimSpeed = 0.1f;
		shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

		animator->Set_AnimationSpeed(m_fAnimSpeed);
	}
}

void Gacha_FSM::SQ_SpecialHero_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_CurrentAnim(m_Desc.strAnimTag, false, 1.f);
	m_fAnimSpeed = 1.f;
	m_fAcc = 0.f;

	if (!m_Desc.strWeaponName.empty())
	{
		shared_ptr<GameObject> ObjWeapon = make_shared<GameObject>();

		ObjWeapon->GetOrAddTransform()->Set_WorldMat(Get_Transform()->Get_WorldMatrix());
		{
			shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

			shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
			{
				shared_ptr<Model> model = RESOURCES.Get<Model>(m_Desc.strWeaponName);
				renderer->Set_Model(model);
			}

			ObjWeapon->Add_Component(renderer);

			WeaponScript::WEAPONDESC desc;
			desc.strBoneName = L"Bip001-Prop1";
			desc.matPivot = _float4x4::CreateRotationX(-XM_PI / 2.f) * _float4x4::CreateRotationZ(XM_PI);
			desc.pWeaponOwner = m_pOwner;

			ObjWeapon->Add_Component(make_shared<WeaponScript>(desc));
		}
		ObjWeapon->Get_Transform()->Set_State(Transform_State::POS, _float4{ 0.f, -1000.f,0.f,1.f });
		ObjWeapon->Set_Name(m_Desc.strWeaponName);
		EVENTMGR.Create_Object(ObjWeapon);
	}
}
