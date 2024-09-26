#include "pch.h"
#include "UiSettingController.h" 

#include "BaseUI.h"
#include "MeshRenderer.h"
#include "FontRenderer.h"

UiSettingController::UiSettingController()
{
}

HRESULT UiSettingController::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;

    m_bIsRender = false;

    auto pScene = CUR_SCENE;
    m_pBg               = pScene->Get_UI(L"UI_Setting_Bg");
    m_pBrightness       = pScene->Get_UI(L"UI_Setting_Star_Brightness");
    m_pContrast         = pScene->Get_UI(L"UI_Setting_Star_Contrast");
    m_pSaturation       = pScene->Get_UI(L"UI_Setting_Star_Saturation");
    m_pBloom            = pScene->Get_UI(L"UI_Setting_Star_Bloom");
    m_pToneMapping      = pScene->Get_UI(L"UI_Setting_Star_ToneMapping");
    m_pToneMappingMax   = pScene->Get_UI(L"UI_Setting_Star_ToneMapping_Brightness");
    m_pSSAO_Radius      = pScene->Get_UI(L"UI_Setting_Star_SSAO_Radius");
    m_pSSAO_Start       = pScene->Get_UI(L"UI_Setting_Star_SSAO_Start");
    m_pSSAO_End         = pScene->Get_UI(L"UI_Setting_Star_SSAO_End");
    m_pMotionBlur       = pScene->Get_UI(L"UI_Setting_Star_MotionBlur");
    //m_pPBR_Attenuation  = pScene->Get_UI(L"UI_Setting_Star__PBR_Attenuation");
    //m_pPBR_Ratio        = pScene->Get_UI(L"UI_Setting_Star__PBR_Ratio");
    //m_pPBR_ShadowBius   = pScene->Get_UI(L"UI_Setting_Star__PBR_ShadowBius");

    m_pCheck_Bloom      = pScene->Get_UI(L"UI_Setting_Check_Bloom");
    m_pCheck_SSAO       = pScene->Get_UI(L"UI_Setting_Check_SSAO");
    m_pCheck_MotionBlur = pScene->Get_UI(L"UI_Setting_Check_MotionBlur");
    m_pCheck_LensFlare  = pScene->Get_UI(L"UI_Setting_Check_LensFlare");
    m_pCheck_Outline    = pScene->Get_UI(L"UI_Setting_Check_Outline");
    m_pCheck_FXAA       = pScene->Get_UI(L"UI_Setting_Check_FXAA");
    m_pCheck_PBR        = pScene->Get_UI(L"UI_Setting_Check_PBR");
    m_pCheck_FPS        = pScene->Get_UI(L"UI_Setting_Check_FPS");
    m_pFPS_Font         = pScene->Get_UI(L"UI_FPS_Font");

    m_pTextureCheck     = RESOURCES.Get<Texture>(L"UI_Setting_Check");
    m_pTextureNoCheck   = RESOURCES.Get<Texture>(L"UI_Setting_No_Check");

    if (true == Check_Expire())
        return E_FAIL;

    _float fBrightness       = pScene->g_fBrightness;
    _float fContrast         = pScene->g_fContrast;
    _float fSaturation       = pScene->g_Saturation;
    _float fBloom            = pScene->g_BloomData.g_BloomMin;
    _uint  fToneMapping      = pScene->g_iTMIndex;
    _float fToneMappingMax   = pScene->g_fMaxWhite;
    _float fSSAO_Radius      = pScene->g_SSAOData.g_fOcclusionRadius;
    _float fSSAO_Start       = pScene->g_SSAOData.g_OcclusionFadeStart;
    _float fSSAO_End         = pScene->g_SSAOData.g_OcclusionFadeEnd;
    _uint  fMotionBlur       = pScene->g_MotionBlurData.g_iBlurCount;
    _float fPBR_Attenuation  = pScene->g_lightAttenuation;
    _float fPBR_Ratio        = pScene->g_ambientRatio;
    _float fPBR_ShadowBius   = pScene->g_fShadowBias;

    m_bBloom            = pScene->g_BloomData.g_BloomOn;
    m_bSSAO             = pScene->g_SSAOData.g_bSSAO_On;
    m_bMotionBlur       = pScene->g_MotionBlurData.g_bMotionBlurOn;
    m_bLensFlare        = pScene->g_bLensFlare;
    m_bOutline          = pScene->g_bDrawOutline;
    m_bFXAA             = pScene->g_bFXAAOn;
    m_bPBR              = pScene->g_bPBR_On;

    if (true == m_bBloom)
        m_pCheck_Bloom.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);
    if(true == m_bSSAO)
        m_pCheck_SSAO.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);
    if(true == m_bMotionBlur)
        m_pCheck_MotionBlur.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);
    if(true == m_bLensFlare)
        m_pCheck_LensFlare.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);
    if(true == m_bOutline)
        m_pCheck_Outline.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);
    if(true == m_bFXAA)
        m_pCheck_FXAA.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);
    if(true == m_bPBR)
        m_pCheck_PBR.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);

    m_Brightness        = pair(-1.f, 1.f);
    m_Contrast          = pair(0.f, 2.f);
    m_Saturation        = pair(0.01f, 5.f);
    m_Bloom             = pair(0.01f, 1.f);
    m_White             = pair(0.f, 5.f);
    m_SSAORadius        = pair(0.f, 1.f);
    m_SSAOStart         = pair(0.f, 1.f);
    m_SSAOEnd           = pair(0.f, 1.f);
    //m_PBRAttenuation    = pair(-100.f, 100.f);
    //m_PBRRatio          = pair(0.1f, 1.5f);
    //m_PBRShadowBius     = pair(0.f, 0.05f);

    m_ToneMappingIndex  = pair(0, 3);
    m_MotionBlurCount   = pair(0,30);

    m_fMinPos = -660.f;
   
    m_pBg.lock()->Get_Button()->AddOnClickedEvent([&]()
        {
            this->Set_Render(false);
        });

    m_pCheck_Bloom.lock()->Get_Button()->AddOnClickedEvent([&]()
        {
            this->Change_Bloom();
        });
    
    m_pCheck_SSAO.lock()->Get_Button()->AddOnClickedEvent([&]()
        {
            this->Change_SSAO();
        });
    
    m_pCheck_MotionBlur.lock()->Get_Button()->AddOnClickedEvent([&]()
        {
            this->Change_MotionBlur();
        });
    
    m_pCheck_LensFlare.lock()->Get_Button()->AddOnClickedEvent([&]()
        {
            this->Change_LensFlare();
        });
    
    m_pCheck_Outline.lock()->Get_Button()->AddOnClickedEvent([&]()
        {
            this->Change_Outline();
        });
    
    m_pCheck_FXAA.lock()->Get_Button()->AddOnClickedEvent([&]()
        {
            this->Change_FXAA();
        });
    
    m_pCheck_PBR.lock()->Get_Button()->AddOnClickedEvent([&]()
        {
            this->Change_PBR();
        });
    
    m_pCheck_FPS.lock()->Get_Button()->AddOnClickedEvent([&]()
        {
            this->Change_FPS();
        });
    

    m_pBrightness.lock()->Get_Button()->Set_Type(false);
    m_pBrightness.lock()->Get_Button()->AddOnClickedEvent([this]()
        {
            POINT ptMouse = INPUT.GetMousePosToPoint();
            _float4 vecPos = m_pBrightness.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
            if (-660.f > vecPos.x)
                vecPos.x = -660.f;
            if (-360.f < vecPos.x)
                vecPos.x = -360.f;
            m_pBrightness.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
        });
    
    m_pContrast.lock()->Get_Button()->Set_Type(false);
    m_pContrast.lock()->Get_Button()->AddOnClickedEvent([this]()
        {
            POINT ptMouse = INPUT.GetMousePosToPoint();
            _float4 vecPos = m_pContrast.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
            if (-660.f > vecPos.x)
                vecPos.x = -660.f;
            if (-360.f < vecPos.x)
                vecPos.x = -360.f;
            m_pContrast.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
        });
    
    m_pSaturation.lock()->Get_Button()->Set_Type(false);
    m_pSaturation.lock()->Get_Button()->AddOnClickedEvent([this]()
        {
            POINT ptMouse = INPUT.GetMousePosToPoint();
            _float4 vecPos = m_pSaturation.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
            if (-660.f > vecPos.x)
                vecPos.x = -660.f;
            if (-360.f < vecPos.x)
                vecPos.x = -360.f;
            m_pSaturation.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
        });
    
    m_pBloom.lock()->Get_Button()->Set_Type(false);
    m_pBloom.lock()->Get_Button()->AddOnClickedEvent([this]()
        {
            POINT ptMouse = INPUT.GetMousePosToPoint();
            _float4 vecPos = m_pBloom.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
            if (-660.f > vecPos.x)
                vecPos.x = -660.f;
            if (-360.f < vecPos.x)
                vecPos.x = -360.f;
            m_pBloom.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
        });
    
    m_pToneMapping.lock()->Get_Button()->Set_Type(false);
    m_pToneMapping.lock()->Get_Button()->AddOnClickedEvent([this]()
        {
            POINT ptMouse = INPUT.GetMousePosToPoint();
            _float4 vecPos = m_pToneMapping.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
            if (-660.f > vecPos.x)
                vecPos.x = -660.f;
            if (-360.f < vecPos.x)
                vecPos.x = -360.f;
            m_pToneMapping.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
        });
    
    m_pToneMappingMax.lock()->Get_Button()->Set_Type(false);
    m_pToneMappingMax.lock()->Get_Button()->AddOnClickedEvent([this]()
        {
            POINT ptMouse = INPUT.GetMousePosToPoint();
            _float4 vecPos = m_pToneMappingMax.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
            if (-660.f > vecPos.x)
                vecPos.x = -660.f;
            if (-360.f < vecPos.x)
                vecPos.x = -360.f;
            m_pToneMappingMax.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
        });
    
    m_pSSAO_Radius.lock()->Get_Button()->Set_Type(false);
    m_pSSAO_Radius.lock()->Get_Button()->AddOnClickedEvent([this]()
        {
            POINT ptMouse = INPUT.GetMousePosToPoint();
            _float4 vecPos = m_pSSAO_Radius.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
            if (-660.f > vecPos.x)
                vecPos.x = -660.f;
            if (-360.f < vecPos.x)
                vecPos.x = -360.f;
            m_pSSAO_Radius.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
        });
    
    m_pSSAO_Start.lock()->Get_Button()->Set_Type(false);
    m_pSSAO_Start.lock()->Get_Button()->AddOnClickedEvent([this]()
        {
            POINT ptMouse = INPUT.GetMousePosToPoint();
            _float4 vecPos = m_pSSAO_Start.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
            if (-660.f > vecPos.x)
                vecPos.x = -660.f;
            if (-360.f < vecPos.x)
                vecPos.x = -360.f;
            m_pSSAO_Start.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
        });
    
    m_pSSAO_End.lock()->Get_Button()->Set_Type(false);
    m_pSSAO_End.lock()->Get_Button()->AddOnClickedEvent([this]()
        {
            POINT ptMouse = INPUT.GetMousePosToPoint();
            _float4 vecPos = m_pSSAO_End.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
            if (-660.f > vecPos.x)
                vecPos.x = -660.f;
            if (-360.f < vecPos.x)
                vecPos.x = -360.f;
            m_pSSAO_End.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
        });
    
    m_pMotionBlur.lock()->Get_Button()->Set_Type(false);
    m_pMotionBlur.lock()->Get_Button()->AddOnClickedEvent([this]()
        {
            POINT ptMouse = INPUT.GetMousePosToPoint();
            _float4 vecPos = m_pMotionBlur.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
            vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
            if (-660.f > vecPos.x)
                vecPos.x = -660.f;
            if (-360.f < vecPos.x)
                vecPos.x = -360.f;
            m_pMotionBlur.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
        });
    
    //m_pPBR_Attenuation.lock()->Get_Button()->Set_Type(false);
    //m_pPBR_Attenuation.lock()->Get_Button()->AddOnClickedEvent([this]()
    //    {
    //        POINT ptMouse = INPUT.GetMousePosToPoint();
    //        _float4 vecPos = m_pPBR_Attenuation.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    //        vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
    //        if (-660.f > vecPos.x)
    //            vecPos.x = -660.f;
    //        if (-360.f < vecPos.x)
    //            vecPos.x = -360.f;
    //        m_pPBR_Attenuation.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    //    });
    //
    //m_pPBR_Ratio.lock()->Get_Button()->Set_Type(false);
    //m_pPBR_Ratio.lock()->Get_Button()->AddOnClickedEvent([this]()
    //    {
    //        POINT ptMouse = INPUT.GetMousePosToPoint();
    //        _float4 vecPos = m_pPBR_Ratio.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    //        vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
    //        if (-660.f > vecPos.x)
    //            vecPos.x = -660.f;
    //        if (-360.f < vecPos.x)
    //            vecPos.x = -360.f;
    //        m_pPBR_Ratio.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    //    });
    //
    //m_pPBR_ShadowBius.lock()->Get_Button()->Set_Type(false);
    //m_pPBR_ShadowBius.lock()->Get_Button()->AddOnClickedEvent([this]()
    //    {
    //        POINT ptMouse = INPUT.GetMousePosToPoint();
    //        _float4 vecPos = m_pPBR_ShadowBius.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    //        vecPos.x = static_cast<_float>(ptMouse.x - g_iWinSizeX / 2.f);
    //        if (-660.f > vecPos.x)
    //            vecPos.x = -660.f;
    //        if (-360.f < vecPos.x)
    //            vecPos.x = -360.f;
    //        m_pPBR_ShadowBius.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    //    });


    _float fTotal = m_Brightness.second - m_Brightness.first;
    _float fRatio = fabs((fBrightness - m_Brightness.first) / fTotal);
    _float fValue = -660 + 300.f * fRatio;
    _float4 vecPos = m_pBrightness.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x = fValue;
    m_pBrightness.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    

    fTotal = m_Contrast.second - m_Contrast.first;
    fRatio = fabs((fContrast - m_Contrast.first) / fTotal);
    fValue = -660 + 300.f * fRatio;
    vecPos = m_pContrast.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x = fValue;
    m_pContrast.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    

    fTotal = m_Saturation.second - m_Saturation.first;
    fRatio = fabs((fSaturation - m_Saturation.first) / fTotal);
    fValue = -660 + 300.f * fRatio;
    vecPos = m_pSaturation.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x = fValue;
    m_pSaturation.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    

    fTotal = m_Bloom.second - m_Bloom.first;
    fRatio = fabs((fBloom - m_Bloom.first) / fTotal);
    fValue = -660 + 300.f * fRatio;
    vecPos = m_pBloom.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x = fValue;
    m_pBloom.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    
    
    fTotal = static_cast<_float>(m_ToneMappingIndex.second - m_ToneMappingIndex.first);
    fRatio = fabs((static_cast<_float>(fToneMapping) - static_cast<_float>(m_ToneMappingIndex.first)) / fTotal);
    fValue = -660 + 300.f * fRatio;
    vecPos = m_pToneMapping.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x = fValue;
    m_pToneMapping.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    

    fTotal = m_White.second - m_White.first;
    fRatio = fabs((fToneMappingMax - m_White.first) / fTotal);
    fValue = -660 + 300.f * fRatio;
    vecPos = m_pToneMappingMax.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x = fValue;
    m_pToneMappingMax.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    

    fTotal = m_SSAORadius.second - m_SSAORadius.first;
    fRatio = fabs((fSSAO_Radius - m_SSAORadius.first) / fTotal);
    fValue = -660 + 300.f * fRatio;
    vecPos = m_pSSAO_Radius.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x = fValue;
    m_pSSAO_Radius.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    

    fTotal = m_SSAOStart.second - m_SSAOStart.first;
    fRatio = fabs((fSSAO_Start - m_SSAOStart.first) / fTotal);
    fValue = -660 + 300.f * fRatio;
    vecPos = m_pSSAO_Start.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x = fValue;
    m_pSSAO_Start.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    

    fTotal = m_SSAOEnd.second - m_SSAOEnd.first;
    fRatio = fabs((fSSAO_End - m_SSAOEnd.first) / fTotal);
    fValue = -660 + 300.f * fRatio;
    vecPos = m_pSSAO_End.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x = fValue;
    m_pSSAO_End.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    

    fTotal = static_cast<_float>(m_MotionBlurCount.second - m_MotionBlurCount.first);
    fRatio = fabs((static_cast<_float>(fMotionBlur) - static_cast<_float>(m_MotionBlurCount.first)) / fTotal);
    fValue = -660 + 300.f * fRatio;
    vecPos = m_pMotionBlur.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x = fValue;
    m_pMotionBlur.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    

    //fTotal = m_PBRAttenuation.second - m_PBRAttenuation.first;
    //fRatio = fabs((fPBR_Attenuation - m_PBRAttenuation.first) / fTotal);
    //fValue = -660 + 300.f * fRatio;
    //vecPos = m_pPBR_Attenuation.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    //vecPos.x = fValue;
    //m_pPBR_Attenuation.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    //
    //
    //fTotal = m_PBRRatio.second - m_PBRRatio.first;
    //fRatio = fabs((fPBR_Ratio - m_PBRRatio.first) / fTotal);
    //fValue = -660 + 300.f * fRatio;
    //vecPos = m_pPBR_Ratio.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    //vecPos.x = fValue;
    //m_pPBR_Ratio.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    //
    //
    //fTotal = m_PBRShadowBius.second - m_PBRShadowBius.first;
    //fRatio = fabs((fPBR_ShadowBius - m_PBRShadowBius.first) / fTotal);
    //fValue = -660 + 300.f * fRatio;
    //vecPos = m_pPBR_ShadowBius.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    //vecPos.x = fValue;
    //m_pPBR_ShadowBius.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
    


    return S_OK;
}

