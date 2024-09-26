#include "pch.h"
#include "UiCharChange.h"

#include "Material.h"
#include "MeshRenderer.h"
#include "HeroChangeScript.h"
#include "Friend_FSM.h"

UiCharChange::UiCharChange()
{
}

HRESULT UiCharChange::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;

    auto pScene = CUR_SCENE;

    m_vecDesc.resize(3);
    m_vecDesc[0].fCoolTime = 10.f;
    m_vecDesc[1].fCoolTime = 10.f;
    m_vecDesc[2].fCoolTime = 10.f;

    m_vecDesc[0].fAccTime = m_vecDesc[0].fCoolTime + 1.f;
    m_vecDesc[1].fAccTime = m_vecDesc[1].fCoolTime + 1.f;
    m_vecDesc[2].fAccTime = m_vecDesc[2].fCoolTime + 1.f;

    m_pObj.resize(3);
    m_pObj[0] = pScene->Get_UI(L"UI_Char_Change0");
    m_pObj[1] = pScene->Get_UI(L"UI_Char_Change1");
    m_pObj[2] = pScene->Get_UI(L"UI_Char_Change2");

    m_pElement.resize(3);
    m_pElement[0] = pScene->Get_UI(L"UI_Char_Change_Element0");
    m_pElement[1] = pScene->Get_UI(L"UI_Char_Change_Element1");
    m_pElement[2] = pScene->Get_UI(L"UI_Char_Change_Element2");

    m_pSkillHelp.resize(3);
    m_pSkillHelp[0] = pScene->Get_UI(L"UI_Char_Help_Skill0");
    m_pSkillHelp[1] = pScene->Get_UI(L"UI_Char_Help_Skill1");
    m_pSkillHelp[2] = pScene->Get_UI(L"UI_Char_Help_Skill2");
    for (_uint i = 0; i < 3; ++i)
        if (false == m_pSkillHelp[i].expired())
        {
            m_pSkillHelp[i].lock()->Set_Tick(false);
            m_pSkillHelp[i].lock()->Set_Render(false);
        }

    m_bIsChange.resize(3);

    m_SkillHelpDesc.resize(3);
    m_SkillHelpDesc[0].fCoolTime = 10.f;
    m_SkillHelpDesc[1].fCoolTime = 10.f;
    m_SkillHelpDesc[2].fCoolTime = 10.f;

    m_SkillHelpDesc[0].fAccTime = m_SkillHelpDesc[0].fCoolTime + 1.f;
    m_SkillHelpDesc[1].fAccTime = m_SkillHelpDesc[1].fCoolTime + 1.f;
    m_SkillHelpDesc[2].fAccTime = m_SkillHelpDesc[2].fCoolTime + 1.f;

    return S_OK;
}

void UiCharChange::Tick()
{
    if (m_pOwner.expired())
        return;

    _uint iSize = IDX(m_pObj.size());
    for (_uint i = 0; i < iSize; ++i)
        if (true == m_pObj[i].expired())
            return;

    iSize = IDX(m_pElement.size());
    for (_uint i = 0; i < iSize; ++i)
        if (true == m_pElement[i].expired())
            return;

    Check_Change_Cool();
    Set_Param_Value();
    Change_Hero();

    Check_Skill_Help();
    Change_Skill_Help_Param();
    Use_Skill_Help();
}

void UiCharChange::Set_Hero(_uint iIndex)
{
    if (m_pOwner.expired() || 2 < iIndex)
        return;

    if (true == m_pObj[iIndex].expired() || true == m_pElement[iIndex].expired())
        return;

    // UI의 Index 위치에 업데이트할 영웅 사진 껴주기
    auto& pData = DATAMGR;
    HERO eHero = pData.Get_Cur_Set_Hero(iIndex);

    auto& pResource = RESOURCES;
    if (HERO::MAX == eHero)
    {
        m_pObj[iIndex].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(L"Card_None"), TextureMapType::DIFFUSE);
        m_pElement[iIndex].lock()->Set_Tick(false);
        m_pElement[iIndex].lock()->Set_Render(false);
        m_vecDesc[iIndex].bIsSet = false;

        if (false == m_pSkillHelp[iIndex].expired())
            m_pSkillHelp[iIndex].lock()->Set_Render(false);
    }
    else
    {
        m_pObj[iIndex].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap((pResource.Get<Texture>(GET_DATA(eHero).KeyChangeCard)), TextureMapType::DIFFUSE);
        m_pElement[iIndex].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(GET_ELEMENT(eHero)), TextureMapType::DIFFUSE);
        m_pElement[iIndex].lock()->Set_Tick(true);
        m_pElement[iIndex].lock()->Set_Render(true);
        m_vecDesc[iIndex].bIsSet = true;

        if (false == m_pSkillHelp[iIndex].expired())
        {
            m_pSkillHelp[iIndex].lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap((pResource.Get<Texture>(GET_DATA(eHero).SkillHelp)), TextureMapType::DIFFUSE);
            m_pSkillHelp[iIndex].lock()->Set_Render(true);
        }
    }
}

