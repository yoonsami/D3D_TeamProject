#include "pch.h"
#include "Debug_CreateMotionTrail.h"
#include "MotionTrailRenderer.h"
#include "ModelAnimator.h"
#include "Model.h"
#include "MotionTrailDisappear.h"

void Debug_CreateMotionTrail::Tick()
{
	m_fAcc += fDT;
	if (m_fAcc >= 0.05f)
	{
		shared_ptr<GameObject> motionTrail = make_shared<GameObject>();
		motionTrail->GetOrAddTransform()->Set_WorldMat(Get_Transform()->Get_WorldMatrix());
		motionTrail->Add_Component(make_shared<MotionTrailRenderer>(RESOURCES.Get<Shader>(L"Shader_Model.fx"), Get_Owner()->Get_Animator()->Get_TweenDesc(), Get_Owner()->Get_Animator()->Get_Model()));
		motionTrail->Add_Component(make_shared<MotionTrailDisappear>(1.f, Color(1.f)));
		motionTrail->Get_Script<MotionTrailDisappear>()->Init();
		EVENTMGR.Create_Object(motionTrail);

		m_fAcc = 0.f;
	}
}
