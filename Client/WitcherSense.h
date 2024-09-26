#pragma once
#include "MonoBehaviour.h"
class WitcherSense :
    public MonoBehaviour
{
public:
    virtual shared_ptr<MonoBehaviour> Copy_Script() override { return make_shared<WitcherSense>(); }


public:
    virtual void Tick() override;

};

