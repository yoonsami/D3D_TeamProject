#pragma once
#include "MonoBehaviour.h"

class CoolTimeCheckScript : public MonoBehaviour
{
public:
    CoolTimeCheckScript();

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<CoolTimeCheckScript>(); }

    void Start_Attack_Button_Effect();
    void Next_Combo(SkillType eSkillType);

    _bool GetIsWork()   { return m_bIsSkillWork; }
    HERO Get_Cur_Hero() { return m_eCurHero; }
    
    _bool IsAvailable(SkillType eSkillType);

    void Set_Skill_End();
    void Set_Cur_Hero(HERO eType);

private:
    void Check_Cool_Time();
    void Change_Skills_Value();
    void Start_Effect(SkillType eSkillType);
    void Start_ButtonEndEffect(_uint iIndex);
    
    void Change_Pos_Two_Letter(_uint iIndex);
    void Change_Pos_One_Letter(_uint iIndex);
    void Change_Text_Zero(_uint iIndex);

    void Change_Skill_Info(HERO eType);

private:
    vector<vector<TOTALSKILLINFO>>      m_CoolTime;
    vector<TOTALSKILLINFO>              m_tagDefault;

    //vector<vector<wstring>>             m_TextureKey;
    vector<weak_ptr<GameObject>>        m_pUiSkill;
    vector<weak_ptr<GameObject>>        m_pUiSkill_Effect;
    vector<weak_ptr<GameObject>>        m_pUi_Cool_End;
    
    vector<weak_ptr<GameObject>>        m_pUi_Skill0_Combo;
    vector<weak_ptr<GameObject>>        m_pUi_Skill2_Combo;
    vector<weak_ptr<GameObject>>        m_pUi_Skill3_Combo;
    vector<weak_ptr<GameObject>>        m_pUi_Skill4_Combo;
    vector<weak_ptr<GameObject>>        m_pUi_Skill5_Combo;
    vector<weak_ptr<GameObject>>        m_pUi_Skill6_Combo;
    
    vector<weak_ptr<GameObject>>        m_pUi_Skill_Type;

    vector<_bool>                       m_bIsCoolChangeOne;
    vector<weak_ptr<GameObject>>        m_pUi_Skill_Cool;

    weak_ptr<GameObject>                m_pCombo_Effect;

    COOLTIMEINFO                        m_tagEvade              = {};
    HERO                                m_eCurHero              = { HERO::MAX };
    _uint                               m_iWorkSkillIndex       = { 0 };
    _bool                               m_bIsSkillWork          = { false };

    vector<vector<weak_ptr<Texture>>>   m_pTexture;
    weak_ptr<Texture>                   m_pCombo0;
    weak_ptr<Texture>                   m_pCombo1;
    weak_ptr<Texture>                   m_pCharge;
    weak_ptr<Texture>                   m_pHold;
    weak_ptr<Texture>                   m_pMove;
    /* const resources */

};

