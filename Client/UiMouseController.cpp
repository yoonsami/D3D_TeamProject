#include "pch.h"
#include "UiMouseController.h" 

#include "MeshRenderer.h"

_bool g_bIsCanMouseMove = true;

UiMouseController::UiMouseController()
{
}

HRESULT UiMouseController::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;
    
    m_fMaxTime = 0.25f;
    m_fCheckTime = 0.f;
    m_fRatio = 1.f / m_fMaxTime;

    m_pOwner.lock()->Set_Render(false);
    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;

    m_pMouse = CUR_SCENE->Get_GameObject(L"UI_Mouse");
    if (true == m_pMouse.expired())
        return E_FAIL;
    //m_pMouse.lock()->Set_Render(false);
    
    //INPUT.Set_Show_Cursor(false);
    ::ShowCursor(false);

    return S_OK;
}

void UiMouseController::Tick()
{
	if (true == m_pOwner.expired() ||
        true == m_pMouse.expired())
		return;

    if (KEYTAP(KEY_TYPE::LALT))
        Change_Mouse_State();

    if (true == g_bIsCanMouseMove)
        Change_Mouse_Pos();

    if (true == g_bIsCanMouseMove && KEYTAP(KEY_TYPE::LBUTTON))
        Start_Effect();

    if (STATE::START == m_eState)
        Start();
}

void UiMouseController::Change_Mouse_State(_bool bValue)
{
    if (bValue == g_bIsCanMouseMove)
        return;

    Change_Mouse_State();
}

void UiMouseController::Change_Mouse_State()
{
    g_bIsCanMouseMove = !g_bIsCanMouseMove;

    INPUT.Set_Mouse_Move(g_bIsCanMouseMove);

    if (true == g_bIsCanMouseMove)
    {
        m_pMouse.lock()->Set_Render(true);
    }
    else if (false == g_bIsCanMouseMove)
    {
        m_pMouse.lock()->Set_Render(false);
    }
}

void UiMouseController::Start_Effect()
{
    m_fCheckTime = 0.f;
    m_eState = STATE::START;

    m_pOwner.lock()->Set_Render(true);
    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;

    POINT ptMouse = INPUT.GetMousePosToPoint();
    _float4 vecPos = m_pOwner.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
    vecPos.y = static_cast<_float>(ptMouse.y * -1.f + g_iWinSizeY / 2.f);
    m_pOwner.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
}

void UiMouseController::Start()
{
    m_fCheckTime += fDT;
    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f - m_fCheckTime * m_fRatio;

    if (m_fMaxTime < m_fCheckTime)
    {
        m_fCheckTime = 0.f;
        m_eState = STATE::NONE;
        m_pOwner.lock()->Set_Render(false);
        m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
    }
}

void UiMouseController::Change_Mouse_Pos()
{
    POINT ptMouse = INPUT.GetMousePosToPoint();
    _float4 vecPos = m_pOwner.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
    vecPos.y = static_cast<_float>(ptMouse.y * -1.f + g_iWinSizeY / 2.f);
    m_pMouse.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
}
