#include "pch.h"
#include "UiGachaController.h" 

#include "BaseUI.h"
#include "GachaScene.h"
#include "UiGachaCardMove.h"
#include "UiGachaEffectController.h"
#include "UiQuestController.h"

UiGachaController::UiGachaController()
{
}

HRESULT UiGachaController::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;

    m_fEffectCheckTime = 0.f;
    m_fEffectMaxTime = 0.5f;
    m_fEffectRatio = 1.f / m_fEffectMaxTime;

    m_vecObjTag.resize(10);
    m_vecObjTag[0] = L"UI_Gacha_Card0";
    m_vecObjTag[1] = L"UI_Gacha_Card1";
    m_vecObjTag[2] = L"UI_Gacha_Card2";
    m_vecObjTag[3] = L"UI_Gacha_Card3";
    m_vecObjTag[4] = L"UI_Gacha_Card4";
    m_vecObjTag[5] = L"UI_Gacha_Card5";
    m_vecObjTag[6] = L"UI_Gacha_Card6";
    m_vecObjTag[7] = L"UI_Gacha_Card7";
    m_vecObjTag[8] = L"UI_Gacha_Card8";
    m_vecObjTag[9] = L"UI_Gacha_Card9";

    m_vecObjEffectTag.resize(4);
    m_vecObjEffectTag[0] = L"UI_Gacha_Effect0";
    m_vecObjEffectTag[1] = L"UI_Gacha_Effect1";
    m_vecObjEffectTag[2] = L"UI_Gacha_Effect2";
    m_vecObjEffectTag[3] = L"UI_Gacha_Effect3";

    m_vecObjBgTag.resize(4);
    m_vecObjBgTag[0] = L"UI_Gacha_Bg0";
    m_vecObjBgTag[1] = L"UI_Gacha_Bg1";
    m_vecObjBgTag[2] = L"UI_Gacha_Bg2";
    m_vecObjBgTag[3] = L"UI_Gacha_Bg3";

    m_vecObjButton.resize(4);
    m_vecObjButton[0] = L"UI_Gacha_Open_Button";
    m_vecObjButton[1] = L"UI_Gacha_Close_Button";
    m_vecObjButton[2] = L"UI_Gacha_Font_Open";
    m_vecObjButton[3] = L"UI_Gacha_Font_Close";

    m_iSize = IDX(m_vecObjTag.size());

    return S_OK;
}

void UiGachaController::Tick()
{
    if (m_pOwner.expired())
        return;

    if (true == m_bIsStartOpen)
        Start_All_Open();

    //test code
    if (KEYTAP(KEY_TYPE::C))
    {
        Create_Gacha_Card();
    }
    //if (KEYTAP(KEY_TYPE::E))
    //{
    //    Delete_All();
    //}

    switch (m_eState)
    {
    case EFFECT_STATE::NONE:
        break;
    case EFFECT_STATE::START:
         Effect_Start();
        break;
    case EFFECT_STATE::CHANGE:
        Effect_Change();
        break;
    case EFFECT_STATE::RESET:
        Effect_Reset();
        break;
    }

}

void UiGachaController::Create_Gacha_Card()
{
    if (true == m_bIsCreate)
        return;

    g_bIsCanRotation = false;
    m_bIsCreate = true;
    auto pScene = CUR_SCENE;
    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Gacha.dat", list<shared_ptr<GameObject>>());

    auto pObj = pScene->Get_UI(L"UI_Dialog_Controller");
    pObj->Get_Script<UiQuestController>()->Change_Value();

    _uint iRand = rand() % 10;
    _uint iSize = IDX(m_vecObjTag.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        weak_ptr<GameObject> pObj = pScene->Get_UI(m_vecObjTag[i]);
        if (true == pObj.expired())
            continue;

        shared_ptr<UiGachaCardMove> pScript;
        if(iRand == i)
            pScript = make_shared<UiGachaCardMove>(i, true);
        else
            pScript = make_shared<UiGachaCardMove>(i, false);

        pObj.lock()->Add_Component(pScript);
        pObj.lock()->Init();

        pObj.lock()->Get_Button()->AddOnClickedEvent([pObj]()
            {
                pObj.lock()->Get_Script<UiGachaCardMove>()->Card_Open();
            });
    }

    iSize = IDX(m_vecObjEffectTag.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        weak_ptr<GameObject> pObj = pScene->Get_UI(m_vecObjEffectTag[i]);
        if (true == pObj.expired())
            continue;
        auto pScript = make_shared<UiGachaEffectController>();
        pObj.lock()->Add_Component(pScript);
        pObj.lock()->Init();
    }


    {
        weak_ptr<GameObject> pObj = pScene->Get_UI(m_vecObjButton[0]);
        if (true == pObj.expired())
            return;
        pObj.lock()->Get_Button()->AddOnClickedEvent([]()
            {
                weak_ptr<GameObject> pObject = CUR_SCENE->Get_GameObject(L"UI_Gacha_Controller");
                if (true == pObject.expired())
                    return;

                pObject.lock()->Get_Script<UiGachaController>()->Open_All_Card();
            });

        pObj = pScene->Get_UI(m_vecObjButton[1]);
        if (true == pObj.expired())
            return;
        pObj.lock()->Get_Button()->AddOnClickedEvent([]()
            {
                weak_ptr<GameObject> pObject = CUR_SCENE->Get_GameObject(L"UI_Gacha_Controller");
                if (true == pObject.expired())
                    return;

                pObject.lock()->Get_Script<UiGachaController>()->Delete_All();
            });
    }


}

