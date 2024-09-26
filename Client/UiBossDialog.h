#pragma once
#include "MonoBehaviour.h"

class UiBossDialog : public MonoBehaviour
{
public:
    UiBossDialog();

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<UiBossDialog>(); }

    void Create_Dialog(BOSS eType);
    void Next_Dialog();

    _bool Is_Finish();

private:
    void Remove_Dialog();

    void Move_Next();
    void Next_Down();
    void Next_Up();

private:
    weak_ptr<GameObject> m_pUiMouseController;
    weak_ptr<GameObject> m_pUiTotalController;
    weak_ptr<GameObject> m_pNext;
    weak_ptr<GameObject> m_pNpcName;
    weak_ptr<GameObject> m_pNpcBg;
    weak_ptr<GameObject> m_pNpcDialog;
    weak_ptr<GameObject> m_pPlayerBg;
    weak_ptr<GameObject> m_pPlayerDialog;

    _bool           m_bIsCreated    = {};
    _uint           m_iMaxIndex     = {};
    _uint           m_iCurIndex     = {};

    enum class NEXT_STATE { DOWN, UP };
    NEXT_STATE      m_eState        = { NEXT_STATE::DOWN };
    _float          m_fCheckTime    = {};
    _float          m_fMaxTime      = {};
    _float          m_fSpeed        = {};

    BOSS            m_eType         = {};
    _bool           m_bIsFinish     = {};
};

