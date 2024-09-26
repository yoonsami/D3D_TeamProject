#pragma once
#include "MonoBehaviour.h"
typedef struct tagInstallationSkillDesc
{
    _float4 vSkillDir = { 0.f,0.f,0.f,0.f };
    _float fMoveSpeed = 1.f;
    _float fLimitDistance = 0.f;
    _float fAttackTickTime = 0.f;
    _uint iLimitAttackCnt = 1;
    wstring strAttackType = NORMAL_ATTACK;
    wstring strLastAttackType = NORMAL_ATTACK;
    _bool bFirstAttack = true;
    _float fAttackDamage = 0.f;
    _float fLastAttackDamage = 0.f;
   
}INSTALLATIONSKILLDESC;

class InstallationSkill_Script : public MonoBehaviour
{
public:
    InstallationSkill_Script(INSTALLATIONSKILLDESC desc);
    ~InstallationSkill_Script();

public:

    virtual HRESULT Init() override;
    virtual void Tick() override;
    virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<InstallationSkill_Script>(INSTALLATIONSKILLDESC{m_vLook,m_fMoveSpeed,m_fLimitDistance,m_tAttackTickTime.fCoolTime,m_iLimitAttackCnt,m_strAttackType,m_strLastAttackType,m_bFirstAttack,m_fAttackDamage,m_fLastAttackDamage}); }

private:
    _float4 m_vFirstPos = { 0.f,0.f,0.f,1.f };
    _float4 m_vLook = { 0.f,0.f,0.f,0.f };
    
    _float m_fMoveSpeed = 1.f;
    _float m_fLimitDistance = 0.f;

    _float m_fFirstAttackDamage = 0.f;
    _float m_fAttackDamage = 0.f;
    _float m_fLastAttackDamage = 0.f;

    COOLTIMEINFO m_tAttackTickTime = { 2.f,0.f };

    _uint m_iCurrentAttackCnt = 0;
    _uint m_iLimitAttackCnt = 1;
    
    wstring m_strAttackType = NORMAL_ATTACK;
    wstring m_strLastAttackType = NORMAL_ATTACK;
    

    _bool m_bFirstAttack = true;
    _bool m_bAttackOn = false;

};

