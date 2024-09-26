#pragma once
#include "MonoBehaviour.h"

class WeaponScript : public MonoBehaviour
{
public:
	typedef struct tagWeaponDesc
	{
        wstring strBoneName = L"";
        _float4x4 matPivot = XMMatrixIdentity();
        weak_ptr<GameObject> pWeaponOwner;
	}WEAPONDESC;

public:
    WeaponScript(WEAPONDESC weapondesc);
    ~WeaponScript();

public:
    virtual HRESULT     Init() override;
    virtual void        Late_Tick() override;
    void                Set_ModelChanged() { m_bModelChanged = true; }
    virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<WeaponScript>(WEAPONDESC{m_strBoneName,m_matPivot,m_pWeaponOwner.lock()}); }

    // Effect 
    void		        Add_Effect(const wstring& strSkilltag, shared_ptr<MonoBehaviour> pScript = nullptr);
    void                Add_Effect(const wstring& strSkilltag, const _float4x4& matWorldMat,shared_ptr<MonoBehaviour> pScript = nullptr);
    void		        Add_And_Set_Effect(const wstring& strSkilltag, shared_ptr<MonoBehaviour> pScript = nullptr);
    void		        Add_GroupEffectOwner(const wstring& strSkilltag, _float3 vPosOffset, _bool usePosAs, shared_ptr<MonoBehaviour> pScript = nullptr);
    void		        Update_GroupEffectWorldPos(const _float4x4& mWorldMatrix);
private:
    wstring             m_strBoneName = L"";
    _float4x4           m_matPivot = XMMatrixIdentity();
    _bool               m_bModelChanged = false;
    weak_ptr<GameObject>            m_pWeaponOwner;

    //For Effect.Create
    vector<weak_ptr<GameObject>>	m_vGroupEffect;
};

