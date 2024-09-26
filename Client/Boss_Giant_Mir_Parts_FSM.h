#pragma once
#include "Client_FSM.h"
class Boss_Giant_Mir_Parts_FSM :
    public Client_FSM
{
public:
    enum class STATE
    {
        Idle,
        NONE
    };
public:
    virtual void Tick() override;
	virtual void OnCollision(shared_ptr<BaseCollider> pCollider, _float fGap) override {}
	virtual void OnCollisionEnter(shared_ptr<BaseCollider> pCollider, _float fGap) override;
	virtual void OnCollisionExit(shared_ptr<BaseCollider> pCollider, _float fGap) override {}
    virtual void Set_State(_uint iIndex) override { m_eCurState = static_cast<STATE>(iIndex); }
    virtual void Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget) override;

protected:
    virtual void State_Tick() override;
    virtual void State_Init() override;


private:
    STATE m_eCurState = STATE::Idle;
    STATE m_ePreState = STATE::NONE;


};

