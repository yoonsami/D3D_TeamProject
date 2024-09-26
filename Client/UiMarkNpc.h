#pragma once
#include "MonoBehaviour.h"

class UiMarkNpc : public MonoBehaviour
{
public:
    UiMarkNpc(NPCTYPE eType);


public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
    virtual void Final_Tick() override;

    virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<UiMarkNpc>(m_eType); }
    void Delete_Mark();
    void Change_Render(_bool bValue);
    void Change_Set_On(_bool bValue);

private:
    void Check_Distance();
    void Check_In_Screen();
    void Update_Pos();

private:
    weak_ptr<GameObject>    m_pCamera;
    weak_ptr<GameObject>    m_pMark;
    
    NPCTYPE     m_eType     = { NPCTYPE::MAX };
    _bool       m_bIsRender = {};
    _bool       m_bSetOn    = {};
};

