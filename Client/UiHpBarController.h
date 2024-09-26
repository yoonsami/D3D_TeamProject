#pragma once
#include "MonoBehaviour.h"

class UiHpBarController : public MonoBehaviour
{
public:
    UiHpBarController(_uint iType);

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<UiHpBarController>(m_iObjType); }

private:
    void Change_Param();
    void Change_Hp_Ratio();
    void Change_Hp_Slow();

private:
    weak_ptr<GameObject>    m_pFrontHp;
    weak_ptr<GameObject>    m_pBackHp;
    weak_ptr<GameObject>    m_pFont;

    _uint   m_iObjType      = { 0 };
    _float  m_fValue        = { 0.f };
    _float  m_fPreHp        = { 1.f };

    _float  m_fRatio        = { 0.f };
    _float  m_fMaxHp        = { 0.f };
    _float  m_fTargetRatio  = { 0.f };
    _float  m_fWorkRatio    = { 0.f };
    _float  m_fSpeed        = { 0.f };
    _bool   m_bIsWork       = { false };
};

