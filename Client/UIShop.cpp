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

    _uint iIndex = 0;
    m_vecName.resize(38);
    m_vecName[iIndex++] = L"���ָӴ�";
    m_vecName[iIndex++] = L"ü�� ���� 10������";
    m_vecName[iIndex++] = L"��� ����ġ ��ũ��";
    m_vecName[iIndex++] = L"���� ����ġ ī��";
    m_vecName[iIndex++] = L"������� ���";
    m_vecName[iIndex++] = L"�� 2�� ��������";
    m_vecName[iIndex++] = L"�� 3�� ��������";
    m_vecName[iIndex++] = L"�� 4�� ��������";
    m_vecName[iIndex++] = L"�� 5�� ��������";
    m_vecName[iIndex++] = L"�� 6�� ��������";
    m_vecName[iIndex++] = L"���� 2�� ��������";
    m_vecName[iIndex++] = L"���� 3�� ��������";
    m_vecName[iIndex++] = L"���� 4�� ��������";
    m_vecName[iIndex++] = L"���� 5�� ��������";
    m_vecName[iIndex++] = L"���� 6�� ��������";
    m_vecName[iIndex++] = L"���� ���� ��Ʈ";
    m_vecName[iIndex++] = L"�������� ��Ĳ�� ��Ʈ";
    m_vecName[iIndex++] = L"�ر��屳 ���� ��Ʈ";
    m_vecName[iIndex++] = L"�������� 2�� ��������";
    m_vecName[iIndex++] = L"�������� 3�� ��������";
    m_vecName[iIndex++] = L"�������� 4�� ��������";
    m_vecName[iIndex++] = L"�������� 5�� ��������";
    m_vecName[iIndex++] = L"�������� 6�� ��������";
    m_vecName[iIndex++] = L"���� 2�� ��������";
    m_vecName[iIndex++] = L"���� 3�� ��������";
    m_vecName[iIndex++] = L"���� 4�� ��������";
    m_vecName[iIndex++] = L"���� 5�� ��������";
    m_vecName[iIndex++] = L"���� 6�� ��������";
    m_vecName[iIndex++] = L"����� 2�� ��������";
    m_vecName[iIndex++] = L"����� 3�� ��������";
    m_vecName[iIndex++] = L"����� 4�� ��������";
    m_vecName[iIndex++] = L"����� 5�� ��������";
    m_vecName[iIndex++] = L"����� 6�� ��������";
    m_vecName[iIndex++] = L"���� 6�� ��������";
    m_vecName[iIndex++] = L"���� 6�� ��������";
    m_vecName[iIndex++] = L"���� 6�� ��������";
    m_vecName[iIndex++] = L"�尩 6�� ��������";
    m_vecName[iIndex++] = L"�Ź� 6�� ��������";

    m_vecIsBuy.resize(38);
    m_vecPrice.resize(38);
    for (_uint i = 0; i < IDX(m_vecPrice.size()); ++i)
        m_vecPrice[i] = 200000;

    return S_OK;
}

void UIShop::Tick()
{
    if (m_pOwner.expired())
        return;

    if (false == m_bIsCreated)
        return;

    Check_Scroll();


}

