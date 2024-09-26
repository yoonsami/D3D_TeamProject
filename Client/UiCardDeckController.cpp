#include "pch.h"
#include "UiCardDeckController.h"

#include "BaseUI.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "FontRenderer.h"
#include "UiCharChange.h"
#include "UiCardDeckSwitch.h"

_bool g_bIsCanRotation = true;

UiCardDeckController::UiCardDeckController()
{
}

HRESULT UiCardDeckController::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;

    m_fMaxTime = 1.f;
    m_fSpeed = 30.f;
    m_fCheckTime = 0.f;
    m_bIsMoveDown = true;


    auto pScene = CUR_SCENE;

    m_pCharChange = pScene->Get_UI(L"UI_Char_Change");

    m_vecCardDeckObj.resize(14);
    m_vecCardDeckObj[0] = pScene->Get_UI(L"UI_Card_Deck_Total_Bg0");
    m_vecCardDeckObj[1] = pScene->Get_UI(L"UI_Card_Deck_Total_Bg1");
    m_vecCardDeckObj[2] = pScene->Get_UI(L"UI_Card_Deck_Inven_Bg");
    m_vecCardDeckObj[3] = pScene->Get_UI(L"UI_Card_Deck0");
    m_vecCardDeckObj[4] = pScene->Get_UI(L"UI_Card_Deck1");
    m_vecCardDeckObj[5] = pScene->Get_UI(L"UI_Card_Deck2");
    m_vecCardDeckObj[6] = pScene->Get_UI(L"UI_Card_Deck_Line0");
    m_vecCardDeckObj[7] = pScene->Get_UI(L"UI_Card_Deck_Line1");
    m_vecCardDeckObj[8] = pScene->Get_UI(L"UI_Card_Deck_Line2");
    m_vecCardDeckObj[9] = pScene->Get_UI(L"UI_Card_Deck_X0");
    m_vecCardDeckObj[10] = pScene->Get_UI(L"UI_Card_Deck_X1");
    m_vecCardDeckObj[11] = pScene->Get_UI(L"UI_Card_Deck_X2");
    //m_vecCardDeckObj[12] = pScene->Get_UI(L"UI_Card_Deck_Element0");
    //m_vecCardDeckObj[13] = pScene->Get_UI(L"UI_Card_Deck_Element1");
    //m_vecCardDeckObj[14] = pScene->Get_UI(L"UI_Card_Deck_Element2");
    m_vecCardDeckObj[12] = pScene->Get_UI(L"UI_Card_Deck_Exit");
    m_vecCardDeckObj[13] = pScene->Get_UI(L"UI_Card_Deck_Font");

    m_vecInvenObj.resize(32);
    m_vecInvenObj[0] = pScene->Get_UI(L"UI_Card_Deck_Inven0");
    m_vecInvenObj[1] = pScene->Get_UI(L"UI_Card_Deck_Inven1");
    m_vecInvenObj[2] = pScene->Get_UI(L"UI_Card_Deck_Inven2");
    m_vecInvenObj[3] = pScene->Get_UI(L"UI_Card_Deck_Inven3");
    m_vecInvenObj[4] = pScene->Get_UI(L"UI_Card_Deck_Inven4");
    m_vecInvenObj[5] = pScene->Get_UI(L"UI_Card_Deck_Inven5");
    m_vecInvenObj[6] = pScene->Get_UI(L"UI_Card_Deck_Inven6");
    m_vecInvenObj[7] = pScene->Get_UI(L"UI_Card_Deck_Inven7");
    m_vecInvenObj[8] = pScene->Get_UI(L"UI_Card_Deck_Inven8");
    m_vecInvenObj[9] = pScene->Get_UI(L"UI_Card_Deck_Inven9");
    m_vecInvenObj[10] = pScene->Get_UI(L"UI_Card_Deck_Inven10");
    m_vecInvenObj[11] = pScene->Get_UI(L"UI_Card_Deck_Inven11");
    m_vecInvenObj[12] = pScene->Get_UI(L"UI_Card_Deck_Inven12");
    m_vecInvenObj[13] = pScene->Get_UI(L"UI_Card_Deck_Inven13");
    m_vecInvenObj[14] = pScene->Get_UI(L"UI_Card_Deck_Inven14");
    m_vecInvenObj[15] = pScene->Get_UI(L"UI_Card_Deck_Inven15");
    m_vecInvenObj[16] = pScene->Get_UI(L"UI_Card_Deck_Inven16");
    m_vecInvenObj[17] = pScene->Get_UI(L"UI_Card_Deck_Inven17");
    m_vecInvenObj[18] = pScene->Get_UI(L"UI_Card_Deck_Inven18");
    m_vecInvenObj[19] = pScene->Get_UI(L"UI_Card_Deck_Inven19");
    m_vecInvenObj[20] = pScene->Get_UI(L"UI_Card_Deck_Inven20");
    m_vecInvenObj[21] = pScene->Get_UI(L"UI_Card_Deck_Inven21");
    m_vecInvenObj[22] = pScene->Get_UI(L"UI_Card_Deck_Inven22");
    m_vecInvenObj[23] = pScene->Get_UI(L"UI_Card_Deck_Inven23");
    m_vecInvenObj[24] = pScene->Get_UI(L"UI_Card_Deck_Inven24");
    m_vecInvenObj[25] = pScene->Get_UI(L"UI_Card_Deck_Inven25");
    m_vecInvenObj[26] = pScene->Get_UI(L"UI_Card_Deck_Inven26");
    m_vecInvenObj[27] = pScene->Get_UI(L"UI_Card_Deck_Inven27");
    m_vecInvenObj[28] = pScene->Get_UI(L"UI_Card_Deck_Inven28");
    m_vecInvenObj[29] = pScene->Get_UI(L"UI_Card_Deck_Inven29");
    m_vecInvenObj[30] = pScene->Get_UI(L"UI_Card_Deck_Inven30");
    m_vecInvenObj[31] = pScene->Get_UI(L"UI_Card_Deck_Inven31");

    m_vecCardDeckBg.resize(32);
    m_vecCardDeckBg[0] = pScene->Get_UI(L"UI_Card_Deck_InvenBg0");
    m_vecCardDeckBg[1] = pScene->Get_UI(L"UI_Card_Deck_InvenBg1");
    m_vecCardDeckBg[2] = pScene->Get_UI(L"UI_Card_Deck_InvenBg2");
    m_vecCardDeckBg[3] = pScene->Get_UI(L"UI_Card_Deck_InvenBg3");
    m_vecCardDeckBg[4] = pScene->Get_UI(L"UI_Card_Deck_InvenBg4");
    m_vecCardDeckBg[5] = pScene->Get_UI(L"UI_Card_Deck_InvenBg5");
    m_vecCardDeckBg[6] = pScene->Get_UI(L"UI_Card_Deck_InvenBg6");
    m_vecCardDeckBg[7] = pScene->Get_UI(L"UI_Card_Deck_InvenBg7");
    m_vecCardDeckBg[8] = pScene->Get_UI(L"UI_Card_Deck_InvenBg8");
    m_vecCardDeckBg[9] = pScene->Get_UI(L"UI_Card_Deck_InvenBg9");
    m_vecCardDeckBg[10] = pScene->Get_UI(L"UI_Card_Deck_InvenBg10");
    m_vecCardDeckBg[11] = pScene->Get_UI(L"UI_Card_Deck_InvenBg11");
    m_vecCardDeckBg[12] = pScene->Get_UI(L"UI_Card_Deck_InvenBg12");
    m_vecCardDeckBg[13] = pScene->Get_UI(L"UI_Card_Deck_InvenBg13");
    m_vecCardDeckBg[14] = pScene->Get_UI(L"UI_Card_Deck_InvenBg14");
    m_vecCardDeckBg[15] = pScene->Get_UI(L"UI_Card_Deck_InvenBg15");
    m_vecCardDeckBg[16] = pScene->Get_UI(L"UI_Card_Deck_InvenBg16");
    m_vecCardDeckBg[17] = pScene->Get_UI(L"UI_Card_Deck_InvenBg17");
    m_vecCardDeckBg[18] = pScene->Get_UI(L"UI_Card_Deck_InvenBg18");
    m_vecCardDeckBg[19] = pScene->Get_UI(L"UI_Card_Deck_InvenBg19");
    m_vecCardDeckBg[20] = pScene->Get_UI(L"UI_Card_Deck_InvenBg20");
    m_vecCardDeckBg[21] = pScene->Get_UI(L"UI_Card_Deck_InvenBg21");
    m_vecCardDeckBg[22] = pScene->Get_UI(L"UI_Card_Deck_InvenBg22");
    m_vecCardDeckBg[23] = pScene->Get_UI(L"UI_Card_Deck_InvenBg23");
    m_vecCardDeckBg[24] = pScene->Get_UI(L"UI_Card_Deck_InvenBg24");
    m_vecCardDeckBg[25] = pScene->Get_UI(L"UI_Card_Deck_InvenBg25");
    m_vecCardDeckBg[26] = pScene->Get_UI(L"UI_Card_Deck_InvenBg26");
    m_vecCardDeckBg[27] = pScene->Get_UI(L"UI_Card_Deck_InvenBg27");
    m_vecCardDeckBg[28] = pScene->Get_UI(L"UI_Card_Deck_InvenBg28");
    m_vecCardDeckBg[29] = pScene->Get_UI(L"UI_Card_Deck_InvenBg29");
    m_vecCardDeckBg[30] = pScene->Get_UI(L"UI_Card_Deck_InvenBg30");
    m_vecCardDeckBg[31] = pScene->Get_UI(L"UI_Card_Deck_InvenBg31");

    m_vecCardDeckElement.resize(32);
    m_vecCardDeckElement[0] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element0");
    m_vecCardDeckElement[1] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element1");
    m_vecCardDeckElement[2] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element2");
    m_vecCardDeckElement[3] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element3");
    m_vecCardDeckElement[4] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element4");
    m_vecCardDeckElement[5] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element5");
    m_vecCardDeckElement[6] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element6");
    m_vecCardDeckElement[7] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element7");
    m_vecCardDeckElement[8] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element8");
    m_vecCardDeckElement[9] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element9");
    m_vecCardDeckElement[10] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element10");
    m_vecCardDeckElement[11] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element11");
    m_vecCardDeckElement[12] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element12");
    m_vecCardDeckElement[13] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element13");
    m_vecCardDeckElement[14] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element14");
    m_vecCardDeckElement[15] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element15");
    m_vecCardDeckElement[16] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element16");
    m_vecCardDeckElement[17] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element17");
    m_vecCardDeckElement[18] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element18");
    m_vecCardDeckElement[19] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element19");
    m_vecCardDeckElement[20] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element20");
    m_vecCardDeckElement[21] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element21");
    m_vecCardDeckElement[22] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element22");
    m_vecCardDeckElement[23] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element23");
    m_vecCardDeckElement[24] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element24");
    m_vecCardDeckElement[25] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element25");
    m_vecCardDeckElement[26] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element26");
    m_vecCardDeckElement[27] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element27");
    m_vecCardDeckElement[28] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element28");
    m_vecCardDeckElement[29] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element29");
    m_vecCardDeckElement[30] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element30");
    m_vecCardDeckElement[31] = pScene->Get_UI(L"UI_Card_Deck_Inven_Element31");

    m_vecFont.resize(6);
    m_vecFont[0] = pScene->Get_UI(L"UI_Card_Deck0_Hero_Info");
    m_vecFont[1] = pScene->Get_UI(L"UI_Card_Deck0_Hero");
    m_vecFont[2] = pScene->Get_UI(L"UI_Card_Deck1_Hero_Info");
    m_vecFont[3] = pScene->Get_UI(L"UI_Card_Deck1_Hero");
    m_vecFont[4] = pScene->Get_UI(L"UI_Card_Deck2_Hero_Info");
    m_vecFont[5] = pScene->Get_UI(L"UI_Card_Deck2_Hero");

    for (_uint i = 0; i < IDX(m_vecInvenObj.size()); ++i)
    {
        auto& pObj = m_vecInvenObj[i];
        if (true == pObj.expired())
            continue;

        pObj.lock()->Get_Button()->AddOnClickedEvent([pObj, this]()
            {
                this->Click_Deck_Inven(pObj.lock()->Get_Name());
            });
    }

    for (_uint i = 3; i < 6; ++i)
    {
        auto& pObj = m_vecCardDeckObj[i];
        if (true == pObj.expired())
            continue;

        pObj.lock()->Get_Button()->AddOnClickedEvent([pObj, this]()
            {
                this->Click_Deck_Select(pObj.lock()->Get_Name());
            });
    }

    for (_uint i = 9; i < 12; ++i)
    {
        auto& pObj = m_vecCardDeckObj[i];
        if (true == pObj.expired())
            continue;

        pObj.lock()->Get_Button()->AddOnClickedEvent([pObj, this]()
            {
                this->Click_Deck_X(pObj.lock()->Get_Name());
            });
    }

    return S_OK;
}

