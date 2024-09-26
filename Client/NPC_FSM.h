#pragma once
#include "Client_FSM.h"
class NPC_FSM abstract:
    public Client_FSM
{
public: 
    enum NPC_FSM_TYPE
    {
        POTION,
        GUARD1,
        GUARD2,
        TRAVELER,
        CHILDREN,
    };
public:

public:
    virtual void InteractWithPlayer() abstract;
    virtual _bool Can_Interact();


    void Set_MinMovePos(const _float3& pos) { m_vMinMovingPoint = pos; }
    void Set_MaxMovePos(const _float3& pos) { m_vMaxMovingPoint = pos; }

protected:
    _float3 m_vMinMovingPoint{};
    _float3 m_vMaxMovingPoint{};

    _float m_fStateAcc = 0.f;

    _float m_fInteractRange = 1.f;
    _float m_fMoveTimer = 7.f;
    _float m_fIdleTimer = 5.f; 
};

