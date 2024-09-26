#include "pch.h"
#include "MotionTrailDisappear.h"
#include "MotionTrailRenderer.h"

MotionTrailDisappear::MotionTrailDisappear(_float fDisappearSpeed, const Color& vInitColor)
    : m_fDisappearSpeed(fDisappearSpeed)
    , m_vInitColor(vInitColor)
{
}

HRESULT MotionTrailDisappear::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    m_pOwner.lock()->Get_MotionTrailRenderer()->Get_RenderParamDesc().SetVec4(0, m_vInitColor);

    return S_OK;
}

void MotionTrailDisappear::Tick()
{
	if (m_pOwner.expired())
		return;

    m_fAcc += m_fDisappearSpeed * fDT;

	m_pOwner.lock()->Get_MotionTrailRenderer()->Get_RenderParamDesc().vec4Params[0].w -= m_fDisappearSpeed * fDT;

    if(m_fAcc >= 1.f)
        EVENTMGR.Delete_Object(Get_Owner());
}
