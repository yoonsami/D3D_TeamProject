#include "pch.h"
#include "CoolTimeCheckScript.h"

#include "Material.h"
#include "MeshRenderer.h"
#include "FontRenderer.h"
#include "UiComboEffect.h"
#include "UiCoolEndEffect.h"
#include "UiSkillButtonEffect.h"

CoolTimeCheckScript::CoolTimeCheckScript()
{
}

HRESULT CoolTimeCheckScript::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;
    m_eCurHero = HERO::PLAYER;
    m_CoolTime.resize(IDX(HERO::MAX));
    m_tagDefault.resize(IDX(HERO::MAX));
    for (_uint i = 0; i < IDX(HERO::MAX); ++i)
    {
        m_CoolTime[i].resize(5);

       auto& tagData = GET_DATA(static_cast<HERO>(i));
       m_CoolTime[i][0].CoolInfo.fCoolTime = tagData.Skill1Cool;
       m_CoolTime[i][1].CoolInfo.fCoolTime = tagData.Skill2Cool;
       m_CoolTime[i][2].CoolInfo.fCoolTime = tagData.Skill3Cool;
       m_CoolTime[i][3].CoolInfo.fCoolTime = tagData.Skill4Cool;
       m_CoolTime[i][4].CoolInfo.fCoolTime = tagData.Skill5Cool;

       m_CoolTime[i][0].CoolInfo.fAccTime = m_CoolTime[i][0].CoolInfo.fCoolTime + 0.1f;
       m_CoolTime[i][1].CoolInfo.fAccTime = m_CoolTime[i][1].CoolInfo.fCoolTime + 0.1f;
       m_CoolTime[i][2].CoolInfo.fAccTime = m_CoolTime[i][2].CoolInfo.fCoolTime + 0.1f;
       m_CoolTime[i][3].CoolInfo.fAccTime = m_CoolTime[i][3].CoolInfo.fCoolTime + 0.1f;
       m_CoolTime[i][4].CoolInfo.fAccTime = m_CoolTime[i][4].CoolInfo.fCoolTime + 0.1f;

       m_CoolTime[i][0].iSkillCombo = tagData.Skill1Combo;
       m_CoolTime[i][1].iSkillCombo = tagData.Skill2Combo;
       m_CoolTime[i][2].iSkillCombo = tagData.Skill3Combo;
       m_CoolTime[i][3].iSkillCombo = tagData.Skill4Combo;
       m_CoolTime[i][4].iSkillCombo = tagData.Skill5Combo;

       m_tagDefault[i].iSkillCombo = tagData.Skill0Combo;
    }

    m_tagEvade.fCoolTime = 3.f;

    m_pUiSkill.resize(7);

    auto pScene = CUR_SCENE;
    m_pUiSkill[0] = pScene->Get_UI(L"UI_Skill0");
    m_pUiSkill[1] = pScene->Get_UI(L"UI_Skill1");
    m_pUiSkill[2] = pScene->Get_UI(L"UI_Skill2");
    m_pUiSkill[3] = pScene->Get_UI(L"UI_Skill3");
    m_pUiSkill[4] = pScene->Get_UI(L"UI_Skill4");
    m_pUiSkill[5] = pScene->Get_UI(L"UI_Skill5");
    m_pUiSkill[6] = pScene->Get_UI(L"UI_Skill6");

    m_pUiSkill_Effect.resize(7);
    m_pUiSkill_Effect[0] = pScene->Get_UI(L"UI_Skill2_Effect");
    m_pUiSkill_Effect[1] = pScene->Get_UI(L"UI_Skill3_Effect");
    m_pUiSkill_Effect[2] = pScene->Get_UI(L"UI_Skill4_Effect");
    m_pUiSkill_Effect[3] = pScene->Get_UI(L"UI_Skill5_Effect");
    m_pUiSkill_Effect[4] = pScene->Get_UI(L"UI_Skill6_Effect");
    m_pUiSkill_Effect[5] = pScene->Get_UI(L"UI_Skill1_Effect");
    m_pUiSkill_Effect[6] = pScene->Get_UI(L"UI_Skill0_Effect");

    m_pUi_Cool_End.resize(5);
    m_pUi_Cool_End[0] = pScene->Get_UI(L"UI_Cool_End2");
    m_pUi_Cool_End[1] = pScene->Get_UI(L"UI_Cool_End3");
    m_pUi_Cool_End[2] = pScene->Get_UI(L"UI_Cool_End4");
    m_pUi_Cool_End[3] = pScene->Get_UI(L"UI_Cool_End5");
    m_pUi_Cool_End[4] = pScene->Get_UI(L"UI_Cool_End6");

    m_pUi_Skill_Cool.resize(5);
    m_pUi_Skill_Cool[0] = pScene->Get_UI(L"UI_Skill2_Cool");
    m_pUi_Skill_Cool[1] = pScene->Get_UI(L"UI_Skill3_Cool");
    m_pUi_Skill_Cool[2] = pScene->Get_UI(L"UI_Skill4_Cool");
    m_pUi_Skill_Cool[3] = pScene->Get_UI(L"UI_Skill5_Cool");
    m_pUi_Skill_Cool[4] = pScene->Get_UI(L"UI_Skill6_Cool");
    for (_uint i = 0; i < 5; ++i)
        m_pUi_Skill_Cool[i].lock()->Get_FontRenderer()->Get_Text() = L"";

    m_bIsCoolChangeOne.resize(5);
    m_bIsCoolChangeOne[0] = false;
    m_bIsCoolChangeOne[1] = false;
    m_bIsCoolChangeOne[2] = false;
    m_bIsCoolChangeOne[3] = false;
    m_bIsCoolChangeOne[4] = false;

    m_pUi_Skill0_Combo.resize(4);
    for (_uint i = 0; i < 4; ++i)
    {
        wstring strTemp = L"UI_Skill0_Combo";
        strTemp += to_wstring(i);
        m_pUi_Skill0_Combo[i] = pScene->Get_UI(strTemp);
    }

    m_pUi_Skill2_Combo.resize(4);
    for (_uint i = 0; i < 4; ++i)
    {
        wstring strTemp = L"UI_Skill2_Combo";
        strTemp += to_wstring(i);
        m_pUi_Skill2_Combo[i] = pScene->Get_UI(strTemp);
    }

    m_pUi_Skill3_Combo.resize(4);
    for (_uint i = 0; i < 4; ++i)
    {
        wstring strTemp = L"UI_Skill3_Combo";
        strTemp += to_wstring(i);
        m_pUi_Skill3_Combo[i] = pScene->Get_UI(strTemp);
    }

    m_pUi_Skill4_Combo.resize(4);
    for (_uint i = 0; i < 4; ++i)
    {
        wstring strTemp = L"UI_Skill4_Combo";
        strTemp += to_wstring(i);
        m_pUi_Skill4_Combo[i] = pScene->Get_UI(strTemp);
    }

    m_pUi_Skill5_Combo.resize(4);
    for (_uint i = 0; i < 4; ++i)
    {
        wstring strTemp = L"UI_Skill5_Combo";
        strTemp += to_wstring(i);
        m_pUi_Skill5_Combo[i] = pScene->Get_UI(strTemp);
    }

    m_pUi_Skill6_Combo.resize(4);
    for (_uint i = 0; i < 4; ++i)
    {
        wstring strTemp = L"UI_Skill6_Combo";
        strTemp += to_wstring(i);
        m_pUi_Skill6_Combo[i] = pScene->Get_UI(strTemp);
    }
    
    m_pUi_Skill_Type.resize(5);
    m_pUi_Skill_Type[0] = pScene->Get_UI(L"UI_Skill2_Type");
    m_pUi_Skill_Type[1] = pScene->Get_UI(L"UI_Skill3_Type");
    m_pUi_Skill_Type[2] = pScene->Get_UI(L"UI_Skill4_Type");
    m_pUi_Skill_Type[3] = pScene->Get_UI(L"UI_Skill5_Type");
    m_pUi_Skill_Type[4] = pScene->Get_UI(L"UI_Skill6_Type");


    m_pCombo_Effect = pScene->Get_UI(L"UI_Combo_Effect");

    


    m_pTexture.resize(IDX(HERO::MAX));
    for (_uint i = 0; i < IDX(HERO::MAX); ++i)
    {
        m_pTexture[i].resize(7);
        auto& tagData = GET_DATA(static_cast<HERO>(i));
        m_pTexture[i][0] = RESOURCES.Get<Texture>(tagData.KeyAttack);
        m_pTexture[i][1] = RESOURCES.Get<Texture>(tagData.KeyEvade);
        m_pTexture[i][2] = RESOURCES.Get<Texture>(tagData.KeySkill1);
        m_pTexture[i][3] = RESOURCES.Get<Texture>(tagData.KeySkill2);
        m_pTexture[i][4] = RESOURCES.Get<Texture>(tagData.KeySkill3);
        m_pTexture[i][5] = RESOURCES.Get<Texture>(tagData.KeySkill4);
        m_pTexture[i][6] = RESOURCES.Get<Texture>(tagData.KeySkill5);
    }

    m_pCombo0 = RESOURCES.Get<Texture>(L"UI_Skill_Combo0");
    m_pCombo1 = RESOURCES.Get<Texture>(L"UI_Skill_Combo1");
    m_pCharge = RESOURCES.Get<Texture>(L"UI_Skill_Charge");
    m_pHold = RESOURCES.Get<Texture>(L"UI_Skill_Hold");
    m_pMove = RESOURCES.Get<Texture>(L"UI_Skill_Move");

    for (_uint i = 1; i < IDX(HERO::MAX); ++i)
    {
        Set_Cur_Hero(static_cast<HERO>(i));
    }
    Set_Cur_Hero(HERO::PLAYER);

    return S_OK;
}

