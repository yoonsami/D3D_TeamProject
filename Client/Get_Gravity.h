#pragma once
#include "MonoBehaviour.h"
class Get_Gravity :
    public MonoBehaviour
{
public:
    Get_Gravity(_float fFinalDistY);

public:
    virtual void Final_Tick() override;
    virtual shared_ptr<MonoBehaviour> Copy_Script() override { return make_shared<Get_Gravity>(m_fFinalDistY); }

private:
    _float m_fFinalDistY = 0.f;
    _float m_fVelY = 0.f;
    

};

