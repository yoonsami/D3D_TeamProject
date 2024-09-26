#pragma once
#include "MonoBehaviour.h"

class UiDamageMove : public MonoBehaviour
{
public:
    UiDamageMove(weak_ptr<GameObject> pObj);


public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<UiDamageMove>(m_pTarget.lock()); }


private:
    void Check_Render_State();
    void Change_Pos();
    void Change_Size();
    void Change_Alpha();
    void Check_Remove();
    void Change_Pos_2D();

private:
    weak_ptr<GameObject>    m_pCamera;
    weak_ptr<GameObject>    m_pTarget;

    _float4 m_vecPos        = {};

    _float  m_fOriginSize   = {};
    _float  m_fMinSize      = {};
    _float  m_fChangeScale  = {};

    _float  m_fCheckTime    = {};
    _float  m_fMaxTime      = {};
    _float  m_fRatio        = {};
    _bool   m_bIsRender     = {};
};

