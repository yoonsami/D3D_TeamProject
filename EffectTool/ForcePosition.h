#pragma once
#include "MonoBehaviour.h"
class ForcePosition : public MonoBehaviour
{
public:
    virtual void Final_Tick() override;
    virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<ForcePosition>(); }

public:
    _bool bForcePosition = false;
};

