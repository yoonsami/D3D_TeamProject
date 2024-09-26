#pragma once
#include "MonoBehaviour.h"

class UiSettingController : public MonoBehaviour
{
public:
    UiSettingController();


public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<UiSettingController>(); }

    void Set_Render(_bool bValue);
    
    void Change_Bloom();
    void Change_SSAO();
    void Change_MotionBlur();
    void Change_LensFlare();
    void Change_Outline();
    void Change_FXAA();
    void Change_PBR();
    void Change_FPS();

private:
    _bool Check_Expire();
    void Change_All_Value();
    
    void Change_Value_Brightness();
    void Change_Value_Contrast();
    void Change_Value_Saturation();
    void Change_Value_Bloom();
    void Change_Value_ToneMappingIndex();
    void Change_Value_ToneMappingMaxWhite();
    void Change_Value_SSAO_Radius();
    void Change_Value_SSAO_Start();
    void Change_Value_SSAO_End();
    void Change_Value_MotionBlur();
    //void Change_Value_PBR_Attenuation();
    //void Change_Value_PBR_Ratio();
    //void Change_Value_PBR_ShadowBius();

    void Change_FPS_Font();
    
private:
    _bool                   m_bIsRender         = {};
     
    weak_ptr<GameObject>    m_pBg;
    weak_ptr<GameObject>    m_pBrightness;
    weak_ptr<GameObject>    m_pContrast;
    weak_ptr<GameObject>    m_pSaturation;
    weak_ptr<GameObject>    m_pBloom;
    weak_ptr<GameObject>    m_pToneMapping;
    weak_ptr<GameObject>    m_pToneMappingMax;
    weak_ptr<GameObject>    m_pSSAO_Radius;
    weak_ptr<GameObject>    m_pSSAO_Start;
    weak_ptr<GameObject>    m_pSSAO_End;
    weak_ptr<GameObject>    m_pMotionBlur;
    //weak_ptr<GameObject>    m_pPBR_Attenuation;
    //weak_ptr<GameObject>    m_pPBR_Ratio;
    //weak_ptr<GameObject>    m_pPBR_ShadowBius;

    weak_ptr<GameObject>    m_pCheck_Bloom;
    weak_ptr<GameObject>    m_pCheck_SSAO;
    weak_ptr<GameObject>    m_pCheck_MotionBlur;
    weak_ptr<GameObject>    m_pCheck_LensFlare;
    weak_ptr<GameObject>    m_pCheck_Outline;
    weak_ptr<GameObject>    m_pCheck_FXAA;
    weak_ptr<GameObject>    m_pCheck_PBR;
    weak_ptr<GameObject>    m_pCheck_FPS;

    weak_ptr<GameObject>    m_pFPS_Font;

    weak_ptr<Texture>       m_pTextureCheck;
    weak_ptr<Texture>       m_pTextureNoCheck;

    //_float                  m_fBrightness       = {};
    //_float                  m_fContrast         = {};
    //_float                  m_fSaturation       = {};
    //_float                  m_fBloom            = {};
    //_uint                   m_fToneMapping      = {};
    //_float                  m_fToneMappingMax   = {};
    //_float                  m_fSSAO_Radius      = {};
    //_float                  m_fSSAO_Start       = {};
    //_float                  m_fSSAO_End         = {};
    //_uint                   m_fMotionBlur       = {};
    //_float                  m_fPBR_Attenuation  = {};
    //_float                  m_fPBR_Ratio        = {};
    //_float                  m_fPBR_ShadowBius   = {};

    _bool                   m_bBloom            = {};
    _bool                   m_bSSAO             = {};
    _bool                   m_bMotionBlur       = {};
    _bool                   m_bLensFlare        = {};
    _bool                   m_bOutline          = {};
    _bool                   m_bFXAA             = {};
    _bool                   m_bPBR              = {};
    _bool                   m_bFPS              = {};

    pair<_float, _float>    m_Brightness        = {};
    pair<_float, _float>    m_Contrast          = {};
    pair<_float, _float>    m_Saturation        = {};
    pair<_float, _float>    m_Bloom             = {};
    pair<_uint, _uint>      m_ToneMappingIndex  = {};
    pair<_float, _float>    m_White             = {};
    pair<_float, _float>    m_SSAORadius        = {};
    pair<_float, _float>    m_SSAOStart         = {};
    pair<_float, _float>    m_SSAOEnd           = {};
    pair<_uint, _uint>      m_MotionBlurCount   = {};
    //pair<_float, _float>    m_PBRAttenuation    = {};
    //pair<_float, _float>    m_PBRRatio          = {};
    //pair<_float, _float>    m_PBRShadowBius     = {};


    _float                  m_fMinPos           = {};
};