void UiSettingController::Tick()
{
    if (true == m_bFPS)
        Change_FPS_Font();

    if (false == m_bIsRender)
        return;

    if (true == Check_Expire())
		return;

    Change_All_Value();
}

void UiSettingController::Set_Render(_bool bValue)
{
    if (true == Check_Expire())
        return;

    m_bIsRender = bValue;

    m_pBg.lock()->Set_Render(bValue);
    m_pBrightness.lock()->Set_Render(bValue);
    m_pContrast.lock()->Set_Render(bValue);
    m_pSaturation.lock()->Set_Render(bValue);
    m_pBloom.lock()->Set_Render(bValue);
    m_pToneMapping.lock()->Set_Render(bValue);
    m_pToneMappingMax.lock()->Set_Render(bValue);
    m_pSSAO_Radius.lock()->Set_Render(bValue);
    m_pSSAO_Start.lock()->Set_Render(bValue);
    m_pSSAO_End.lock()->Set_Render(bValue);
    m_pMotionBlur.lock()->Set_Render(bValue);
    //m_pPBR_Attenuation.lock()->Set_Render(bValue);
    //m_pPBR_Ratio.lock()->Set_Render(bValue);
    //m_pPBR_ShadowBius.lock()->Set_Render(bValue);

    m_pCheck_Bloom.lock()->Set_Render(bValue);
    m_pCheck_SSAO.lock()->Set_Render(bValue);
    m_pCheck_MotionBlur.lock()->Set_Render(bValue);
    m_pCheck_LensFlare.lock()->Set_Render(bValue);
    m_pCheck_Outline.lock()->Set_Render(bValue);
    m_pCheck_FXAA.lock()->Set_Render(bValue);
    m_pCheck_PBR.lock()->Set_Render(bValue);
    m_pCheck_FPS.lock()->Set_Render(bValue);


    m_pBg.lock()->Set_Tick(bValue);
    m_pBrightness.lock()->Set_Tick(bValue);
    m_pContrast.lock()->Set_Tick(bValue);
    m_pSaturation.lock()->Set_Tick(bValue);
    m_pBloom.lock()->Set_Tick(bValue);
    m_pToneMapping.lock()->Set_Tick(bValue);
    m_pToneMappingMax.lock()->Set_Tick(bValue);
    m_pSSAO_Radius.lock()->Set_Tick(bValue);
    m_pSSAO_Start.lock()->Set_Tick(bValue);
    m_pSSAO_End.lock()->Set_Tick(bValue);
    m_pMotionBlur.lock()->Set_Tick(bValue);
    m_pCheck_Bloom.lock()->Set_Tick(bValue);
    m_pCheck_SSAO.lock()->Set_Tick(bValue);
    m_pCheck_MotionBlur.lock()->Set_Tick(bValue);
    m_pCheck_LensFlare.lock()->Set_Tick(bValue);
    m_pCheck_Outline.lock()->Set_Tick(bValue);
    m_pCheck_FXAA.lock()->Set_Tick(bValue);
    m_pCheck_PBR.lock()->Set_Tick(bValue);
    m_pCheck_FPS.lock()->Set_Tick(bValue);
}

