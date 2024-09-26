#pragma once
#include "MonoBehaviour.h"

class DemoCameraScript1 :
    public MonoBehaviour
{
public:
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<DemoCameraScript1>(); }

};