void UiGachaController::Delete_Gacha_Card()
{
    auto pScene = CUR_SCENE;

    _uint iSize = IDX(m_vecObjTag.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        weak_ptr<GameObject> pObj = pScene->Get_UI(m_vecObjTag[i]);
        if (true == pObj.expired())
            continue;

        pObj.lock()->Get_Script<UiGachaCardMove>()->Add_Data();
        EVENTMGR.Delete_Object(pObj.lock());
    }

    DATAMGR.Sort_By_Star();
}

void UiGachaController::Delete_Gacha_Bg()
{
    auto pScene = CUR_SCENE;

    _uint iSize = IDX(m_vecObjBgTag.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        weak_ptr<GameObject> pObj = pScene->Get_UI(m_vecObjBgTag[i]);
        if (true == pObj.expired())
            continue;

        EVENTMGR.Delete_Object(pObj.lock());
    }
}

void UiGachaController::Delete_Gacha_Button()
{
    auto pScene = CUR_SCENE;

    _uint iSize = IDX(m_vecObjButton.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        weak_ptr<GameObject> pObj = pScene->Get_UI(m_vecObjButton[i]);
        if (true == pObj.expired())
            continue;

        EVENTMGR.Delete_Object(pObj.lock());
    }
}

void UiGachaController::Delete_Gacha_Effect()
{
    auto pScene = CUR_SCENE;

    _uint iSize = IDX(m_vecObjEffectTag.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        weak_ptr<GameObject> pObj = pScene->Get_UI(m_vecObjEffectTag[i]);
        if (true == pObj.expired())
            continue;

        EVENTMGR.Delete_Object(pObj.lock());
    }
}

void UiGachaController::Start_All_Open()
{
    if (m_iIndex == m_iSize)
        return;

    m_fCheckTime += fDT;
    if (m_fMaxTime < m_fCheckTime)
    {
        weak_ptr<GameObject> pObj = CUR_SCENE->Get_UI(m_vecObjTag[m_iIndex]);
        if (true == pObj.expired())
            return;

        pObj.lock()->Get_Script<UiGachaCardMove>()->Card_Open();

        m_fCheckTime = 0.f;
        m_iIndex++;
    }
}

void UiGachaController::Effect_Start()
{
    m_fEffectCheckTime += fDT;
    if (m_fEffectMaxTime < m_fEffectCheckTime)
    {
        m_eState = EFFECT_STATE::CHANGE;
        m_fEffectCheckTime = 0.f;

        return;
    }

    CUR_SCENE->g_fBrightness = m_fEffectCheckTime * 2.f;
}

void UiGachaController::Effect_Reset()
{
    m_eState = EFFECT_STATE::NONE;

    CUR_SCENE->g_fBrightness = m_fOriginBrightness;

    auto pScene = CUR_SCENE;
    _uint iSize = IDX(m_vecObjEffectTag.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        weak_ptr<GameObject> pObj = pScene->Get_UI(m_vecObjEffectTag[i]);
        if (false == pObj.expired())
            pObj.lock()->Set_Render(false);
    }
}

void UiGachaController::Effect_Change()
{
    m_eState = EFFECT_STATE::RESET;

    GachaSceneDesc sceneDesc = {};
    _bool bIsIt = false;
    switch (m_eGachaHero)
    {
    case HERO::ACE3:
        break;
    case HERO::KYLE:
        sceneDesc = { L"KyleMap", HERO::KYLE };
        bIsIt = true;
        break;
    case HERO::YEOPO:
        sceneDesc = { L"YeopoMap", HERO::YEOPO };
        bIsIt = true;
        break;
    case HERO::DELLONS:
        break;
    case HERO::SPIKE:
        break;
    case HERO::YEONHEE:
        break;
    case HERO::SHANE:
        break;
    }

    if (true == bIsIt)
    {
        SCENE.Add_SubScene(make_shared<GachaScene>(sceneDesc));
        SCENE.Exchange_Scene();
    }
}

void UiGachaController::Open_All_Card()
{
    if (true == m_bIsStartOpen)
        return;

    m_bIsStartOpen = true;
    m_fCheckTime = 0.f;
    m_fMaxTime = 0.15f;
    m_iIndex = 0;
}

void UiGachaController::Delete_All()
{
    g_bIsCanRotation = true;
    m_bIsCreate = false;
    m_bIsStartOpen = false;

    Delete_Gacha_Card();
    Delete_Gacha_Bg();
    Delete_Gacha_Button();
    Delete_Gacha_Effect();
}

void UiGachaController::Start_Effect(_float4 vPos, HERO eHero)
{
    // if hero gacha scene id added, add switch case
    _bool bIsit = false;
    switch (eHero)
    {
    case HERO::KYLE:
    case HERO::YEOPO:
        bIsit = true;
    }

    if (false == bIsit)
        return;

    m_fEffectCheckTime = 0.f;
    m_eState = EFFECT_STATE::START;
    m_eGachaHero = eHero;
    m_fOriginBrightness = CUR_SCENE->g_fBrightness;

    auto pScene = CUR_SCENE;
    _uint iSize = IDX(m_vecObjEffectTag.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        weak_ptr<GameObject> pObj = pScene->Get_UI(m_vecObjEffectTag[i]);
        if (true == pObj.expired())
            continue;

        pObj.lock()->Get_Script<UiGachaEffectController>()->Start_Effect(vPos);
    }
}