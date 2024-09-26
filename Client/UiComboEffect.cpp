#include "pch.h"
#include "UiComboEffect.h" 

#include "MeshRenderer.h"

UiComboEffect::UiComboEffect()
{
}

HRESULT UiComboEffect::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;

    m_fMaxTime = 0.25f;
    m_fCheckTime = 0.f;
    m_fRatio = 1.f / m_fMaxTime;

    return S_OK;
}

void UiComboEffect::Tick()
{
	if (m_pOwner.expired())
		return;

    if (STATE::START == m_eState)
        Start();
}

void UiComboEffect::Start_Effect(_float4 vecPos)
{
    if (m_pOwner.expired())
        return;

    m_fCheckTime = 0.f;
    m_eState = STATE::START;
    m_pOwner.lock()->Set_Tick(true);
    m_pOwner.lock()->Set_Render(true);
    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
    m_pOwner.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
}

void UiComboEffect::Start()
{
    m_fCheckTime += fDT;
    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f - m_fCheckTime * m_fRatio;
    
    if (m_fMaxTime < m_fCheckTime)
    {
        m_fCheckTime = 0.f;
        m_eState = STATE::NONE;
        m_pOwner.lock()->Set_Tick(false);
        m_pOwner.lock()->Set_Render(false);
        m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
    }
}
