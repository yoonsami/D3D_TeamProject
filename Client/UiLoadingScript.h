#pragma once
#include "MonoBehaviour.h"

class UiLoadingScript : public MonoBehaviour
{
public:
    UiLoadingScript(_bool bIsEven);

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<UiLoadingScript>(m_bIsEven); }

    void Change_Texture();

private:
    void On_Idle();
    void Off();
    void Change();
    void Off_Idle();
    void On();

private:
    static _uint    iTextureIndex;
    _uint           m_iMaxIndex         = {};
    _bool           m_bIsEven           = { false };
    wstring         m_strTextureName;

    enum class CHANGE_TYPE { ON_IDLE, OFF, CHANGE, OFF_IDLE, ON };
    CHANGE_TYPE     m_eType = { CHANGE_TYPE::ON_IDLE };

    _float          m_fMaxIdleTime      = {};
    _float          m_fOnOffTime        = {};
    _float          m_fCheckTime        = {};

};