void UiSettingController::Change_Bloom()
{
    m_bBloom = !m_bBloom;

    if (true == Check_Expire())
        return;

    if (true == m_bBloom)
        m_pCheck_Bloom.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);
    else
        m_pCheck_Bloom.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureNoCheck.lock(), TextureMapType::DIFFUSE);

    CUR_SCENE->g_BloomData.g_BloomOn = m_bBloom;
}

void UiSettingController::Change_SSAO()
{
    m_bSSAO = !m_bSSAO;

    if (true == Check_Expire())
        return;

    if (true == m_bSSAO)
        m_pCheck_SSAO.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);
    else
        m_pCheck_SSAO.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureNoCheck.lock(), TextureMapType::DIFFUSE);

    CUR_SCENE->g_SSAOData.g_bSSAO_On = m_bSSAO;
}

void UiSettingController::Change_MotionBlur()
{
    m_bMotionBlur = !m_bMotionBlur;

    if (true == Check_Expire())
        return;

    if (true == m_bMotionBlur)
        m_pCheck_MotionBlur.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);
    else
        m_pCheck_MotionBlur.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureNoCheck.lock(), TextureMapType::DIFFUSE);

    CUR_SCENE->g_MotionBlurData.g_bMotionBlurOn = m_bMotionBlur;
}

