#pragma once
#include "MonoBehaviour.h"

class UiQuestController : public MonoBehaviour
{
public:
    UiQuestController();

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
    virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<UiQuestController>(); }
    void Create_Dialog(NPCTYPE eType, QUESTINDEX eIndex, shared_ptr<GameObject> obj);
    void Next_Dialog();
    void Change_Value();

    _bool Get_Dialog_End() { return m_bIsCreated; }
    CUR_QUEST Get_CurState(QUESTINDEX eIndex);
    _bool Get_QuestState(QUESTINDEX eIndex) { return m_ClearCheck[_int(eIndex)]; }

    shared_ptr<GameObject> Get_AccessObject() { return m_pAccessObject.lock(); }

private:
    void Clear_Quest();
    void Remove_Dialog();
    void Move_Next();
    void Next_Down();
    void Next_Up();
    void Set_Cur_Quest();
    void Set_Render(_bool _bValue);

private:
    weak_ptr<GameObject> m_pUiTotalController;
    weak_ptr<GameObject> m_pUiCurQuest;
    weak_ptr<GameObject> m_pUiCurQuestName;
    weak_ptr<GameObject> m_pUiCurQuestInfo;
    weak_ptr<GameObject> m_pUiCurQuestCount;

    weak_ptr<GameObject> m_pNext;
    weak_ptr<GameObject> m_pNpcName;
    weak_ptr<GameObject> m_pNpcBg;
    weak_ptr<GameObject> m_pNpcDialog;
    weak_ptr<GameObject> m_pPlayerBg;
    weak_ptr<GameObject> m_pPlayerDialog;

    weak_ptr<GameObject> m_pAccessObject;

    enum class NEXT_STATE { DOWN, UP };
    NEXT_STATE          m_eState            = { NEXT_STATE::DOWN };
    _float              m_fCheckTime        = {};
    _float              m_fMaxTime          = {};
    _float              m_fSpeed            = {};

    QUESTINDEX          m_eIndex            = {};
    QUESTDATA           m_tagCurQuestData   = {};
    _bool               m_bIsCreated        = {};
    _uint               m_iMaxIndex         = {};
    _uint               m_iCurIndex         = {};

    vector<_bool>       m_ClearCheck;
    
    CUR_QUEST           m_eCurType          = {};
    CUR_QUEST           m_ePreType          = {};
};

