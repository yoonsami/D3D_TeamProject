#include "pch.h"
#include "UiMarkNpc.h" 

#include "Camera.h"
#include "MeshRenderer.h"

UiMarkNpc::UiMarkNpc(NPCTYPE eType)
    : m_eType(eType)
{
}

HRESULT UiMarkNpc::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    auto pScene = CUR_SCENE;
    vector<weak_ptr<GameObject>> addedObj;
    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Mark_Npc.dat", addedObj);

    m_pMark = addedObj[0];
    m_pMark.lock()->Set_Render(false);

    m_pCamera = pScene->Get_Camera(L"Default");
    m_bSetOn = true;

    wstring strTextureTag;
    switch (m_eType)
    {
    case NPCTYPE::QUEST:
        strTextureTag = L"UI_Mark_Quest";
        break;
    case NPCTYPE::GACHA:
        strTextureTag = L"UI_Mark_Gacha";
        break;
    case NPCTYPE::POTION:
        strTextureTag = L"UI_Mark_Shop";
        break;
    case NPCTYPE::MAX:
        return E_FAIL;
    }

    if (false == m_pMark.expired())
        m_pMark.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(strTextureTag), TextureMapType::DIFFUSE);


    return S_OK;
}

void UiMarkNpc::Tick()
{
    if (true == m_pOwner.expired() ||
        true == m_pCamera.expired() ||
        true == m_pMark.expired())
        return;

    if (false == m_bSetOn)
        return;

    Check_Distance();
    if (false == m_bIsRender)
        return;

    Check_In_Screen();
}

void UiMarkNpc::Final_Tick()
{
    if (true == m_pOwner.expired() ||
        true == m_pCamera.expired() ||
        true == m_pMark.expired())
        return;

    if (false == m_bSetOn)
        return;

    if (false == m_bIsRender)
        return;

    Update_Pos();
}

void UiMarkNpc::Delete_Mark()
{
    if (false == m_pMark.expired() && false == m_pOwner.expired())
        EVENTMGR.Delete_Object(m_pMark.lock());
}

void UiMarkNpc::Change_Render(_bool bValue)
{
    if (false == m_pMark.expired() && false == m_pOwner.expired())
    {
        m_pMark.lock()->Set_Render(bValue);
        m_bIsRender = bValue;
    }
}

void UiMarkNpc::Change_Set_On(_bool bValue)
{
    m_bSetOn = bValue;
    Change_Render(m_bSetOn);
}

void UiMarkNpc::Check_Distance()
{
    _float3 pos = Get_Transform()->Get_State(Transform_State::POS).xyz();
    auto pPlayer = GET_PLAYER;

    if (pPlayer)
    {
        _float lengthSQ = (pPlayer->Get_Transform()->Get_State(Transform_State::POS).xyz() - pos).LengthSquared();

        if (lengthSQ > 9.f * 9.f)
            Change_Render(false);
        else
            Change_Render(true);
    }

}

void UiMarkNpc::Check_In_Screen()
{
    _float3 cullPos = m_pOwner.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).xyz();
    _float cullRadius = 1.5f;
    Frustum frustum = m_pCamera.lock()->Get_Camera()->Get_Frustum();

    if (frustum.Contain_Sphere(cullPos, cullRadius))
        m_pMark.lock()->Set_Render(true);
    else
        m_pMark.lock()->Set_Render(false);
}

void UiMarkNpc::Update_Pos()
{
    _float4 vecPos = m_pOwner.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.y += 2.5f;
    m_pMark.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[1] = vecPos;

    _float4x4 matView = m_pCamera.lock()->Get_Camera()->Get_ViewMat();
    m_pMark.lock()->Get_MeshRenderer()->Get_RenderParamDesc().matParams[0] = matView;

    _float4x4 matProj = m_pCamera.lock()->Get_Camera()->Get_ProjMat();
    m_pMark.lock()->Get_MeshRenderer()->Get_RenderParamDesc().matParams[1] = matProj;
}
