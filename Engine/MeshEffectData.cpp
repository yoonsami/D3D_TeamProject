#include "pch.h"
#include "MeshEffectData.h"

#include "Utils.h"
#include "FileUtils.h"

MeshEffectData::MeshEffectData()
    :ResourceBase(ResourceType::MeshEffectData)
{
}

MeshEffectData::~MeshEffectData()
{
}

void MeshEffectData::Load(const wstring& path)
{
    _float2 tiling = { 0.f, 0.f };
    _float2 UVSpeed = { 0.f, 0.f };

    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(path, FileMode::Read);

    // For. Load Desc 
    /* Property */
    m_tDesc.strTag = file->Read<string>();
    m_tDesc.fDuration = file->Read<_float>();
    m_tDesc.bLightOn = file->Read<_bool>();    
    m_tDesc.bUseFadeOut = file->Read<_bool>();
    m_tDesc.iMeshCnt = file->Read<_int>();
    m_tDesc.fCreateInterval = file->Read<_float>();
    m_tDesc.vParticleDuration = file->Read<_float2>();
    m_tDesc.iSamplerType = file->Read<_int>();

    /* Mesh */
    m_tDesc.strVfxMesh = file->Read<string>();

    /* Sprite Animation */
    m_tDesc.bUseSpriteAnim = file->Read<_bool>();
    m_tDesc.iNumSprite_Col = file->Read<_int>();
    m_tDesc.iNumSprite_Row = file->Read<_int>();
    m_tDesc.fAnimSpeed = file->Read<_float>();

    /* Coloring Option */
    m_tDesc.bColorChangingOn = file->Read<_bool>();

    /* Option1 */
    m_tDesc.strTexture_Op1 = file->Read<string>();
    m_tDesc.iFlipOption_Op1 = file->Read<_int>();
    m_tDesc.bIsUseTextureColor_Op1 = file->Read<_int>();
    m_tDesc.vBaseColor1_Op1 = file->Read<_float4>();
    m_tDesc.vBaseColor2_Op1 = file->Read<_float4>();
    m_tDesc.vDestColor1_Op1 = file->Read<_float4>();
    m_tDesc.vDestColor2_Op1 = file->Read<_float4>();
    m_tDesc.vTiling_Op1 = file->Read<_float2>();
    m_tDesc.vUVSpeed_Op1 = file->Read<_float2>();
    m_tDesc.fContrast_Op1 = file->Read<_float>();
    m_tDesc.fAlphaOffset_Op1 = file->Read<_float>();

    /* Option2 */
    m_tDesc.strTexture_Op2 = file->Read<string>();
    m_tDesc.iFlipOption_Op2 = file->Read<_int>();
    m_tDesc.bIsUseTextureColor_Op2 = file->Read<_int>();
    m_tDesc.vBaseColor1_Op2 = file->Read<_float4>();
    m_tDesc.vBaseColor2_Op2 = file->Read<_float4>();
    m_tDesc.vDestColor1_Op2 = file->Read<_float4>();
    m_tDesc.vDestColor2_Op2 = file->Read<_float4>();
    m_tDesc.vTiling_Op2 = file->Read<_float2>();
    m_tDesc.vUVSpeed_Op2 = file->Read<_float2>();
    m_tDesc.fContrast_Op2 = file->Read<_float>();
    m_tDesc.fAlphaOffset_Op2 = file->Read<_float>();

    /* Option3 */
    m_tDesc.strTexture_Op3 = file->Read<string>();
    m_tDesc.iFlipOption_Op3 = file->Read<_int>();
    m_tDesc.bIsUseTextureColor_Op3 = file->Read<_int>();
    m_tDesc.vBaseColor1_Op3 = file->Read<_float4>();
    m_tDesc.vBaseColor2_Op3 = file->Read<_float4>();
    m_tDesc.vDestColor1_Op3 = file->Read<_float4>();
    m_tDesc.vDestColor2_Op3 = file->Read<_float4>();
    m_tDesc.vTiling_Op3 = file->Read<_float2>();
    m_tDesc.vUVSpeed_Op3 = file->Read<_float2>();
    m_tDesc.fContrast_Op3 = file->Read<_float>();
    m_tDesc.fAlphaOffset_Op3 = file->Read<_float>();

    /* Blend */
    m_tDesc.strTexture_Blend = file->Read<string>();
    m_tDesc.vTiling_Blend = file->Read<_float2>();
    m_tDesc.vUVSpeed_Blend = file->Read<_float2>();
    m_tDesc.fAlphaOffset_Blend = file->Read<_float>();

    /* Overlay */
    m_tDesc.bIsOverlayOn = file->Read<_bool>();
    m_tDesc.strOverlayTexture = file->Read<string>();
    m_tDesc.vBaseColor_Overlay = file->Read<_float4>();
    m_tDesc.vDestColor_Overlay = file->Read<_float4>();
    m_tDesc.vTiling_Overlay = file->Read<_float2>();
    m_tDesc.vUVSpeed_Overlay = file->Read<_float2>();

    /* RimLight */
    m_tDesc.bIsRimLightOn = file->Read<_bool>();
    m_tDesc.vBaseColor_RimLight = file->Read<_float4>();
    m_tDesc.vDestColor_RimLight = file->Read<_float4>();
    for (_int i = 0; i < 4; i++)
        m_tDesc.vCurvePoint_RimLight[i] = file->Read<_float2>();

    /* Normal */
    m_tDesc.strNormalTexture = file->Read<string>();

    /* Dissolve */
    m_tDesc.strDissolveTexture = file->Read<string>();
    m_tDesc.vTiling_Dissolve = file->Read<_float2>();
    m_tDesc.vUVSpeed_Dissolve = file->Read<_float2>();
    for(_int i = 0 ; i < 4 ; i ++)
        m_tDesc.vCurvePoint_Dissolve[i] = file->Read<_float2>();

    /* Distortion */
    m_tDesc.strDistortionTexture = file->Read<string>();
    m_tDesc.vTiling_Distortion = file->Read<_float2>();
    m_tDesc.vUVSpeed_Distortion = file->Read<_float2>();

    /* ETC */
    _float4x4 mTemp = file->Read<_float4x4>();
    m_tDesc.bIsLoop = (_int)mTemp._11;
    m_tDesc.bIsFollowingGroup_OnlyTranslate = (_int)mTemp._12;
    m_tDesc.bIsFollowingGroup_LooKSameDir = (_int)mTemp._13;
    m_tDesc.fLightIntensity = mTemp._41;
    m_tDesc.bIsFDistortion = mTemp._42;
    m_tDesc.bIsSSD = mTemp._43;

    m_tTransformDesc.iScaleSpeedType = (_int)mTemp._14;
    m_tTransformDesc.vCurvePoint_Scale[0] = _float2(mTemp._21, mTemp._22);
    m_tTransformDesc.vCurvePoint_Scale[1] = _float2(mTemp._23, mTemp._24);
    m_tTransformDesc.vCurvePoint_Scale[2] = _float2(mTemp._31, mTemp._32);
    m_tTransformDesc.vCurvePoint_Scale[3] = _float2(mTemp._33, mTemp._34);

    // For. Load Transform_Desc 
    /* Init Position */
    m_tTransformDesc.vPosRange = file->Read<_float3>();

    /* Init scale */
    m_tTransformDesc.vInitScale_Min = file->Read<_float3>();
    m_tTransformDesc.vInitScale_Max = file->Read<_float3>();

    /* Init Rotation */
    m_tTransformDesc.vInitRotation_Min = file->Read<_float3>();
    m_tTransformDesc.vInitRotation_Max = file->Read<_float3>();

    /* Translate */
    m_tTransformDesc.iTranslateOption = file->Read<_int>();
    m_tTransformDesc.fTranslateSpeed = file->Read<_float>();
    m_tTransformDesc.vEndPosOffset_Min = file->Read<_float3>();
    m_tTransformDesc.vEndPosOffset_Max = file->Read<_float3>();
    m_tTransformDesc.iSpeedType = file->Read<_int>();
    for (_int i = 0; i < 4; i++)
        m_tTransformDesc.vCurvePoint_Force[i] = file->Read<_float2>();

    /* Scaling */
    m_tTransformDesc.iScalingOption = file->Read<_int>();
    m_tTransformDesc.vEndScale = file->Read<_float3>();

    /* Turn */
    m_tTransformDesc.iTurnOption = file->Read<_int>();
    m_tTransformDesc.fTurnSpeed = file->Read<_float>();
    m_tTransformDesc.vRandomAxis_Min = file->Read<_float3>();
    m_tTransformDesc.vRandomAxis_Max = file->Read<_float3>();
    for (_int i = 0; i < 2; i++)
        m_tTransformDesc.bBillbordAxes[i] = file->Read<_bool>();
}