#pragma once
#include "MonoBehaviour.h"

class UiDialogController : public MonoBehaviour
{
public:
    UiDialogController();

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
    virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<UiDialogController>(); }
    void Create_Dialog(NPCTYPE eType, shared_ptr<GameObject> obj);
    void Next_Dialog();

    _bool Get_Dialog_End() { return m_bIsCreated; }
    shared_ptr<GameObject> Get_AccessObject() { return m_pAccessObject.lock(); }

private:
    void Remove_Dialog();
    void Move_Next();
    void Next_Down();
    void Next_Up();

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

    _bool               m_bIsCreated        = {};
    _uint               m_iMaxIndex         = {};
    _uint               m_iCurIndex         = {};

    NPCTYPE             m_eNpcType          = {};
    _uint               m_iDialogIndex      = {};
};