void UiSettingController::Change_LensFlare()
{
    m_bLensFlare = !m_bLensFlare;

    if (true == Check_Expire())
        return;

    if (true == m_bLensFlare)
        m_pCheck_LensFlare.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);
    else
        m_pCheck_LensFlare.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureNoCheck.lock(), TextureMapType::DIFFUSE);

    CUR_SCENE->g_bLensFlare = m_bLensFlare;
}

void UiSettingController::Change_Outline()
{
    m_bOutline = !m_bOutline;

    if (true == Check_Expire())
        return;

    if (true == m_bOutline)
        m_pCheck_Outline.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);
    else
        m_pCheck_Outline.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureNoCheck.lock(), TextureMapType::DIFFUSE);

    CUR_SCENE->g_bDrawOutline = m_bOutline;
}

void UiSettingController::Change_FXAA()
{
    m_bFXAA = !m_bFXAA;

    if (true == Check_Expire())
        return;

    if (true == m_bFXAA)
        m_pCheck_FXAA.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);
    else
        m_pCheck_FXAA.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureNoCheck.lock(), TextureMapType::DIFFUSE);

    CUR_SCENE->g_bFXAAOn = m_bFXAA;
}

void UiSettingController::Change_PBR()
{
    m_bPBR = !m_bPBR;

    if (true == Check_Expire())
        return;

    if (true == m_bPBR)
        m_pCheck_PBR.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);
    else
        m_pCheck_PBR.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureNoCheck.lock(), TextureMapType::DIFFUSE);

    CUR_SCENE->g_bPBR_On = m_bPBR;
}

