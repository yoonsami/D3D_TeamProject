#pragma once
#include "Component.h"
#include "MeshEffectData.h"
class StructuredBuffer;
class MeshEffect : public Component
{
public:
    MeshEffect(shared_ptr<Shader> shader);
    virtual ~MeshEffect();

public:
    virtual void            Init(void* pArg);
    virtual void            Tick() override;
    virtual void            Final_Tick() override;
	virtual void            MeshEffect_Tick() ;
	virtual void            MeshEffect_Final_Tick();
    void                    Render();
    void                    Render_Instancing(shared_ptr<InstancingBuffer> buffer, shared_ptr<StructuredBuffer> pRenderParamBuffer);
    void                    Update_RenderParams();
    void                    Update_Desc();

    void                    InitialTransform(_float4x4 mParentWorldMatrix, const _float3& vInitPos_inGroup, const _float3& vInitScale_inGroup, const _float3& vInitRotation_inGroup);

    /* Setter */
    void                    Set_Desc(MeshEffectData::DESC tDesc) { m_tDesc = tDesc; }
    void                    Set_TransformDesc(void* pArg);
    void                    Set_Lock(_bool bIsLocked) { m_bIsLocked = bIsLocked; }
    void                    Set_Loop(_bool bIsLoop) { m_tDesc.bIsLoop = bIsLoop; }
    void                    Set_ToolModeOn(_bool bIsToolMode) { m_bToolMode_On = bIsToolMode; }
    void                    Set_RenderPriority(_int iPriority) { m_iRenderPriority = iPriority; }
    void                    Set_Material();

    /* Getter */
    shared_ptr<Material>    Get_Material() { return m_pMaterial; }
    shared_ptr<Shader>      Get_Shader() { return m_pShader; }
    _bool                   Get_IsFollowGroup_OnlyTranslate() { return m_tDesc.bIsFollowingGroup_OnlyTranslate; }
    _bool                   Get_IsFollowGroup_LooKSameDir() { return m_tDesc.bIsFollowingGroup_LooKSameDir; }
    _bool                   Get_IsLoop() { return m_tDesc.bIsLoop; }
    const _float4x4&        Get_InGroupMatrix() { return m_mInGroupWorldMatrix; }
    const _float4x4&        Get_InitGroupMatrix() { return m_mInitGroupMatrix; }
    MeshEffectData::DESC&   Get_Desc() { return m_tDesc; }
    _float4x4               Get_LocalMatrix();
    _int                    Get_RenderPriority() { return m_iRenderPriority; }
    InstanceID              Get_InstanceID();
    const RenderParams&     Get_RenderParamDesc() { return m_RenderParams; }
    _float3&                Get_LocalPos() { return m_vLocalPos; }

    void                    Translate();
    void                    Scaling();
    void                    Turn();
    void                    BillBoard();

private:
    void                    Run_SpriteAnimation();

    void                    Init_RenderParams();

    void                    Bind_UpdatedColor_ToShader();
    void                    Bind_UpdatedTexUVOffset_ToShader();
    void                    Bind_RenderParams_ToShader();

    _float                  Calc_Spline(_int iType, _float* vSplineInput);

private:
    MeshEffectData::DESC            m_tDesc;
    MeshEffectData::Transform_Desc  m_tTransform_Desc;

    /* For. Group effect heirarchy*/
    _float4x4                       m_mInitGroupMatrix;
    _float4x4                       m_mInGroupWorldMatrix;
    _float3                         m_vLocalPos = _float3(0.f);
    _float3                         m_vLocalScale = _float3(1.f);
    Quaternion                      m_qRotation = Quaternion();

    /* For. Mesh Effect Tool mode */
    _bool                           m_bToolMode_On = { false };

    /* */
    _float                          m_fDuration = { 0.f };
    _float                          m_fCurrAge = { 0.f };
    _float                          m_fLifeTimeRatio = { 0.f };
    _float                          m_fTimeAcc_SpriteAnimation = { 0.f };
    _bool                           m_bIsPlayFinished = { false };
    _bool                           m_bIsLocked = { false };

    /* Tex UV offset */
    _float2                         m_vCurrTexUVOffset_Op1 = { 0.f, 0.f };
    _float2                         m_vCurrTexUVOffset_Op2 = { 0.f, 0.f };
    _float2                         m_vCurrTexUVOffset_Op3 = { 0.f, 0.f };
    _float2                         m_vCurrTexUVOffset_Blend = { 0.f, 0.f };
    _float2                         m_vCurrTexUVOffset_Overlay = { 0.f, 0.f };
    _float2                         m_vCurrTexUVOffset_Dissolve = { 0.f, 0.f };
    _float2                         m_vCurrTexUVOffset_Distortion = { 0.f, 0.f };

    /* Spline */
    _float                          m_SplineInput_Dissolve[8];
    _float                          m_fCurrDissolveWeight = { 0.f };
    _float                          m_SplineInput_RimLight[8];
    _float                          m_fCurrRimLightIntensity = { 0.f };

    /* Sprite Animation */
    _float2                         m_vSpriteSize = { 0.f, 0.f };
    _int                            m_iNumSprite = { 0 };
    _int                            m_iCurrSpriteIndex = { 0 };
    _float2                         m_UVTexRangeX = { 0.f, 0.f };
    _float2                         m_UVTexRangeY = { 0.f, 0.f };

    /* Initalize Transform */
    _float3                         m_vStartPos;
    _float3                         m_vStartScale;
    _float3                         m_vStartRotation;

    /* Translate */
    _int                            m_iTranslateOption;
    _float                          m_fTranslateSpeed;
    _float3                         m_vEndPos;
    _float                          m_SplineInput_Force[8];
    _float                          m_fCurrYspeed = { 0.f };

    _float3                         m_vEndScale;
    _float                          m_SplineInput_ScaleSpeed[8];

    _int                            m_iTurnOption;
    _float                          m_fTurnSpeed;
    _float3                         m_vRandomAxis;

    /* Component */
    shared_ptr<Shader>              m_pShader = { nullptr };
    shared_ptr<Mesh>                m_pMesh = { nullptr };
    shared_ptr<Model>               m_pModel = { nullptr };
    shared_ptr<Material>            m_pMaterial = { nullptr };

    /* Data to bind Shader */
    RenderParams                    m_RenderParams{};

    /* Render Priority */
    _int                            m_iRenderPriority = { 0 };

};