void UiCardDeckController::Tick()
{
    if (true == m_pOwner.expired())
        return;

    Move_Select_Mark();
}

void UiCardDeckController::Set_Render(_bool bValue)
{
    m_bIsRender = bValue;
    if (true == m_bIsRender)
        g_bIsCanRotation = false;
    else
        g_bIsCanRotation = true;

    auto pScene = CUR_SCENE;

    _uint iSize = IDX(m_vecCardDeckObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_vecCardDeckObj[i];
        if (false == pObj.expired())
        {
            pObj.lock()->Set_Tick(bValue);
            pObj.lock()->Set_Render(bValue);
        }
    }

    _uint iUseSize = DATAMGR.Get_Card_Inven_Use_Size();
    iSize = IDX(m_vecInvenObj.size());
    if (iSize < iUseSize)
        iUseSize = iSize;
    for (_uint i = 0; i < iUseSize; ++i)
    {
        auto& pObj = m_vecInvenObj[i];
        if (false == pObj.expired())
        {
            pObj.lock()->Set_Tick(bValue);
            pObj.lock()->Set_Render(bValue);
        }
    }

    for (_uint i = 0; i < iUseSize; ++i)
    {
        auto& pObj = m_vecCardDeckBg[i];
        if (false == pObj.expired())
        {
            pObj.lock()->Set_Tick(bValue);
            pObj.lock()->Set_Render(bValue);
        }
    }

    for (_uint i = 0; i < iUseSize; ++i)
    {
        auto& pObj = m_vecCardDeckElement[i];
        if (false == pObj.expired())
        {
            pObj.lock()->Set_Tick(bValue);
            pObj.lock()->Set_Render(bValue);
        }
    }

    iSize = IDX(m_vecFont.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_vecFont[i];
        if (false == pObj.expired())
        {
            pObj.lock()->Set_Tick(bValue);
            pObj.lock()->Set_Render(bValue);
        }
    }

    if (true == bValue)
        Set_Card();
}