void UiSettingController::Change_FPS()
{
    m_bFPS = !m_bFPS;

    if (true == Check_Expire())
        return;

    if (true == m_bFPS)
    {
        m_pFPS_Font.lock()->Set_Tick(true);
        m_pFPS_Font.lock()->Set_Render(true);
        m_pCheck_FPS.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureCheck.lock(), TextureMapType::DIFFUSE);
    }
    else
    {
        m_pFPS_Font.lock()->Set_Tick(false);
        m_pFPS_Font.lock()->Set_Render(false);
        m_pCheck_FPS.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(m_pTextureNoCheck.lock(), TextureMapType::DIFFUSE);
    }
}

_bool UiSettingController::Check_Expire()
{
    _bool bValue = false;

    bValue |= m_pOwner.expired();
    bValue |= m_pBg.expired();
    bValue |= m_pBrightness.expired();
    bValue |= m_pContrast.expired();
    bValue |= m_pSaturation.expired();
    bValue |= m_pBloom.expired();
    bValue |= m_pToneMapping.expired();
    bValue |= m_pToneMappingMax.expired();
    bValue |= m_pSSAO_Radius.expired();
    bValue |= m_pSSAO_Start.expired();
    bValue |= m_pSSAO_End.expired();
    bValue |= m_pMotionBlur.expired();
    //bValue |= m_pPBR_Attenuation.expired();
    //bValue |= m_pPBR_Ratio.expired();
    //bValue |= m_pPBR_ShadowBius.expired();

    bValue |= m_pCheck_Bloom.expired();
    bValue |= m_pCheck_SSAO.expired();
    bValue |= m_pCheck_MotionBlur.expired();
    bValue |= m_pCheck_LensFlare.expired();
    bValue |= m_pCheck_Outline.expired();
    bValue |= m_pCheck_FXAA.expired();
    bValue |= m_pCheck_PBR.expired();
    bValue |= m_pCheck_FPS.expired();

    bValue |= m_pTextureCheck.expired();
    bValue |= m_pTextureNoCheck.expired();

    return bValue;
}

