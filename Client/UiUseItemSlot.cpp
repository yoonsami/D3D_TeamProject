#include "pch.h"
#include "UiUseItemSlot.h" 

#include "BaseUI.h"
#include "MeshRenderer.h"
#include "FontRenderer.h"

UiUseItemSlot::UiUseItemSlot()
{
}

HRESULT UiUseItemSlot::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;

    auto pScene = CUR_SCENE;

    m_pUseItem_Slot = pScene->Get_UI(L"UI_UseItem_Slot");
    m_pUseItem_Count = pScene->Get_UI(L"UI_UseItem_Count");

    m_pUseItem_Slot.lock()->Get_Button()->AddOnClickedEvent([this]()
        {
            this->Click_Slot();
        });

    m_iCount = 0;

    m_fMaxTime = 8.f;
    m_fCheckTime = m_fMaxTime + 1.f;

    return S_OK;
}

void UiUseItemSlot::Tick()
{
	if (m_pOwner.expired())
		return;

    if (KEYTAP(KEY_TYPE::R))
        Use_Item();

    if (false == m_bIsCanUse)
        Check_Cooltime();
}

void UiUseItemSlot::Click_Slot()
{
    Create_Inven();
}

void UiUseItemSlot::Create_Inven()
{
    if (true == m_bIsCreated)
        return;

    m_bIsCreated = true;
    auto pScene = CUR_SCENE;
    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_UseItem_Inven.dat", m_addedObj);

    _uint iSize = IDX(m_addedObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_addedObj[i];
        if (true == pObj.expired())
            continue;

        wstring strName = pObj.lock()->Get_Name();
        if (L"UI_UseItem_Bg" == strName)
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]() 
                {
                    this->Remove_Inven();
                });
        if (L"UI_UseItem_Inven0" == strName)
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]() 
                {
                    this->Click_Inven(0);
                });
        else if (L"UI_UseItem_Inven0_Count" == strName)
        {
            _uint iTemp = m_iCount;
            if (0 > iTemp)
                iTemp = 0;
            pObj.lock()->Get_FontRenderer()->Get_Text() = to_wstring(m_iCount);
        }
    }
}

void UiUseItemSlot::Check_Cooltime()
{
    m_fCheckTime += fDT;
    if (m_fMaxTime < m_fCheckTime)
    {
        m_bIsCanUse = true;
    }

    m_pUseItem_Slot.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = m_fCheckTime / m_fMaxTime;
}

void UiUseItemSlot::Click_Inven(_uint iIndex)
{
    if (true == m_pUseItem_Slot.expired() ||
        true == m_pUseItem_Count.expired())
        return;

    if (0 == m_iCount)
        return;

    m_bIsSet = true;
    m_pUseItem_Slot.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(L"UI_UseItem_Slot_Potion"), TextureMapType::DIFFUSE);
    if(0 < m_iCount)
        m_pUseItem_Count.lock()->Get_FontRenderer()->Get_Text() = to_wstring(m_iCount);

}

void UiUseItemSlot::Remove_Inven()
{
    if (false == m_bIsCreated)
        return;

    m_bIsCreated = false;

    auto& pEventMgr = EVENTMGR;

    for (_uint i = 0; i < IDX(m_addedObj.size()); ++i)
    {
        auto& pObj = m_addedObj[i];
        if (false == pObj.expired())
        {
            pEventMgr.Delete_Object(pObj.lock());
            pObj.reset();
        }
    }

    m_addedObj.clear();
}

void UiUseItemSlot::Use_Item()
{
    if (true == m_pUseItem_Slot.expired() ||
        true == m_pUseItem_Count.expired())
        return;

    if (false == m_bIsSet || false == m_bIsCanUse)
        return;

    // 플레이어 체력 증가


    m_bIsCanUse = false;
    m_fCheckTime = 0.f;

    --m_iCount;
    if (0 == m_iCount)
    {
        m_bIsSet = false;
        m_pUseItem_Slot.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(L"UI_UseItem_Slot_None"), TextureMapType::DIFFUSE);
        m_pUseItem_Count.lock()->Get_FontRenderer()->Get_Text() = L"";
    }
    else
        m_pUseItem_Count.lock()->Get_FontRenderer()->Get_Text() = to_wstring(m_iCount);

}

void UiUseItemSlot::Add_Potion(_uint iCount)
{
    m_iCount += iCount;

    if(true == m_bIsSet)
        m_pUseItem_Count.lock()->Get_FontRenderer()->Get_Text() = to_wstring(m_iCount);
}
