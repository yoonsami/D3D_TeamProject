#include "pch.h"
#include "Mir_13100_Fireball.h"

#include "GroupEffect.h"
#include "GroupEffectOwner.h"

Mir_13100_Fireball::Mir_13100_Fireball()
{
}

Mir_13100_Fireball::~Mir_13100_Fireball()
{

}

void Mir_13100_Fireball::Tick()
{
	m_fCoolTime_CreateFloorEffect += fDT;

	wstring str = Get_Owner()->Get_Name();

	if (m_fCoolTime_CreateFloorEffect > 0.1f)
	{
		Add_GroupEffectOwner(L"Mir_13100_floor", _float3(0.f, 3.6f, 0.f));
		m_fCoolTime_CreateFloorEffect = 0.f;
	}
}

void Mir_13100_Fireball::Add_Effect(const wstring& wstrEffectTag, shared_ptr<MonoBehaviour> pScript)
{
	shared_ptr<GameObject> pGroupEffectObj = make_shared<GameObject>();

	// For. Transform 
	pGroupEffectObj->GetOrAddTransform();
	_float4x4 fmat = m_pOwner.lock()->Get_Transform()->Get_WorldMatrix();
	pGroupEffectObj->Get_Transform()->Set_WorldMat(m_pOwner.lock()->Get_Transform()->Get_WorldMatrix());

	// For. GroupEffectData 
	wstring wstrFileName = wstrEffectTag + L".dat";
	wstring wtsrFilePath = TEXT("..\\Resources\\EffectData\\GroupEffectData\\") + wstrFileName;
	shared_ptr<GroupEffectData> pGroupEffectData = RESOURCES.GetOrAddGroupEffectData(wstrEffectTag, wtsrFilePath);

	if (pGroupEffectData == nullptr)
		return;

	// For. GroupEffect component 
	shared_ptr<GroupEffect> pGroupEffect = make_shared<GroupEffect>();
	pGroupEffectObj->Add_Component(pGroupEffect);
	pGroupEffectObj->Get_GroupEffect()->Set_Tag(pGroupEffectData->Get_GroupEffectDataTag());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectData(pGroupEffectData->Get_MemberEffectData());
	pGroupEffectObj->Get_GroupEffect()->Set_InitWorldMatrix(pGroupEffectObj->Get_Transform()->Get_WorldMatrix());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectMaterials();
	pGroupEffectObj->Set_Name(wstrEffectTag);
	pGroupEffectObj->Init();

	// For. Script 
	if (nullptr != pScript)
		pGroupEffectObj->Add_Component(pScript);

	// For. Add Effect GameObject to current scene
	EVENTMGR.Create_Object(pGroupEffectObj);
}

void Mir_13100_Fireball::Add_GroupEffectOwner(const wstring& strSkilltag, _float3 vPosOffset, _bool usePosAs)
{
	shared_ptr<GameObject> pGroupEffectOwnerObj = make_shared<GameObject>();

	// For. Transform 
	pGroupEffectOwnerObj->GetOrAddTransform();
	if (!usePosAs)
	{
		_float4 vOwnerLook = m_pOwner.lock()->Get_Transform()->Get_State(Transform_State::LOOK);
		vOwnerLook.Normalize();
		_float4 vOwnerRight = m_pOwner.lock()->Get_Transform()->Get_State(Transform_State::RIGHT);
		vOwnerRight.Normalize();
		_float4 vOwnerUp = m_pOwner.lock()->Get_Transform()->Get_State(Transform_State::UP);
		vOwnerUp.Normalize();
		_float4 vOwnerPos = m_pOwner.lock()->Get_Transform()->Get_State(Transform_State::POS)
			+ vOwnerRight * vPosOffset.x
			+ vOwnerUp * vPosOffset.y
			+ vOwnerLook * vPosOffset.z;
		pGroupEffectOwnerObj->Get_Transform()->Set_State(Transform_State::POS, vOwnerPos);
	}
	else
		pGroupEffectOwnerObj->Get_Transform()->Set_State(Transform_State::POS, _float4(vPosOffset, 1.f));
	pGroupEffectOwnerObj->Get_Transform()->Set_Quaternion(Get_Transform()->Get_Rotation());

	// For. GroupEffect component 
	shared_ptr<GroupEffectOwner> pGroupEffect = make_shared<GroupEffectOwner>();
	pGroupEffectOwnerObj->Add_Component(pGroupEffect);
	pGroupEffectOwnerObj->Get_GroupEffectOwner()->Set_GroupEffectTag(strSkilltag);

	// For. Add Effect GameObject to current scene
	EVENTMGR.Create_Object(pGroupEffectOwnerObj);
}
