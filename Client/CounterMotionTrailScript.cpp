#include "pch.h"
#include "CounterMotionTrailScript.h"
#include "MotionTrailRenderer.h"
#include "ModelAnimator.h"
#include "Model.h"
#include "MotionTrailDisappear.h"

HRESULT CounterMotionTrailScript::Init()
{
	shared_ptr<GameObject> motionTrail = make_shared<GameObject>();
	motionTrail->GetOrAddTransform()->Set_WorldMat(Get_Transform()->Get_WorldMatrix());
	motionTrail->Add_Component(make_shared<MotionTrailRenderer>(RESOURCES.Get<Shader>(L"Shader_Model.fx"), Get_Owner()->Get_Animator()->Get_TweenDesc(), Get_Owner()->Get_Animator()->Get_Model()));
	motionTrail->Add_Component(make_shared<MotionTrailDisappear>(2.f, Color(0.2f, 0.2f, 2.f, 1.f)));
	motionTrail->Get_Script<MotionTrailDisappear>()->Init();
	EVENTMGR.Create_Object(motionTrail);
	m_pMotionTrail = motionTrail;

	return S_OK;
}

void CounterMotionTrailScript::Tick()
{
	if (!m_pMotionTrail.expired())
	{
		m_fTrailScale += fDT * 0.5f;
		m_pMotionTrail.lock()->Get_MotionTrailRenderer()->Set_Scale(m_fTrailScale);
	}
	else
	{
		m_fTrailScale = 1.f;
	}
}
