#include "pch.h"
#include "UiTargetLockOn.h" 

#include "Camera.h"
#include "MeshRenderer.h"

UiTargetLockOn::UiTargetLockOn()
{
}

HRESULT UiTargetLockOn::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    m_pCamera = CUR_SCENE->Get_Camera(L"Default");

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;

    m_pLockOn0 = CUR_SCENE->Get_UI(L"UI_Target_LockOn0");
    m_pLockOn1 = CUR_SCENE->Get_UI(L"UI_Target_LockOn1");

    if (false == m_pLockOn1.expired())
    {
        m_fOriginScale = m_vecScale.x = m_pLockOn1.lock()->GetOrAddTransform()->Get_Scale().x;
        m_vecScale.y = m_pLockOn1.lock()->GetOrAddTransform()->Get_Scale().y;
        
        m_fMaxScale = 100.f;
        m_fSpeed = m_fMaxScale - m_fOriginScale;
    }

    return S_OK;
}

void UiTargetLockOn::Tick()
{
	if (true == m_pOwner.expired() ||
        true == m_pCamera.expired() ||
        true == m_pLockOn0.expired() ||
        true == m_pLockOn1.expired() ||
        true == m_pTarget.expired())
		return;

    Check_Render_State();
    if (false == m_bIsRender)
        return;

    Check_Target();
    Change_Scale();
}

void UiTargetLockOn::Late_Tick()
{
    /*if (true == m_pOwner.expired() ||
        true == m_pCamera.expired() ||
        true == m_pLockOn0.expired() ||
        true == m_pLockOn1.expired() ||
        true == m_pTarget.expired())
        return;

    if (false == m_bIsRender)
        return;

    Update_Target_Pos();*/
}

void UiTargetLockOn::Final_Tick()
{
    if (true == m_pOwner.expired() ||
        true == m_pCamera.expired() ||
        true == m_pLockOn0.expired() ||
        true == m_pLockOn1.expired() ||
        true == m_pTarget.expired())
        return;

    if (false == m_bIsRender)
        return;

    Update_Target_Pos();
}

void UiTargetLockOn::Set_Target(shared_ptr<GameObject> pObj)
{
    if (true == m_pOwner.expired() ||
        true == m_pCamera.expired() ||
        true == m_pLockOn0.expired() ||
        true == m_pLockOn1.expired())
        return;

    if (nullptr != pObj)
    {
        m_pTarget = pObj;
        m_eState =  STATE::UP;
        m_vecScale.x = m_vecScale.y = m_fOriginScale;
        m_pLockOn1.lock()->GetOrAddTransform()->Scaled(m_vecScale);
        m_pLockOn0.lock()->Set_Render(true);
        m_pLockOn1.lock()->Set_Render(true);
        m_pLockOn0.lock()->Set_Tick(true);
        m_pLockOn1.lock()->Set_Tick(true);
    }
    else
    {
        m_pTarget.reset();
        m_pLockOn0.lock()->Set_Render(false);
        m_pLockOn1.lock()->Set_Render(false);
        m_pLockOn0.lock()->Set_Tick(false);
        m_pLockOn1.lock()->Set_Tick(false);
    }
}

void UiTargetLockOn::Check_Render_State()
{
    _float3 cullPos = m_pTarget.lock()->Get_Transform()->Get_State(Transform_State::POS).xyz();
    _float cullRadius = 1.5f;
    Frustum frustum = m_pCamera.lock()->Get_Camera()->Get_Frustum();

    _bool bValue = false;
    if (frustum.Contain_Sphere(cullPos, cullRadius))
        bValue = true;

    if (m_bIsRender != bValue)
    {
        m_bIsRender = bValue;
        m_pLockOn0.lock()->Set_Render(m_bIsRender);
        m_pLockOn1.lock()->Set_Render(m_bIsRender);
        m_pLockOn0.lock()->Set_Tick(m_bIsRender);
        m_pLockOn1.lock()->Set_Tick(m_bIsRender);
    }
}

void UiTargetLockOn::Change_Scale()
{
    if (STATE::UP == m_eState)
    {
        m_vecScale.x = m_vecScale.y += m_fSpeed * fDT;

        if (m_vecScale.x > m_fMaxScale)
        {
            m_eState = STATE::DOWN;
        }
    }
    else if (STATE::DOWN == m_eState)
    {
        m_vecScale.x = m_vecScale.y -= m_fSpeed * fDT;

        if (m_vecScale.x < m_fOriginScale)
        {
            m_eState = STATE::UP;
        }
    }

    m_pLockOn1.lock()->GetOrAddTransform()->Scaled(m_vecScale);
}

void UiTargetLockOn::Update_Target_Pos()
{
    _float4 vecPos = m_pTarget.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.y += 1.f;
    m_pLockOn0.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[1] = vecPos;
    m_pLockOn1.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[1] = vecPos;
    
    _float4x4 matView = m_pCamera.lock()->Get_Camera()->Get_ViewMat();
    _float4x4 matProj = m_pCamera.lock()->Get_Camera()->Get_ProjMat();

    m_pLockOn0.lock()->Get_MeshRenderer()->Get_RenderParamDesc().matParams[0] = matView;
    m_pLockOn0.lock()->Get_MeshRenderer()->Get_RenderParamDesc().matParams[1] = matProj;

    m_pLockOn1.lock()->Get_MeshRenderer()->Get_RenderParamDesc().matParams[0] = matView;
    m_pLockOn1.lock()->Get_MeshRenderer()->Get_RenderParamDesc().matParams[1] = matProj;
}

void UiTargetLockOn::Check_Target()
{
    if (0 >= m_pTarget.lock()->Get_CurHp())
    {
        m_pTarget.reset();
        m_pLockOn0.lock()->Set_Render(false);
        m_pLockOn1.lock()->Set_Render(false);
        m_pLockOn0.lock()->Set_Tick(false);
        m_pLockOn1.lock()->Set_Tick(false);
    }
}
