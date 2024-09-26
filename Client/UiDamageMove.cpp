#include "pch.h"
#include "UiDamageMove.h"

#include "Camera.h"
#include "FontRenderer.h"

UiDamageMove::UiDamageMove(weak_ptr<GameObject> pObj)
    : m_pTarget(pObj)
{
}

HRESULT UiDamageMove::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    m_pCamera = CUR_SCENE->Get_Camera(L"Default");
    
    //m_fOriginSize = m_pOwner.lock()->Get_FontRenderer()->Get_Size();
    //m_fMinSize = 1.f;
    //m_fChangeScale = m_fOriginSize - m_fMinSize;

    m_fMaxTime = 0.5f;
    m_fRatio = 1.f / m_fMaxTime;

    return S_OK;
}

void UiDamageMove::Tick()
{
    m_fCheckTime += fDT;
    Check_Remove();

	if (true == m_pOwner.expired() ||
        true == m_pTarget.expired() ||
        true == m_pCamera.expired())
		return;

    Check_Render_State();
    if (false == m_bIsRender)
        return;

    Change_Pos_2D();
    Change_Pos();
    //Change_Size();
    Change_Alpha();
}

void UiDamageMove::Check_Render_State()
{
    _float3 cullPos = m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS).xyz();
    _float cullRadius = 1.5f;
    Frustum frustum = m_pCamera.lock()->Get_Camera()->Get_Frustum();

    m_bIsRender = false;
    if (frustum.Contain_Sphere(cullPos, cullRadius))
    {
        m_pOwner.lock()->Set_Render(true);
        m_bIsRender = true;
    }
    else
        m_pOwner.lock()->Set_Render(false);
}

void UiDamageMove::Change_Pos()
{
    //m_vecPos.x += fDT * 200.f;
    //m_vecPos.y += 100.f;
    m_vecPos.y = 100.f * m_fCheckTime * m_fRatio;
    m_pOwner.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, m_vecPos);
}

void UiDamageMove::Change_Size()
{
    _float fSize = m_fOriginSize - (m_fChangeScale * m_fCheckTime * m_fRatio);
    m_pOwner.lock()->Get_FontRenderer()->Get_Size() = fSize;
}

void UiDamageMove::Change_Alpha()
{
    Color vColor = m_pOwner.lock()->Get_FontRenderer()->Get_Color();
    vColor.w = 1.f - m_fCheckTime * m_fRatio;
    m_pOwner.lock()->Get_FontRenderer()->Get_Color() = vColor;
}

void UiDamageMove::Check_Remove()
{
    if (m_fMaxTime < m_fCheckTime && false == m_pOwner.expired())
        EVENTMGR.Delete_Object(m_pOwner.lock());
}

void UiDamageMove::Change_Pos_2D()
{
    _float4x4 viewPos = m_pCamera.lock()->Get_Camera()->Get_ViewMat();
    _float4x4 projPos = m_pCamera.lock()->Get_Camera()->Get_ProjMat();

    m_vecPos = m_pTarget.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);

    m_vecPos = XMVector3TransformCoord(m_vecPos, viewPos);
    m_vecPos = XMVector3TransformCoord(m_vecPos, projPos);

    m_vecPos.x = (m_vecPos.x + 1.f) * 0.5f * g_iWinSizeX;
    m_vecPos.y = ((m_vecPos.y * -1.f) + 1.f) * 0.5f * g_iWinSizeY;
    
    m_vecPos.x -= g_iWinSizeX * 0.5f;
    m_vecPos.y = (m_vecPos.y * -1.f) + g_iWinSizeY * 0.5f;
    
    m_vecPos.x -= 150.f;
    m_vecPos.y += 300.f;
}
