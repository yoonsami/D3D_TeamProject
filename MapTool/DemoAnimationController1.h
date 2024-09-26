#pragma once
#include "MonoBehaviour.h"
class DemoAnimationController1 :
    public MonoBehaviour
{
public:
    virtual void Tick() override;;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<DemoAnimationController1>(); }

private:
    _int m_iCurAnimIndex = 0;
};

