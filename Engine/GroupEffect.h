#pragma once
#include "Component.h"
#include "GroupEffectData.h"
#define MAX_EFFECT_INSTANCE 1000

class StructuredBuffer;
class GroupEffect final : public Component, public enable_shared_from_this<GroupEffect>
{
public:
	GroupEffect();
	virtual ~GroupEffect();

public:
	virtual HRESULT     Init() override;
	virtual void        Tick() override;
	virtual void        Final_Tick() override;

	void				Render();
	void				Render_Decal();
	void				Render_Distortion();

	void				Save(const wstring& path);
	void				DeleteMember(const wstring& wstrTag);

	void				FreeLoopMember();
	void				Kill_All();

	/* Setter */
	void				Set_MemberEffectMaterials();
	void				Set_Tag(wstring wstrTag) { m_wstrTag = wstrTag; }
	void				Set_MemberEffectData(vector <GroupEffectData::MemberEffect_Desc> vData) { m_vMemberEffectData = vData; }
	void				Set_MemberEffectData_index(_int iIndex, GroupEffectData::MemberEffect_Desc& tTemp) { m_vMemberEffectData[iIndex] = tTemp; }
	void				Set_Member_StartTime(_int iIndex, _float fStartTime) { m_vMemberEffectData[iIndex].fCreateTime = fStartTime; }
	void				Set_Member_PivotPos(_int iIndex, _float3 vPos) { m_vMemberEffectData[iIndex].vPivot_Pos = vPos; }
	void				Set_Member_PivotScale(_int iIndex, _float3 vScale) { m_vMemberEffectData[iIndex].vPivot_Scale = vScale; }
	void				Set_Member_PivotRotation(_int iIndex, _float3 vRotation) { m_vMemberEffectData[iIndex].vPivot_Rotation = vRotation; }
	void				Set_InitWorldMatrix(const _float4x4& mInitMat) { m_mInitWorldMatrix = mInitMat; }
	void				Set_DistortionGroup(_bool flag) { m_bDistortionGroup = flag; }
	void				Set_Script(shared_ptr<MonoBehaviour> pScript) { m_pScript = pScript; }
	void				Set_TickRenderState(_bool bTickOn, _bool bRenderOn) { m_bTickOn = bTickOn; m_bRenderOn = bRenderOn; }

	/* Getter */
	vector <GroupEffectData::MemberEffect_Desc>&	Get_MemberEffectData() { return m_vMemberEffectData; }
	GroupEffectData::MemberEffect_Desc&				Get_MemberEffectData_index(_int iIndex) { return m_vMemberEffectData[iIndex]; }
	list <weak_ptr<GameObject>>&					Get_MemberEffects() { return m_lMemberEffects; }
	
	_bool				Is_DistortionGroup() { return m_bDistortionGroup; }

private:
	void				Create_MeshEffect(_int iIndex);
	void				Create_Particle(_int iIndex);
	void				OrganizeMemberEffectList();
private:
	/* Data */
	wstring										m_wstrTag;
	vector <GroupEffectData::MemberEffect_Desc> m_vMemberEffectData;

	_float										m_fCurrAge = { 0.f };
	_float										m_fTimeAcc_CreatCoolTime = { 0.f };
	vector<_float>								m_vCreateCoolTime;

	_bool										m_bIsFirstTick = { false };
	_bool										m_bDistortionGroup = false;	
	_bool										m_bRenderOn = true;
	_bool										m_bTickOn = true;
	_float4x4									m_mInitWorldMatrix;

	list <weak_ptr<GameObject>>					m_lMemberEffects;


	map<InstanceID, vector<shared_ptr<GameObject>>>	m_RenderGroup;
	vector<shared_ptr<StructuredBuffer>>			m_RenderParamBuffer;


	shared_ptr<MonoBehaviour>					m_pScript;

};
