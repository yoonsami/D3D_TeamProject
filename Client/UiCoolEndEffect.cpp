#include "pch.h"
#include "UiCoolEndEffect.h" 

#include "MeshRenderer.h"

UiCoolEndEffect::UiCoolEndEffect()
{
}

HRESULT UiCoolEndEffect::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;

    m_fMaxTime = 0.5f;
    m_fRatio = 1.2f / m_fMaxTime;

    return S_OK;
}

void UiCoolEndEffect::Tick()
{
	if (m_pOwner.expired() || STATE::NONE == m_eState)
		return;

    if (STATE::START == m_eState)
        Start();
    else if (STATE::END == m_eState)
        End();
}

void UiCoolEndEffect::Start_Effect()
{
    if (m_pOwner.expired())
        return;

    m_eState = STATE::START;
    m_pOwner.lock()->Set_Tick(true);
    m_pOwner.lock()->Set_Render(true);
    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = 0.f;
    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[1] = -0.2f;
}

void UiCoolEndEffect::Start()
{
    m_fCheckTime += fDT;
    if (m_fMaxTime < m_fCheckTime)
    {
        m_eState = STATE::END;
        m_fCheckTime = 0;
    }

    _float fValue = fDT * m_fRatio;
    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] += fValue;
    if (1.f < m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0])
        m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = 0.99f;

    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[1] += fValue;
}

void UiCoolEndEffect::End()
{
    m_pOwner.lock()->Set_Tick(false);
    m_pOwner.lock()->Set_Render(false);
    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = 0.f;
    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[1] = 0.f;
}
