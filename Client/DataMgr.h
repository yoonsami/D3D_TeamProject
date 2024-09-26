#pragma once

class DataMgr
{
    DECLARE_SINGLETON(DataMgr)

public:
    void Initialize();
    
    const HERODATA&     Get_Data(HERO eHero)                                { return m_HeroData[IDX(eHero)]; }
    wstring             Get_Element(HERO eHero)                             { return m_KeyElement[m_HeroData[IDX(eHero)].Element] + L"0"; }
    wstring             Get_Element_Line(HERO eHero)                        { return m_KeyElement[m_HeroData[IDX(eHero)].Element] + L"1"; }

    const BOSSDATA&     Get_Data(BOSS eBoss)                                { return m_BossData[IDX(eBoss)]; }
    wstring             Get_Element(BOSS eBoss)                             { return m_KeyElement[m_BossData[IDX(eBoss)].Element] + L"0"; }
    wstring             Get_Element_Line(BOSS eBoss)                        { return m_KeyElement[m_BossData[IDX(eBoss)].Element] + L"1"; }

    const wstring&      Get_Npc_Name(NPCTYPE eType)                         { return m_NpcName[IDX(eType)]; }
    
    _uint               Get_Dialog_Size(QUESTINDEX eIndex, CUR_QUEST eType);
    const wstring&      Get_Dialog(QUESTINDEX eIndex, CUR_QUEST eType, _uint iIndex);
    const QUESTDATA&    Get_Quest(QUESTINDEX eIndex)                        { return m_Quest[IDX(eIndex)]; }

    _uint               Get_Boss_Dialog_Size(BOSS eType)                    { return IDX(m_BossDialog[IDX(eType)].size()); }
    const wstring&      Get_Boss_Dialog(BOSS eType, _uint iIndex)           { return m_BossDialog[IDX(eType)][iIndex]; }

    const wstring&      Get_NoQuest_Dialog(QUESTINDEX eIndex)               { return m_NoQuestDialog[IDX(eIndex)]; }
    const wstring&      Get_NoAccept_Dialog(QUESTINDEX eIndex)              { return m_NoAcceptDialog[IDX(eIndex)]; }

    const HERODATA&     Get_Data(DUMMY_HERO eHero)                          { return m_DummyData[IDX(eHero)]; }
    wstring             Get_Element(DUMMY_HERO eHero)                       { return m_KeyElement[m_DummyData[IDX(eHero)].Element] + L"0"; }
    wstring             Get_Element_Line(DUMMY_HERO eHero)                  { return m_KeyElement[m_DummyData[IDX(eHero)].Element] + L"1"; }

    _uint               Get_Card_Inven_Max_Size()                           { return IDX(m_CardInven.size()); }
    _uint               Get_Card_Inven_Use_Size();
    HERODATA&           Get_Card_Inven(_uint iIndex);
    wstring             Get_Card_Inven_Element(_uint iIndex)                { return m_KeyElement[m_CardInven[iIndex].second.Element] + L"0"; }
    wstring             Get_Card_Inven_Element_Line(_uint iIndex)           { return m_KeyElement[m_CardInven[iIndex].second.Element] + L"1"; }
    void                Set_Card_Inven(HERO eType);
    void                Set_Card_Inven(DUMMY_HERO eType);
    void                Remove_Card_Inven(_uint iIndex);

    HERO                Get_Cur_Set_Hero(_uint iIndex);
    void                Set_Cur_Hero(_uint iIndex, _uint iCardIndex);
    void                Remove_Cur_Hero(_uint iIndex);

    void                Sort_By_Star();

    _uint               Get_NpcDialog_Count(NPCTYPE eType)                  { return IDX(m_NpcDialog[IDX(eType)].size()); }
    _uint               Get_Dialog_Size(NPCTYPE eType, _uint iDialogIndex)  { return IDX(m_NpcDialog[IDX(eType)][iDialogIndex].size()); }
    const wstring&      Get_Dialog(NPCTYPE eType, _uint iDialogIndex, _uint iIndex) { return m_NpcDialog[IDX(eType)][iDialogIndex][iIndex]; }


private:
    vector<HERODATA>                m_HeroData;
    vector<wstring>                 m_KeyElement;
    vector<BOSSDATA>                m_BossData;
    vector<wstring>                 m_NpcName;
                                    
    vector<wstring>                 m_DialogGameClear;
    vector<vector<wstring>>         m_DialogAccept;
    vector<vector<wstring>>         m_DialogNoClear;
    vector<vector<wstring>>         m_DialogClear;
    vector<QUESTDATA>               m_Quest;
                                    
    vector<vector<wstring>>         m_BossDialog;
                                    
    vector<wstring>                 m_NoQuestDialog;
    vector<wstring>                 m_NoAcceptDialog;
                                    
    vector<HERODATA>                m_DummyData;

    vector<pair<_bool, HERODATA>>   m_CardInven;
    vector<_int>                    m_CurSetHero;

    vector<vector<vector<wstring>>> m_NpcDialog;

    wstring                         strTemp;
    HERODATA                        tagTemp;
};

