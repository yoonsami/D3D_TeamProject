#include "pch.h"
#include "UiSkillGauge.h"

#include "MeshRenderer.h"

UiSkillGauge::UiSkillGauge()
{
}

HRESULT UiSkillGauge::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;

    auto pScene = CUR_SCENE;
    m_pGaugeBg  = pScene->Get_UI(L"UI_Skill_Use_GaugeBg");
    if (true == m_pGaugeBg.expired())
        return E_FAIL;

    m_pOwner.lock()->Set_Tick(false);
    m_pGaugeBg.lock()->Set_Tick(false);
    m_pOwner.lock()->Set_Render(false);
    m_pGaugeBg.lock()->Set_Render(false);
    m_bIsRender = false;

    m_fMinGauge = 0.44f;
    m_fMaxGauge = 0.08f;
    m_fValue = m_fMinGauge - m_fMaxGauge;

    return S_OK;
}

void UiSkillGauge::Change_Ratio(_float fRatio)
{
    if (true == m_pOwner.expired() ||
        true == m_pGaugeBg.expired())
        return;

    _float fFinalRatio = 0.f;
    switch (m_eInfo)
    {
    case CHARGING:
        fFinalRatio = m_fMinGauge - m_fValue * fRatio;
        break;
    case HOLDING:
        fFinalRatio = m_fMaxGauge + m_fValue * fRatio;
        break;
    }
    
    m_pOwner.lock()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[0] = fFinalRatio;
}

void UiSkillGauge::Change_Render(_bool bSet, SkillInfo eInfo)
{
    if (true == m_pOwner.expired() ||
        true == m_pGaugeBg.expired())
        return;

    m_bIsRender = bSet;
    m_pOwner.lock()->Set_Tick(bSet);
    m_pGaugeBg.lock()->Set_Tick(bSet);
    m_pOwner.lock()->Set_Render(bSet);
    m_pGaugeBg.lock()->Set_Render(bSet);

    if (SkillInfo::NONE != eInfo)
    {
        m_eInfo = eInfo;
        if (true == bSet)
        {
            wstring strGaugeName;
            wstring strGaugeBgName;

            switch (m_eInfo)
            {
            case CHARGING:
                strGaugeName = L"UI_Skill_Gauge_Charge";
                strGaugeBgName = L"UI_Skill_Gauge_Bg1";
                break;
            case HOLDING:
                strGaugeName = L"UI_Skill_Gauge_Hold";
                strGaugeBgName = L"UI_Skill_Gauge_Bg0";
                break;
            }
            m_pGaugeBg.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(strGaugeBgName), TextureMapType::DIFFUSE);
            m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(strGaugeName), TextureMapType::DIFFUSE);
        }
    }
}
