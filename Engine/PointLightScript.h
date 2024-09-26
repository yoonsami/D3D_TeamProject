#pragma once
#include "MonoBehaviour.h"

class PointLightScript :
    public MonoBehaviour
{
public:
    PointLightScript();
    virtual ~PointLightScript() = default;

public:
    HRESULT Init();
    // 현재정보를 그대로 가져오기, Init에서 사용하면 클라에선 처음에 초기화되기때문에 Clear사용
    void Clear();
    // 현재색깔과 레인지에 스타트색깔과레인지를 대입
    HRESULT Init_Reverse();
    virtual void Tick() override;

    _float Lerp(_float& f1, _float& f2, _float _fWeight);

public:
    _bool Get_bUseEffect() { return m_bUseEffect; }
    _float4 Get_AmbientStart() { return m_AmbientStartColor; }
    _float4 Get_AmbientTarget() { return m_AmbientEndColor; }
    _float4 Get_DiffuseStart() { return m_DiffuseStartColor; }
    _float4 Get_DiffuseTarget() { return m_DiffuseEndColor; }
    _float Get_StartRange() { return m_fStartRange; }
    _float Get_TargetRange() { return m_fEndRange; }
    _float Get_Speed() { return m_fEffectSpeed; }

public:
    void Set_bUseEffect(_bool _bUseEffect) { m_bUseEffect = _bUseEffect; }
    void Set_AmbientStart(_float4 _AmbientStart) { m_AmbientStartColor = _AmbientStart; }
    void Set_AmbientTarget(_float4 _AmbientTarget) { m_AmbientEndColor = _AmbientTarget; }
    void Set_DiffuseStart(_float4 _DiffuseStart) { m_DiffuseStartColor = _DiffuseStart; }
    void Set_DiffuseTarget(_float4 _DiffuseTarget) { m_DiffuseEndColor = _DiffuseTarget; }
    void Set_StartRange(_float _fRange) { m_fStartRange = _fRange; }
    void Set_TargetRange(_float _fRange) { m_fEndRange = _fRange; }
    void Set_Speed(_float _fEffectSpeed) { m_fEffectSpeed = _fEffectSpeed; }
    void Set_DeltaTime(_float _fDeltaTime) { m_fDeltaTime = _fDeltaTime; }
    void Set_DeltaPM(_float _fDeltaPM) { m_fDeltaPM = _fDeltaPM; }
    virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<PointLightScript>(); }
private:
    _bool m_bUseEffect = { false };
    _float4 m_AmbientStartColor = { 1.f, 1.f, 1.f, 1.f };
    _float4 m_AmbientEndColor = { 1.f, 1.f, 1.f, 1.f };
    _float4 m_DiffuseStartColor = { 1.f, 1.f, 1.f, 1.f };
    _float4 m_DiffuseEndColor = { 1.f, 1.f, 1.f, 1.f };
    _float m_fStartRange = { 0.f };
    _float m_fEndRange = { 0.f };
    _float m_fEffectSpeed = { 1.f };

    _float m_fDeltaTime = { 0.f };
    // 델타의 증가량의 부호 Plus 1 or Minus 1
    _float m_fDeltaPM = { 1.f };

};