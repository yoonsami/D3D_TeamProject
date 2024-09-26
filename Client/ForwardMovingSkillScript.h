#pragma once
#include "MonoBehaviour.h"
typedef struct tagForwardMovingSkillDesc
{
    _float4 vSkillDir;
    _float fMoveSpeed = 1.f;
    _float fLifeTime = 2.f;
    _float fLimitDistance = 0.f;
}FORWARDMOVINGSKILLDESC;

class ForwardMovingSkillScript : public MonoBehaviour
{
public:
    ForwardMovingSkillScript(FORWARDMOVINGSKILLDESC desc);
    ~ForwardMovingSkillScript();

public:

    virtual HRESULT Init() override;
    virtual void Tick() override;
    virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<ForwardMovingSkillScript>(FORWARDMOVINGSKILLDESC{m_vLook,m_fMoveSpeed,m_tLifeTime.fCoolTime,m_fLimitDistance}); }

private:
    _float4 m_vLook = { 0.f,0.f,0.f,0.f };
    _float m_fMoveSpeed = 1.f;
    COOLTIMEINFO m_tLifeTime = { 2.f,0.f };
    _float m_fLimitDistance = 0.f;
    _float4 m_vFirstPos = { 0.f,0.f,0.f,1.f };

};

