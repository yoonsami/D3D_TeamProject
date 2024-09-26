#include "pch.h"
#include "WeaponScript.h"
#include "Model.h"
#include "ModelAnimator.h"
#include "Utils.h"
#include "GroupEffect.h"
#include "GroupEffectOwner.h"
#include "ModelRenderer.h"

WeaponScript::WeaponScript(WEAPONDESC weapondesc)
{
	m_strBoneName = weapondesc.strBoneName;
	m_matPivot = weapondesc.matPivot;
	m_pWeaponOwner = weapondesc.pWeaponOwner;
}

WeaponScript::~WeaponScript()
{
}

HRESULT WeaponScript::Init()
{
	_uint index = m_pWeaponOwner.lock()->Get_Model()->Get_BoneIndexByName(m_strBoneName);

	//MEMO :: current animation's selected Index bonematrix
	_float4x4 matSetting = m_pWeaponOwner.lock()->Get_Animator()->Get_CurAnimTransform(index)
		* _float4x4::CreateRotationX(XM_PI * -0.5f) * Utils::m_matPivot * m_pWeaponOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

	Get_Transform()->Set_WorldMat(m_matPivot * matSetting);
	Get_Transform()->Scaled(_float3(1.f));

	return S_OK;
}

void WeaponScript::Late_Tick()
{
	// weak_ptr expired() -> 이 포인터가 이미 소멸해서 사용할 수 없는 상태면 true 반환
	if (m_pOwner.expired() || m_pWeaponOwner.expired())
		return;

	if (Get_Owner()->Get_Name().empty())
		return;

	if (Get_Owner()->Get_ModelRenderer())
	{
		Get_Owner()->Get_ModelRenderer()->Get_RenderParamDesc().floatParams[0] -= 3.f * fDT;
		if(Get_Owner()->Get_ModelRenderer()->Get_RenderParamDesc().floatParams[0]<0)Get_Owner()->Get_ModelRenderer()->Get_RenderParamDesc().floatParams[0] =0.f;
		_float4& param = Get_Owner()->Get_ModelRenderer()->Get_RenderParamDesc().vec4Params[1];
		param -= _float4(fDT);
		if (param.x < 0) param.x = 0.f;
		if (param.y < 0) param.y = 0.f;
		if (param.z < 0) param.z = 0.f;
		if (param.w < 0) param.w = 0.f;
	}

	_uint index = m_pWeaponOwner.lock()->Get_Model()->Get_BoneIndexByName(m_strBoneName);

	_float4x4 matSetting = m_pWeaponOwner.lock()->Get_Animator()->Get_CurAnimTransform(index)
		* _float4x4::CreateRotationX(XM_PI * -0.5f) * Utils::m_matPivot * m_pWeaponOwner.lock()->GetOrAddTransform()->Get_WorldMatrix();

	Get_Transform()->Set_WorldMat(m_matPivot * matSetting);
	Get_Transform()->Scaled(_float3(1.f));

	if (m_bModelChanged)
	{
		m_bModelChanged = false;
		Get_Transform()->Reset_Position();
	}
}

void WeaponScript::Add_Effect(const wstring& strSkilltag, shared_ptr<MonoBehaviour> pScript)
{
	shared_ptr<GameObject> pGroupEffectObj = make_shared<GameObject>();

	// For. Transform 
	pGroupEffectObj->GetOrAddTransform();
	pGroupEffectObj->Get_Transform()->Set_WorldMat(m_pOwner.lock()->Get_Transform()->Get_WorldMatrix());

	// For. GroupEffectData 
	wstring wstrFileName = strSkilltag + L".dat";
	wstring wtsrFilePath = TEXT("..\\Resources\\EffectData\\GroupEffectData\\") + wstrFileName;
	shared_ptr<GroupEffectData> pGroupEffectData = RESOURCES.GetOrAddGroupEffectData(strSkilltag, wtsrFilePath);
	if (pGroupEffectData == nullptr)
		return;

	// For. GroupEffect component 
	shared_ptr<GroupEffect> pGroupEffect = make_shared<GroupEffect>();

	pGroupEffectObj->Add_Component(pGroupEffect);
	pGroupEffectObj->Get_GroupEffect()->Set_Tag(pGroupEffectData->Get_GroupEffectDataTag());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectData(pGroupEffectData->Get_MemberEffectData());
	pGroupEffectObj->Get_GroupEffect()->Set_InitWorldMatrix(pGroupEffectObj->Get_Transform()->Get_WorldMatrix());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectMaterials();
	pGroupEffectObj->Set_Name(strSkilltag);
	pGroupEffectObj->Init();

	// For. Script 
	if (nullptr != pScript)
		pGroupEffectObj->Get_GroupEffect()->Set_Script(pScript);

	// For. Add Effect GameObject to current scene
	EVENTMGR.Create_Object(pGroupEffectObj);
}

