#pragma once
#include "MonoBehaviour.h"

class Mir_13100_Fireball : public MonoBehaviour
{
public:
    Mir_13100_Fireball();
    ~Mir_13100_Fireball();

public:
    virtual void Tick()         override;
	virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<Mir_13100_Fireball>(); }

private:
    void         Add_Effect(const wstring& wstrEffectTag, shared_ptr<MonoBehaviour> pScript = nullptr);
    void         Add_GroupEffectOwner(const wstring& strSkilltag, _float3 vPosOffset, _bool usePosAs = false);

private:
    _float      m_fCoolTime_CreateFloorEffect = { 0.f };
};

