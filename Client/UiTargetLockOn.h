#pragma once
#include "MonoBehaviour.h"

class UiTargetLockOn : public MonoBehaviour
{
public:
    UiTargetLockOn();

public:
    virtual HRESULT Init()      override;
    virtual void Tick()         override;
    virtual void Late_Tick()    override;
    virtual void Final_Tick()   override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<UiTargetLockOn>(); }

    void Set_Target(shared_ptr<GameObject> pObj);

private:
    void Check_Render_State();
    void Change_Scale();
    void Update_Target_Pos();
    void Check_Target();

private:
    weak_ptr<GameObject>    m_pCamera;
    weak_ptr<GameObject>    m_pLockOn0;
    weak_ptr<GameObject>    m_pLockOn1;
    weak_ptr<GameObject>    m_pTarget;

    enum class STATE { DOWN, UP };
    STATE       m_eState        = { STATE::UP };
    _float3     m_vecScale      = {};
    _float      m_fSpeed        = {};
    _float      m_fOriginScale  = {};
    _float      m_fMaxScale     = {};
    _bool       m_bIsRender     = { false };
};

