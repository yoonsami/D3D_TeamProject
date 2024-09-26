#include "pch.h"
#include "UiMonsterHp.h" 

#include "Camera.h"
#include "MeshRenderer.h"

UiMonsterHp::UiMonsterHp(_bool bIsPosChange)
    : m_bIsPosChange(bIsPosChange)
{
}

HRESULT UiMonsterHp::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    auto pScene = CUR_SCENE;
    vector<weak_ptr<GameObject>> addedObj;
    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_MonsterHp.dat", addedObj);

    _uint iSize = IDX(addedObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        if (true == addedObj[i].expired())
            continue;

        wstring strName = addedObj[i].lock()->Get_Name();
        if (L"UI_MonsterHp0" == strName)
            m_pFrontHp = addedObj[i];
        else if(L"UI_MonsterHp1" == strName)
            m_pBackHp = addedObj[i];
        else if(L"UI_MonsterHp2" == strName)
            m_pBgHp = addedObj[i];
    }

    m_pCamera = pScene->Get_Camera(L"Default");

    /*if(false == m_pFrontHp.expired())
        m_pFrontHp.lock()->Set_Render(false);

    if (false == m_pBackHp.expired())
        m_pBackHp.lock()->Set_Render(false);

    if (false == m_pBgHp.expired())
        m_pBgHp.lock()->Set_Render(false);*/

    return S_OK;
}

void UiMonsterHp::Tick()
{
	if (true == m_pOwner.expired()      ||
        true == m_pFrontHp.expired()    ||
        true == m_pBackHp.expired()     ||
        true == m_pCamera.expired()     ||
        true == m_pBgHp.expired())
		return;

    Check_Target();
    Check_Render_State();
    if (false == m_bIsRender)
        return;

    Change_Hp_Ratio();
    Change_Hp_Slow();
    Update_Target_Pos();

    /*if (KEYTAP(KEY_TYPE::F5))
        m_pTarget.lock()->Get_Hurt(10.f);*/
}

//void UiMonsterHp::Set_Target(shared_ptr<GameObject> pObj)
//{
//    if (true == m_pOwner.expired()      ||
//        true == m_pFrontHp.expired()    ||
//        true == m_pBackHp.expired()     ||
//        true == m_pCamera.expired()     ||
//        true == m_pBgHp.expired())
//        return;
//
//    if (nullptr == pObj)
//    {
//        m_pTarget.reset();
//        m_pFrontHp.lock()->Set_Render(false);
//        m_pBackHp.lock()->Set_Render(false);
//        m_pBgHp.lock()->Set_Render(false);
//    }
//    else
//    {
//        m_pTarget = pObj;
//        _float HpRatio = m_pTarget.lock()->Get_HpRatio();
//        m_pFrontHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = HpRatio;
//        m_pBackHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = HpRatio;
//        m_bIsWork = false;
//        m_fRatio = HpRatio;
//        m_pFrontHp.lock()->Set_Render(true);
//        m_pBackHp.lock()->Set_Render(true);
//        m_pBgHp.lock()->Set_Render(true);
//    }
//}

void UiMonsterHp::Check_Render_State()
{
    _float3 cullPos = m_pOwner.lock()->Get_Transform()->Get_State(Transform_State::POS).xyz();
    _float cullRadius = 1.5f;
    Frustum frustum = m_pCamera.lock()->Get_Camera()->Get_Frustum();

    _bool bValue = false;
    if (frustum.Contain_Sphere(cullPos, cullRadius))
        bValue = true;

    if (m_bIsRender != bValue)
    {
        m_bIsRender = bValue;
        m_pFrontHp.lock()->Set_Render(m_bIsRender);
        m_pBackHp.lock()->Set_Render(m_bIsRender);
        m_pBgHp.lock()->Set_Render(m_bIsRender);
    }
}

void UiMonsterHp::Check_Target()
{
    if (0.f >= m_pOwner.lock()->Get_CurHp())
    {
        auto pScene = CUR_SCENE;
        //pScene->Remove_GameObject(m_pOwner.lock());
        //m_pFrontHp.lock()->Set_Render(false);
        //m_pBackHp.lock()->Set_Render(false);
        //m_pBgHp.lock()->Set_Render(false);
        EVENTMGR.Delete_Object(m_pFrontHp.lock());
        EVENTMGR.Delete_Object(m_pBackHp.lock());
        EVENTMGR.Delete_Object(m_pBgHp.lock());
    }
}

void UiMonsterHp::Change_Hp_Ratio()
{
    _float HpRatio = m_pOwner.lock()->Get_HpRatio();
    m_pFrontHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = HpRatio;

    m_fRatio = m_pBackHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0];

    if (HpRatio != m_fRatio && false == m_bIsWork)
    {
        m_bIsWork = true;
        m_fSpeed = m_fRatio - HpRatio;
        m_fTargetRatio = HpRatio;
    }
}

void UiMonsterHp::Change_Hp_Slow()
{
    if (false == m_bIsWork)
        return;

    m_fRatio -= m_fSpeed * fDT;
    if (m_fRatio < m_fTargetRatio)
    {
        m_fRatio = m_fTargetRatio;
        m_pBackHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = m_fTargetRatio;
        m_bIsWork = false;
    }
    else
    {
        m_pBackHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = m_fRatio;
    }
}

void UiMonsterHp::Update_Target_Pos()
{
    if (false == m_bIsRender)
        return;

    _float4 vecPos = {};
    if (false == m_bIsPosChange)
        vecPos = m_pOwner.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    else
        vecPos = m_vecChangePos;

    vecPos.y += 2.f;
    m_pFrontHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[1] = vecPos;
    m_pBackHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[1] = vecPos;
    m_pBgHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[1] = vecPos;

    _float4x4 matWorld = m_pCamera.lock()->GetOrAddTransform()->Get_WorldMatrix();
    _float4x4 matView = m_pCamera.lock()->Get_Camera()->Get_ViewMat();
    _float4x4 matProj = m_pCamera.lock()->Get_Camera()->Get_ProjMat();

    m_pFrontHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().matParams[0] = matView;
    m_pFrontHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().matParams[1] = matProj;

    m_pBackHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().matParams[0] = matView;
    m_pBackHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().matParams[1] = matProj;

    m_pBgHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().matParams[0] = matView;
    m_pBgHp.lock()->Get_MeshRenderer()->Get_RenderParamDesc().matParams[1] = matProj;
}
