#pragma once
#include "MonoBehaviour.h"

class UiCharChange : public MonoBehaviour
{
public:
    UiCharChange();

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;

    void Set_Hero(_uint iIndex, HERO eHero);        // ī�� �ٲٱ�
    _bool IsChangePossible(_uint iIndex);           // ���� ���� ����

private:
    void Check_Change_Cool();
    void Set_Param_Value();

private:
    typedef struct tagCharChangeDesc
    {
        HERO    eHero       = HERO::MAX;
        _float  fCoolTime   = 0.f;
        _float  fAccTime    = 0.f;
    }CHARCHANGEDESC;

    vector<CHARCHANGEDESC>          m_vecDesc;
    vector<weak_ptr<GameObject>>    m_pObj;
    vector<weak_ptr<GameObject>>    m_pElement;
    vector<wstring>                 m_TextureKey;

};