void CoolTimeCheckScript::Tick()
{
	if (m_pOwner.expired() || HERO::MAX == m_eCurHero)
		return;

    Check_Cool_Time();

    Change_Skills_Value();

}

void CoolTimeCheckScript::Set_Cur_Hero(HERO eType)
{
    if (m_pOwner.expired())
        return;

    Set_Skill_End();

    m_eCurHero = eType;
    _uint iIndex = IDX(m_eCurHero);

    _uint iSize = IDX(m_pUiSkill.size());
    for (_uint i = 0; i < iSize; ++i)
        if (true == m_pUiSkill[i].expired())
            return;

    /*for(_uint i = 0 ; i < iSize; ++i)
        m_pUiSkill[i].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(m_TextureKey[iIndex][i]), TextureMapType::DIFFUSE);*/

    //auto& tagData = GET_DATA(eType);
    //m_pUiSkill[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(tagData.KeyAttack), TextureMapType::DIFFUSE);
    //m_pUiSkill[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(tagData.KeyEvade), TextureMapType::DIFFUSE);
    //m_pUiSkill[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(tagData.KeySkill1), TextureMapType::DIFFUSE);
    //m_pUiSkill[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(tagData.KeySkill2), TextureMapType::DIFFUSE);
    //m_pUiSkill[4].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(tagData.KeySkill3), TextureMapType::DIFFUSE);
    //m_pUiSkill[5].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(tagData.KeySkill4), TextureMapType::DIFFUSE);
    //m_pUiSkill[6].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(tagData.KeySkill5), TextureMapType::DIFFUSE);

    for (_uint i = 0; i < 7; ++i)
        if (true == m_pTexture[iIndex][i].expired())
            return;
    
    m_pUiSkill[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTexture[iIndex][0].lock(), TextureMapType::DIFFUSE);
    m_pUiSkill[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTexture[iIndex][1].lock(), TextureMapType::DIFFUSE);
    m_pUiSkill[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTexture[iIndex][2].lock(), TextureMapType::DIFFUSE);
    m_pUiSkill[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTexture[iIndex][3].lock(), TextureMapType::DIFFUSE);
    m_pUiSkill[4].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTexture[iIndex][4].lock(), TextureMapType::DIFFUSE);
    m_pUiSkill[5].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTexture[iIndex][5].lock(), TextureMapType::DIFFUSE);
    m_pUiSkill[6].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTexture[iIndex][6].lock(), TextureMapType::DIFFUSE);




    for (_uint i = 0; i < IDX(m_pUi_Skill_Cool.size()); ++i)
        if (true == m_pUi_Skill_Cool[i].expired())
            return;


    for (_uint i = 0; i < IDX(m_pUi_Skill_Cool.size()); ++i)
    {
        Change_Pos_Two_Letter(i);
    }
    
    auto& vecCool = m_CoolTime[IDX(eType)];
    _uint iSkill1Cool = IDX(vecCool[0].CoolInfo.fCoolTime - vecCool[0].CoolInfo.fAccTime + 1.f);
    _uint iSkill2Cool = IDX(vecCool[1].CoolInfo.fCoolTime - vecCool[1].CoolInfo.fAccTime + 1.f);
    _uint iSkill3Cool = IDX(vecCool[2].CoolInfo.fCoolTime - vecCool[2].CoolInfo.fAccTime + 1.f);
    _uint iSkill4Cool = IDX(vecCool[3].CoolInfo.fCoolTime - vecCool[3].CoolInfo.fAccTime + 1.f);
    _uint iSkill5Cool = IDX(vecCool[4].CoolInfo.fCoolTime - vecCool[4].CoolInfo.fAccTime + 1.f);

    if (0 < iSkill1Cool && iSkill1Cool < 10 && false == m_bIsCoolChangeOne[0])
        Change_Pos_One_Letter(0);
    if (0 < iSkill2Cool && iSkill2Cool < 10 && false == m_bIsCoolChangeOne[1])
        Change_Pos_One_Letter(1);
    if (0 < iSkill3Cool && iSkill3Cool < 10 && false == m_bIsCoolChangeOne[2])
        Change_Pos_One_Letter(2);
    if (0 < iSkill4Cool && iSkill4Cool < 10 && false == m_bIsCoolChangeOne[3])
        Change_Pos_One_Letter(3);
    if (0 < iSkill5Cool && iSkill5Cool < 10 && false == m_bIsCoolChangeOne[4])
        Change_Pos_One_Letter(4);

    if (0 < iSkill1Cool)
        m_pUi_Skill_Cool[0].lock()->Get_FontRenderer()->Get_Text() = to_wstring(iSkill1Cool);
    if (0 < iSkill2Cool)
        m_pUi_Skill_Cool[1].lock()->Get_FontRenderer()->Get_Text() = to_wstring(iSkill2Cool);
    if (0 < iSkill3Cool)
        m_pUi_Skill_Cool[2].lock()->Get_FontRenderer()->Get_Text() = to_wstring(iSkill3Cool);
    if (0 < iSkill4Cool)
        m_pUi_Skill_Cool[3].lock()->Get_FontRenderer()->Get_Text() = to_wstring(iSkill4Cool);
    if (0 < iSkill5Cool)
        m_pUi_Skill_Cool[4].lock()->Get_FontRenderer()->Get_Text() = to_wstring(iSkill5Cool);

    Change_Skill_Info(eType);
}

