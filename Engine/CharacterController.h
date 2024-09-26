#pragma once
#include "Component.h"
class CharacterController :
    public Component
{
public:
    CharacterController();
    virtual ~CharacterController();

    PxCapsuleControllerDesc& Get_CapsuleControllerDesc() { return m_CapsuleControllerDesc; }
    PxBoxControllerDesc& Get_BoxControllerDesc() { return m_BoxControllerDesc; }

    PxController* Get_Actor() { return m_pController; }
    void Create_Controller(_bool IsBoxCollider = false);

    virtual HRESULT Init() override;
    virtual void Tick() override;
    void Set_ControllerNull() { m_pController = nullptr; }
    void Release_Controller();
    void Add_Velocity(const _float vel);
private:


    friend class Transform;

private:
    PxCapsuleControllerDesc m_CapsuleControllerDesc = PxCapsuleControllerDesc();
    PxBoxControllerDesc m_BoxControllerDesc = PxBoxControllerDesc();
    PxController* m_pController = nullptr;
    PxMaterial* m_pMaterial = nullptr;
    _bool m_bOnGround = false;
    _bool m_bJump = false;
    _float m_fYVelocity = 0.f;
};

