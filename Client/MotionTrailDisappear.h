#pragma once
#include "MonoBehaviour.h"
class MotionTrailDisappear :
    public MonoBehaviour
{
public:
    MotionTrailDisappear(_float fDisappearSpeed, const Color& vInitColor);
public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<MotionTrailDisappear>(m_fDisappearSpeed,m_vInitColor); }

private:
    _float m_fAcc = 0.f;
    _float m_fDisappearSpeed = 0.f;
    Color m_vInitColor = Color(0.f);
};

