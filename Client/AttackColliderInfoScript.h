#pragma once
#include "MonoBehaviour.h"
class AttackColliderInfoScript : public MonoBehaviour
{

public:
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<AttackColliderInfoScript>(); }


public:
    void Set_SkillName(const wstring& name) { m_strSkillName = name; }
    void Set_ColliderOwner(shared_ptr<GameObject> pColliderOwner);
    void Set_AttackDamage(_float fAttackDamage) { m_fAttackDamage = fAttackDamage; }
    void Set_AttackElementType(ElementType eType) { m_eElementType = eType; }
    void Set_HitEffectTag(const wstring& name) { m_strHitEffectTag = name; }

    const wstring& Get_SkillName() { return m_strSkillName; }
    const wstring& Get_HitEffectTag() { return m_strHitEffectTag; }
    
    _float Get_AttackDamage() { return m_fAttackDamage; }
    ElementType Get_AttackElementType() { return m_eElementType; }
    
    shared_ptr<GameObject> Get_ColliderOwner() { return m_pColliderOwner.lock(); }

private:
    wstring m_strSkillName = L"";
    _float m_fAttackDamage = 0.f;
    weak_ptr<GameObject> m_pColliderOwner;
    ElementType m_eElementType = FIRE;
    wstring m_strHitEffectTag = L"";
};



