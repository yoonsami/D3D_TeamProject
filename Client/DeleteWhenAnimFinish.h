#pragma once
#include "MonoBehaviour.h"
class DeleteWhenAnimFinish :
    public MonoBehaviour
{
public:
    DeleteWhenAnimFinish(const wstring& strAnimName, _float fWaitTime, _bool bAnimLoop = false, _float fAnimSpeed = 1.f);

public:
    virtual HRESULT Init() override;
    virtual void Final_Tick() override;
    virtual shared_ptr<MonoBehaviour> Copy_Script() override { return make_shared<DeleteWhenAnimFinish>(m_strAnimName,m_fWaitTime,m_bAnimLoop,m_fAnimSpeed); }

    void Set_DissolveOption(_float fDissolveSpeed, shared_ptr<Texture> pTexture) { m_fDissolveSpeed = fDissolveSpeed; m_pDissolveTexture = pTexture; }

private:
    wstring m_strAnimName;
    _bool m_bAnimLoop = false;
    _float m_fAnimSpeed = 1.f;
    _float m_fWaitTime = 0.f;
    _bool m_bDissolveWhenFinished = false;
    _float m_fDissolveSpeed = 0.f;
    shared_ptr<Texture> m_pDissolveTexture;

    _bool m_bAnimFinished = false;
    _float m_fAcc = 0.f;

};

