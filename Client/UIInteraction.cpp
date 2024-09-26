#include "pch.h"
#include "UIInteraction.h"

#include "UIShop.h"
#include "BaseUI.h"
#include "UiMarkNpc.h"
#include "FontRenderer.h"
#include "UiGachaController.h"
#include "UiQuestController.h"
#include "UiDialogController.h"

UIInteraction::UIInteraction()
{
}

HRESULT UIInteraction::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;
    
    m_bIsInit = true;

    m_bIsCreated = false;

    auto pScene = CUR_SCENE;
    m_pDialog_Controller = pScene->Get_UI(L"UI_Dialog_Controller");
    m_pGachaController = pScene->Get_UI(L"UI_Gacha_Controller");
    m_pShopController = pScene->Get_UI(L"UI_Shop_Controller");
    m_pNpcDialogController = pScene->Get_UI(L"UI_NpcDialog_Controller");
    
    return S_OK;
}

void UIInteraction::Tick()
{
    if (true == m_pOwner.expired() ||
        true == m_pDialog_Controller.expired() ||
        true == m_pAccessObj.expired())
        return;

    if(false == m_bIsMarkSetOn)
        if (false == m_pDialog_Controller.lock()->Get_Script<UiQuestController>()->Get_Dialog_End())
        {
            m_bIsMarkSetOn = true;
            if(m_pAccessObj.lock()->Get_Script<UiMarkNpc>())
                m_pAccessObj.lock()->Get_Script<UiMarkNpc>()->Change_Set_On(m_bIsMarkSetOn);
        }
}

void UIInteraction::Create_Interaction(NPCTYPE eType, shared_ptr<GameObject> pAccessObj)
{
    if (NPCTYPE::MAX == eType)
        return;

    if (true == m_bIsCreated)
        return;

    m_pAccessObj = pAccessObj;

    m_bIsCreated = true;
    m_bIsActivate = false;

    auto pScene = CUR_SCENE;
    vector<weak_ptr<GameObject>> addedObj;
    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Interaction.dat", addedObj);

    _uint iSize = IDX(addedObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        if (true == addedObj[i].expired())
            continue;

        wstring strName = addedObj[i].lock()->Get_Name();
        if (L"UI_Interaction" == strName)
            m_pInteraction_Bg = addedObj[i];
        else if (L"UI_Interaction_Font" == strName)
        {
            m_pInteraction_Font = addedObj[i];
            if(false == m_pInteraction_Font.expired())
                m_pInteraction_Font.lock()->Get_FontRenderer()->Get_Text() = GET_NPC_NAME(eType);
        }
    }

    if (false == m_pInteraction_Bg.expired())
    {
        _bool JustDialog = false;
        weak_ptr<GameObject> pObj;
        QUESTINDEX eIndex = QUESTINDEX::MAX;
        switch (eType)
        {
        case NPCTYPE::QUEST:
            eIndex = QUESTINDEX::KILL_DELLONS;
            break;
        case NPCTYPE::GACHA:
            eIndex = QUESTINDEX::TRY_GACHA;
            break;
        case NPCTYPE::HIDE_KID:
            eIndex = QUESTINDEX::HIDE_AND_SEEK;
            break;
        case NPCTYPE::POTION:
            break;
        default:
            JustDialog = true;
            break;
        }

        if (true == JustDialog)
        {
            pObj = m_pNpcDialogController;
            m_pInteraction_Bg.lock()->Get_Button()->AddOnClickedEvent([pObj, eType, this, eIndex]()
                {
                    if (false == pObj.expired())
                    {
                        pObj.lock()->Get_Script<UiDialogController>()->Create_Dialog(eType, m_pAccessObj.lock());
                        this->Remove_Interaction();
                    }
                });
        }

        else if (QUESTINDEX::MAX != eIndex)
        {
            pObj = m_pDialog_Controller;
            m_pInteraction_Bg.lock()->Get_Button()->AddOnClickedEvent([pObj, eType, this, eIndex]()
                {
                    if (false == pObj.expired())
                    {
                        pObj.lock()->Get_Script<UiQuestController>()->Create_Dialog(eType, eIndex, m_pAccessObj.lock());
                        this->Remove_Interaction();
                    }
                });
        }

        else if (NPCTYPE::POTION == eType)
        {
            pObj = m_pShopController;
            m_pInteraction_Bg.lock()->Get_Button()->AddOnClickedEvent([pObj, this]()
                {
                    if (false == pObj.expired())
                    {
                        pObj.lock()->Get_Script<UIShop>()->Create_Shop();
                        this->Remove_Interaction();
                    }
                });
        }
        
        //pObj = m_pGachaController;
        //m_pInteraction_Bg.lock()->Get_Button()->AddOnClickedEvent([pObj, this]()
        //    {
        //        if (false == pObj.expired())
        //        {
        //            pObj.lock()->Get_Script<UiGachaController>()->Create_Gacha_Card();
        //            this->Remove_Interaction();
        //        }
        //    });
    }

}

void UIInteraction::Remove_Interaction(shared_ptr<GameObject> pObj)
{
    if (false == m_pInteraction_Bg.expired() ||
        false == m_pInteraction_Font.expired() ||
        false == m_pAccessObj.expired())
    {
        if (m_pAccessObj.lock() != pObj)
            return;

        m_pAccessObj.reset();

        m_bIsCreated = false;
        m_bIsActivate = true;

        EVENTMGR.Delete_Object(m_pInteraction_Bg.lock());
        EVENTMGR.Delete_Object(m_pInteraction_Font.lock());
    }
}

_bool UIInteraction::Get_Is_Activate(shared_ptr<GameObject> pObj)
{
    if (m_pAccessObj.lock() != pObj)
        return false;

    if (true == m_bIsActivate)
    {
        m_bIsActivate = false;
        return true;
    }

    return false;
}

void UIInteraction::Remove_Interaction()
{
    if (false == m_pInteraction_Bg.expired() ||
        false == m_pInteraction_Font.expired())
    {
        m_bIsCreated = false;
        m_bIsActivate = true;
       
        m_bIsMarkSetOn = false;
        if (false == m_pAccessObj.expired())
            if(m_pAccessObj.lock()->Get_Script<UiMarkNpc>())
                m_pAccessObj.lock()->Get_Script<UiMarkNpc>()->Change_Set_On(m_bIsMarkSetOn);

        EVENTMGR.Delete_Object(m_pInteraction_Bg.lock());
        EVENTMGR.Delete_Object(m_pInteraction_Font.lock());
    }
}
