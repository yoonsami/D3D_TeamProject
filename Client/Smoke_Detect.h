#pragma once
#include "MonoBehaviour.h"
class Smoke_Detect :
    public MonoBehaviour
{
public:
    Smoke_Detect();

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
    virtual shared_ptr<MonoBehaviour> Copy_Script() override { return make_shared<Smoke_Detect>(); }

private:
    weak_ptr<GameObject> m_pPlayer;

};

