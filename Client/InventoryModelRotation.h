#pragma once
#include "MonoBehaviour.h"
class InventoryModelRotation :
    public MonoBehaviour
{
public:
    virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<InventoryModelRotation>(); }

    virtual void Tick() override;
};

