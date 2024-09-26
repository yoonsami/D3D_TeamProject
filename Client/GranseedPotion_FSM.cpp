#include "pch.h"
#include "GranseedPotion_FSM.h"
#include "ModelAnimator.h"
#include "UIInteraction.h"

GranseedPotion_FSM::GranseedPotion_FSM()
{
}

GranseedPotion_FSM::~GranseedPotion_FSM()
{
}

HRESULT GranseedPotion_FSM::Init()
{
    m_eCurState = STATE::n_idle;
    return S_OK;
}

void GranseedPotion_FSM::Tick()
{
	State_Tick();
}

void GranseedPotion_FSM::InteractWithPlayer()
{
	auto pPlayer = GET_PLAYER;
	m_vDirToTarget = (pPlayer->Get_Transform()->Get_State(Transform_State::POS) - Get_Transform()->Get_State(Transform_State::POS)).xyz();
	m_vDirToTarget.y = 0;
	m_eCurState = STATE::talk;
}

void GranseedPotion_FSM::State_Tick()
{
	State_Init();
	switch (m_eCurState)
	{
	case STATE::n_idle:
		n_idle();
        //if (Can_Interact())
        //{
        //    if (KEYTAP(KEY_TYPE::E))
        //        InteractWithPlayer();
        //}
		break;

		break;
	case STATE::talk:
		talk();
		break;

	}
}

void GranseedPotion_FSM::State_Init()
{
	if (m_eCurState != m_ePreState)
	{
		switch (m_eCurState)
		{
		case STATE::n_idle:
			n_idle_Init();
			break;

		case STATE::talk:
			talk_Init();
			break;

		}
		m_ePreState = m_eCurState;
	}
}

void GranseedPotion_FSM::n_idle()
{
	if (Can_Interact())
	{
		{
			auto pObj = CUR_SCENE->Get_UI(L"UI_Interaction");
			if (pObj && pObj->Get_Script<UIInteraction>()->Get_Is_Activate(m_pOwner.lock()))
				InteractWithPlayer();
			else if (pObj && !pObj->Get_Script<UIInteraction>()->Is_Created())
				pObj->Get_Script<UIInteraction>()->Create_Interaction(NPCTYPE::POTION, m_pOwner.lock());
		}

	}
	else
	{
		auto pObj = CUR_SCENE->Get_UI(L"UI_Interaction");
		if (pObj)
			pObj->Get_Script<UIInteraction>()->Remove_Interaction(m_pOwner.lock());
	}
}

void GranseedPotion_FSM::n_idle_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"n_idle", 0.15f, true, 1.f);
}

void GranseedPotion_FSM::talk()
{
	Look_DirToTarget(XM_PI * 0.5f);
	if (false == Can_Interact())
		m_eCurState = STATE::n_idle;
}

void GranseedPotion_FSM::talk_Init()
{
	shared_ptr<ModelAnimator> animator = Get_Owner()->Get_Animator();

	animator->Set_NextTweenAnim(L"talk", 0.15f, true, 1.f);
}

void GranseedPotion_FSM::Set_State(_uint iIndex)
{
}
