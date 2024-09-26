#include "pch.h"
#include "Hide_OrctongScript.h"
#include "Camera.h"
#include <MathUtils.h>
#include "ObjectDissolve.h"
#include "Model.h"
#include "ModelRenderer.h"
#include "UiQuestController.h"
#include "UIInteraction.h"
#include "ModelAnimator.h"
#include "GranseedChildren_FSM.h"
#include "ObjectDissolveCreate.h"

void Hide_OrctongScript::Tick()
{
	if (m_pOwner.expired())
		return;

	auto pPlayer = GET_PLAYER;
	if (!pPlayer)
		return;

	_float3 vOwnerPos = Get_Transform()->Get_State(Transform_State::POS).xyz();
	_float3 vPlayerPos = pPlayer->Get_Transform()->Get_State(Transform_State::POS).xyz();

	_float lengthSQ = (vOwnerPos - vPlayerPos).LengthSquared();

	if (lengthSQ >= 3.f * 3.f)
		return;


	if (!KEYPUSH(KEY_TYPE::E))
		return;

	auto camera = CUR_SCENE->Get_MainCamera()->Get_Camera();
	
	if (camera->Get_Frustum().Contain_Sphere(Get_Owner()->Get_CullPos(), Get_Owner()->Get_CullRadius()))
	{
		auto script = make_shared<ObjectDissolve>(0.3f);
		Get_Owner()->Add_Component(script);
		script->Init();

		switch (m_iOrctongIndex)
		{
		case 0:
		{
			auto pObj = CUR_SCENE->Get_UI(L"UI_Dialog_Controller");
			if (pObj && pObj->Get_Script<UiQuestController>())
			{
				pObj->Get_Script<UiQuestController>()->Change_Value();

				auto accessObj = pObj->Get_Script<UiQuestController>()->Get_AccessObject();

				accessObj->Get_Transform()->Set_State(Transform_State::POS, Get_Transform()->Get_State(Transform_State::POS) - _float4(0.f, 0.4f, 0.f, 0.f));
				accessObj->Get_Animator()->Set_RenderState(true);
				static_pointer_cast<GranseedChildren_FSM>(accessObj->Get_FSM())->Set_State(_int(GranseedChildren_FSM::STATE::Seek));

				accessObj->Add_Component(make_shared<ObjectDissolveCreate>(1.f));
			}
		}
			break;
		default:
			// Create Horse Balloon
			break;
		}

	}
	
	

}
