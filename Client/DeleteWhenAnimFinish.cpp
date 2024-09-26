#include "pch.h"
#include "DeleteWhenAnimFinish.h"
#include "ModelAnimator.h"
#include "ObjectDissolve.h"

DeleteWhenAnimFinish::DeleteWhenAnimFinish(const wstring& strAnimName, _float fWaitTime, _bool bAnimLoop, _float fAnimSpeed)
	: m_strAnimName(strAnimName)
	, m_fWaitTime(fWaitTime)
	, m_bAnimLoop(bAnimLoop)
	, m_fAnimSpeed(fAnimSpeed)
{
}

HRESULT DeleteWhenAnimFinish::Init()
{
	if (Get_Owner()->Get_Animator())
	{
		Get_Owner()->Get_Animator()->Set_CurrentAnim(m_strAnimName, m_bAnimLoop, m_fAnimSpeed);
	}
	return S_OK;
}

void DeleteWhenAnimFinish::Final_Tick()
{
	if(!m_bAnimFinished)
	{
		auto animator = Get_Owner()->Get_Animator();
		auto& desc = Get_Owner()->Get_Animator()->Get_TweenDesc();
		if (desc.next.animIndex < 0 && animator->Is_Finished())
		{
			m_bAnimFinished = true;
			if (m_bDissolveWhenFinished)
			{
				Get_Owner()->Add_Component(make_shared<ObjectDissolve>(m_fDissolveSpeed, m_pDissolveTexture));
			}
		}
	}
	else
	{
		m_fAcc += fDT;
		if (m_fWaitTime <= m_fAcc)
		{
			EVENTMGR.Delete_Object(Get_Owner());
		}
	}

	/* How to use
	{
		shared_ptr<GameObject> obj = make_shared<GameObject>();
		obj->GetOrAddTransform()->Set_WorldMat(Get_Transform()->Get_WorldMatrix());
		{
			shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(RESOURCES.Get<Shader>(L"Shader_Model.fx"));
			shared_ptr<Model> model = RESOURCES.Get<Model>(ModelTag);
			animator->Set_Model(model);
			obj->Add_Component(animator);
		}

		auto script = make_shared<DeleteWhenAnimFinish>(strAnimName,fWaitTime,bAnimLoop,fAnimSpeed);
		obj->Add_Component(script);

		// if want to use dissolve Texture
		// script->Set_DissolveOption(fDissolveSpeed,pDissolveTexture)

		script->Init();
		EVENTMGR.Create_Object(obj);
	}
	*/
}
