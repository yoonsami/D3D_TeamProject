#pragma once
#include "MonoBehaviour.h"

class UiGachaEffectController : public MonoBehaviour
{
public:
    UiGachaEffectController();

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<UiGachaEffectController>(); }

    void Start_Effect(_float4 vPos);

private:
    void Decrease_Alpha();
    void Remove();
    void Start();

private:
   enum class STATE { DECREASE, REMOVE, START };

   STATE    m_eState        = { STATE::DECREASE };
   _float   m_fRatio        = {};
   _float   m_fMaxTime      = {};
   _float   m_fCheckTime    = {};
};