void UiCardDeckController::Click_Deck_Select(wstring strObjName)
{
    if (false == m_bIsRender || false == m_bIsClickSet)
        return;

    m_bIsClickSet = false;
    for (_uint i = 3; i < 6; ++i)
    {
        if (false == m_vecCardDeckObj[i].expired())
        {
            if (m_vecCardDeckObj[i].lock()->Get_Name() == strObjName)
            {
                // 장착 성공 창 띄우기
                Create_Switch_Complete(true);

                // 정보창에서 장착 누른 후 카드 클릭 시 -> 카드 바꿔주고 데이터 셋해야함
                _uint iIndex = i - 3;
                DATAMGR.Set_Cur_Hero(iIndex, m_iSetIndex);
                wstring strKey = DATAMGR.Get_Card_Inven(m_iSetIndex).KeyDeckSelect;
                m_vecCardDeckObj[i].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(strKey), TextureMapType::DIFFUSE);
                
                if (false == m_pCharChange.expired())
                    m_pCharChange.lock()->Get_Script<UiCharChange>()->Set_Hero(iIndex);

                // 9X , 12Ele
                iIndex = i + 6;
                if (true == m_vecCardDeckObj[iIndex].expired())
                    return;
                m_vecCardDeckObj[iIndex].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;

                //iIndex = i + 9;
                //if (true == m_vecCardDeckObj[iIndex].expired())
                //    return;
                //m_vecCardDeckObj[iIndex].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
                //m_vecCardDeckObj[iIndex].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(DATAMGR.Get_Card_Inven_Element_Line(m_iSetIndex)), TextureMapType::DIFFUSE);
            
                Set_Font(i - 3);

                Remove_Select_Mark();
            }
        }
    }
}

