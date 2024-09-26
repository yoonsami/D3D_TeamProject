#pragma once
#include "Component.h"

enum class Transform_State
{
    RIGHT,
    UP,
    LOOK,
    POS
};

class Transform :
    public Component
{
public:
    Transform();
    virtual ~Transform();

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
    virtual void Late_Tick() override;
    virtual void Fixed_Tick() override;

    const _float4x4& Get_WorldMatrix() { return m_matWorld; }
    const _float4x4& Get_preWorldMatrix() { return m_matPreWorld; }
    _float3& Get_CurrentDir() { return m_CurrentDir; }
    _float Get_Speed() { return m_fMoveSpeed; }
    _float4 Get_State(Transform_State eState)
    {
        return *(_float4*)(m_matWorld.m[(_uint)(eState)]);
    }

    _float3 Get_Scale();
    Quaternion Get_Rotation();
    _float3 Get_RollPitchYaw();

    void Set_State(Transform_State eState, const _float4& vState);
    void Set_Speed(_float speed) { m_fMoveSpeed = speed; }
    _bool Go_Dir(const _float3& vVel); // FDT 곱한 값 넘겨주셈
    _bool Go_Straight();
    _bool Go_Backward();
    _bool Go_Left();
    _bool Go_Right();
    _bool Go_Up();
    _bool Go_Down();
    void Go_Horizontally_Up();     
    void Go_Horizontally_Down();   
    void Rotation(const _float3& vAxis, _float fRadian);
    void Turn(const _float3& vAxis, _float fRadianPerSec);
    void Scaled(const _float3& vScale);
    void LookAt(const _float4& vTargetPos);
    void Set_Rotation(const _float3& vRollPitchYaw);
    void Set_WorldMat(const _float4x4& matWorld) { m_matWorld = matWorld; }
    void Set_LookDir(const _float3& vDir);
    void Set_Quaternion(const Quaternion& q);

    static _float4x4 Get_WorldFromLook(const _float3& vLook, const _float3& vPos);
    static Quaternion Get_QuatFromLook(const _float3& vLook);

    static _float4x4 SLerpMatrix(_float4x4& m0, _float4x4& m1, _float ratio);
    void Reset_Position() { m_matPreWorld = m_matWorld; }

private:
    _float4x4 m_matWorld = _float4x4::Identity;
    _float4x4 m_matLocal = _float4x4::Identity;
    _float m_fMoveSpeed = 1.f;
    _float3 m_CurrentDir = _float3{ 0.f, 0.f, 0.f };

    _float4x4 m_matPreWorld = _float4x4::Identity;

};
