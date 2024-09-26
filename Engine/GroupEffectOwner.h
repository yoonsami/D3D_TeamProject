#pragma once
#include "Component.h"

class GroupEffectOwner : public Component
{
public:    
    GroupEffectOwner();
    virtual ~GroupEffectOwner();

public:
    virtual void        Tick() override;

    void                PlayEffect();
    
    void                Set_GroupEffectTag(const wstring& wstrTag) { m_wstrGroupEffectTag = wstrTag; }
    void                Set_GroupEffectScript(shared_ptr<MonoBehaviour> pScript);
private:
    _bool                   m_bIsFirstTick = {true};
    wstring                 m_wstrGroupEffectTag;
    weak_ptr<GameObject>    m_pGroupEffect;
};

