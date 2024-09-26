#pragma once
#include "Client_FSM.h"
class SpearAce_Clone_FSM :
    public Client_FSM
{
public:
    SpearAce_Clone_FSM(_uint iCloneIndex) : m_iCloneIndex(iCloneIndex) {}
public:
    enum class STATE
    {
        Skill_Use,
        Skill_End,
        NONE,
    };
public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
    virtual void Set_State(_uint iIndex) override { m_eCurState = static_cast<STATE>(iIndex); }
    virtual void Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget) override {}

protected:
    virtual void State_Tick() override;
    virtual void State_Init() override;

private:
	void Skill_Use();
	void Skill_Use_Init();
	void Skill_End();
	void Skill_End_Init();

private:
    _uint m_iCloneIndex = 0;
    STATE m_eCurState = STATE::Skill_Use;
    STATE m_ePreState = STATE::NONE;
    

};

