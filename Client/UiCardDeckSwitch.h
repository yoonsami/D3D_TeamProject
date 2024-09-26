#pragma once
#include "MonoBehaviour.h"

class UiCardDeckSwitch : public MonoBehaviour
{
public:
    UiCardDeckSwitch(_bool bIsFont);
	virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<UiCardDeckSwitch>(m_bIsFont); }


public:
    virtual HRESULT Init() override;
    virtual void Tick() override;


private:
    void Change_Time();
    void Change_Alpha();
    void Change_Color();
    void Remove_Owner();

private:
    _bool   m_bIsFont       = {};
    _float  m_fCheckTime    = {};
    _float  m_fMaxTime      = {};
    _float  m_fRatio        = {};

    _bool   m_bIsEnd        = {};

};

