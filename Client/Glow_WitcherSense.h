#pragma once
#include "MonoBehaviour.h"
class Glow_WitcherSense :
    public MonoBehaviour
{
public:
    Glow_WitcherSense(shared_ptr<GameObject> pPlayer);
public:
    virtual void Tick() override;

    virtual shared_ptr<MonoBehaviour> Copy_Script() override { return make_shared<Glow_WitcherSense>(m_pPlayer.lock()); }


private:
    weak_ptr<GameObject> m_pPlayer;

};

