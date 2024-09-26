#pragma once
#include "MonoBehaviour.h"

class UIBossHpBar : public MonoBehaviour
{
public:
    UIBossHpBar(BOSS eBoss);


public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<UIBossHpBar>(m_eBoss); }
    void Remove_HpBar();

private:
    void Check_Target();
    void Change_Hp_Ratio();
    void Change_Hp_Slow();
    void Change_Param();

private:
    weak_ptr<GameObject>    m_pBgHp;
    weak_ptr<GameObject>    m_pFrontHp;
    weak_ptr<GameObject>    m_pBackHp;
    weak_ptr<GameObject>    m_pHpFont;
    weak_ptr<GameObject>    m_pBgGroggy;
    weak_ptr<GameObject>    m_pFrontGroggy;
    weak_ptr<GameObject>    m_pElement;
    weak_ptr<GameObject>    m_pBossName;

    _float                  m_fRatio        = { 0.f };
    _bool                   m_bIsWork       = { false };
    _float                  m_fSpeed        = { 0.f };
    _float                  m_fTargetRatio  = { 0.f };

    BOSS                    m_eBoss         = { BOSS::MAX };
};

