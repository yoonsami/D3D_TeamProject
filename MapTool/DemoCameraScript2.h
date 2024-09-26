#pragma once
#include "MonoBehaviour.h"
class DemoCameraScript2 :
    public MonoBehaviour
{
public:
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<DemoCameraScript2>(); }

private:
    _float m_fRotationSpeed = XM_PI / 4.f;
};

