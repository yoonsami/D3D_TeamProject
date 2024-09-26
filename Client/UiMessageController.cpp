#include "pch.h"
#include "UiMessageController.h" 

#include "Camera.h"
#include "FontRenderer.h"
#include "MeshRenderer.h"

UiMessageController::UiMessageController(shared_ptr<GameObject> pTarget, _bool bIsBg)
    : m_pTarget(pTarget)
    , m_bIsBg(bIsBg)
{
}

HRESULT UiMessageController::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    m_pCamera = CUR_SCENE->Get_Camera(L"Default");

    m_fMaxIdleTime = 0.5f;
    m_fMaxDecreaseTime = 1.f;
    m_fCheckTime = 0.f;;
    m_fDecreaseRatio = 1.f / m_fMaxDecreaseTime;

    m_eState = MESSAGE_STATE::IDLE;

    // check before start
    Check_Distance();
    Change_Pos();

    return S_OK;
}

void UiMessageController::Tick()
{
    if (true == m_pOwner.expired())
        return;

    if (false == m_pTarget.expired())
    {
        Check_Distance();
        Change_Pos();
    }
    
    switch (m_eState)
    {
    case MESSAGE_STATE::IDLE:
        IDLE();
        break;
    case MESSAGE_STATE::DECREASE:
        DECREASE();
        break;
    case MESSAGE_STATE::REMOVE:
        REMOVE();
        break;
    case MESSAGE_STATE::NONE:
        break;
    }
}

void UiMessageController::Check_Distance()
{
    _float3 pos = m_pTarget.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).xyz();
    auto pPlayer = GET_PLAYER;

    if (pPlayer)
    {
        _float lengthSQ = (pPlayer->Get_Transform()->Get_State(Transform_State::POS).xyz() - pos).LengthSquared();

        if (lengthSQ > 9.f * 9.f)
            m_pOwner.lock()->Set_Render(false);
        else
            m_pOwner.lock()->Set_Render(true);
    }
}

void UiMessageController::Change_Pos()
{
    _float4x4 viewPos = m_pCamera.lock()->Get_Camera()->Get_ViewMat();
    _float4x4 projPos = m_pCamera.lock()->Get_Camera()->Get_ProjMat();

    _float4 vecPos = m_pTarget.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.y += 3.f;

    vecPos = XMVector3TransformCoord(vecPos, viewPos);
    vecPos = XMVector3TransformCoord(vecPos, projPos);

    vecPos.x = (vecPos.x + 1.f) * 0.5f * g_iWinSizeX;
    vecPos.y = ((vecPos.y * -1.f) + 1.f) * 0.5f * g_iWinSizeY;

    vecPos.x -= g_iWinSizeX * 0.5f;
    vecPos.y = (vecPos.y * -1.f) + g_iWinSizeY * 0.5f;

    //vecPos.x += 200.f;
    if(true == m_bIsBg)
        m_pOwner.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    else
    {
        // 글자수만큼 밀기
        vecPos.x -= 100.f;
        vecPos.y += 15.f;
        m_pOwner.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    }
    //vecPos.x -= 150.f;
    //vecPos.y += 300.f;
}

void UiMessageController::IDLE()
{
    m_fCheckTime += fDT;
    if (m_fMaxIdleTime < m_fCheckTime)
    {
        m_fCheckTime = 0.f;
        m_eState = MESSAGE_STATE::DECREASE;
    }
}

void UiMessageController::DECREASE()
{
    // 1 -> 0
    m_fCheckTime += fDT;
    if (m_fMaxDecreaseTime < m_fCheckTime)
    {
        m_fCheckTime = m_fMaxDecreaseTime;
        m_eState = MESSAGE_STATE::REMOVE;
    }

    _float fTemp = 1.f - (m_fCheckTime / m_fMaxDecreaseTime) * m_fDecreaseRatio;
    if (true == m_bIsBg)
        m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = fTemp;
    else
        m_pOwner.lock()->Get_FontRenderer()->Get_Color() = Color(fTemp, fTemp, fTemp, fTemp);
}

void UiMessageController::REMOVE()
{
    //m_eState = MESSAGE_STATE::NONE;

    EVENTMGR.Delete_Object(m_pOwner.lock());
}