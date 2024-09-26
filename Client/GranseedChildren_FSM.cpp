#include "pch.h"
#include "GranseedChildren_FSM.h"
#include "ModelAnimator.h"
#include <MathUtils.h>
#include "UIInteraction.h"
#include "UiQuestController.h"
#include "ModelRenderer.h"
#include "SphereCollider.h"
#include "Hide_OrctongScript.h"
#include "Glow_WitcherSense.h"

GranseedChildren_FSM::GranseedChildren_FSM()
{
}

GranseedChildren_FSM::~GranseedChildren_FSM()
{
}

HRESULT GranseedChildren_FSM::Init()
{
    return S_OK;
}

void GranseedChildren_FSM::Tick()
{
	State_Tick();
}

void GranseedChildren_FSM::InteractWithPlayer()
{
	auto pPlayer = GET_PLAYER;
	m_vDirToTarget = (pPlayer->Get_Transform()->Get_State(Transform_State::POS) - Get_Transform()->Get_State(Transform_State::POS)).xyz();
	m_vDirToTarget.y = 0;
	m_eCurState = STATE::talk_01;
}

void GranseedChildren_FSM::State_Tick()
{
	State_Init();
	switch (m_eCurState)
	{
	case STATE::n_idle:
		n_idle();
		break;
		
	case STATE::n_run:
		n_run();

		break;
	case STATE::n_walk:
		n_walk();

		break;
	case STATE::talk_01:
		talk_01();
		break;

	case STATE::Hide:
		Hide();
		break;

	case STATE::Seek:
		Seek();
		break;
	case STATE::Reset:
		Reset();
		break;
	}
}

void GranseedChildren_FSM::State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case STATE::n_idle:
			n_idle_Init();
			break;
		case STATE::n_run:
			n_run_Init();
			break;
		case STATE::n_walk:
			n_walk_Init();
			break;
		case STATE::talk_01:
			talk_Init();
			break;
		case STATE::Hide:
			Hide_Init();
			break;
		case STATE::Seek:
			Seek_Init();
			break;
		case STATE::Reset:
			Reset_Init();
			break;
		}
		m_ePreState = m_eCurState;
	}
}

void GranseedChildren_FSM::n_idle()
{
	m_fStateAcc += fDT;
	if (m_fStateAcc >= m_fIdleTimer)
	{
		m_fStateAcc = 0.f;
		m_eCurState = STATE::n_run;
	}

	if (Can_Interact())
	{
		{
			auto pObj = CUR_SCENE->Get_UI(L"UI_Interaction");
			if (pObj && pObj->Get_Script<UIInteraction>()->Get_Is_Activate(m_pOwner.lock()))
				InteractWithPlayer();
			else if (pObj && !pObj->Get_Script<UIInteraction>()->Is_Created())
				pObj->Get_Script<UIInteraction>()->Create_Interaction(NPCTYPE::HIDE_KID, m_pOwner.lock());
		}

	}
	else
	{
		auto pObj = CUR_SCENE->Get_UI(L"UI_Interaction");
		if (pObj)
			pObj->Get_Script<UIInteraction>()->Remove_Interaction(m_pOwner.lock());
	}
}

void GranseedChildren_FSM::n_idle_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"n_idle", 0.15f, true, 1.f);

	m_fIdleTimer = MathUtils::Get_RandomFloat(2.f, 4.f);
}

void GranseedChildren_FSM::talk_01()
{
	Look_DirToTarget(XM_PI * 0.5f);

	auto obj = CUR_SCENE->Get_UI(L"UI_Dialog_Controller");
	if (obj && !obj->Get_Script<UiQuestController>()->Get_Dialog_End())
	{
		if (obj->Get_Script<UiQuestController>()->Get_CurState(QUESTINDEX::HIDE_AND_SEEK) == CUR_QUEST::PROGRESS)
			m_eCurState = STATE::Hide;
		else if (obj->Get_Script<UiQuestController>()->Get_QuestState(QUESTINDEX::HIDE_AND_SEEK))
		{
			m_eCurState = STATE::Reset;
		}
		else
			m_eCurState = STATE::n_idle;
	}

}

void GranseedChildren_FSM::talk_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"talk_01", 0.15f, true, 1.f);
}