void UiSettingController::Change_All_Value()
{
    Change_Value_Brightness();
    Change_Value_Contrast();
    Change_Value_Saturation();
    Change_Value_Bloom();
    Change_Value_ToneMappingIndex();
    Change_Value_ToneMappingMaxWhite();
    Change_Value_SSAO_Radius();
    Change_Value_SSAO_Start();
    Change_Value_SSAO_End();
    Change_Value_MotionBlur();
    //Change_Value_PBR_Attenuation();
    //Change_Value_PBR_Ratio();
    //Change_Value_PBR_ShadowBius();
}

void UiSettingController::Change_Value_Brightness()
{
    _float fX = m_pBrightness.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).x;
    fX -= m_fMinPos;

    _float fRatio = fX / 300.f;
    _float fTotal = m_Brightness.second - m_Brightness.first;
    fTotal *= fRatio;

    fTotal += m_Brightness.first;

    CUR_SCENE->g_fBrightness = fTotal;
}

void UiSettingController::Change_Value_Contrast()
{
    _float fX = m_pContrast.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).x;
    fX -= m_fMinPos;

    _float fRatio = fX / 300.f;
    _float fTotal = m_Contrast.second - m_Contrast.first;
    fTotal *= fRatio;

    fTotal += m_Contrast.first;

    CUR_SCENE->g_fContrast = fTotal;
}

