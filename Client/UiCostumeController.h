#pragma once
#include "MonoBehaviour.h"

class UiCostumeController : public MonoBehaviour
{
public:
    enum class COSTUME_TYPE { HAIR, UNIFORM };

public:
    UiCostumeController();

public:
    virtual HRESULT Init() override;
    virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<UiCostumeController>(); }

    virtual void Tick() override;

    void Create_Costume();
    void Remove_Costume();
    void Change_Costume_Type(COSTUME_TYPE eType);

    void Change_Costume();
    void Select_Inven(_uint iIndex);
    void Change_Costume_Have(_uint iIndex);

private:


private:
    weak_ptr<GameObject>            m_pController;
    vector<weak_ptr<GameObject>>    m_vecAddedObj;

    COSTUME_TYPE        m_eType = { COSTUME_TYPE::HAIR };
    _bool               m_bIsCreated        = {};

    vector<pair<wstring, wstring>>  m_vecHair;
    vector<pair<wstring, wstring>>  m_vecUniform;

    pair<_uint, _uint>  m_OriginSet;
    pair<_uint, _uint>  m_TempSet;

    vector<_bool>       m_bIsHave;
};