void CoolTimeCheckScript::Set_Skill_End()
{
    //if (m_pOwner.expired() || HERO::MAX == m_eCurHero)
    //    return;

    //m_bIsSkillWork = false;2
    //m_CoolTime[IDX(m_eCurHero)][m_iWorkSkillIndex].bIsWork = false;
    //m_CoolTime[IDX(m_eCurHero)][m_iWorkSkillIndex].fAccTime = 0.f;


    if (m_pOwner.expired() || HERO::MAX == m_eCurHero)
        return;
    
    if(SkillType::SKILL5 >= m_iWorkSkillIndex)
    {
        auto& CoolInfo = m_CoolTime[IDX(m_eCurHero)][m_iWorkSkillIndex];
        if (SkillType::DEFAULT > m_iWorkSkillIndex && true == CoolInfo.bIsWork)
        {
            m_bIsSkillWork = false;
            CoolInfo.bIsWork = false;
            CoolInfo.CoolInfo.fAccTime = 0.f;
        }
    }

    if (m_iWorkSkillIndex > SkillType::SKILL5)
    {
        m_tagDefault[IDX(m_eCurHero)].iCurCombo = 0;
    }
    else
    {
        m_CoolTime[IDX(m_eCurHero)][m_iWorkSkillIndex].iCurCombo = 0;
    }

    _uint iSize = IDX(m_pUi_Skill0_Combo.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        if (true == m_pUi_Skill0_Combo[i].expired() ||
            true == m_pUi_Skill2_Combo[i].expired() ||
            true == m_pUi_Skill3_Combo[i].expired() ||
            true == m_pUi_Skill4_Combo[i].expired() ||
            true == m_pUi_Skill5_Combo[i].expired() ||
            true == m_pUi_Skill6_Combo[i].expired())
            return;
    }

    if (true == m_pCombo0.expired() ||
        true == m_pCombo1.expired())
        return;

    _float4 vecPos = {};
    switch (static_cast<SkillType>(m_iWorkSkillIndex))
    {
    case SKILL1:
        m_pUi_Skill2_Combo[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill2_Combo[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill2_Combo[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill2_Combo[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        vecPos = m_pUi_Skill2_Combo[0].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
        break;
    case SKILL2:
        m_pUi_Skill3_Combo[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill3_Combo[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill3_Combo[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill3_Combo[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        vecPos = m_pUi_Skill3_Combo[0].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
        break;
    case SKILL3:
        m_pUi_Skill4_Combo[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill4_Combo[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill4_Combo[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill4_Combo[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        vecPos = m_pUi_Skill4_Combo[0].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
        break;
    case SKILL4:
        m_pUi_Skill5_Combo[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill5_Combo[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill5_Combo[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill5_Combo[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        vecPos = m_pUi_Skill5_Combo[0].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
        break;
    case SKILL5:
        m_pUi_Skill6_Combo[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill6_Combo[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill6_Combo[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill6_Combo[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        vecPos = m_pUi_Skill6_Combo[0].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
        break;
    case DEFAULT:
        m_pUi_Skill0_Combo[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill0_Combo[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill0_Combo[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        m_pUi_Skill0_Combo[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
        vecPos = m_pUi_Skill0_Combo[0].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
        break;

    }
}

_bool CoolTimeCheckScript::IsAvailable(SkillType eSkillType)
{
    if (m_pOwner.expired() || HERO::MAX == m_eCurHero || 5 < eSkillType)
        return false;

    // 5 : evade
    else if (5 == eSkillType)
    {
        if (m_tagEvade.fCoolTime < m_tagEvade.fAccTime)
        {
            Set_Skill_End();
            m_tagEvade.fAccTime = 0.f;

            return true;
        }
        else
        {
            return false;
        }
    }

    // 0 ~ 4 : skill 
    auto& CoolInfo = m_CoolTime[IDX(m_eCurHero)][eSkillType];
    if (CoolInfo.CoolInfo.fCoolTime < CoolInfo.CoolInfo.fAccTime)
    {
        Set_Skill_End();
        CoolInfo.bIsEnd = false;
        CoolInfo.bIsWork = true;
        //CoolInfo.fAccTime = 0.f;
        m_bIsSkillWork = true;
        m_iWorkSkillIndex = eSkillType;

        Start_Effect(eSkillType);

        return true;
    }

    return false;
}

void CoolTimeCheckScript::Start_Attack_Button_Effect()
{
    Start_Effect(DEFAULT);
}

void CoolTimeCheckScript::Next_Combo(SkillType eSkillType)
{
    _uint iSize = IDX(m_pUi_Skill0_Combo.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        if (true == m_pUi_Skill0_Combo[i].expired() ||
            true == m_pUi_Skill2_Combo[i].expired() ||
            true == m_pUi_Skill3_Combo[i].expired() ||
            true == m_pUi_Skill4_Combo[i].expired() ||
            true == m_pUi_Skill5_Combo[i].expired() ||
            true == m_pUi_Skill6_Combo[i].expired())
            return;
    }

    Start_Effect(eSkillType);
    _uint iCombo = 0;
    _uint iMaxCombo = 0;
    _float4 vecPos = {};
    if (m_iWorkSkillIndex > SkillType::SKILL5)
    {
        iCombo = m_tagDefault[IDX(m_eCurHero)].iCurCombo++;
        iMaxCombo = m_tagDefault[IDX(m_eCurHero)].iSkillCombo;
    }
    else
    {
        iCombo = m_CoolTime[IDX(m_eCurHero)][eSkillType].iCurCombo++;
        iMaxCombo = m_CoolTime[IDX(m_eCurHero)][eSkillType].iSkillCombo;
    }
    
    if (true == m_pCombo0.expired() ||
        true == m_pCombo1.expired())
        return;

    if (iCombo < iMaxCombo - 1)
    {
        switch (eSkillType)
        {
        case SKILL1:
            m_pUi_Skill2_Combo[iCombo++].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
            m_pUi_Skill2_Combo[iCombo].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
            vecPos = m_pUi_Skill2_Combo[iCombo].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            break;
        case SKILL2:
            m_pUi_Skill3_Combo[iCombo++].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
            m_pUi_Skill3_Combo[iCombo].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
            vecPos = m_pUi_Skill3_Combo[iCombo].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            break;
        case SKILL3:
            m_pUi_Skill4_Combo[iCombo++].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
            m_pUi_Skill4_Combo[iCombo].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
            vecPos = m_pUi_Skill4_Combo[iCombo].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            break;
        case SKILL4:
            m_pUi_Skill5_Combo[iCombo++].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
            m_pUi_Skill5_Combo[iCombo].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
            vecPos = m_pUi_Skill5_Combo[iCombo].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            break;
        case SKILL5:
            m_pUi_Skill6_Combo[iCombo++].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
            m_pUi_Skill6_Combo[iCombo].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
            vecPos = m_pUi_Skill6_Combo[iCombo].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            break;
        case DEFAULT:
            m_pUi_Skill0_Combo[iCombo++].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
            m_pUi_Skill0_Combo[iCombo].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
            vecPos = m_pUi_Skill0_Combo[iCombo].lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            break;
        }

        m_pCombo_Effect.lock()->Get_Script<UiComboEffect>()->Start_Effect(vecPos);
    }
}

void CoolTimeCheckScript::Check_Cool_Time()
{
    _float fDt = fDT;
    for (_uint i = 0; i < IDX(HERO::MAX); ++i)
    {
        auto& vecCool = m_CoolTime[i];
        for (_uint j = 0; j < 5; ++j)
        {
            if (false == vecCool[j].bIsWork && vecCool[j].CoolInfo.fCoolTime > vecCool[j].CoolInfo.fAccTime)
            {
                vecCool[j].CoolInfo.fAccTime += fDt;
                if (false == vecCool[j].bIsEnd && vecCool[j].CoolInfo.fCoolTime < vecCool[j].CoolInfo.fAccTime)
                {
                    vecCool[j].bIsEnd = true;
                    Change_Pos_Two_Letter(j);
                    
                    if(i == IDX(m_eCurHero))
                        Start_ButtonEndEffect(j);
                }
            }
        }
    }

    if (m_tagEvade.fCoolTime > m_tagEvade.fAccTime)
        m_tagEvade.fAccTime += fDt;
}

void CoolTimeCheckScript::Change_Skills_Value()
{
    if (m_pOwner.expired() || HERO::MAX == m_eCurHero)
        return;

    auto& vecCool = m_CoolTime[IDX(m_eCurHero)];

    auto pGameobjectGauge = CUR_SCENE->Get_GameObject(L"UI_Skill_Gauge");

    if (nullptr == pGameobjectGauge)
        return;

    for (_uint i = 0; i < IDX(m_pUiSkill.size()); ++i)
        if (true == m_pUiSkill[i].expired())
            return;

    for (_uint i = 0; i < IDX(m_pUi_Skill_Cool.size()); ++i)
        if (true == m_pUi_Skill_Cool[i].expired())
            return;

    pGameobjectGauge->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = m_tagEvade.fAccTime / m_tagEvade.fCoolTime;
    m_pUiSkill[2].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = vecCool[0].CoolInfo.fAccTime / vecCool[0].CoolInfo.fCoolTime;
    m_pUiSkill[3].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = vecCool[1].CoolInfo.fAccTime / vecCool[1].CoolInfo.fCoolTime;
    m_pUiSkill[4].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = vecCool[2].CoolInfo.fAccTime / vecCool[2].CoolInfo.fCoolTime;
    m_pUiSkill[5].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = vecCool[3].CoolInfo.fAccTime / vecCool[3].CoolInfo.fCoolTime;
    m_pUiSkill[6].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = vecCool[4].CoolInfo.fAccTime / vecCool[4].CoolInfo.fCoolTime;

    
    _uint iSkill1Cool = IDX(vecCool[0].CoolInfo.fCoolTime - vecCool[0].CoolInfo.fAccTime + 1.f);
    _uint iSkill2Cool = IDX(vecCool[1].CoolInfo.fCoolTime - vecCool[1].CoolInfo.fAccTime + 1.f);
    _uint iSkill3Cool = IDX(vecCool[2].CoolInfo.fCoolTime - vecCool[2].CoolInfo.fAccTime + 1.f);
    _uint iSkill4Cool = IDX(vecCool[3].CoolInfo.fCoolTime - vecCool[3].CoolInfo.fAccTime + 1.f);
    _uint iSkill5Cool = IDX(vecCool[4].CoolInfo.fCoolTime - vecCool[4].CoolInfo.fAccTime + 1.f);

    if (0 < iSkill1Cool && iSkill1Cool < 10 && false == m_bIsCoolChangeOne[0])
        Change_Pos_One_Letter(0);
    if (0 < iSkill2Cool && iSkill2Cool < 10 && false == m_bIsCoolChangeOne[1])
        Change_Pos_One_Letter(1);
    if (0 < iSkill3Cool && iSkill3Cool < 10 && false == m_bIsCoolChangeOne[2])
        Change_Pos_One_Letter(2);
    if (0 < iSkill4Cool && iSkill4Cool < 10 && false == m_bIsCoolChangeOne[3])
        Change_Pos_One_Letter(3);
    if (0 < iSkill5Cool && iSkill5Cool < 10 && false == m_bIsCoolChangeOne[4])
        Change_Pos_One_Letter(4);

    if (0 < iSkill1Cool)
        m_pUi_Skill_Cool[0].lock()->Get_FontRenderer()->Get_Text() = to_wstring(iSkill1Cool);
    if (0 < iSkill2Cool)
        m_pUi_Skill_Cool[1].lock()->Get_FontRenderer()->Get_Text() = to_wstring(iSkill2Cool);
    if (0 < iSkill3Cool)
        m_pUi_Skill_Cool[2].lock()->Get_FontRenderer()->Get_Text() = to_wstring(iSkill3Cool);
    if (0 < iSkill4Cool)
        m_pUi_Skill_Cool[3].lock()->Get_FontRenderer()->Get_Text() = to_wstring(iSkill4Cool);
    if (0 < iSkill5Cool)
        m_pUi_Skill_Cool[4].lock()->Get_FontRenderer()->Get_Text() = to_wstring(iSkill5Cool);

}

void CoolTimeCheckScript::Start_Effect(SkillType eSkillType)
{
    if (true == m_pUiSkill_Effect[eSkillType].expired())
        return;

    m_iWorkSkillIndex = eSkillType;
    m_pUiSkill_Effect[eSkillType].lock()->Get_Script<UiSkillButtonEffect>()->Start_Effect();
}

void CoolTimeCheckScript::Start_ButtonEndEffect(_uint iIndex)
{
    if (true == m_pUi_Cool_End[iIndex].expired())
        return;

    m_pUi_Cool_End[iIndex].lock()->Get_Script<UiCoolEndEffect>()->Start_Effect();
}

void CoolTimeCheckScript::Change_Pos_Two_Letter(_uint iIndex)
{
    if (true == m_pUi_Skill_Cool[iIndex].expired())
        return;

    auto pTransform = m_pUi_Skill_Cool[iIndex].lock()->GetOrAddTransform();
    _float4 vecPos = pTransform->Get_State(Transform_State::POS);

    switch (iIndex)
    {
    case 0:
        vecPos.x = 552;
        break;
    case 1:
        vecPos.x = 682;
        break;
    case 2:
        vecPos.x = 812;
        break;
    case 3:
        vecPos.x = 812;
        break;
    case 4:
        vecPos.x = 682;
        break;
    default:
        return;
    }

    pTransform->Set_State(Transform_State::POS, vecPos);
    
    Change_Text_Zero(iIndex);
}

void CoolTimeCheckScript::Change_Pos_One_Letter(_uint iIndex)
{
    if (true == m_pUi_Skill_Cool[iIndex].expired())
        return;

    auto pTransform = m_pUi_Skill_Cool[iIndex].lock()->GetOrAddTransform();
    _float4 vecPos = pTransform->Get_State(Transform_State::POS);
    m_bIsCoolChangeOne[iIndex] = true;

    switch (iIndex)
    {
    case 0:
        vecPos.x = 563;
        break;
    case 1:
        vecPos.x = 693;
        break;
    case 2:
        vecPos.x = 823;
        break;
    case 3:
        vecPos.x = 823;
        break;
    case 4:
        vecPos.x = 693;
        break;
    default:
        return;
    }

    pTransform->Set_State(Transform_State::POS, vecPos);
}

void CoolTimeCheckScript::Change_Text_Zero(_uint iIndex)
{
    if (true == m_pUi_Skill_Cool[iIndex].expired())
        return;

    m_pUi_Skill_Cool[iIndex].lock()->Get_FontRenderer()->Get_Text() = L"";
    m_bIsCoolChangeOne[iIndex] = false;
}

void CoolTimeCheckScript::Change_Skill_Info(HERO eType)
{
    //shared_ptr<Texture>    pTexture_Combo0 = RESOURCES.Get<Texture>(L"UI_Skill_Combo0");
    //shared_ptr<Texture>    pTexture_Combo1 = RESOURCES.Get<Texture>(L"UI_Skill_Combo1");

    if (true == m_pCombo0.expired() ||
        true == m_pCombo1.expired())
        return;

    m_pUi_Skill2_Combo[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill2_Combo[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill2_Combo[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill2_Combo[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    
    m_pUi_Skill3_Combo[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill3_Combo[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill3_Combo[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill3_Combo[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
   
    m_pUi_Skill4_Combo[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill4_Combo[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill4_Combo[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill4_Combo[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
   
    m_pUi_Skill5_Combo[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill5_Combo[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill5_Combo[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill5_Combo[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    
    m_pUi_Skill6_Combo[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill6_Combo[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill6_Combo[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill6_Combo[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    
    m_pUi_Skill0_Combo[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo1.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill0_Combo[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill0_Combo[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);
    m_pUi_Skill0_Combo[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCombo0.lock(), TextureMapType::DIFFUSE);

    auto& tagData = GET_DATA(eType);
    
    for (_uint i = 0; i < 4; i++)
    {
        if (i < tagData.Skill0Combo)
        {
            //m_pUi_Skill0_Combo[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
            m_pUi_Skill0_Combo[i].lock()->Set_Render(true);
            m_pUi_Skill0_Combo[i].lock()->Set_Tick(true);
        }
        else
        {
            //m_pUi_Skill0_Combo[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
            m_pUi_Skill0_Combo[i].lock()->Set_Render(false);
            m_pUi_Skill0_Combo[i].lock()->Set_Tick(false);
        }


        if (i < tagData.Skill1Combo)
        {
            //m_pUi_Skill2_Combo[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
            m_pUi_Skill2_Combo[i].lock()->Set_Render(true);
            m_pUi_Skill2_Combo[i].lock()->Set_Tick(true);
        }
        else
        {
            //m_pUi_Skill2_Combo[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
            m_pUi_Skill2_Combo[i].lock()->Set_Render(false);
            m_pUi_Skill2_Combo[i].lock()->Set_Tick(false);
        }


        if (i < tagData.Skill2Combo)
        {
            //m_pUi_Skill3_Combo[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
            m_pUi_Skill3_Combo[i].lock()->Set_Render(true);
            m_pUi_Skill3_Combo[i].lock()->Set_Tick(true);
        }
        else
        {
            //m_pUi_Skill3_Combo[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
            m_pUi_Skill3_Combo[i].lock()->Set_Render(false);
            m_pUi_Skill3_Combo[i].lock()->Set_Tick(false);
        }


        if (i < tagData.Skill3Combo)
        {
            //m_pUi_Skill4_Combo[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
            m_pUi_Skill4_Combo[i].lock()->Set_Render(true);
            m_pUi_Skill4_Combo[i].lock()->Set_Tick(true);
        }
        else
        {
            //m_pUi_Skill4_Combo[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
            m_pUi_Skill4_Combo[i].lock()->Set_Render(false);
            m_pUi_Skill4_Combo[i].lock()->Set_Tick(false);
        }


        if (i < tagData.Skill4Combo)
        {
            //m_pUi_Skill5_Combo[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
            m_pUi_Skill5_Combo[i].lock()->Set_Render(true);
            m_pUi_Skill5_Combo[i].lock()->Set_Tick(true);
        }
        else
        {
            //m_pUi_Skill5_Combo[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
            m_pUi_Skill5_Combo[i].lock()->Set_Render(false);
            m_pUi_Skill5_Combo[i].lock()->Set_Tick(false);
        }


        if (i < tagData.Skill5Combo)
        {
            //m_pUi_Skill6_Combo[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
            m_pUi_Skill6_Combo[i].lock()->Set_Render(true);
            m_pUi_Skill6_Combo[i].lock()->Set_Tick(true);
        }
        else
        {
            //m_pUi_Skill6_Combo[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
            m_pUi_Skill6_Combo[i].lock()->Set_Render(false);
            m_pUi_Skill6_Combo[i].lock()->Set_Tick(false);
        }

    }

    if (true == m_pCharge.expired() ||
        true == m_pHold.expired() ||
        true == m_pMove.expired())
        return;


    if (SkillInfo::NONE == tagData.Skill1Info)
    {
        //m_pUi_Skill_Type[0].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
        m_pUi_Skill_Type[0].lock()->Set_Render(false);
        m_pUi_Skill_Type[0].lock()->Set_Tick(false);
    }
    else if (SkillInfo::CHARGING == tagData.Skill1Info)
    {
        //m_pUi_Skill_Type[0].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[0].lock()->Set_Render(true);
        m_pUi_Skill_Type[0].lock()->Set_Tick(true);
        m_pUi_Skill_Type[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCharge.lock(), TextureMapType::DIFFUSE);
    }
    else if (SkillInfo::HOLDING == tagData.Skill1Info)
    {
        //m_pUi_Skill_Type[0].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[0].lock()->Set_Render(true);
        m_pUi_Skill_Type[0].lock()->Set_Tick(true);
        m_pUi_Skill_Type[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pHold.lock(), TextureMapType::DIFFUSE);
    }
    else if (SkillInfo::MOVING == tagData.Skill1Info)
    {
        //m_pUi_Skill_Type[0].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[0].lock()->Set_Render(true);
        m_pUi_Skill_Type[0].lock()->Set_Tick(true);
        m_pUi_Skill_Type[0].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pMove.lock(), TextureMapType::DIFFUSE);
    }

    if (SkillInfo::NONE == tagData.Skill2Info)
    {
        //m_pUi_Skill_Type[1].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
        m_pUi_Skill_Type[1].lock()->Set_Render(false);
        m_pUi_Skill_Type[1].lock()->Set_Tick(false);
    }
    else if (SkillInfo::CHARGING == tagData.Skill2Info)
    {
        //m_pUi_Skill_Type[1].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[1].lock()->Set_Render(true);
        m_pUi_Skill_Type[1].lock()->Set_Tick(true);
        m_pUi_Skill_Type[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCharge.lock(), TextureMapType::DIFFUSE);
    }
    else if (SkillInfo::HOLDING == tagData.Skill2Info)
    {
        //m_pUi_Skill_Type[1].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[1].lock()->Set_Render(true);
        m_pUi_Skill_Type[1].lock()->Set_Tick(true);
        m_pUi_Skill_Type[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pHold.lock(), TextureMapType::DIFFUSE);
    }
    else if (SkillInfo::MOVING == tagData.Skill2Info)
    {
        //m_pUi_Skill_Type[1].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[1].lock()->Set_Render(true);
        m_pUi_Skill_Type[1].lock()->Set_Tick(true);
        m_pUi_Skill_Type[1].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pMove.lock(), TextureMapType::DIFFUSE);
    }

    if (SkillInfo::NONE == tagData.Skill3Info)
    {
        //m_pUi_Skill_Type[2].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
        m_pUi_Skill_Type[2].lock()->Set_Render(false);
        m_pUi_Skill_Type[2].lock()->Set_Tick(false);
    }
    else if (SkillInfo::CHARGING == tagData.Skill3Info)
    {
        //m_pUi_Skill_Type[2].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[2].lock()->Set_Render(true);
        m_pUi_Skill_Type[2].lock()->Set_Tick(true);
        m_pUi_Skill_Type[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCharge.lock(), TextureMapType::DIFFUSE);
    }
    else if (SkillInfo::HOLDING == tagData.Skill3Info)
    {
        //m_pUi_Skill_Type[2].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[2].lock()->Set_Render(true);
        m_pUi_Skill_Type[2].lock()->Set_Tick(true);
        m_pUi_Skill_Type[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pHold.lock(), TextureMapType::DIFFUSE);
    }
    else if (SkillInfo::MOVING == tagData.Skill3Info)
    {
        //m_pUi_Skill_Type[2].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[2].lock()->Set_Render(true);
        m_pUi_Skill_Type[2].lock()->Set_Tick(true);
        m_pUi_Skill_Type[2].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pMove.lock(), TextureMapType::DIFFUSE);
    }


    if (SkillInfo::NONE == tagData.Skill4Info)
    {
        //m_pUi_Skill_Type[3].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
        m_pUi_Skill_Type[3].lock()->Set_Render(false);
        m_pUi_Skill_Type[3].lock()->Set_Tick(false);
    }
    else if (SkillInfo::CHARGING == tagData.Skill4Info)
    {
        //m_pUi_Skill_Type[3].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[3].lock()->Set_Render(true);
        m_pUi_Skill_Type[3].lock()->Set_Tick(true);
        m_pUi_Skill_Type[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCharge.lock(), TextureMapType::DIFFUSE);
    }
    else if (SkillInfo::HOLDING == tagData.Skill4Info)
    {
        //m_pUi_Skill_Type[3].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[3].lock()->Set_Render(true);
        m_pUi_Skill_Type[3].lock()->Set_Tick(true);
        m_pUi_Skill_Type[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pHold.lock(), TextureMapType::DIFFUSE);
    }
    else if (SkillInfo::MOVING == tagData.Skill4Info)
    {
        //m_pUi_Skill_Type[3].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[3].lock()->Set_Render(true);
        m_pUi_Skill_Type[3].lock()->Set_Tick(true);
        m_pUi_Skill_Type[3].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pMove.lock(), TextureMapType::DIFFUSE);
    }


    if (SkillInfo::NONE == tagData.Skill5Info)
    {
        //m_pUi_Skill_Type[4].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 0.f;
        m_pUi_Skill_Type[4].lock()->Set_Render(false);
        m_pUi_Skill_Type[4].lock()->Set_Tick(false);
    }
    else if (SkillInfo::CHARGING == tagData.Skill5Info)
    {
        //m_pUi_Skill_Type[4].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[4].lock()->Set_Render(true);
        m_pUi_Skill_Type[4].lock()->Set_Tick(true);
        m_pUi_Skill_Type[4].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pCharge.lock(), TextureMapType::DIFFUSE);
    }
    else if (SkillInfo::HOLDING == tagData.Skill5Info)
    {
        //m_pUi_Skill_Type[4].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[4].lock()->Set_Render(true);
        m_pUi_Skill_Type[4].lock()->Set_Tick(true);
        m_pUi_Skill_Type[4].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pHold.lock(), TextureMapType::DIFFUSE);
    }
    else if (SkillInfo::MOVING == tagData.Skill5Info)
    {
        //m_pUi_Skill_Type[4].lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
        m_pUi_Skill_Type[4].lock()->Set_Render(true);
        m_pUi_Skill_Type[4].lock()->Set_Tick(true);
        m_pUi_Skill_Type[4].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pMove.lock(), TextureMapType::DIFFUSE);
    }
}
