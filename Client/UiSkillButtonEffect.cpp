#include "pch.h"
#include "UiSkillButtonEffect.h"

#include "MeshRenderer.h"

UiSkillButtonEffect::UiSkillButtonEffect()
{
}

HRESULT UiSkillButtonEffect::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;
    
    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;

    m_vecOriginScale = m_pOwner.lock()->GetOrAddTransform()->Get_Scale();
    m_vecMaxScale = m_vecOriginScale * 3.f;

    return S_OK;
}

void UiSkillButtonEffect::Tick()
{
	if (m_pOwner.expired() || STATE::NONE == m_eState)
		return;
 
    if (STATE::START == m_eState)
        Start();
    else
        End();
}

void UiSkillButtonEffect::Start_Effect()
{
    if (m_pOwner.expired())
        return;

    m_fCheckTime = 0.f;
    m_eState = STATE::START;
    m_pOwner.lock()->Set_Render(true);
    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
}

void UiSkillButtonEffect::Start()
{
    m_fCheckTime += fDT;

    if (m_fMaxTime < m_fCheckTime)
    {
        m_fCheckTime = 0.f;
        m_eState = STATE::END;
        
        m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;

        return;
    }

    _float fRatio = m_fCheckTime / m_fMaxTime;
    _float3 Scale = (m_vecMaxScale - m_vecOriginScale) * fRatio;
    Scale += m_vecOriginScale;

    m_pOwner.lock()->GetOrAddTransform()->Scaled(Scale);
    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f - fRatio;
}

void UiSkillButtonEffect::End()
{
    m_eState = STATE::NONE;
    m_pOwner.lock()->Set_Render(false);
    m_pOwner.lock()->GetOrAddTransform()->Scaled(m_vecOriginScale);
}
