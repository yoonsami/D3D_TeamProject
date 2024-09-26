#pragma once
#include "MonoBehaviour.h"
class WaterUVSliding :
    public MonoBehaviour
{

public:
    virtual void Tick() override;

public:
    void Set_Color1(_float4 _Color1) { m_Color1 = _Color1; }
    void Set_Color2(_float4 _Color2) { m_Color2 = _Color2; }
    void Set_Int0(_int _int0) { m_int0 = _int0; }
    void Set_Int1(_int _int1) { m_int1 = _int1; }
    virtual shared_ptr<MonoBehaviour> Copy_Script() { return nullptr; }

private:
    _float4 m_Color1 = { -36.f / 255.f, 124.f / 255.f, 1.f, 1.f };
    _float4 m_Color2 = { -69.f / 255.f, 62.f / 255.f, 1.f, 1.f };
    _int m_int0 = { 100 };
    _int m_int1 = { 10 };
};