void UiCardDeckController::Click_Deck_Inven(wstring strInvenName)
{
    if (false == m_bIsRender)
        return;

    _uint iSize = IDX(m_vecInvenObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        if (true == m_vecInvenObj[i].expired())
            continue;

        if (m_vecInvenObj[i].lock()->Get_Name() == strInvenName)
        {
            // 클릭 한 인벤토리 인덱스의 해당하는 데이터 가져와서 정보창 켜기
            Create_Info(i);

            return;
        }
    }
}

void UiCardDeckController::Click_Deck_X(wstring strObjName)
{
    if (false == m_bIsRender)
        return;

    for (_uint i = 9; i < 12; ++i)
    {
        if (m_vecCardDeckObj[i].lock()->Get_Name() == strObjName)
        {
            if (true == m_vecCardDeckObj[i].expired())
                continue;
            m_vecCardDeckObj[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
            //pObj.lock()->Set_Tick(false);
            //pObj.lock()->Set_Render(false);

            _uint iIndex = i - 6;
            if (true == m_vecCardDeckObj[iIndex].expired())
                continue;
            m_vecCardDeckObj[iIndex].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(L"Card_Deck_Bg_None"), TextureMapType::DIFFUSE);

            //iIndex = i + 3;
            //if (true == m_vecCardDeckObj[iIndex].expired())
            //    continue;
            //m_vecCardDeckObj[iIndex].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
            //pObj.lock()->Set_Tick(false);
            //pObj.lock()->Set_Render(false);

            // 장착 해제 창 띄우기
            Create_Switch_Complete(false);

            // 데이터에서 빼야함
            iIndex = i - 9;
            DATAMGR.Remove_Cur_Hero(iIndex);
            if (false == m_pCharChange.expired())
                m_pCharChange.lock()->Get_Script<UiCharChange>()->Set_Hero(iIndex);

            Remove_Font(i - 9);
        }
    }
}

