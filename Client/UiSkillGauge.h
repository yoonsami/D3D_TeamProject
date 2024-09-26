#pragma once
#include "MonoBehaviour.h"

class UiSkillGauge : public MonoBehaviour
{
public:
    UiSkillGauge();


public:
    virtual HRESULT Init() override;
    virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<UiSkillGauge>(); }

    void Change_Ratio(_float fRatio);
    void Change_Render(_bool bSet, SkillInfo eInfo = SkillInfo::NONE);

private:


private:
    weak_ptr<GameObject> m_pGaugeBg;

    _float      m_fMinGauge = {};
    _float      m_fMaxGauge = {};
    _float      m_fValue    = {};
    _bool       m_bIsRender = {};

    SkillInfo   m_eInfo = {};
};

