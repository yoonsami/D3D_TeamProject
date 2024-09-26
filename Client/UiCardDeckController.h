#pragma once
#include "MonoBehaviour.h"

class UiCardDeckController : public MonoBehaviour
{
public:
    UiCardDeckController();

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<UiCardDeckController>(); }

    void Set_Render(_bool bValue);

    void Click_Deck_Select(wstring strObjName);
    void Click_Deck_Inven(wstring strInvenName);
    void Click_Deck_X(wstring strObjName);
    void Click_Info_Set(_uint iIndex);

    void Remove_Info();

private:
    void Set_Card();
    void Create_Switch_Complete(_bool bValue);

    void Create_Info(_uint iIndex);
    void Set_Font(_uint iIndex);
    void Remove_Font(_uint iIndex);

    void Create_Select_Mark();
    void Remove_Select_Mark();
    void Move_Select_Mark();


private:
    weak_ptr<GameObject>            m_pCharChange;

    vector<weak_ptr<GameObject>>    m_vecCardDeckObj;
    vector<weak_ptr<GameObject>>    m_vecInvenObj;
    vector<weak_ptr<GameObject>>    m_vecCardDeckBg;
    vector<weak_ptr<GameObject>>    m_vecCardDeckElement;
    vector<weak_ptr<GameObject>>    m_vecFont;
    
    vector<weak_ptr<GameObject>>    m_vecAddedObj;
    vector<weak_ptr<GameObject>>    m_vecSelectMark;

    _bool       m_bIsRender             = { false };
    _uint       m_iPickingIndex         = { 0 };
    _uint       m_iPreSize              = {};

    _bool       m_bIsInfoCreate         = {};

    _bool       m_bIsClickSet           = {};
    _uint       m_iSetIndex             = {};

    _bool       m_bIsSelectMarkCreate   = {};
    
    _bool       m_bIsMoveDown           = {};
    _float      m_fCheckTime            = {};
    _float      m_fMaxTime              = {};
    _float      m_fSpeed                = {};
};