void UiCardDeckController::Click_Info_Set(_uint iIndex)
{
    m_bIsClickSet = true;
    m_iSetIndex = iIndex;
    Remove_Info();

    Create_Select_Mark();
}

void UiCardDeckController::Remove_Info()
{
    if (false == m_bIsInfoCreate)
        return;

    m_bIsInfoCreate = false;

    auto& pEventMgr = EVENTMGR;

    for (_uint i = 0; i < IDX(m_vecAddedObj.size()); ++i)
    {
        auto& pObj = m_vecAddedObj[i];
        if (false == pObj.expired())
        {
            pEventMgr.Delete_Object(pObj.lock());
            pObj.reset();
        }
    }

    m_vecAddedObj.clear();
}

void UiCardDeckController::Set_Card()
{
    auto& pDataMgr = DATAMGR;
    _uint iUseSize = pDataMgr.Get_Card_Inven_Use_Size();
    _uint iSize = IDX(m_vecInvenObj.size());
    if (iSize < iUseSize)
        iUseSize = iSize;

    if (m_iPreSize != iUseSize)
        m_iPreSize = iUseSize;
    else
        return;

    for (_uint i = 0; i < iUseSize; ++i)
    {
        if (i < iUseSize)
        {
            if (true == m_vecInvenObj[i].expired() || 
                true == m_vecCardDeckElement[i].expired() || 
                true == m_vecCardDeckBg[i].expired())
                continue;

            auto& Data = pDataMgr.Get_Card_Inven(i);
            m_vecInvenObj[i].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(Data.KeyDeckMini), TextureMapType::DIFFUSE);
            m_vecCardDeckElement[i].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(pDataMgr.Get_Card_Inven_Element(i)), TextureMapType::DIFFUSE);

            wstring strTextureKey = L"Card_Deck_Inven_Bg" + to_wstring(Data.Star);
              m_vecCardDeckBg[i].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(strTextureKey), TextureMapType::DIFFUSE);

        }
    }
}

