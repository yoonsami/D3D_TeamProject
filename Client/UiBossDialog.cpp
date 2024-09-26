#include "pch.h"
#include "UiBossDialog.h" 

#include "BaseUI.h"
#include "MeshRenderer.h"
#include "FontRenderer.h"
#include "MainUiController.h"
#include "UiMouseController.h"

UiBossDialog::UiBossDialog()
{
}

HRESULT UiBossDialog::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;
    m_bIsCreated = false;

    auto pScene = CUR_SCENE;
    m_pUiTotalController = pScene->Get_UI(L"Main_UI_Controller");
    m_pUiMouseController = pScene->Get_GameObject(L"UI_Mouse_Click");

    m_iMaxIndex = m_iCurIndex = 0;
    m_bIsFinish = false;

    return S_OK;
}

void UiBossDialog::Tick()
{
    if (true == m_pOwner.expired() ||
        true == m_pNext.expired() ||
        true == m_pNpcName.expired() ||
        true == m_pNpcBg.expired() ||
        true == m_pNpcDialog.expired() ||
        true == m_pPlayerBg.expired() ||
        true == m_pPlayerDialog.expired())
        return;

    Move_Next();
}

void UiBossDialog::Create_Dialog(BOSS eType)
{
    if (true == m_bIsCreated)
        return;

    m_eType = eType;
    m_bIsCreated = true;

    auto pScene = CUR_SCENE;
    vector<weak_ptr<GameObject>> addedObj;
    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Dialog.dat", addedObj);

    m_pUiMouseController.lock()->Get_Script<UiMouseController>()->Change_Mouse_State(true);

    _uint iSize = IDX(addedObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        if (true == addedObj[i].expired())
            continue;

        wstring strName = addedObj[i].lock()->Get_Name();
        if (L"UI_Dialog_Next" == strName)
            m_pNext = addedObj[i];
        else if (L"UI_Dialog_Npc_Name" == strName)
            m_pNpcName = addedObj[i];
        else if (L"UI_Dialog_Npc_Bg" == strName)
            m_pNpcBg = addedObj[i];
        else if (L"UI_Dialog_Npc" == strName)
            m_pNpcDialog = addedObj[i];
        else if (L"UI_Dialog_Player_Bg" == strName)
            m_pPlayerBg = addedObj[i];
        else if (L"UI_Dialog_Player" == strName)
            m_pPlayerDialog = addedObj[i];
    }


    if (false == m_pNpcName.expired())
    {

        wstring strNpcName = GET_DATA(eType).Name;
        m_pNpcName.lock()->Get_FontRenderer()->Get_Text() = strNpcName;
        _float4 vecPos = m_pNpcName.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
        vecPos.x = (strNpcName.length() / 2.f) * -28.f;
        m_pNpcName.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    }
    if (false == m_pNpcDialog.expired())
    {

        m_iMaxIndex = DATAMGR.Get_Boss_Dialog_Size(eType);
        m_pNpcDialog.lock()->Get_FontRenderer()->Get_Text() = DATAMGR.Get_Boss_Dialog(eType, 0);
        m_pNpcDialog.lock()->Get_FontRenderer()->Set_TimePerChar(0.05f);
    }
    if (false == m_pNext.expired())
    {

        m_pNext.lock()->Get_Button()->AddOnClickedEvent([&]()
            {
                this->Next_Dialog();
            });
    }
    if (false == m_pUiTotalController.expired())
    {

        m_pUiTotalController.lock()->Get_Script<MainUiController>()->Set_MainUI_Render(false);
    }


}

void UiBossDialog::Next_Dialog()
{
    if (false == m_pNpcDialog.lock()->Get_FontRenderer()->Is_Finished())
    {
        m_pNpcDialog.lock()->Get_FontRenderer()->Set_Finished();
        return;
    }

    ++m_iCurIndex;
    if (m_iMaxIndex <= m_iCurIndex)
    {
        m_bIsFinish = true;
        m_iCurIndex = 0;
        Remove_Dialog();

        if (false == m_pUiTotalController.expired())
        {
            m_pUiTotalController.lock()->Get_Script<MainUiController>()->Set_MainUI_Render(true);
        }
    }

    else
    {
        m_pNpcDialog.lock()->Get_FontRenderer()->Get_Text() = DATAMGR.Get_Boss_Dialog(m_eType, m_iCurIndex);
        m_pNpcDialog.lock()->Get_FontRenderer()->Set_TimePerChar(0.05f);
    }
}

_bool UiBossDialog::Is_Finish()
{
    if (true == m_bIsCreated && true == m_bIsFinish)
    {
        m_bIsCreated = false;
        m_bIsFinish = false;

        m_pUiMouseController.lock()->Get_Script<UiMouseController>()->Change_Mouse_State(false);

        return true;
    }

    else
        return false;
}

void UiBossDialog::Remove_Dialog()
{
    if (false == m_pNext.expired() ||
        false == m_pNpcName.expired() ||
        false == m_pNpcBg.expired() ||
        false == m_pNpcDialog.expired() ||
        false == m_pPlayerBg.expired() ||
        false == m_pPlayerDialog.expired())
    {
        auto pScene = CUR_SCENE;
        EVENTMGR.Delete_Object(m_pNext.lock());
        EVENTMGR.Delete_Object(m_pNpcName.lock());
        EVENTMGR.Delete_Object(m_pNpcBg.lock());
        EVENTMGR.Delete_Object(m_pNpcDialog.lock());
        EVENTMGR.Delete_Object(m_pPlayerBg.lock());
        EVENTMGR.Delete_Object(m_pPlayerDialog.lock());
    }
}

void UiBossDialog::Move_Next()
{
    switch (m_eState)
    {
    case NEXT_STATE::DOWN:
        Next_Down();
        break;
    case NEXT_STATE::UP:
        Next_Up();
        break;
    }
}

void UiBossDialog::Next_Down()
{
    m_fCheckTime += fDT;
    if (m_fMaxTime < m_fCheckTime)
    {
        m_fCheckTime = 0.f;
        m_eState = NEXT_STATE::UP;

        return;
    }

    _float PosY = -460 - m_fSpeed * m_fCheckTime / m_fMaxTime;
    _float4 vecPos = m_pNext.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.y = PosY;
    m_pNext.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
}

void UiBossDialog::Next_Up()
{
    m_fCheckTime += fDT;
    if (m_fMaxTime < m_fCheckTime)
    {
        m_fCheckTime = 0.f;
        m_eState = NEXT_STATE::DOWN;

        return;
    }

    _float PosY = -480 + m_fSpeed * m_fCheckTime / m_fMaxTime;
    _float4 vecPos = m_pNext.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.y = PosY;
    m_pNext.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
}