void UiSettingController::Change_Value_Saturation()
{
    _float fX = m_pSaturation.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).x;
    fX -= m_fMinPos;

    _float fRatio = fX / 300.f;
    _float fTotal = m_Saturation.second - m_Saturation.first;
    fTotal *= fRatio;

    fTotal += m_Saturation.first;

    CUR_SCENE->g_Saturation = fTotal;
}

void UiSettingController::Change_Value_Bloom()
{
    _float fX = m_pBloom.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).x;
    fX -= m_fMinPos;

    _float fRatio = fX / 300.f;
    _float fTotal = m_Bloom.second - m_Bloom.first;
    fTotal *= fRatio;

    fTotal += m_Bloom.first;

    CUR_SCENE->g_BloomData.g_BloomMin = fTotal;
}

void UiSettingController::Change_Value_ToneMappingIndex()
{
    _float fX = m_pToneMapping.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).x;
    fX -= m_fMinPos;

    _float fRatio = fX / 300.f;
    _float fTotal = static_cast<_float>(m_ToneMappingIndex.second - m_ToneMappingIndex.first);
    fTotal *= fRatio;

    fTotal += m_ToneMappingIndex.first;

    if (2.9f < fTotal)
        fTotal = 3.1f;

    CUR_SCENE->g_iTMIndex = IDX(fTotal);
}

