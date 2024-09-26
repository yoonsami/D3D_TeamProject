#pragma once
#include "Component.h"

class  MonoBehaviour : public Component
{
public:
    MonoBehaviour();
    virtual ~MonoBehaviour();

    virtual HRESULT Init()      override;
    virtual void Tick()         override;
    virtual void Late_Tick()    override;
    virtual void Final_Tick()   override;

    virtual shared_ptr<MonoBehaviour> Copy_Script() = 0;
    /* Setter */ 
    void            Set_Tag(const wstring& wstrTag) { m_wstrTag = wstrTag; }

    /* Getter */
    const wstring&  Get_Tag() { return m_wstrTag; }

protected:
    _bool           m_bIsInit = { false };
    wstring         m_wstrTag = { L"" };
};