void WeaponScript::Add_Effect(const wstring& strSkilltag, const _float4x4& matWorldMat, shared_ptr<MonoBehaviour> pScript)
{
	shared_ptr<GameObject> pGroupEffectObj = make_shared<GameObject>();

	// For. Transform 
	pGroupEffectObj->GetOrAddTransform();
	pGroupEffectObj->Get_Transform()->Set_WorldMat(matWorldMat);

	// For. GroupEffectData 
	wstring wstrFileName = strSkilltag + L".dat";
	wstring wtsrFilePath = TEXT("..\\Resources\\EffectData\\GroupEffectData\\") + wstrFileName;
	shared_ptr<GroupEffectData> pGroupEffectData = RESOURCES.GetOrAddGroupEffectData(strSkilltag, wtsrFilePath);
	if (pGroupEffectData == nullptr)
		return;

	// For. GroupEffect component 
	shared_ptr<GroupEffect> pGroupEffect = make_shared<GroupEffect>();

	pGroupEffectObj->Add_Component(pGroupEffect);
	pGroupEffectObj->Get_GroupEffect()->Set_Tag(pGroupEffectData->Get_GroupEffectDataTag());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectData(pGroupEffectData->Get_MemberEffectData());
	pGroupEffectObj->Get_GroupEffect()->Set_InitWorldMatrix(pGroupEffectObj->Get_Transform()->Get_WorldMatrix());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectMaterials();
	pGroupEffectObj->Set_Name(strSkilltag);
	pGroupEffectObj->Init();

	// For. Script 
	if (nullptr != pScript)
		pGroupEffectObj->Get_GroupEffect()->Set_Script(pScript);

	// For. Add Effect GameObject to current scene
	EVENTMGR.Create_Object(pGroupEffectObj);
}

void WeaponScript::Add_And_Set_Effect(const wstring& strSkilltag, shared_ptr<MonoBehaviour> pScript)
{
	shared_ptr<GameObject> pGroupEffectObj = make_shared<GameObject>();

	// For. Transform 
	pGroupEffectObj->GetOrAddTransform();
	pGroupEffectObj->Get_Transform()->Set_State(Transform_State::POS, m_pOwner.lock()->Get_Transform()->Get_State(Transform_State::POS));
	pGroupEffectObj->Get_Transform()->Set_Quaternion(Get_Transform()->Get_Rotation());
	pGroupEffectObj->Set_Name(strSkilltag);
	// For. GroupEffectData 
	wstring wstrFileName = strSkilltag + L".dat";
	wstring wtsrFilePath = TEXT("..\\Resources\\EffectData\\GroupEffectData\\") + wstrFileName;
	shared_ptr<GroupEffectData> pGroupEffectData = RESOURCES.GetOrAddGroupEffectData(strSkilltag, wtsrFilePath);

	if (pGroupEffectData == nullptr)
		return;

	// For. GroupEffect component 
	shared_ptr<GroupEffect> pGroupEffect = make_shared<GroupEffect>();
	pGroupEffectObj->Add_Component(pGroupEffect);
	pGroupEffectObj->Get_GroupEffect()->Set_Tag(pGroupEffectData->Get_GroupEffectDataTag());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectData(pGroupEffectData->Get_MemberEffectData());
	pGroupEffectObj->Get_GroupEffect()->Set_InitWorldMatrix(Get_Transform()->Get_WorldMatrix());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectMaterials();
	pGroupEffectObj->Set_Name(strSkilltag);
	m_vGroupEffect.push_back(pGroupEffectObj);
	pGroupEffectObj->Init();

	// For. Script 
	if (nullptr != pScript)
		pGroupEffectObj->Get_GroupEffect()->Set_Script(pScript);

	// For. Add Effect GameObject to current scene
	for (auto& iter : m_vGroupEffect)
	{
		if (!iter.expired())
			EVENTMGR.Create_Object(iter.lock());
	}
}

void WeaponScript::Add_GroupEffectOwner(const wstring& strSkilltag, _float3 vPosOffset, _bool usePosAs, shared_ptr<MonoBehaviour> pScript)
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
	if (nullptr != pScript)
		pGroupEffectOwnerObj->Get_GroupEffectOwner()->Set_GroupEffectScript(pScript);

	// For. Add Effect GameObject to current scene
	EVENTMGR.Create_Object(pGroupEffectOwnerObj);
}

void WeaponScript::Update_GroupEffectWorldPos(const _float4x4& mWorldMatrix)
{
	for (auto& iter : m_vGroupEffect)
	{
		if (!iter.expired())
			iter.lock()->Get_Transform()->Set_WorldMat(mWorldMatrix);
	}
}
