#pragma once
#include "MonoBehaviour.h"
class ObjectDissolveCreate :
    public MonoBehaviour
{
public:
    ObjectDissolveCreate(_float fDissolveSpeed, shared_ptr<Texture> dissolveTexture = nullptr);

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<ObjectDissolveCreate>(m_fDissolveSpeed,m_pTexture.lock()); }


public:
    weak_ptr<Texture> m_pTexture;
    _float m_fDissolveSpeed = 0.f;
    _bool m_bCreate = false;

};

