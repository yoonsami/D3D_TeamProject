#pragma once
#include "MonoBehaviour.h"

class UiComboEffect : public MonoBehaviour
{
public:
    UiComboEffect();


public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<UiComboEffect>(); }

    void Start_Effect(_float4 vecPos);

private:
    void Start();

private:
    enum class STATE { START, NONE };
    STATE   m_eState        = { STATE::NONE };

    _float  m_fMaxTime      = {};
    _float  m_fCheckTime    = {};
    _float  m_fRatio        = {};

};

