#include "pch.h"
#include "ObjectTransformDebug.h"
#include "FontRenderer.h"
#include "MainCameraScript.h"
#include "Model.h"
#include "ModelAnimator.h"
#include "Camera.h"
#include "FSM.h"

void ObjectTransformDebug::Tick()
{
	if (m_pTarget.expired())
	{
		EVENTMGR.Delete_Object(Get_Owner());
		return;
	}

	auto fontRenderer = Get_Owner()->Get_FontRenderer();

	
	//fontRenderer->Get_Text() = L"X : " + to_wstring(vTargetPos.x) + L"/ Y : " + to_wstring(vTargetPos.y) + L"/ Z : " + to_wstring(vTargetPos.z);
	
	auto pPlayer = GET_PLAYER;

	if (!pPlayer)
		return;

}