void UiSettingController::Change_Value_ToneMappingMaxWhite()
{
    _float fX = m_pToneMappingMax.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).x;
    fX -= m_fMinPos;

    _float fRatio = fX / 300.f;
    _float fTotal = m_White.second - m_White.first;
    fTotal *= fRatio;

    fTotal += m_White.first;

    CUR_SCENE->g_fMaxWhite = fTotal;
}

void UiSettingController::Change_Value_SSAO_Radius()
{
    _float fX = m_pSSAO_Radius.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).x;
    fX -= m_fMinPos;

    _float fRatio = fX / 300.f;
    _float fTotal = m_SSAORadius.second - m_SSAORadius.first;
    fTotal *= fRatio;

    fTotal += m_SSAORadius.first;

    CUR_SCENE->g_SSAOData.g_fOcclusionRadius = fTotal;
}

void UiSettingController::Change_Value_SSAO_Start()
{
    _float fX = m_pSSAO_Start.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).x;
    fX -= m_fMinPos;

    _float fRatio = fX / 300.f;
    _float fTotal = m_SSAOStart.second - m_SSAOStart.first;
    fTotal *= fRatio;

    fTotal += m_SSAOStart.first;

    CUR_SCENE->g_SSAOData.g_OcclusionFadeStart = fTotal;
}

void UiSettingController::Change_Value_SSAO_End()
{
    _float fX = m_pSSAO_End.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).x;
    fX -= m_fMinPos;

    _float fRatio = fX / 300.f;
    _float fTotal = m_SSAOEnd.second - m_SSAOEnd.first;
    fTotal *= fRatio;

    fTotal += m_SSAOEnd.first;

    CUR_SCENE->g_SSAOData.g_OcclusionFadeEnd = fTotal;
}

void UiSettingController::Change_Value_MotionBlur()
{
    _float fX = m_pMotionBlur.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).x;
    fX -= m_fMinPos;

    _float fRatio = fX / 300.f;
    _float fTotal = static_cast<_float>(m_MotionBlurCount.second - m_MotionBlurCount.first);
    fTotal *= fRatio;

    fTotal += m_MotionBlurCount.first;

    CUR_SCENE->g_MotionBlurData.g_iBlurCount = IDX(fTotal);
}

void UiSettingController::Change_FPS_Font()
{
    if (true == m_pCheck_FPS.expired())
        return;

    _uint iFps = TIME.GetFPS();

    wstring strTemp = L"FPS : " + to_wstring(iFps);
    m_pFPS_Font.lock()->Get_FontRenderer()->Get_Text() = strTemp;
}

//void UiSettingController::Change_Value_PBR_Attenuation()
//{
//    _float fX = m_pPBR_Attenuation.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).x;
//    fX -= m_fMinPos;
//
//    _float fRatio = fX / 300.f;
//    _float fTotal = m_PBRAttenuation.second - m_PBRAttenuation.first;
//    fTotal *= fRatio;
//
//    fTotal += m_PBRAttenuation.first;
//
//    CUR_SCENE->g_lightAttenuation = fTotal;
//}
//
//void UiSettingController::Change_Value_PBR_Ratio()
//{
//    _float fX = m_pPBR_Ratio.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).x;
//    fX -= m_fMinPos;
//
//    _float fRatio = fX / 300.f;
//    _float fTotal = m_PBRRatio.second - m_PBRRatio.first;
//    fTotal *= fRatio;
//
//    fTotal += m_PBRRatio.first;
//
//    CUR_SCENE->g_ambientRatio = fTotal;
//}
//
//void UiSettingController::Change_Value_PBR_ShadowBius()
//{
//    _float fX = m_pPBR_ShadowBius.lock()->GetOrAddTransform()->Get_State(Transform_State::POS).x;
//    fX -= m_fMinPos;
//
//    _float fRatio = fX / 300.f;
//    _float fTotal = m_PBRShadowBius.second - m_PBRShadowBius.first;
//    fTotal *= fRatio;
//
//    fTotal += m_PBRShadowBius.first;
//
//    CUR_SCENE->g_fShadowBias = fTotal;
//}