void GranseedChildren_FSM::n_run()
{
	m_fStateAcc += fDT;

	_float3 vPos = Get_Transform()->Get_State(Transform_State::POS).xyz();
	_float3 vLook = Get_Transform()->Get_State(Transform_State::LOOK).xyz();

	Get_Transform()->Go_Straight();

	if (vPos.x > m_vMaxMovingPoint.x)
	{
		vPos.x = m_vMaxMovingPoint.x;

		if (m_vDirToTarget.x > 0.f)
			m_vDirToTarget.x *= -1.f;
	}

	if (vPos.x < m_vMinMovingPoint.x)
	{
		vPos.x = m_vMinMovingPoint.x;

		if (m_vDirToTarget.x < 0.f)
			m_vDirToTarget.x *= -1.f;
	}

	if (vPos.z > m_vMaxMovingPoint.z)
	{
		vPos.z = m_vMaxMovingPoint.z;

		if (m_vDirToTarget.z > 0.f)
			m_vDirToTarget.z *= -1.f;
	}

	if (vPos.z < m_vMinMovingPoint.z)
	{
		vPos.z = m_vMinMovingPoint.z;

		if (m_vDirToTarget.z < 0.f)
			m_vDirToTarget.z *= -1.f;
	}

	Look_DirToTarget(XM_PI * 0.5f);

	if (m_fStateAcc >= m_fMoveTimer)
	{
		m_fStateAcc = 0.f;
		m_eCurState = STATE::n_idle;
	}
	if (Can_Interact())
	{
		{
			auto pObj = CUR_SCENE->Get_UI(L"UI_Interaction");
			if (pObj && pObj->Get_Script<UIInteraction>()->Get_Is_Activate(m_pOwner.lock()))
				InteractWithPlayer();
			else if (pObj && !pObj->Get_Script<UIInteraction>()->Is_Created())
				pObj->Get_Script<UIInteraction>()->Create_Interaction(NPCTYPE::HIDE_KID, m_pOwner.lock());
		}

	}
	else
	{
		auto pObj = CUR_SCENE->Get_UI(L"UI_Interaction");
		if (pObj)
			pObj->Get_Script<UIInteraction>()->Remove_Interaction(m_pOwner.lock());
	}
}

void GranseedChildren_FSM::n_run_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"n_run", 0.15f, true, 1.f);

	Get_Transform()->Set_Speed(6.f);
	m_vDirToTarget = MathUtils::Get_RandomVector(_float3(-1, 0, -1), _float3(1, 0, 1));
	m_vDirToTarget.Normalize();

	_float3 vLook = Get_Transform()->Get_State(Transform_State::LOOK).xyz();
	vLook = _float3::TransformNormal(vLook, _float4x4::CreateRotationY(MathUtils::Get_RandomFloat(-XM_PI * 0.1f, XM_PI * 0.1f)));
	m_fMoveTimer = MathUtils::Get_RandomFloat(7.f, 10.f);
	m_vDirToTarget = vLook;
}

void GranseedChildren_FSM::n_walk()
{
	m_fStateAcc += fDT;

	_float3 vPos = Get_Transform()->Get_State(Transform_State::POS).xyz();
	_float3 vLook = Get_Transform()->Get_State(Transform_State::LOOK).xyz();

	Get_Transform()->Go_Straight();

	if (vPos.x > m_vMaxMovingPoint.x)
	{
		vPos.x = m_vMaxMovingPoint.x;

		if (m_vDirToTarget.x > 0.f)
			m_vDirToTarget.x *= -1.f;
	}

	if (vPos.x < m_vMinMovingPoint.x)
	{
		vPos.x = m_vMinMovingPoint.x;

		if (m_vDirToTarget.x < 0.f)
			m_vDirToTarget.x *= -1.f;
	}

	if (vPos.z > m_vMaxMovingPoint.z)
	{
		vPos.z = m_vMaxMovingPoint.z;

		if (m_vDirToTarget.z > 0.f)
			m_vDirToTarget.z *= -1.f;
	}

	if (vPos.z < m_vMinMovingPoint.z)
	{
		vPos.z = m_vMinMovingPoint.z;

		if (m_vDirToTarget.z < 0.f)
			m_vDirToTarget.z *= -1.f;
	}

	Look_DirToTarget(XM_PI * 0.5f);

	if (m_fStateAcc >= m_fMoveTimer)
	{
		m_fStateAcc = 0.f;
		m_eCurState = STATE::n_idle;
	}
	if (Can_Interact())
	{
		{
			auto pObj = CUR_SCENE->Get_UI(L"UI_Interaction");
			if (pObj && pObj->Get_Script<UIInteraction>()->Get_Is_Activate(m_pOwner.lock()))
				InteractWithPlayer();
			else if (pObj && !pObj->Get_Script<UIInteraction>()->Is_Created())
				pObj->Get_Script<UIInteraction>()->Create_Interaction(NPCTYPE::HIDE_KID, m_pOwner.lock());
		}

	}
	else
	{
		auto pObj = CUR_SCENE->Get_UI(L"UI_Interaction");
		if (pObj)
			pObj->Get_Script<UIInteraction>()->Remove_Interaction(m_pOwner.lock());
	}
}

