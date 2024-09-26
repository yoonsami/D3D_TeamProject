#pragma once
#include "MonoBehaviour.h"

class CreateEffect_Script : public MonoBehaviour
{
public:
	CreateEffect_Script();
	~CreateEffect_Script();

public:
	virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script()override;

	void		Set_CoolTime(_float fCoolTime) { m_fCoolTime = fCoolTime; }
	void		Set_SkillTag(const wstring& wstrSkillTag) { m_wstrSkillTag = wstrSkillTag; }
	void		Set_OffsetMatrix(const _float4x4& matOffset) { m_matOffset = matOffset; }
	void		Set_UseOwnerPosOnly(_bool bUseOwnerPosOnly) { m_bUseOwnerPosOnly = bUseOwnerPosOnly;}

private:
	void		Add_Effect();

private:
	_float		m_fCoolTime = { 1.f };
	wstring		m_wstrSkillTag;
	_float4x4	m_matOffset = _float4x4::Identity;
	_bool		m_bUseOwnerPosOnly = { false };

	_float      m_fCoolTime_CreateFloorEffect = { 0.f };
};

