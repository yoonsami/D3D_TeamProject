#pragma once
#include "Client_FSM.h"
#include "ForwardMovingSkillScript.h"
#include "FloorSkill_Script.h"

class DragonBallMonster_FSM :
    public Client_FSM
{
public:
    enum class STATE
    {
        Idle,
        Crash,
        NONE
    };
public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
    virtual void Set_State(_uint iIndex) override { m_eCurState = static_cast<STATE>(iIndex); }
    virtual void Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget) override;

protected:
    virtual void State_Tick() override;
    virtual void State_Init() override;

private:
    void Idle();
    void Idle_Init();
    void Crash();
    void Crash_Init();

    void Summon_CrossFloor(); // +
    void Summon_X_Floor(); // X
    void Summon_Hash_Floor();  // #
    void Summon_Web_Floor();   
    void Summon_HalfCircle_Floor(); // D
    void Summon_Star_Floor(); 

    void Create_Meteor();
    void Create_FloorSkillEffect();


    void Set_AttackPattern();
    
private:
    STATE m_eCurState = STATE::Idle;
    STATE m_ePreState = STATE::NONE;

    _uint m_iPreAttack = 100;
    _uint m_iCurMeteorCnt = 0;
    _uint m_iLimitMeteorCnt = 0;

    _bool m_bSummonMeteor = false;

    COOLTIMEINFO m_tPatternCoolTime = { 4.f, 0.f };
    COOLTIMEINFO m_tMeteorCoolTime = { 2.f, 0.f };

    _float  m_fTimer_CreateFloorSkillEffect = { 0.f };
    _bool   m_bIsCreateFloorSkillEffectDone = { false };

    weak_ptr<GameObject> m_pRigidBody;
};

