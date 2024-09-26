#include "pch.h"
#include "UiLoadingScript.h"

#include "MeshRenderer.h"
#include "Material.h"

_uint UiLoadingScript::iTextureIndex = 0;

UiLoadingScript::UiLoadingScript(_bool bIsEven)
    : m_bIsEven(bIsEven)
{
}

HRESULT UiLoadingScript::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    m_fMaxIdleTime  = 3.f;
    m_fOnOffTime    = 2.f;
    m_fCheckTime    = 0.f;

    m_iMaxIndex = 40;
    m_strTextureName = L"UI_Loading";

    wstring TextureName = m_strTextureName + to_wstring(iTextureIndex++);
    if(true == m_bIsEven)
    {
        m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(TextureName), TextureMapType::DIFFUSE);
    }
    else
    {
        m_eType = CHANGE_TYPE::OFF_IDLE;
        m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(TextureName), TextureMapType::DIFFUSE);
    }

    return S_OK;
}

void UiLoadingScript::Tick()
{
	if (m_pOwner.expired())
		return;

    m_fCheckTime += fDT;

    if(true == m_bIsEven)
    {
        switch (m_eType)
        {
        case CHANGE_TYPE::ON_IDLE:
            On_Idle();
            break;

        case CHANGE_TYPE::OFF:
            Off();
            break;

        case CHANGE_TYPE::CHANGE:
            Change();
            break;

        case CHANGE_TYPE::OFF_IDLE:
            Off_Idle();
            break;

        case CHANGE_TYPE::ON:
            On();
            break;
        }
    }
}

void UiLoadingScript::Change_Texture()
{
    ++iTextureIndex;
    if (m_iMaxIndex <= iTextureIndex)
        iTextureIndex = 0;

    wstring TextureName = m_strTextureName + to_wstring(iTextureIndex);
    m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(TextureName), TextureMapType::DIFFUSE);
}

void UiLoadingScript::On_Idle()
{
    if (m_fMaxIdleTime < m_fCheckTime)
    {
        m_fCheckTime = 0.f;
        m_eType = CHANGE_TYPE::OFF;
    }
}

void UiLoadingScript::Off()
{
    if (m_fOnOffTime < m_fCheckTime)
    {
        m_fCheckTime = 0.f;
        m_eType = CHANGE_TYPE::CHANGE;
        m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;

        return;
    }

    _float fRatio = 1.f - (m_fCheckTime / m_fOnOffTime);

    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = fRatio;
}

void UiLoadingScript::Change()
{
    m_fCheckTime = 0.f;
    m_eType = CHANGE_TYPE::OFF_IDLE;

    ++iTextureIndex;
    if (m_iMaxIndex <= iTextureIndex)
        iTextureIndex = 0;

    wstring TextureName = m_strTextureName + to_wstring(iTextureIndex);
    if (true == m_bIsEven)
        m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(TextureName), TextureMapType::DIFFUSE);
    else
        m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(TextureName), TextureMapType::DIFFUSE);
}

void UiLoadingScript::Off_Idle()
{
    if (m_fMaxIdleTime < m_fCheckTime)
    {
        m_fCheckTime = 0.f;
        m_eType = CHANGE_TYPE::ON;
    }
}

void UiLoadingScript::On()
{
    if (m_fOnOffTime < m_fCheckTime)
    {
        m_fCheckTime = 0.f;
        m_eType = CHANGE_TYPE::ON_IDLE;
        m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;

        CUR_SCENE->Get_GameObject(L"UI_Loading_Odd")->Get_Script<UiLoadingScript>()->Change_Texture();

        return;
    }

    _float fRatio = m_fCheckTime / m_fOnOffTime;

    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = fRatio;
}
