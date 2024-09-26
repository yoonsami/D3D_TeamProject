#pragma once
#include "MonoBehaviour.h"
class Smoke_WitcherSense :
    public MonoBehaviour
{
public:
    Smoke_WitcherSense(shared_ptr<GameObject> pPlayer);

public:
    virtual void Tick() override;
    virtual shared_ptr<MonoBehaviour> Copy_Script() override { return make_shared<Smoke_WitcherSense>(m_pPlayer.lock()); }

private:
    weak_ptr<GameObject> m_pPlayer;
    _bool m_bAlreadyMeet = false;

};