void UiCardDeckController::Create_Switch_Complete(_bool bValue)
{
    wstring strPath = L"";
    if (true == bValue)
        strPath = L"..\\Resources\\UIData\\UI_Card_Deck_Switch_In.dat";
    else
        strPath = L"..\\Resources\\UIData\\UI_Card_Deck_Switch_Out.dat";

    auto pScene = CUR_SCENE;
    vector<weak_ptr<GameObject>> addedObj;
    pScene->Load_UIFile(strPath, addedObj);

    _uint iSize = IDX(addedObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = addedObj[i];
        if (true == pObj.expired())
            continue;

        wstring strName = pObj.lock()->Get_Name();
        _bool bIsFont = false;
        if (L"UI_Card_Deck_Switch_Font" == strName)
            bIsFont = true;

        auto pScript = make_shared<UiCardDeckSwitch>(bIsFont);
        pScene->Init();
        pObj.lock()->Add_Component(pScript);
    }
}

void UiCardDeckController::Create_Info(_uint iIndex)
{
    if (true == m_bIsInfoCreate)
        return;

    auto& pData = DATAMGR;
    auto& tagData = pData.Get_Card_Inven(iIndex);
    if (ElementType::ElementEnd == tagData.Element)
        return;

    m_bIsInfoCreate = true;

    auto pScene = CUR_SCENE;
    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Card_Deck_Info.dat", m_vecAddedObj);

    auto& pResource = RESOURCES;

    _uint iSize = IDX(m_vecAddedObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_vecAddedObj[i];
        if (true == pObj.expired())
            continue;

        wstring strName = pObj.lock()->Get_Name();
        if (L"Card_Deck_info_Element" == strName)
            pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(pData.Get_Card_Inven_Element(iIndex)), TextureMapType::DIFFUSE);
        else if (L"Card_Deck_info_Hero" == strName)
            pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(tagData.KeyDeckMini), TextureMapType::DIFFUSE);
        else if (L"Card_Deck_info_Hero_Info" == strName)
            pObj.lock()->Get_FontRenderer()->Get_Text() = tagData.KeyHeroInfo;
        else if (L"Card_Deck_info_Hero_Name" == strName)
            pObj.lock()->Get_FontRenderer()->Get_Text() = tagData.KeyHeroName;
        else if (L"Card_Deck_info_Hero_Level_Value" == strName)
            pObj.lock()->Get_FontRenderer()->Get_Text() = to_wstring(tagData.Level);
        else if (L"Card_Deck_info_Hero_Weapon" == strName)
            pObj.lock()->Get_FontRenderer()->Get_Text() = tagData.KeyWeapon;
        else if (L"Card_Deck_info_Hero_Star" == strName)
        {
            _uint iStar = tagData.Star;
            wstring strTextureTag = L"";

            switch (iStar)
            {
            case 1:
                strTextureTag = L"Card_Deck_info_Star1";
                break;
            case 2:
                strTextureTag = L"Card_Deck_info_Star2";
                break;
            case 3:
                strTextureTag = L"Card_Deck_info_Star3";
                break;
            case 4:
                strTextureTag = L"Card_Deck_info_Star4";
                break;
            }

            pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(strTextureTag), TextureMapType::DIFFUSE);
        }
        else if (L"Card_Deck_info_Hero_Attack_Value" == strName)
            pObj.lock()->Get_FontRenderer()->Get_Text() = to_wstring(IDX(tagData.AttackDamage));
        else if (L"Card_Deck_info_Hero_Deffence_Value" == strName)
            pObj.lock()->Get_FontRenderer()->Get_Text() = to_wstring(IDX(tagData.Deffence));
        else  if (L"Card_Deck_info_Skill1" == strName)
            pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(tagData.KeySkill1), TextureMapType::DIFFUSE);
        else if (L"Card_Deck_info_Skill2" == strName)
            pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(tagData.KeySkill2), TextureMapType::DIFFUSE);
        else if (L"Card_Deck_info_Skill3" == strName)
            pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(tagData.KeySkill3), TextureMapType::DIFFUSE);
        else if (L"Card_Deck_info_Skill4" == strName)
            pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(tagData.KeySkill4), TextureMapType::DIFFUSE);
        else if (L"Card_Deck_info_Skill5" == strName)
            pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(tagData.KeySkill5), TextureMapType::DIFFUSE);
        else if (L"Card_Deck_info_Close_Button" == strName)
            pObj.lock()->Get_Button()->AddOnClickedEvent([]()
                {
                    CUR_SCENE->Get_UI(L"UI_Card_Deck_Controller")->Get_Script<UiCardDeckController>()->Remove_Info();
                });
        else if (L"Card_Deck_info_Set_Button" == strName)
            pObj.lock()->Get_Button()->AddOnClickedEvent([iIndex]()
                {
                    CUR_SCENE->Get_UI(L"UI_Card_Deck_Controller")->Get_Script<UiCardDeckController>()->Click_Info_Set(iIndex);
                });

    }
}

