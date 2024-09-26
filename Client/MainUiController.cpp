#include "pch.h"
#include "MainUiController.h" 

MainUiController::MainUiController()
{
}

HRESULT MainUiController::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;

    auto pScene = CUR_SCENE;
    m_vecUi.push_back(pScene->Get_UI(L"UI_HpBar"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_HpBarBg"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_SkillBg"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill_Gauge"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill0"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill1"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill2"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill3"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill4"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill5"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill6"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill0_Effect"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill2_Effect"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill3_Effect"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill4_Effect"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill5_Effect"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill6_Effect"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Cool_End2"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Cool_End3"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Cool_End4"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Cool_End5"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Cool_End6"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill2_Cool"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill3_Cool"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill4_Cool"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill5_Cool"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill6_Cool"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_HpBar1"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_HpFont"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill2_Type"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill3_Type"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill4_Type"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill5_Type"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill6_Type"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill0_Combo0"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill0_Combo1"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill0_Combo2"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill0_Combo3"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill2_Combo0"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill2_Combo1"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill2_Combo2"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill2_Combo3"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill3_Combo0"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill3_Combo1"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill3_Combo2"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill3_Combo3"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill4_Combo0"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill4_Combo1"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill4_Combo2"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill4_Combo3"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill5_Combo0"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill5_Combo1"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill5_Combo2"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill5_Combo3"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill6_Combo0"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill6_Combo1"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill6_Combo2"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Skill6_Combo3"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Combo_Effect"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Main_Button0"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Main_Button1"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Main_Button2"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Main_Button3"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Char_Change0"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Char_Change1"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Char_Change2"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Char_Change_Line0"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Char_Change_Line1"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Char_Change_Line2"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Char_Change_Element0"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Char_Change_Element1"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Char_Change_Element2"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Char_Help_Skill0"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Char_Help_Skill1"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_Char_Help_Skill2"));
    m_vecUi.push_back(pScene->Get_UI(L"UI_UseItem_Slot"));

    return S_OK;
}

void MainUiController::Set_MainUI_Render(_bool bValue)
{
    _uint iSize = IDX(m_vecUi.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        if (false == m_vecUi[i].expired())
        {
            _bool bTemp = bValue;
            if (false == m_vecUi[i].lock()->Is_Tick())
                bTemp = false;

            m_vecUi[i].lock()->Set_Render(bTemp);
        }
    }
}
