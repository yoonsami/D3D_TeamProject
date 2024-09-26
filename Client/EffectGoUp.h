#pragma once
#include "MonoBehaviour.h"
class EffectGoUp :
    public MonoBehaviour
{
public:
    EffectGoUp(_float fUpSpeed,shared_ptr<GameObject> owner);
public:
    virtual void Final_Tick() override;
    virtual shared_ptr<MonoBehaviour> Copy_Script() override { return make_shared<EffectGoUp>(m_fSpeed, m_pFollowObj.lock()); }

private:
    _float m_fSpeed = 0.f;
    _float m_fAcc = 0.f;
    weak_ptr<GameObject> m_pFollowObj;
};

