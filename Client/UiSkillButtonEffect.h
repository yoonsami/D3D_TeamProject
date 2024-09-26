#pragma once
#include "MonoBehaviour.h"

class UiSkillButtonEffect : public MonoBehaviour
{
public:
    UiSkillButtonEffect();

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<UiSkillButtonEffect>(); }

    void Start_Effect();

private:
    void Start();
    void End();


private:
    enum class STATE { START, END, NONE };
    STATE       m_eState            = { STATE::NONE };

    _float3     m_vecOriginScale    = {};
    _float3     m_vecMaxScale       = {};

    _float      m_fMaxTime          = { 0.5f };
    _float      m_fCheckTime        = { 0.f };
};