void UIShop::Create_Shop()
{
    if (true == m_bIsCreated)
        return;

    g_bIsCanRotation = false;

    m_bIsCreated = true;
    auto pScene = CUR_SCENE;

    // bg�� item���� �и�. ���� �����ϸ� �̵��� �����ۿ��� �ε��� ��ü ���ͷ� ����� ����
    // 

    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Shop_Bg.dat", m_addedObj);

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
        else if (L"UI_Shop_Money_Value" == strName)
        {
            m_pMoneyValue = pObj;
            m_pMoneyValue.lock()->Get_FontRenderer()->Get_Text() = to_wstring(m_iMoney);
        }
        else if (L"UI_Shop_Scroll_Button" == strName)
        {
            m_pScroll = pObj;
            pObj.lock()->Get_Button()->Set_Type(false);
            pObj.lock()->Get_Button()->AddOnClickedEvent([this, pObj]()
                {
                    POINT ptMouse = INPUT.GetMousePosToPoint();
                    _float4 vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                    vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
                    if (250.f > vecPos.x)
                        vecPos.x = 250.f;
                    if (850.f < vecPos.x)
                        vecPos.x = 850.f;
                    pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
                });
        }
    }

    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Shop_Test.dat", m_ItemObj);

    iSize = IDX(m_ItemObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_ItemObj[i];
        if (true == pObj.expired())
            continue;

        if (i < iSize / 2)
        {
            if (i == iSize / 2 - 1)
                m_fMaxPosX = -700.f + 280.f * i;

            pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(-700.f + 280.f * i, 180.f, 4.8f, 1.f));
        }
        else
            pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(-700.f + 280.f * (i - iSize / 2), -160.f, 4.8f, 1.f));
    
        pObj.lock()->Get_Button()->Get_Desc().ptCenter.x = static_cast<LONG>(-700.f + 280.f * i + g_iWinSizeX / 2);


        pObj.lock()->Get_Button()->AddOnClickedEvent([this, i]()
            {
                this->Click_Item(i);
            });

        

        for (_uint j = 0; j < iSize; ++j)
        {
            if (false == m_vecIsBuy[j])
                continue;
        
            if (1 == j)
                continue;
        
            wstring strItemName = L"UI_Shop_Item_" + to_wstring(j);
            if (true == pObj.expired())
                continue;
        
            if (strItemName == pObj.lock()->Get_Name())
                pObj.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = 0.f;
        }

    }

    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Shop_Item_Name.dat", m_ItemName);

    iSize = IDX(m_ItemName.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_ItemName[i];
        if (true == pObj.expired())
            continue;

        pObj.lock()->Get_FontRenderer()->Get_Text() = m_vecName[i];
        _float4 vecPos = m_ItemObj[i].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
        vecPos.x += static_cast<_float>(m_vecName[i].length()) / 2.f * -25.f;
        vecPos.y += 155.f;
        vecPos.z = 4.7f;
        pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    }

    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Shop_Item_Price.dat", m_ItemPrice);

    iSize = IDX(m_ItemPrice.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_ItemPrice[i];
        if (true == pObj.expired())
            continue;

        pObj.lock()->Get_FontRenderer()->Get_Text() = to_wstring(m_vecPrice[i]);
        _float4 vecPos = m_ItemObj[i].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
        vecPos.x += static_cast<_float>(to_wstring(m_vecPrice[i]).length()) / 2.f * -15.f;
        vecPos.y -= 95.f;
        vecPos.z = 4.7f;
        pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);

        for (_uint j = 0; j < iSize; ++j)
        {
            if (false == m_vecIsBuy[j])
                continue;

            if (1 == j)
                continue;

            wstring strPrice = L"UI_Shop_Item_Price_" + to_wstring(j);
            if (true == pObj.expired())
                continue;

            if (strPrice == pObj.lock()->Get_Name())
                pObj.lock()->Get_FontRenderer()->Get_Text() = L"���� �Ϸ�";
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

    for (_uint i = 0; i < IDX(m_ItemObj.size()); ++i)
    {
        auto& pObj = m_ItemObj[i];
        if (false == pObj.expired())
        {
            pEventMgr.Delete_Object(pObj.lock());
            pObj.reset();
        }
    }
    m_ItemObj.clear();

    for (_uint i = 0; i < IDX(m_ItemName.size()); ++i)
    {
        auto& pObj = m_ItemName[i];
        if (false == pObj.expired())
        {
            pEventMgr.Delete_Object(pObj.lock());
            pObj.reset();
        }
    }
    m_ItemName.clear();

    for (_uint i = 0; i < IDX(m_ItemPrice.size()); ++i)
    {
        auto& pObj = m_ItemPrice[i];
        if (false == pObj.expired())
        {
            pEventMgr.Delete_Object(pObj.lock());
            pObj.reset();
        }
    }
    m_ItemPrice.clear();

    Remove_Buy_Ui();
}

