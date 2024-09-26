#pragma once
#include "ResourceBase.h"

class MeshEffectData : public ResourceBase
{
public:
    typedef struct tagMeshEffectVData
    {
        // Property
        string      strTag = {""};
        _float      fDuration = { 0.f };
        _bool       bLightOn = { false };
        _float      fLightIntensity = { 0.f };
        _bool       bUseFadeOut = { false };
        _int        iMeshCnt = { 1 };
        _float      fCreateInterval = {0.f};
        _float2     vParticleDuration = {0.f, 0.f};
        _int        iSamplerType = {1};
        _bool       bIsLoop = { false };
        _bool       bIsFollowingGroup_OnlyTranslate = { false };
        _bool       bIsFollowingGroup_LooKSameDir = { false };
        _bool       bIsFDistortion = { false };
        _bool       bIsSSD = { false };

        // Mesh 
        string      strVfxMesh = {""};

        /* Sprite Animation */
        _bool       bUseSpriteAnim = { false };
        _int        iNumSprite_Col = { 1 };
        _int        iNumSprite_Row = { 1 };
        _float      fAnimSpeed = { 0.f };

        /* Coloring Options */
        _bool       bColorChangingOn = { false };

        // Option1
        string      strTexture_Op1 = { " " };
        _int        iFlipOption_Op1 = { 0 };
        _int        bIsUseTextureColor_Op1 = { 0 };
        Color       vBaseColor1_Op1 = {0.f, 0.f, 0.f, 0.f};
        Color       vBaseColor2_Op1 = { 0.f, 0.f, 0.f, 0.f };
        Color       vDestColor1_Op1 = { 0.f, 0.f, 0.f, 0.f };
        Color       vDestColor2_Op1 = { 0.f, 0.f, 0.f, 0.f };
        _float2     vTiling_Op1 = {0.f, 0.f};
        _float2     vUVSpeed_Op1 = { 0.f, 0.f };
        _float      fContrast_Op1 = { 0.f };
        _float      fAlphaOffset_Op1 = {0.f};

        // Option2
        string      strTexture_Op2 = { " " };
        _int        iFlipOption_Op2 = { 0 };
        _int        bIsUseTextureColor_Op2 = { 0 };
        Color       vBaseColor1_Op2 = { 0.f, 0.f, 0.f, 0.f };
        Color       vBaseColor2_Op2 = { 0.f, 0.f, 0.f, 0.f };
        Color       vDestColor1_Op2 = { 0.f, 0.f, 0.f, 0.f };
        Color       vDestColor2_Op2 = { 0.f, 0.f, 0.f, 0.f };
        _float2     vTiling_Op2 = { 0.f, 0.f };
        _float2     vUVSpeed_Op2 = { 0.f, 0.f };
        _float      fContrast_Op2 = { 0.f };
        _float      fAlphaOffset_Op2 = { 0.f };

        // Option3
        string      strTexture_Op3 = { " " };
        _int        iFlipOption_Op3 = { 0 };
        _int        bIsUseTextureColor_Op3 = { 0 };
        Color       vBaseColor1_Op3 = { 0.f, 0.f, 0.f, 0.f };
        Color       vBaseColor2_Op3 = { 0.f, 0.f, 0.f, 0.f };
        Color       vDestColor1_Op3 = { 0.f, 0.f, 0.f, 0.f };
        Color       vDestColor2_Op3 = { 0.f, 0.f, 0.f, 0.f };
        _float2     vTiling_Op3 = { 0.f, 0.f };
        _float2     vUVSpeed_Op3 = { 0.f, 0.f };
        _float      fContrast_Op3 = { 0.f };
        _float      fAlphaOffset_Op3 = { 0.f };

        // Blend
        string      strTexture_Blend = { " " };
        _float2     vTiling_Blend = { 0.f, 0.f };
        _float2     vUVSpeed_Blend = { 0.f, 0.f };
        _float      fAlphaOffset_Blend = { 0.f };

        // Overlay
        _bool       bIsOverlayOn = { false };
        string      strOverlayTexture = { " " };
        Color       vBaseColor_Overlay = { 0.f, 0.f, 0.f, 0.f };
        Color       vDestColor_Overlay = { 0.f, 0.f, 0.f, 0.f };
        _float2     vTiling_Overlay = { 0.f, 0.f };
        _float2     vUVSpeed_Overlay = { 0.f, 0.f };

        // Rim Light 
        _bool       bIsRimLightOn = { false };
        Color       vBaseColor_RimLight = { 0.f, 0.f, 0.f, 0.f };
        Color       vDestColor_RimLight = { 0.f, 0.f, 0.f, 0.f };
        _float2		vCurvePoint_RimLight[4] = { { 0.f, 0.f },{ 0.f, 0.f } ,{ 0.f, 0.f } ,{ 0.f, 0.f } };

        // Normal 
        string      strNormalTexture = { " " };

        // Dissolve
        string      strDissolveTexture = { " " };
        _float2     vTiling_Dissolve = { 0.f, 0.f };
        _float2     vUVSpeed_Dissolve = { 0.f, 0.f };
        _float2		vCurvePoint_Dissolve[4] = { { 0.f, 0.f },{ 0.f, 0.f } ,{ 0.f, 0.f } ,{ 0.f, 0.f } };

        // Distortion
        string      strDistortionTexture = { " " };
        _float2     vTiling_Distortion = { 0.f, 0.f };
        _float2     vUVSpeed_Distortion = { 0.f, 0.f };

    }DESC;

