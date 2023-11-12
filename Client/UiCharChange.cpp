#include "pch.h"
#include "UiCharChange.h"

#include "MeshRenderer.h"
#include "Material.h"

UiCharChange::UiCharChange()
{
}

HRESULT UiCharChange::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    m_vecDesc.resize(3);
    m_vecDesc[0].fCoolTime = 10.f;
    m_vecDesc[1].fCoolTime = 10.f;
    m_vecDesc[2].fCoolTime = 10.f;

    /*m_vecDesc[0].fAccTime = m_vecDesc[0].fCoolTime;
    m_vecDesc[1].fAccTime = m_vecDesc[1].fCoolTime;
    m_vecDesc[2].fAccTime = m_vecDesc[2].fCoolTime;*/

    m_pObj.resize(3);
    m_pObj[0] = CUR_SCENE->Get_GameObject(L"UI_Char_Change0");
    m_pObj[1] = CUR_SCENE->Get_GameObject(L"UI_Char_Change1");
    m_pObj[2] = CUR_SCENE->Get_GameObject(L"UI_Char_Change2");
    
    m_pElement.resize(3);
    m_pElement[0] = CUR_SCENE->Get_GameObject(L"UI_Char_Change_Element0");
    m_pElement[1] = CUR_SCENE->Get_GameObject(L"UI_Char_Change_Element1");
    m_pElement[2] = CUR_SCENE->Get_GameObject(L"UI_Char_Change_Element2");

    m_TextureKey.resize(IDX(HERO::MAX));
    m_TextureKey[0] = L"Card_Ace";
    m_TextureKey[1] = L"Card_Kyle";
    m_TextureKey[2] = L"Card_Yeopo";
    m_TextureKey[3] = L"Card_Dellons";



    return S_OK;
}

void UiCharChange::Tick()
{
	if (m_pOwner.expired())
		return;

    Check_Change_Cool();

    Set_Param_Value();
}

void UiCharChange::Set_Hero(_uint iIndex, HERO eHero)
{
    if (m_pOwner.expired() || 2 < iIndex)
        return;
    
    if (true == m_pObj[iIndex].expired() || true == m_pElement[iIndex].expired())
        return;

    // UI�� Index ��ġ�� ������Ʈ�� ���� ���� ���ֱ�
    if (HERO::MAX == eHero)
    {
        m_pObj[iIndex].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(L"Card_None"), TextureMapType::DIFFUSE);
        m_pElement[iIndex].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = 0.f;
    }
    else
    {
        m_pObj[iIndex].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(m_TextureKey[IDX(eHero)]), TextureMapType::DIFFUSE);
        
        // ������ ������ �Ӽ����� �����ͼ� �����ؾ���
        m_pElement[iIndex].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = 1.f;
    }
}

_bool UiCharChange::IsChangePossible(_uint iIndex)
{
    if (m_pOwner.expired() || 2 < iIndex)
        return false;

    auto& Desc = m_vecDesc[iIndex];
    if(HERO::MAX == Desc.eHero)
        return false;

    if (Desc.fCoolTime < Desc.fAccTime)
    {
        // Hero Change Script ȣ��
        // Desc.eHero�� ���� ���� ���� ȣ��
        
        return true;
    }

    return false;
}

void UiCharChange::Check_Change_Cool()
{
    _uint iSize = IDX(m_vecDesc.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& Desc = m_vecDesc[i];
        if (HERO::MAX == Desc.eHero)
        {
            Desc.fAccTime = Desc.fCoolTime;
        }
        else
        {
            if (Desc.fCoolTime > Desc.fAccTime)
                Desc.fAccTime += fDT;
        }
    }
}

void UiCharChange::Set_Param_Value()
{
    _uint iSize = IDX(m_vecDesc.size());
    for (_uint i = 0; i < iSize; ++i)
        if (true == m_pObj[i].expired())
            return;

    for (_uint i = 0; i < iSize; ++i)
    {
        m_pObj[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = m_vecDesc[i].fAccTime / m_vecDesc[i].fCoolTime;
        if (1.f <= m_pObj[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0])
            m_pObj[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = 1.f;
    }
}