void UIShop::Click_Item(_uint iIndex)
{
    if (IDX(m_vecIsBuy.size()) <= iIndex || true == m_bIsCreatedBuyUi)
        return;

    if (1 != iIndex && true == m_vecIsBuy[iIndex])
        return;

    if (m_iMoney < m_vecPrice[iIndex])
        return;

    m_iInteractionItemIndex = iIndex;

    Create_Buy_Ui();
}

void UIShop::Click_Buy_Yes()
{
    Remove_Buy_Ui();

    // 1���̸� ���� ����
    // 15~17 : �ڽ�Ƭ on -> UI_Costume_Controller -> Change_Costume_Have
    if (1 == m_iInteractionItemIndex)
        CUR_SCENE->Get_UI(L"UI_UseItem_Slot_Controller")->Get_Script<UiUseItemSlot>()->Add_Potion(10);
    else if (15 == m_iInteractionItemIndex ||
             16 == m_iInteractionItemIndex ||
             17 == m_iInteractionItemIndex)
        CUR_SCENE->Get_UI(L"UI_Costume_Controller")->Get_Script<UiCostumeController>()->Change_Costume_Have(m_iInteractionItemIndex - 12);

    m_vecIsBuy[m_iInteractionItemIndex] = true;

    m_iMoney -= m_vecPrice[m_iInteractionItemIndex];

    m_pMoneyValue.lock()->Get_FontRenderer()->Get_Text() = to_wstring(m_iMoney);

    // 1�� �����ϰ� �� 0���� ���̰� �ؽ�Ʈ �ٲٱ�
    if (1 == m_iInteractionItemIndex)
        return;

    wstring strName = L"UI_Shop_Item_" + to_wstring(m_iInteractionItemIndex);
    wstring strPrice = L"UI_Shop_Item_Price_" + to_wstring(m_iInteractionItemIndex);

    _uint iSize = IDX(m_ItemObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_ItemObj[i];
        if (true == pObj.expired())
            continue;

        if (strName == pObj.lock()->Get_Name())
            pObj.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = 0.f;
    }
    
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_ItemPrice[i];
        if (true == pObj.expired())
            continue;

        if (strPrice == pObj.lock()->Get_Name())
            pObj.lock()->Get_FontRenderer()->Get_Text() = L"���� �Ϸ�";
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

void UIShop::Check_Scroll()
{
    if (true == m_pScroll.expired())
        return;

    _float fPosX = m_pScroll.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).x;

    fPosX -= 250.f;
    fPosX *= -6.06666f;

    _uint iSize = IDX(m_ItemObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_ItemObj[i];
        if (true == pObj.expired())
            continue;

        if (i < iSize / 2)
        {
            pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(-700.f + 280.f * i + fPosX, 180.f, 4.8f, 1.f));
            pObj.lock()->Get_Button()->Get_Desc().ptCenter.x = static_cast<LONG>(-700.f + 280.f * i + fPosX + g_iWinSizeX / 2);
        }
        else
        {
            pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(-700.f + 280.f * (i - iSize / 2) + fPosX, -160.f, 4.8f, 1.f));
            pObj.lock()->Get_Button()->Get_Desc().ptCenter.x = static_cast<LONG>(-700.f + 280.f * (i - iSize / 2) + fPosX + g_iWinSizeX / 2);
        }
    
        
    }

    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_ItemName[i];
        if (true == pObj.expired())
            continue;

        _float4 vecPos = m_ItemObj[i].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
        vecPos.x += static_cast<_float>(m_vecName[i].length()) / 2.f * -20.f;
        vecPos.y += 155.f;
        vecPos.z = 4.7f;
        pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    }

    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_ItemPrice[i];
        if (true == pObj.expired())
            continue;

        _float4 vecPos = m_ItemObj[i].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
        vecPos.x += static_cast<_float>(to_wstring(m_vecPrice[i]).length()) / 2.f * -15.f;
        vecPos.y -= 95.f;
        vecPos.z = 4.7f;
        pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    }




}
