#pragma once
#include "Client_FSM.h"
class DestroyBuilding_FSM :
    public Client_FSM
{
public:
    enum class STATE
    {
        Crash,
        NONE
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
    void Crash();
    void Crash_Init();

private:
    STATE m_eCurState = STATE::Crash;
    STATE m_ePreState = STATE::NONE;


};

