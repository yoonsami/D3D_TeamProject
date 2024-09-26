#pragma once
#include "Client_FSM.h"
#include "UIInteraction.h"

class Companion_FSM abstract :
    public Client_FSM
{

public:
    virtual _bool Can_Interact();

protected:
    _bool Go_Follow();
    
    void Set_DirToTarget_Companion(_uint eType);
    void Detect_Target();
    void Calculate_EvadeCool();

protected:
    _float m_fTurnSpeed = XM_PI * 5.f;
    _float3 m_vEvadeVector = _float3(0.f);

    COOLTIMEINFO m_tAttackCoolTime = { 1.f, 0.f };
    COOLTIMEINFO m_tSkillCoolTime = { 5.f, 0.f };
    COOLTIMEINFO m_tSprintCoolTime = { 2.5f, 0.f };
    COOLTIMEINFO m_tDetectCoolTime = { 1.f, 0.f };
    COOLTIMEINFO m_tFollowCheckTime = { 0.75f, 0.f };
    COOLTIMEINFO m_tEvadeCoolTime = { 3.f, 0.f };



    _uint m_iPreAttack = 100;

    _bool m_bCantalk = false;
    _bool m_bSprint = false;
    _bool m_bSetAttack = false;
    _bool m_bCanEvade = false;
    _bool m_bEvade = false;

};

