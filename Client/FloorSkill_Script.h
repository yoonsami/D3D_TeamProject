#pragma once
#include "MonoBehaviour.h"
typedef struct tagFloorSkillDesc
{
    _float4 vSkillDir = { 0.f,0.f,0.f,0.f };
    _float fAttackStartGap = 0.f;
    _float fAttackTickTime = 0.f;
    wstring strAttackType = NORMAL_ATTACK;
    wstring strLastAttackType = NORMAL_ATTACK;
    _float fAttackDamage = 0.f;
    _float fLastAttackDamage = 0.f;
    _bool bFirstAttack = false;
    _uint iLimitAttackCnt = 1;

}FLOORSKILLDESC;

class FloorSkill_Script : public MonoBehaviour
{
public:
    FloorSkill_Script(FLOORSKILLDESC desc);
    ~FloorSkill_Script();

public:

    virtual HRESULT Init() override;
    virtual void Tick() override;
    virtual shared_ptr<MonoBehaviour> Copy_Script()override 
    { 
        return make_shared<FloorSkill_Script>(FLOORSKILLDESC{ m_vLook,m_tAttackStartTime.fCoolTime, m_tAttackTickTime.fCoolTime,m_strAttackType,m_strLastAttackType,m_fAttackDamage,m_fLastAttackDamage,m_bFirstAttack, m_iLimitAttackCnt });
    }
    

private:
    _float4 m_vFirstPos = { 0.f,0.f,0.f,1.f };
    _float4 m_vLook = { 0.f,0.f,0.f,0.f };

    _float m_fFirstAttackDamage = 0.f;
    _float m_fAttackDamage = 0.f;
    _float m_fLastAttackDamage = 0.f;

    _bool m_bAttackStart = false;
    _bool m_bFirstAttack = false;
    _bool m_bAttackOn = false;

    COOLTIMEINFO m_tAttackStartTime = { 2.f,0.f };
    COOLTIMEINFO m_tAttackTickTime = { 2.f,0.f };

    _uint m_iCurrentAttackCnt = 0;
    _uint m_iLimitAttackCnt = 1;


    wstring m_strAttackType = NORMAL_ATTACK;
    wstring m_strLastAttackType = NORMAL_ATTACK;

};

