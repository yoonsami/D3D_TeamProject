#include "pch.h"
#include "UIShop.h"

#include "BaseUI.h"
#include "MeshRenderer.h"
#include "FontRenderer.h"
#include "UiUseItemSlot.h"
#include "UiCostumeController.h"

UIShop::UIShop()
{
}

HRESULT UIShop::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_iMoney = 1000000;

    m_vecIsBuy.resize(12);
    m_vecPrice.resize(12);
    for (_uint i = 0; i < IDX(m_vecPrice.size()); ++i)
        m_vecPrice[i] = 200000;

    return S_OK;
}

void UIShop::Tick()
{
	if (m_pOwner.expired())
		return;

}

void UIShop::Create_Shop()
{
    if (true == m_bIsCreated)
        return;

    g_bIsCanRotation = false;

    m_bIsCreated = true;
    auto pScene = CUR_SCENE;
    
    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Shop.dat", m_addedObj);

    _uint iSize = IDX(m_addedObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_addedObj[i];
        if (true == pObj.expired())
            continue;

        wstring strName = pObj.lock()->Get_Name();
        if (L"UI_Shop_Exit" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Remove_Shop();
                });
        }
        else if (L"UI_Shop_Item_0" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Click_Item(0);
                });
        }
        else if (L"UI_Shop_Item_1" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Click_Item(1);
                });
        }
        else if (L"UI_Shop_Item_2" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Click_Item(2);
                });
        }
        else if (L"UI_Shop_Item_3" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Click_Item(3);
                });
        }
        else if (L"UI_Shop_Item_4" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Click_Item(4);
                });
        }
        else if (L"UI_Shop_Item_5" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Click_Item(5);
                });
        }
        else if (L"UI_Shop_Item_6" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Click_Item(6);
                });
        }
        else if (L"UI_Shop_Item_7" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Click_Item(7);
                });
        }
        else if (L"UI_Shop_Item_8" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Click_Item(8);
                });
        }
        else if (L"UI_Shop_Item_9" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Click_Item(9);
                });
        }
        else if (L"UI_Shop_Item_10" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Click_Item(10);
                });
        }
        else if (L"UI_Shop_Item_11" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Click_Item(11);
                });
        }
        else if (L"UI_Shop_Money_Value" == strName)
        {
            m_pMoneyValue = pObj;
            m_pMoneyValue.lock()->Get_FontRenderer()->Get_Text() = to_wstring(m_iMoney);
        }

        for (_uint j = 0; j < 12; ++j)
        {
            if (false == m_vecIsBuy[j])
                continue;

            if (2 == j)
                continue;

            wstring strItemName = L"UI_Shop_Item_" + to_wstring(j);
            wstring strPrice = strItemName + L"_Price";
            if (true == pObj.expired())
                continue;

            if (strItemName == pObj.lock()->Get_Name())
            {
                pObj.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = 0.f;
            }
            else if (strPrice == pObj.lock()->Get_Name())
            {
                pObj.lock()->Get_FontRenderer()->Get_Text() = L"구매 완료";
            }
        }
    }
}

void UIShop::Remove_Shop()
{
    if (false == m_bIsCreated)
        return;

    g_bIsCanRotation = true;

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

    Remove_Buy_Ui();
}

void UIShop::Click_Item(_uint iIndex)
{
    if (12 <= iIndex || true == m_bIsCreatedBuyUi)
        return;

    if (2 != iIndex && true == m_vecIsBuy[iIndex])
        return;

    if (m_iMoney < m_vecPrice[iIndex])
        return;

    m_iInteractionItemIndex = iIndex;
    
    Create_Buy_Ui();
}

void UIShop::Click_Buy_Yes()
{
    Remove_Buy_Ui();

    // 2번이면 포션 증가
    // 9~11 : 코스튬 on -> UI_Costume_Controller -> Change_Costume_Have
    if (2 == m_iInteractionItemIndex)
        CUR_SCENE->Get_UI(L"UI_UseItem_Slot_Controller")->Get_Script<UiUseItemSlot>()->Add_Potion(10);
    else if (9 <= m_iInteractionItemIndex)
        CUR_SCENE->Get_UI(L"UI_Costume_Controller")->Get_Script<UiCostumeController>()->Change_Costume_Have(m_iInteractionItemIndex - 6);

    m_vecIsBuy[m_iInteractionItemIndex] = true;

    m_iMoney -= m_vecPrice[m_iInteractionItemIndex];

    m_pMoneyValue.lock()->Get_FontRenderer()->Get_Text() = to_wstring(m_iMoney);

    // 2번 제외하곤 다 0으로 줄이고 텍스트 바꾸기
    if (2 == m_iInteractionItemIndex)
        return;

    wstring strName = L"UI_Shop_Item_" + to_wstring(m_iInteractionItemIndex);
    wstring strPrice = strName + L"_Price";

    _uint iSize = IDX(m_addedObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_addedObj[i];
        if (true == pObj.expired())
            continue;

        if (strName == pObj.lock()->Get_Name())
        {
            pObj.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = 0.f;
        }
        else if (strPrice == pObj.lock()->Get_Name())
        {
            pObj.lock()->Get_FontRenderer()->Get_Text() = L"구매 완료";
        }
    }

}

void UIShop::Click_Buy_No()
{
    Remove_Buy_Ui();
}

void UIShop::Create_Buy_Ui()
{
    if (true == m_bIsCreatedBuyUi)
        return;

    m_bIsCreatedBuyUi = true;
    auto pScene = CUR_SCENE;

    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Shop_Buy.dat", m_addedBuyUi);

    _uint iSize = IDX(m_addedBuyUi.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_addedBuyUi[i];
        if (true == pObj.expired())
            continue;

        wstring strName = pObj.lock()->Get_Name();
        if (L"UI_Shop_Yes" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Click_Buy_Yes();
                });
        }
        if (L"UI_Shop_No" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Click_Buy_No();
                });
        }
    }
}

void UIShop::Remove_Buy_Ui()
{
    if (false == m_bIsCreatedBuyUi)
        return;

    g_bIsCanRotation = true;

    m_bIsCreatedBuyUi = false;

    auto& pEventMgr = EVENTMGR;

    for (_uint i = 0; i < IDX(m_addedBuyUi.size()); ++i)
    {
        auto& pObj = m_addedBuyUi[i];
        if (false == pObj.expired())
        {
            pEventMgr.Delete_Object(pObj.lock());
            pObj.reset();
        }
    }

    m_addedBuyUi.clear();
}