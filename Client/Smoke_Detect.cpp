#include "pch.h"
#include "Smoke_Detect.h"
#include "UiQuestController.h"

Smoke_Detect::Smoke_Detect()
{
}

HRESULT Smoke_Detect::Init()
{
    m_pPlayer = GET_PLAYER;
    return S_OK;
}

void Smoke_Detect::Tick()
{
    if (m_pPlayer.expired() || m_pOwner.expired())
        return;

	{
		auto obj = CUR_SCENE->Get_UI(L"UI_Dialog_Controller");
		if (obj && !(obj->Get_Script<UiQuestController>()->Get_CurState(QUESTINDEX::HIDE_AND_SEEK) == CUR_QUEST::PROGRESS))
			return;

	}

	_float3 vPlayerPos = m_pPlayer.lock()->Get_Transform()->Get_State(Transform_State::POS).xyz();
	_float3 vOwenrPos = Get_Transform()->Get_State(Transform_State::POS).xyz();

	_float lengthSQ = (vPlayerPos - vOwenrPos).LengthSquared();
	if (lengthSQ < 3.f * 3.f)
	{
		//Create Horse Balloon

		EVENTMGR.Delete_Object(Get_Owner());
	}

}
