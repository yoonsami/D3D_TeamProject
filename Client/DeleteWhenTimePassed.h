#pragma once
#include "MonoBehaviour.h"
class DeleteWhenTimePassed :
    public MonoBehaviour
{
public:
    DeleteWhenTimePassed(_float fTime);


public:
    virtual shared_ptr<MonoBehaviour> Copy_Script() override { return make_shared<DeleteWhenTimePassed>(m_fTime); }
    virtual void Final_Tick() override;
private:
    _float m_fTime = 0.f;
    _float m_fAcc = 0.f;
};

