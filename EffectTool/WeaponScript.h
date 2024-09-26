#pragma once
#include "MonoBehaviour.h"

class WeaponScript : public MonoBehaviour
{
public:
	typedef struct tagWeaponDesc
	{
        wstring strBoneName = L"Bip001-Prop1";
        _float4x4 matPivot = _float4x4::CreateRotationX(-XM_PI / 2.f) * _float4x4::CreateRotationZ(XM_PI);
        weak_ptr<GameObject> pWeaponOwner;
	}WEAPONDESC;

public:
    WeaponScript(WEAPONDESC weapondesc);
    ~WeaponScript();

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;

    virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<WeaponScript>(WEAPONDESC{ m_strBoneName,m_matPivot,m_pWeaponOwner.lock() }); }

private:
    wstring                 m_strBoneName = L"";
    _float4x4               m_matPivot = XMMatrixIdentity();
    weak_ptr<GameObject>    m_pWeaponOwner;
};

