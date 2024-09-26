#pragma once
#include "MonoBehaviour.h"
class CounterMotionTrailScript :
    public MonoBehaviour
{

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<CounterMotionTrailScript>(); }

private:
    weak_ptr<GameObject> m_pMotionTrail;
    _float m_fTrailScale = 1.f;
};

