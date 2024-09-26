#pragma once
#include "MonoBehaviour.h"

class UiCoolEndEffect : public MonoBehaviour
{
public:
    UiCoolEndEffect();

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<UiCoolEndEffect>(); }

    void Start_Effect();

private:
    void Start();
    void End();

private:
   enum class STATE { START, END, NONE };

   STATE    m_eState        = { STATE::NONE };
   _float   m_fRatio        = { 0.f };
   _float   m_fMaxTime      = { 0.25f };
   _float   m_fCheckTime    = { 0.f };
   
};

