#include "pch.h"
#include "UiDialogController.h" 

#include "BaseUI.h"
#include "MeshRenderer.h"
#include "FontRenderer.h"
#include "MainUiController.h"

UiDialogController::UiDialogController()
{
}

HRESULT UiDialogController::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;

    m_fSpeed = 20.f;
    m_fMaxTime = 1.f;

    auto pScene = CUR_SCENE;
    m_pUiTotalController =  pScene->Get_UI(L"Main_UI_Controller");
    m_pUiCurQuest =         pScene->Get_UI(L"UI_Cur_Quest");
    m_pUiCurQuestName =     pScene->Get_UI(L"UI_Cur_Quest_Name");
    m_pUiCurQuestInfo =     pScene->Get_UI(L"UI_Cur_Quest_Info");
    m_pUiCurQuestCount =    pScene->Get_UI(L"UI_Cur_Quest_Count");

    m_bIsCreated = false;
    m_iMaxIndex = m_iCurIndex = 0;

    return S_OK;
}

void UiDialogController::Tick()
{
	if (true == m_pOwner.expired()      ||
        true == m_pNext.expired()       ||
        true == m_pNpcName.expired()    ||
        true == m_pNpcBg.expired()      ||
        true == m_pNpcDialog.expired()  ||
        true == m_pPlayerBg.expired()   ||
        true == m_pPlayerDialog.expired())
		return;

    Move_Next();
}

void UiDialogController::Create_Dialog(NPCTYPE eType, shared_ptr<GameObject> obj)
{
    if (true == m_bIsCreated)
        return;

    m_pAccessObject = obj;

    m_bIsCreated = true;
    auto pScene = CUR_SCENE;
    vector<weak_ptr<GameObject>> addedObj;
    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Dialog.dat", addedObj);

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

    m_eNpcType = eType;

    if (false == m_pNpcName.expired())
    {
        // npc type 정하기
        wstring strNpcName = GET_NPC_NAME(m_eNpcType);
        m_pNpcName.lock()->Get_FontRenderer()->Get_Text() = strNpcName;
        _float4 vecPos = m_pNpcName.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
        vecPos.x = (strNpcName.length() / 2.f) * -28.f;
        m_pNpcName.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    }


    if (false == m_pNpcDialog.expired())
    {
        m_iDialogIndex = rand() % DATAMGR.Get_NpcDialog_Count(m_eNpcType);
        m_iMaxIndex = DATAMGR.Get_Dialog_Size(m_eNpcType, m_iDialogIndex);
        m_pNpcDialog.lock()->Get_FontRenderer()->Get_Text() = DATAMGR.Get_Dialog(m_eNpcType, m_iDialogIndex, m_iCurIndex);
        m_pNpcDialog.lock()->Get_FontRenderer()->Set_TimePerChar(0.05f);
    }
        
    if (false == m_pNext.expired())
    {
        // 버튼 실행 함수 지정
        m_pNext.lock()->Get_Button()->AddOnClickedEvent([&]()
            {
                this->Next_Dialog();
            });
    }
    if (false == m_pUiTotalController.expired())
    {
        // 대화 하는 동안 다 끄기
        m_pUiTotalController.lock()->Get_Script<MainUiController>()->Set_MainUI_Render(false);
    }
}

void UiDialogController::Remove_Dialog()
{
    if (false == m_pNext.expired()      ||
        false == m_pNpcName.expired()   ||
        false == m_pNpcBg.expired()     ||
        false == m_pNpcDialog.expired() ||
        false == m_pPlayerBg.expired()  ||
        false == m_pPlayerDialog.expired())
    {
        m_bIsCreated = false;
        auto pScene = CUR_SCENE;
       EVENTMGR.Delete_Object(m_pNext.lock());
       EVENTMGR.Delete_Object(m_pNpcName.lock());
       EVENTMGR.Delete_Object(m_pNpcBg.lock());
       EVENTMGR.Delete_Object(m_pNpcDialog.lock());
       EVENTMGR.Delete_Object(m_pPlayerBg.lock());
       EVENTMGR.Delete_Object(m_pPlayerDialog.lock());
    }
}

void UiDialogController::Next_Dialog()
{
    // 인덱스 최대 비교후 미만이면 대화 가져오고 최대일때는 메인유아이 키고 다 지우기 
    if (false == m_pNpcDialog.lock()->Get_FontRenderer()->Is_Finished())
    {
        m_pNpcDialog.lock()->Get_FontRenderer()->Set_Finished();
        return;
    }

    ++m_iCurIndex;
    if (m_iMaxIndex <= m_iCurIndex)
    {
        m_iCurIndex = 0;
        
        Remove_Dialog();

        if (false == m_pUiTotalController.expired())
        {
            m_pUiTotalController.lock()->Get_Script<MainUiController>()->Set_MainUI_Render(true);
        }
    }

    else
    {
        m_pNpcDialog.lock()->Get_FontRenderer()->Get_Text() = DATAMGR.Get_Dialog(m_eNpcType, m_iDialogIndex, m_iCurIndex);
        m_pNpcDialog.lock()->Get_FontRenderer()->Set_TimePerChar(0.05f);
    }

}

void UiDialogController::Move_Next()
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

void UiDialogController::Next_Down()
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

void UiDialogController::Next_Up()
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