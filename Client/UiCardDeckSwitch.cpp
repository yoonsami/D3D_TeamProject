#include "pch.h"
#include "UiCardDeckSwitch.h" 

#include "MeshRenderer.h"
#include "FontRenderer.h"

UiCardDeckSwitch::UiCardDeckSwitch(_bool bIsFont)
    : m_bIsFont(bIsFont)
{
}

HRESULT UiCardDeckSwitch::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    m_fMaxTime = 2.f;
    m_fRatio = 1.f / m_fMaxTime;

    return S_OK;
}

void UiCardDeckSwitch::Tick()
{
	if (m_pOwner.expired())
		return;

    Change_Time();

    if (true == m_bIsEnd)
        Remove_Owner();
    else if (false == m_bIsFont)
        Change_Alpha();
    else
        Change_Color();
}

void UiCardDeckSwitch::Change_Time()
{
    m_fCheckTime += fDT;
    if (m_fMaxTime < m_fCheckTime)
        m_bIsEnd = true;
}

void UiCardDeckSwitch::Change_Alpha()
{
    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f - m_fCheckTime * m_fRatio;
}

void UiCardDeckSwitch::Change_Color()
{
    _float fValue = 1.f - m_fCheckTime * m_fRatio;
    m_pOwner.lock()->Get_FontRenderer()->Get_Color() = Color(fValue, fValue, fValue, 0.f);
}

void UiCardDeckSwitch::Remove_Owner()
{
    EVENTMGR.Delete_Object(m_pOwner.lock());
}
