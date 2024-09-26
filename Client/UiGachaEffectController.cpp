#include "pch.h"
#include "UiGachaEffectController.h"

#include "MeshRenderer.h"

UiGachaEffectController::UiGachaEffectController()
{
}

HRESULT UiGachaEffectController::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    m_eState = STATE::DECREASE;
    m_fMaxTime = 0.5f;
    m_fCheckTime = 0.f;

    m_fRatio = 1.f / m_fMaxTime;

    return S_OK;
}

void UiGachaEffectController::Tick()
{
	if (m_pOwner.expired())
		return;

    switch (m_eState)
    {
    case STATE::DECREASE:
        Decrease_Alpha();
        break;
    case STATE::REMOVE:
        Remove();
        break;
    case STATE::START:
        Start();
        break;
    }
}

void UiGachaEffectController::Start_Effect(_float4 vPos)
{
    if (true == m_pOwner.expired())
        return;

    m_eState = STATE::START;
    m_pOwner.lock()->Set_Render(true);

    if (L"UI_Gacha_Effect3" == m_pOwner.lock()->Get_Name())
    {
        _float4 vecPos = m_pOwner.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
        vecPos.x = vPos.x;
        vecPos.y = vPos.y;
        m_pOwner.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    }
}

void UiGachaEffectController::Decrease_Alpha()
{
    m_fCheckTime += fDT;
    if (m_fMaxTime < m_fCheckTime)
    {
        m_eState = STATE::REMOVE;
        return;
    }

    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f - (m_fCheckTime * m_fRatio);
}

void UiGachaEffectController::Remove()
{
    //CUR_SCENE->Remove_GameObject(m_pOwner.lock());
    m_fCheckTime = 0.f;
    m_pOwner.lock()->Set_Render(false);
    //m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;\

}

void UiGachaEffectController::Start()
{
    m_fCheckTime += fDT;
    if (m_fMaxTime + 0.1f < m_fCheckTime)
    {
        m_eState = STATE::REMOVE;
        return;
    }

    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = m_fCheckTime * m_fRatio;
}
