#include "pch.h"
#include "CreateEffect_Script.h"

#include "GroupEffect.h"

CreateEffect_Script::CreateEffect_Script()
{
}

CreateEffect_Script::~CreateEffect_Script()
{
}

void CreateEffect_Script::Tick()
{
	m_fCoolTime_CreateFloorEffect += fDT;

	wstring str = Get_Owner()->Get_Name();

	if (m_fCoolTime_CreateFloorEffect > m_fCoolTime)
	{
		Add_Effect();
		m_fCoolTime_CreateFloorEffect = 0.f;
	}
}

shared_ptr<MonoBehaviour> CreateEffect_Script::Copy_Script()
{
	shared_ptr<CreateEffect_Script> pCopyScript = make_shared<CreateEffect_Script>();
	pCopyScript->Set_CoolTime(m_fCoolTime);
	pCopyScript->Set_SkillTag(m_wstrSkillTag);
	pCopyScript->Set_OffsetMatrix(m_matOffset);
	pCopyScript->Set_UseOwnerPosOnly(m_bUseOwnerPosOnly);
	return pCopyScript;
}

void CreateEffect_Script::Add_Effect()
{
	shared_ptr<GameObject> pGroupEffectObj = make_shared<GameObject>();

	// For. Transform 
	pGroupEffectObj->GetOrAddTransform();

	if (m_bUseOwnerPosOnly)
	{
		_float3 vOwnerPos = m_pOwner.lock()->Get_Transform()->Get_WorldMatrix().Translation();
		_float4x4 matFinalWorldMat = m_matOffset
			* _float4x4::CreateTranslation(vOwnerPos);
		pGroupEffectObj->Get_Transform()->Set_WorldMat(matFinalWorldMat);
	}
	else
		pGroupEffectObj->Get_Transform()->Set_WorldMat(m_matOffset * m_pOwner.lock()->Get_Transform()->Get_WorldMatrix());

	// For. GroupEffectData 
	wstring wstrFileName = m_wstrSkillTag + L".dat";
	wstring wtsrFilePath = TEXT("..\\Resources\\EffectData\\GroupEffectData\\") + wstrFileName;
	shared_ptr<GroupEffectData> pGroupEffectData = RESOURCES.GetOrAddGroupEffectData(m_wstrSkillTag, wtsrFilePath);

	if (pGroupEffectData == nullptr)
		return;

	// For. GroupEffect component 
	shared_ptr<GroupEffect> pGroupEffect = make_shared<GroupEffect>();

	pGroupEffectObj->Add_Component(pGroupEffect);
	pGroupEffectObj->Get_GroupEffect()->Set_Tag(pGroupEffectData->Get_GroupEffectDataTag());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectData(pGroupEffectData->Get_MemberEffectData());
	pGroupEffectObj->Get_GroupEffect()->Set_InitWorldMatrix(pGroupEffectObj->Get_Transform()->Get_WorldMatrix());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectMaterials();
	pGroupEffectObj->Set_Name(m_wstrSkillTag);
	pGroupEffectObj->Init();

	// For. Add Effect GameObject to current scene
	EVENTMGR.Create_Object(pGroupEffectObj);
}
