#pragma once
#include "MonoBehaviour.h"
#include "Utils.h"
class WeedScript :
    public MonoBehaviour
{
public:
    virtual void Tick() override;

public:
    void Set_WindVector(_float4 _WindVector) { m_Wind = _WindVector; }
    //void Set_WindPower(_float _fWindPower) { m_fWindPower = _fWindPower; }
    void Set_WindWeight(_float _fWindWeight) { m_fWindWeight = _fWindWeight; }
    void Set_WindSpeed(_float _fWindSpeed) { m_fWindSpeed = _fWindSpeed; }
    virtual shared_ptr<MonoBehaviour> Copy_Script() { return nullptr; }
    
private:
    // 바람방향
    _float4 m_Wind = { -1.f, 0.f, 0.f, 0.f };
    // 바람파워
    //_float m_fWindPower = { 0.05f };
    // 바람의 현재상태 (0~1)
    _float m_fWindWeight = { Utils::Random_In_Range(0.f, 1.f) };
    _float m_fWindWeightPM = { 1.f };
    // 바람의 속도
    _float m_fWindSpeed = { Utils::Random_In_Range(0.9f, 1.1f) };
};