_bool UiCharChange::IsChangePossible(_uint iIndex)
{
    if (m_pOwner.expired() || 2 < iIndex)
        return false;

    auto eValue = DATAMGR.Get_Cur_Set_Hero(iIndex);
    if (HERO::MAX == eValue)
        return false;

    auto& Desc = m_vecDesc[iIndex];
    if (false == Desc.bIsSet)
        return false;

    if (Desc.fCoolTime < Desc.fAccTime)
    {
        Desc.fAccTime = 0.f;

        return true;
    }

    return false;
}

void UiCharChange::Check_Skill_Help()
{
    _uint iSize = IDX(m_SkillHelpDesc.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& Desc = m_SkillHelpDesc[i];
        if (Desc.fCoolTime > Desc.fAccTime)
            Desc.fAccTime += fDT;
    }
}

void UiCharChange::Change_Skill_Help_Param()
{
    _uint iSize = IDX(m_pSkillHelp.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        m_pSkillHelp[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = m_SkillHelpDesc[i].fAccTime / m_SkillHelpDesc[i].fCoolTime;
        if (1.f <= m_pSkillHelp[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0])
            m_pSkillHelp[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = 1.f;
    }
}

void UiCharChange::Use_Skill_Help()
{
    if (KEYTAP(KEY_TYPE::Q))
    {
        if (true == IsSkillHelp_Available(0))
        {
            auto eValue = DATAMGR.Get_Cur_Set_Hero(0);
            if (HERO::MAX == eValue)
                return;

            auto pPlayer = GET_PLAYER;
            if (nullptr == pPlayer)
                return;

            //if (false == m_pSkillHelp[0].expired() || false == m_pSkillHelp[0].lock()->Is_Render())
            //    return;

            shared_ptr<GameObject> obj = make_shared<GameObject>();
            auto pTransform = pPlayer->GetOrAddTransform();
            obj->GetOrAddTransform()->Set_State(Transform_State::POS, pTransform->Get_State(Transform_State::POS) + pTransform->Get_State(Transform_State::RIGHT) * 3.f);
            obj->GetOrAddTransform()->LookAt(pTransform->Get_State(Transform_State::POS) + pTransform->Get_State(Transform_State::LOOK) * 3.f);

            obj->Add_Component(make_shared<Friend_FSM>(eValue));

            obj->Set_DrawShadow(true);
            obj->Set_VelocityMap(true);
            obj->Set_Name(L"Friend");
            obj->Get_FSM()->Init();

            EVENTMGR.Create_Object(obj);
        }
    }

    if (KEYTAP(KEY_TYPE::E))
    {
        if (true == IsSkillHelp_Available(1))
        {
            auto eValue = DATAMGR.Get_Cur_Set_Hero(1);
            if (HERO::MAX == eValue)
                return;

            auto pPlayer = GET_PLAYER;
            if (nullptr == pPlayer)
                return;

            //if (false == m_pSkillHelp[1].expired() || false == m_pSkillHelp[1].lock()->Is_Render())
            //    return;

            shared_ptr<GameObject> obj = make_shared<GameObject>();
            auto pTransform = pPlayer->GetOrAddTransform();
            obj->GetOrAddTransform()->Set_State(Transform_State::POS, pTransform->Get_State(Transform_State::POS) + pTransform->Get_State(Transform_State::RIGHT) * 3.f);
            obj->GetOrAddTransform()->LookAt(pTransform->Get_State(Transform_State::POS) + pTransform->Get_State(Transform_State::LOOK) * 3.f);

            obj->Add_Component(make_shared<Friend_FSM>(eValue));

            obj->Set_DrawShadow(true);
            obj->Set_VelocityMap(true);
            obj->Set_Name(L"Friend");
            obj->Get_FSM()->Init();

            EVENTMGR.Create_Object(obj);
        }
    }

    if (KEYTAP(KEY_TYPE::R))
    {
        if (true == IsSkillHelp_Available(2))
        {
            auto eValue = DATAMGR.Get_Cur_Set_Hero(2);
            if (HERO::MAX == eValue)
                return;

            auto pPlayer = GET_PLAYER;
            if (nullptr == pPlayer)
                return;

            //if (false == m_pSkillHelp[2].expired() || false == m_pSkillHelp[2].lock()->Is_Render())
            //    return;

            shared_ptr<GameObject> obj = make_shared<GameObject>();
            auto pTransform = pPlayer->GetOrAddTransform();
            obj->GetOrAddTransform()->Set_State(Transform_State::POS, pTransform->Get_State(Transform_State::POS) + pTransform->Get_State(Transform_State::RIGHT) * 3.f);
            obj->GetOrAddTransform()->LookAt(pTransform->Get_State(Transform_State::POS) + pTransform->Get_State(Transform_State::LOOK) * 3.f);

            obj->Add_Component(make_shared<Friend_FSM>(eValue));

            obj->Set_DrawShadow(true);
            obj->Set_VelocityMap(true);
            obj->Set_Name(L"Friend");
            obj->Get_FSM()->Init();

            EVENTMGR.Create_Object(obj);
        }
    }
}

_bool UiCharChange::IsSkillHelp_Available(_uint iIndex)
{
    if (m_pOwner.expired() || 2 < iIndex)
        return false;

    if (false == m_pSkillHelp[iIndex].expired())
        if (false == m_pSkillHelp[iIndex].lock()->Is_Render())
            return false;

    auto& Desc = m_SkillHelpDesc[iIndex];

    if (Desc.fCoolTime < Desc.fAccTime)
    {
        Desc.fAccTime = 0.f;

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
        if (Desc.fCoolTime > Desc.fAccTime)
            Desc.fAccTime += fDT;
    }
}

void UiCharChange::Set_Param_Value()
{
    _uint iSize = IDX(m_pObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        m_pObj[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = m_vecDesc[i].fAccTime / m_vecDesc[i].fCoolTime;
        if (1.f <= m_pObj[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0])
            m_pObj[i].lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = 1.f;
    }
}

void UiCharChange::Change_Hero()
{
    if (KEYTAP(KEY_TYPE::F1))
    {
        if (true == IsChangePossible(0) && false == m_bIsChange[0])
        {
            auto eValue = DATAMGR.Get_Cur_Set_Hero(0);

            if (false == m_pSkillHelp[0].expired())
                m_pSkillHelp[0].lock()->Set_Render(false);
            if (false == m_pSkillHelp[1].expired() && HERO::MAX != DATAMGR.Get_Cur_Set_Hero(1))
                m_pSkillHelp[1].lock()->Set_Render(true);
            if (false == m_pSkillHelp[2].expired() && HERO::MAX != DATAMGR.Get_Cur_Set_Hero(2))
                m_pSkillHelp[2].lock()->Set_Render(true);

            m_bIsChange[0] = true;
            m_bIsChange[1] = false;
            m_bIsChange[2] = false;
            auto pScript = GET_PLAYER->Get_Script<HeroChangeScript>();
            pScript->Change_Hero(eValue);
        }
        else if (true == m_bIsChange[0])
        {
            if (false == m_pSkillHelp[0].expired())
                m_pSkillHelp[0].lock()->Set_Render(true);

            m_bIsChange[0] = false;
            auto pScript = GET_PLAYER->Get_Script<HeroChangeScript>();
            pScript->Change_Player();
        }

    }

    if (KEYTAP(KEY_TYPE::F2))
    {
        if (true == IsChangePossible(1) && false == m_bIsChange[1])
        {
            auto eValue = DATAMGR.Get_Cur_Set_Hero(1);

            if (false == m_pSkillHelp[0].expired() && HERO::MAX != DATAMGR.Get_Cur_Set_Hero(0))
                m_pSkillHelp[0].lock()->Set_Render(true);
            if (false == m_pSkillHelp[1].expired())
                m_pSkillHelp[1].lock()->Set_Render(false);
            if (false == m_pSkillHelp[2].expired() && HERO::MAX != DATAMGR.Get_Cur_Set_Hero(2))
                m_pSkillHelp[2].lock()->Set_Render(true);

            m_bIsChange[0] = false;
            m_bIsChange[1] = true;
            m_bIsChange[2] = false;
            auto pScript = GET_PLAYER->Get_Script<HeroChangeScript>();
            pScript->Change_Hero(eValue);
        }
        else if (true == m_bIsChange[1])
        {
            if (false == m_pSkillHelp[1].expired())
                m_pSkillHelp[1].lock()->Set_Render(true);

            m_bIsChange[1] = false;
            auto pScript = GET_PLAYER->Get_Script<HeroChangeScript>();
            pScript->Change_Player();
        }
    }

    if (KEYTAP(KEY_TYPE::F3))
    {
        if (true == IsChangePossible(2) && false == m_bIsChange[2])
        {
            auto eValue = DATAMGR.Get_Cur_Set_Hero(2);

            if (false == m_pSkillHelp[0].expired() && HERO::MAX != DATAMGR.Get_Cur_Set_Hero(0))
                m_pSkillHelp[0].lock()->Set_Render(true);
            if (false == m_pSkillHelp[1].expired() && HERO::MAX != DATAMGR.Get_Cur_Set_Hero(1))
                m_pSkillHelp[1].lock()->Set_Render(true);
            if (false == m_pSkillHelp[2].expired())
                m_pSkillHelp[2].lock()->Set_Render(false);

            m_bIsChange[0] = false;
            m_bIsChange[1] = false;
            m_bIsChange[2] = true;
            auto pScript = GET_PLAYER->Get_Script<HeroChangeScript>();
            pScript->Change_Hero(eValue);
        }
        else if (true == m_bIsChange[2])
        {
            if (false == m_pSkillHelp[2].expired())
                m_pSkillHelp[2].lock()->Set_Render(true);

            m_bIsChange[2] = false;
            auto pScript = GET_PLAYER->Get_Script<HeroChangeScript>();
            pScript->Change_Player();
        }
    }
}