void UiCardDeckController::Set_Font(_uint iIndex)
{
    Remove_Font(iIndex);

    _uint Index = iIndex * 2;
    auto& tagData = DATAMGR.Get_Card_Inven(m_iSetIndex);

    if (true == m_vecFont[Index].expired())
        return;
    m_vecFont[Index].lock()->Get_FontRenderer()->Get_Text() = tagData.KeyHeroInfo;
    _float fLength = static_cast<_float>(tagData.KeyHeroInfo.length());
    _float4 vecPos = m_vecFont[Index].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x += fLength / 2.f * -27.f;
    m_vecFont[Index].lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);

    if (true == m_vecFont[Index + 1].expired())
        return;
    m_vecFont[Index + 1].lock()->Get_FontRenderer()->Get_Text() = tagData.KeyHeroName;
    fLength = static_cast<_float>(tagData.KeyHeroName.length());
    vecPos = m_vecFont[Index + 1].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x += fLength / 2.f * -27.f;
    m_vecFont[Index + 1].lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
}

void UiCardDeckController::Remove_Font(_uint iIndex)
{
    _uint Index = iIndex * 2;
    if (true == m_vecFont[Index].expired())
        return;
    m_vecFont[Index].lock()->Get_FontRenderer()->Get_Text() = L"";

    if (true == m_vecFont[Index + 1].expired())
        return;
    m_vecFont[Index + 1].lock()->Get_FontRenderer()->Get_Text() = L"";
    
    _float4 vecPos1 = m_vecFont[Index].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    _float4 vecPos2 = m_vecFont[Index + 1].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    switch (Index)
    {
    case 0:
        vecPos1.x = -480;
        vecPos2.x = -480;
        break;
    case 2:
        vecPos1.x = -140;
        vecPos2.x = -140;
        break;
    case 4:
        vecPos1.x = 200;
        vecPos2.x = 200;
        break;
    }
    m_vecFont[Index].lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos1);
    m_vecFont[Index + 1].lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos2);
}

void UiCardDeckController::Create_Select_Mark()
{
    if (true == m_bIsSelectMarkCreate)
        return;

    m_bIsSelectMarkCreate = true;

    auto pScene = CUR_SCENE;
    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Card_Select.dat", m_vecSelectMark);
}

void UiCardDeckController::Remove_Select_Mark()
{
    if (false == m_bIsSelectMarkCreate)
        return;

    m_bIsSelectMarkCreate = false;
    m_bIsMoveDown = true;

    auto& pEventMgr = EVENTMGR;

    for (_uint i = 0; i < IDX(m_vecSelectMark.size()); ++i)
    {
        auto& pObj = m_vecSelectMark[i];
        if (false == pObj.expired())
        {
            pEventMgr.Delete_Object(pObj.lock());
            pObj.reset();
        }
    }

    m_vecSelectMark.clear();
}

void UiCardDeckController::Move_Select_Mark()
{
    if (false == m_bIsSelectMarkCreate)
        return;

    m_fCheckTime += fDT;
    if(m_fMaxTime < m_fCheckTime)
    {
        m_fCheckTime = 0.f;
        m_bIsMoveDown = !m_bIsMoveDown;
    }

    if (true == m_vecSelectMark[0].expired() ||
        true == m_vecSelectMark[1].expired() ||
        true == m_vecSelectMark[2].expired())
        return;

    _float4 vecPos = m_vecSelectMark[0].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    _float fY = {};
    if(true == m_bIsMoveDown)
        fY = vecPos.y - fDT * m_fSpeed;
    else
        fY = vecPos.y + fDT * m_fSpeed;

    vecPos.y = fY;
    m_vecSelectMark[0].lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);

    vecPos = m_vecSelectMark[1].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.y = fY;
    m_vecSelectMark[1].lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);

    vecPos = m_vecSelectMark[2].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.y = fY;
    m_vecSelectMark[2].lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
}