#include "pch.h"
#include "GroupEffectOwner.h"

#include "GroupEffect.h"

GroupEffectOwner::GroupEffectOwner()
	: Component(COMPONENT_TYPE::GroupEffectOwner)
{
}

GroupEffectOwner::~GroupEffectOwner()
{
}

void GroupEffectOwner::Tick()
{
	if (m_bIsFirstTick)
	{
		PlayEffect();
		m_bIsFirstTick = false;
	}

	if (m_pGroupEffect.expired())
	{
		EVENTMGR.Delete_Object(Get_Owner());
		return;
	}
}

void GroupEffectOwner::PlayEffect()
{
	shared_ptr<GameObject> pGroupEffectObj = make_shared<GameObject>();

	// For. Transform 
	pGroupEffectObj->GetOrAddTransform();
	pGroupEffectObj->Get_Transform()->Set_State(Transform_State::POS, m_pOwner.lock()->Get_Transform()->Get_State(Transform_State::POS));
	pGroupEffectObj->Get_Transform()->Set_Quaternion(Get_Transform()->Get_Rotation());

	// For. GroupEffectData 
	wstring wstrFileName = m_wstrGroupEffectTag + L".dat";
	wstring wtsrFilePath = TEXT("..\\Resources\\EffectData\\GroupEffectData\\") + wstrFileName;
	shared_ptr<GroupEffectData> pGroupEffectData = RESOURCES.GetOrAddGroupEffectData(m_wstrGroupEffectTag, wtsrFilePath);

	if (pGroupEffectData == nullptr)
		return;

	// For. GroupEffect component 
	shared_ptr<GroupEffect> pGroupEffect = make_shared<GroupEffect>();
	pGroupEffectObj->Add_Component(pGroupEffect);
	pGroupEffectObj->Get_GroupEffect()->Set_Tag(pGroupEffectData->Get_GroupEffectDataTag());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectData(pGroupEffectData->Get_MemberEffectData());
	pGroupEffectObj->Get_GroupEffect()->Set_InitWorldMatrix(Get_Transform()->Get_WorldMatrix());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectMaterials();
	pGroupEffectObj->Set_Name(m_wstrGroupEffectTag);
	m_pGroupEffect = pGroupEffectObj;
	pGroupEffectObj->Get_GroupEffect()->Init();

	// For. Add Effect GameObject to current scene
	EVENTMGR.Create_Object(m_pGroupEffect.lock());
}

void GroupEffectOwner::Set_GroupEffectScript(shared_ptr<MonoBehaviour> pScript)
{
	if (m_pGroupEffect.expired())
		return;

	m_pGroupEffect.lock()->Get_GroupEffect()->Set_Script(pScript);
}