    typedef struct tagMeshEffectTransformData
    {
        // Init position
        _float3     vPosRange = { 0.f, 0.f, 0.f };

        // Init scale
        _float3     vInitScale_Min = { 0.f, 0.f, 0.f };
        _float3     vInitScale_Max = { 0.f, 0.f, 0.f };

        // Init rotation
        _float3     vInitRotation_Min = { 0.f, 0.f, 0.f };
        _float3     vInitRotation_Max = { 0.f, 0.f, 0.f };

        // Translate
        _int        iTranslateOption = { 0 };
        _float      fTranslateSpeed = { 0.f };
        _float3     vEndPosOffset_Min = { 0.f, 0.f, 0.f };
        _float3     vEndPosOffset_Max = { 0.f, 0.f, 0.f };
        _int        iSpeedType = { 0 };
        _float2     vCurvePoint_Force[4] = { { 0.f, 0.f },{ 0.f, 0.f } ,{ 0.f, 0.f } ,{ 0.f, 0.f } };

        // Scaling 
        _int        iScalingOption = { 0 };
        _float3     vEndScale = { 0.f, 0.f, 0.f };
        _int        iScaleSpeedType = { 0 };
        _float2     vCurvePoint_Scale[4] = { { 0.f, 0.f },{ 0.f, 0.f } ,{ 0.f, 0.f } ,{ 0.f, 0.f } };

        // Turn 
        _int        iTurnOption = { 0 };
        _float      fTurnSpeed = { 0.f };
        _float3     vRandomAxis_Min = { 0.f, 0.f, 0.f };
        _float3     vRandomAxis_Max = { 0.f, 0.f, 0.f };
        _bool       bBillbordAxes[2] = { false, false };

    }Transform_Desc;

public:
    MeshEffectData();
    ~MeshEffectData();

public:
    virtual void    Load(const wstring& path) override;

    /* Setter */
    void            Set_Desc(DESC tDesc) { m_tDesc = tDesc; }
    void            Set_TransformDesc(Transform_Desc tDesc) { m_tTransformDesc = tDesc; }

    /* Getter */
    DESC            Get_Desc() { return m_tDesc; }
    Transform_Desc  Get_TransformDesc() { return m_tTransformDesc; }

private:
    DESC            m_tDesc;
    Transform_Desc  m_tTransformDesc;
};