void GranseedChildren_FSM::n_walk_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"n_walk", 0.15f, true, 1.f);

	Get_Transform()->Set_Speed(3.f);
	m_vDirToTarget = MathUtils::Get_RandomVector(_float3(-1, 0, -1), _float3(1, 0, 1));
	m_vDirToTarget.Normalize();

	_float3 vLook = Get_Transform()->Get_State(Transform_State::LOOK).xyz();
	vLook = _float3::TransformNormal(vLook, _float4x4::CreateRotationY(MathUtils::Get_RandomFloat(-XM_PI * 0.1f, XM_PI * 0.1f)));

	m_vDirToTarget = vLook;
}

void GranseedChildren_FSM::Hide()
{
	m_fStateAcc += fDT;
	if(m_fStateAcc < 1.f)
		CUR_SCENE->g_VignetteData.g_fVignettePower += 15.f * fDT;

	else if(m_fStateAcc >= 1.f && m_fStateAcc < 1.5f)
		Get_Owner()->Get_Animator()->Set_RenderState(false);
	else if (m_fStateAcc >= 1.5f && m_fStateAcc < 2.5f)
	{
		CUR_SCENE->g_VignetteData.g_fVignettePower -= 15.f * fDT;
	}
	else if (m_fStateAcc >= 2.5f)
	{
		CUR_SCENE->g_VignetteData.g_fVignettePower = 0.f;
		CUR_SCENE->g_VignetteData.g_bVignetteOn = false;
	}
	
}

void GranseedChildren_FSM::Hide_Init()
{
	m_fStateAcc = 0.f;

	auto pPlayer = GET_PLAYER;
	for (_uint i = 0; i < 5; ++i)
	{
		auto object = CUR_SCENE->Get_GameObject(L"Hide_Orctong" + to_wstring(i));
		object->Get_ModelRenderer()->Set_RenderState(true);
		object->Add_Component(make_shared<Hide_OrctongScript>(i));
		object->Add_Component(make_shared<Glow_WitcherSense>(pPlayer));
	}
	
	CUR_SCENE->g_VignetteData.g_bVignetteOn = true;
	CUR_SCENE->g_VignetteData.g_fVignettePower = 0.f;
}

void GranseedChildren_FSM::Seek()
{
	if (Can_Interact())
	{
		{
			auto pObj = CUR_SCENE->Get_UI(L"UI_Interaction");
			if (pObj && pObj->Get_Script<UIInteraction>()->Get_Is_Activate(m_pOwner.lock()))
				InteractWithPlayer();
			else if (pObj && !pObj->Get_Script<UIInteraction>()->Is_Created())
				pObj->Get_Script<UIInteraction>()->Create_Interaction(NPCTYPE::HIDE_KID, m_pOwner.lock());
		}

	}
	else
	{
		auto pObj = CUR_SCENE->Get_UI(L"UI_Interaction");
		if (pObj)
			pObj->Get_Script<UIInteraction>()->Remove_Interaction(m_pOwner.lock());
	}
}

void GranseedChildren_FSM::Seek_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"n_idle", 0.15f, true, 1.f);

}

void GranseedChildren_FSM::Reset()
{
	m_fStateAcc += fDT;
	if (m_fStateAcc < 1.f)
		CUR_SCENE->g_VignetteData.g_fVignettePower += 15.f * fDT;

	else if (m_fStateAcc >= 1.f && m_fStateAcc < 1.5f)
	{
		Get_Transform()->Set_State(Transform_State::POS, _float4(17.015f, -4.667f, 64.654f, 1.f));
	}
	else if (m_fStateAcc >= 1.5f && m_fStateAcc < 2.5f)
	{
		CUR_SCENE->g_VignetteData.g_fVignettePower -= 15.f * fDT;
	}
	else if (m_fStateAcc >= 2.5f)
	{
		CUR_SCENE->g_VignetteData.g_fVignettePower = 0.f;
		CUR_SCENE->g_VignetteData.g_bVignetteOn = false;
		m_eCurState = STATE::n_idle;
	}
}

void GranseedChildren_FSM::Reset_Init()
{
	CUR_SCENE->g_VignetteData.g_bVignetteOn = true;
	CUR_SCENE->g_VignetteData.g_fVignettePower = 0.f;

	m_fStateAcc = 0.f;
}

void GranseedChildren_FSM::Set_State(_uint iIndex)
{
	m_eCurState = STATE(iIndex);
}
