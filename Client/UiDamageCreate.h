#pragma once
#include "MonoBehaviour.h"

class UiDamageCreate : public MonoBehaviour
{
public:
    UiDamageCreate();


public:
    virtual HRESULT Init() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<UiDamageCreate>(); }

    void Create_Damage_Font(weak_ptr<GameObject> pObj, _float fDamage);
    void Create_Damage_Hit();

private:
    void Check_In_Screen();
    void Change_Pos_2D();

private:
    weak_ptr<GameObject>    m_pPlayer;
    weak_ptr<GameObject>    m_pCamera;

    vector<Color>   m_Color;
    _float          m_fSize     = {};
    _float4         m_fPos      = {};
    _bool           m_bIsIn     = { false };
};

