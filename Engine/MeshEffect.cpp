#include "pch.h"
#include "MeshEffect.h"

#include "Model.h"
#include "Utils.h"
#include "MathUtils.h"
#include "Camera.h"
#include "ModelMesh.h"
#include "GroupEffect.h"
#include "StructuredBuffer.h"

MeshEffect::MeshEffect(shared_ptr<Shader> shader)
    : Component(COMPONENT_TYPE::MeshEffect)
    , m_pShader(shader)
{

}

MeshEffect::~MeshEffect()
{
}

void MeshEffect::Init(void* pArg)
{
    // For. Setting basic info  
    MeshEffectData::DESC* pDesc = (MeshEffectData::DESC*)pArg;
    m_tDesc = *pDesc;

    Update_Desc();
}

void MeshEffect::Tick()
{
    if (!m_bToolMode_On)
        return;
    string str = m_tDesc.strVfxMesh;
    MeshEffect_Tick();
}

void MeshEffect::Final_Tick()
{
    if (!m_bToolMode_On)
        return;

    MeshEffect_Final_Tick();
}

void MeshEffect::MeshEffect_Tick()
{
    if (m_bIsLocked)
        return;

    Translate();
    Scaling();
    Turn(); 
}

void MeshEffect::MeshEffect_Final_Tick()
{
    if (m_bIsLocked)
        return;
 
    if (m_tDesc.bIsLoop)
    {
        m_fCurrAge = 0.f;
        m_fLifeTimeRatio = 0.f;
        m_fTimeAcc_SpriteAnimation = 0.f;
    }

    // For. Update Time information 
    m_fCurrAge += fDT;
    m_fLifeTimeRatio = m_fCurrAge / m_fDuration;
    m_fTimeAcc_SpriteAnimation += fDT;

    // Calc Curr Dissolve weight 
    if ("None" != m_tDesc.strDissolveTexture)
    {
        _float output[4];
        Utils::Spline(m_SplineInput_Dissolve, 4, 1, m_fLifeTimeRatio, output);
        m_fCurrDissolveWeight = output[0];
    }

    // Calc Curr RimLight intensity 
    if (m_tDesc.bIsRimLightOn)
    {
        _float output[4];
        Utils::Spline(m_SplineInput_RimLight, 4, 1, m_fLifeTimeRatio, output);
        m_fCurrRimLightIntensity = output[0];
    }

    Update_RenderParams();

    // For. Check is dead 
    if (m_fCurrAge >= m_fDuration)
    {
        if (!m_tDesc.bIsLoop)
        {
            EVENTMGR.Delete_Object(Get_Owner());
            return; 
        }
    }

}

void MeshEffect::Render()
{
    if (m_pModel == nullptr)
        return;

    // Bones
    shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();

    const _uint boneCount = m_pModel->Get_BoneCount();

    for (_uint i = 0; i < boneCount; ++i)
    {
        shared_ptr<ModelBone> bone = m_pModel->Get_BoneByIndex(i);
        boneDesc->transform[i] = bone->transform *Utils::m_matPivot;
    }
    m_pShader->Push_BoneData(*boneDesc);

    // For. Bind data to shader 
    auto& world = Get_Transform()->Get_WorldMatrix();
    m_pShader->Push_TransformData(TransformDesc{ world });

    m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());
    Bind_RenderParams_ToShader();

    m_pShader->Push_LightData(CUR_SCENE->Get_LightParams());

    m_pShader->GetSRV("PositionTargetTex")->SetResource(RESOURCES.Get<Texture>(L"PositionForSSD")->Get_SRV().Get());
    m_pShader->GetSRV("originPositionTargetTex")->SetResource(RESOURCES.Get<Texture>(L"PositionTarget")->Get_SRV().Get());
	m_pShader->GetSRV("NormalTargetTex")->SetResource(RESOURCES.Get<Texture>(L"NormalTarget")->Get_SRV().Get());
	_float4x4 matInvWorldMat = Get_Transform()->Get_WorldMatrix().Invert();
    m_pShader->GetMatrix("InvWorldTransformMatrix")->SetMatrix((_float*)&matInvWorldMat);

    // For. Draw meshes 
    const auto& meshes = m_pModel->Get_Meshes();
    for (auto& mesh : meshes)
    {
        // For. Material tick 
        if (!mesh->material.expired())
        {
            mesh->vertexBuffer->Push_Data();
            mesh->indexBuffer->Push_Data();
        }
        m_pMaterial->Tick();
        m_pMaterial->Push_TextureMapData();

        m_pShader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

        // For. Setting Context Topology
        CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        _int techniqueIndex = m_tDesc.bIsSSD ? 2 : 0;
        // For. Draw call
        m_pShader->DrawIndexed(techniqueIndex, m_tDesc.iSamplerType, mesh->indexBuffer->Get_IndicesNum(), 0, 0);
    }
}

void MeshEffect::Render_Instancing(shared_ptr<InstancingBuffer> buffer, shared_ptr<StructuredBuffer> pRenderParamBuffer)
{
	if (m_pModel == nullptr)
		return;

	// Bones
	shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();

	const _uint boneCount = m_pModel->Get_BoneCount();

	for (_uint i = 0; i < boneCount; ++i)
	{
		shared_ptr<ModelBone> bone = m_pModel->Get_BoneByIndex(i);
		boneDesc->transform[i] = bone->transform * Utils::m_matPivot;
	}

	m_pShader->GetSRV("PositionTargetTex")->SetResource(RESOURCES.Get<Texture>(L"PositionForSSD")->Get_SRV().Get());
	m_pShader->GetSRV("originPositionTargetTex")->SetResource(RESOURCES.Get<Texture>(L"PositionTarget")->Get_SRV().Get());
	m_pShader->GetSRV("NormalTargetTex")->SetResource(RESOURCES.Get<Texture>(L"NormalTarget")->Get_SRV().Get());
    _float4x4 matInvWorldMat = Get_Transform()->Get_WorldMatrix().Invert();
	m_pShader->GetMatrix("InvWorldTransformMatrix")->SetMatrix((_float*)&matInvWorldMat);

	m_pShader->Push_BoneData(*boneDesc);

    buffer->Push_Data();

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

    m_pShader->GetSRV("g_effectData")->SetResource(pRenderParamBuffer->Get_SRV().Get());

	m_pShader->Push_LightData(CUR_SCENE->Get_LightParams());

	// For. Draw meshes 
	const auto& meshes = m_pModel->Get_Meshes();
	for (auto& mesh : meshes)
	{
		// For. Material tick 
		if (!mesh->material.expired()) 
		{
			mesh->vertexBuffer->Push_Data();
			mesh->indexBuffer->Push_Data();
		}
		m_pMaterial->Tick();
		m_pMaterial->Push_TextureMapData();

		m_pShader->GetScalar("BoneIndex")->SetInt(mesh->boneIndex);

		// For. Setting Context Topology
		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        _int techniqueIndex = m_tDesc.bIsSSD ? 3 : 1;
		// For. Draw call
		m_pShader->DrawIndexedInstanced(techniqueIndex, m_tDesc.iSamplerType, mesh->indexBuffer->Get_IndicesNum(), buffer->Get_Count());
	}
}

void MeshEffect::Update_RenderParams()
{
	if (m_bIsLocked)
		return;

	m_RenderParams.SetFloat(0, m_fLifeTimeRatio);
	m_RenderParams.SetFloat(1, m_fCurrDissolveWeight);
	m_RenderParams.SetFloat(2, m_fCurrRimLightIntensity);

	Bind_UpdatedColor_ToShader();
	Bind_UpdatedTexUVOffset_ToShader();
}

void MeshEffect::Update_Desc()
{
    m_fDuration = MathUtils::Get_RandomFloat(m_tDesc.vParticleDuration.x, m_tDesc.vParticleDuration.y);

    Init_RenderParams();

    if (m_tDesc.bUseSpriteAnim)
    {
        m_vSpriteSize.x = 1.f / m_tDesc.iNumSprite_Col;
        m_vSpriteSize.y = 1.f / m_tDesc.iNumSprite_Row;
        m_iCurrSpriteIndex = -1;
        m_iNumSprite = m_tDesc.iNumSprite_Col * m_tDesc.iNumSprite_Row;
        m_UVTexRangeX = _float2(0.f, m_vSpriteSize.x);
        m_UVTexRangeY = _float2(0.f, m_vSpriteSize.y);
    }    
    else
    {
        m_vCurrTexUVOffset_Op1 = m_tDesc.vTiling_Op1;
        m_vCurrTexUVOffset_Op2 = m_tDesc.vTiling_Op2;
        m_vCurrTexUVOffset_Op3 = m_tDesc.vTiling_Op3;
    }
    m_vCurrTexUVOffset_Blend = m_tDesc.vTiling_Blend;
    m_vCurrTexUVOffset_Overlay = m_tDesc.vTiling_Overlay;
    m_vCurrTexUVOffset_Dissolve = m_tDesc.vTiling_Dissolve;
    m_vCurrTexUVOffset_Distortion = m_tDesc.vTiling_Distortion;
    
    // For. Init Spline input
    for (int i = 0; i < 4; ++i) {
        // Dissolve 
        m_SplineInput_Dissolve[i * 2 + 0] = m_tDesc.vCurvePoint_Dissolve[i].x;
        m_SplineInput_Dissolve[i * 2 + 1] = m_tDesc.vCurvePoint_Dissolve[i].y;
    }

    // For. Model Components
    m_pModel = RESOURCES.Get<Model>(Utils::ToWString(m_tDesc.strVfxMesh));

    // For. Material Components
    m_pMaterial = RESOURCES.Get<Material>(Utils::ToWString(m_tDesc.strTag));
    if (nullptr == m_pMaterial)
    {
        Set_Material();
        RESOURCES.Add<Material>(Utils::ToWString(m_tDesc.strTag), m_pMaterial, true);
    }
    if (m_bToolMode_On)
        Set_Material();
}

void MeshEffect::InitialTransform(_float4x4 mParentWorldMatrix, const _float3& vInitPos_inGroup, const _float3& vInitScale_inGroup, const _float3& vInitRotation_inGroup)                    
{
    m_mInitGroupMatrix = mParentWorldMatrix;
    m_mInGroupWorldMatrix = _float4x4::CreateScale(vInitScale_inGroup)
                          * _float4x4::CreateRotationX(vInitRotation_inGroup.x)
                          * _float4x4::CreateRotationY(vInitRotation_inGroup.y)
                          * _float4x4::CreateRotationZ(vInitRotation_inGroup.z)
                          // * _float4x4::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(vInitRotation_inGroup.y, vInitRotation_inGroup.x, vInitRotation_inGroup.z))
                          * _float4x4::CreateTranslation(vInitPos_inGroup);
 
   _float4x4 matLocal = _float4x4::CreateScale(m_vStartScale)
		                  * _float4x4::CreateRotationX(m_vStartRotation.x)
		                  * _float4x4::CreateRotationY(m_vStartRotation.y)
		                  * _float4x4::CreateRotationZ(m_vStartRotation.z)
                          //* _float4x4::CreateFromQuaternion(Quaternion::CreateFromYawPitchRoll(m_vStartRotation.y, m_vStartRotation.x, m_vStartRotation.z)) 
                          * _float4x4::CreateTranslation(m_vStartPos);
    
		if (m_tDesc.iMeshCnt > 1)
			int a = 0;


    // move to parent space 
    Get_Transform()->Set_WorldMat(matLocal * m_mInGroupWorldMatrix * mParentWorldMatrix);
    
    matLocal.Decompose(m_vLocalScale, m_qRotation, m_vLocalPos);


    // Billbord 
    if (m_tTransform_Desc.iTurnOption == 3)
    {
        _float3 vCurrDir = Get_Transform()->Get_State(Transform_State::LOOK).xyz();
        vCurrDir.Normalize();

        _float3 vTargetDir = (CUR_SCENE->Get_MainCamera()->Get_Transform()->Get_State(Transform_State::POS) - Get_Transform()->Get_State(Transform_State::POS)).xyz();
        vTargetDir.Normalize();

        if (m_tTransform_Desc.bBillbordAxes[0])
        {
            vTargetDir.x = vCurrDir.x;
            vTargetDir.z = vCurrDir.z;
        }
        if (m_tTransform_Desc.bBillbordAxes[1])
            vTargetDir.y = vCurrDir.y;

        Get_Transform()->Set_LookDir(vTargetDir);
    }

    // For. Setting End Informations
    m_vStartPos += _float3(Get_Transform()->Get_State(Transform_State::POS));
    
    if( 1 != m_iTranslateOption && 2 != m_iTranslateOption && 10 != m_iTranslateOption)
        m_vEndPos += m_vStartPos;

    m_vStartScale *= vInitScale_inGroup;
    m_vEndScale += m_vStartScale;
    if (m_vEndScale.x < 0.f) m_vEndScale.x = 0.f;
    if (m_vEndScale.y < 0.f) m_vEndScale.y = 0.f;
    if (m_vEndScale.z < 0.f) m_vEndScale.z = 0.f;

    m_vStartRotation += vInitRotation_inGroup;
}

void MeshEffect::Set_TransformDesc(void* pArg)
{
    // For. Setting basic info  
    MeshEffectData::Transform_Desc* pDesc = (MeshEffectData::Transform_Desc*)pArg;
    m_tTransform_Desc = *pDesc;

    // For. Initial Transform 
    m_vStartPos = _float3(
        MathUtils::Get_RandomFloat(pDesc->vPosRange.x / 2.f * (-1.f), pDesc->vPosRange.x / 2.f),
        MathUtils::Get_RandomFloat(pDesc->vPosRange.y / 2.f * (-1.f), pDesc->vPosRange.y / 2.f),
        MathUtils::Get_RandomFloat(pDesc->vPosRange.z / 2.f * (-1.f), pDesc->vPosRange.z / 2.f)
    );

    m_vStartScale = _float3(
        MathUtils::Get_RandomFloat(pDesc->vInitScale_Min.x, pDesc->vInitScale_Max.x),
        MathUtils::Get_RandomFloat(pDesc->vInitScale_Min.y, pDesc->vInitScale_Max.y),
        MathUtils::Get_RandomFloat(pDesc->vInitScale_Min.z, pDesc->vInitScale_Max.z)
    );

    m_vStartRotation = _float3(
        MathUtils::Get_RandomFloat(pDesc->vInitRotation_Min.x, pDesc->vInitRotation_Max.x),
        MathUtils::Get_RandomFloat(pDesc->vInitRotation_Min.y, pDesc->vInitRotation_Max.y),
        MathUtils::Get_RandomFloat(pDesc->vInitRotation_Min.z, pDesc->vInitRotation_Max.z)
    );
    m_vStartRotation.x *= (3.141592f / 180.f);
    m_vStartRotation.y *= (3.141592f / 180.f);
    m_vStartRotation.z *= (3.141592f / 180.f);

    // For. Translate
    m_iTranslateOption = pDesc->iTranslateOption;
    m_fTranslateSpeed = pDesc->fTranslateSpeed;
    m_vEndPos = _float3(
        MathUtils::Get_RandomFloat(pDesc->vEndPosOffset_Min.x, pDesc->vEndPosOffset_Max.x),
        MathUtils::Get_RandomFloat(pDesc->vEndPosOffset_Min.y, pDesc->vEndPosOffset_Max.y),
        MathUtils::Get_RandomFloat(pDesc->vEndPosOffset_Min.z, pDesc->vEndPosOffset_Max.z)
    );

    m_vEndScale = pDesc->vEndScale;

    m_iTurnOption = pDesc->iTurnOption;
    m_fTurnSpeed = pDesc->fTurnSpeed;
    m_vRandomAxis = _float3(
        MathUtils::Get_RandomFloat(pDesc->vRandomAxis_Min.x, pDesc->vRandomAxis_Max.x),
        MathUtils::Get_RandomFloat(pDesc->vRandomAxis_Min.y, pDesc->vRandomAxis_Max.y),
        MathUtils::Get_RandomFloat(pDesc->vRandomAxis_Min.z, pDesc->vRandomAxis_Max.z)
    );

    // For. Init Spline input
    for (int i = 0; i < 4; ++i) {
        // Translate Force 
        m_SplineInput_Force[i * 2 + 0] = m_tTransform_Desc.vCurvePoint_Force[i].x;
        m_SplineInput_Force[i * 2 + 1] = m_tTransform_Desc.vCurvePoint_Force[i].y;
        // Scale Speed
        m_SplineInput_ScaleSpeed[i * 2 + 0] = m_tTransform_Desc.vCurvePoint_Scale[i].x;
        m_SplineInput_ScaleSpeed[i * 2 + 1] = m_tTransform_Desc.vCurvePoint_Scale[i].y;
    }

    if (9 == m_iTranslateOption)
        m_fCurrYspeed = m_tTransform_Desc.vCurvePoint_Force[0].y;
}

void MeshEffect::Set_Material()
{
    if(m_pMaterial == nullptr)
        m_pMaterial = make_shared<Material>();

    // Shader
    shared_ptr<Shader> shader;
    if(!m_tDesc.bIsFDistortion)
        shader = RESOURCES.Get<Shader>(L"Shader_Effect2.fx");
	else 
		shader = RESOURCES.Get<Shader>(L"Shader_Distortion.fx");
    m_pMaterial->Set_Shader(shader);

    // Texture 
    wstring wstrKey = Utils::ToWString(m_tDesc.strTexture_Op1);
    wstring wstrPath = TEXT("../Resources/Textures/Universal/") + wstrKey;
    if (TEXT("None") != wstrKey)
        m_pMaterial->Set_TextureMap(RESOURCES.Load<Texture>(wstrKey, wstrPath), TextureMapType::TEXTURE7);
    else 
        m_pMaterial->Set_TextureMap(nullptr, TextureMapType::TEXTURE7);

    wstrKey = Utils::ToWString(m_tDesc.strTexture_Op2);
    wstrPath = TEXT("../Resources/Textures/Universal/") + wstrKey;
    if (TEXT("None") != wstrKey)
        m_pMaterial->Set_TextureMap(RESOURCES.Load<Texture>(wstrKey, wstrPath), TextureMapType::TEXTURE8);
    else
        m_pMaterial->Set_TextureMap(nullptr, TextureMapType::TEXTURE8);

    wstrKey = Utils::ToWString(m_tDesc.strTexture_Op3);
    wstrPath = TEXT("../Resources/Textures/Universal/") + wstrKey;
    if (TEXT("None") != wstrKey)
        m_pMaterial->Set_TextureMap(RESOURCES.Load<Texture>(wstrKey, wstrPath), TextureMapType::TEXTURE9);
    else
        m_pMaterial->Set_TextureMap(nullptr, TextureMapType::TEXTURE9);

    wstrKey = Utils::ToWString(m_tDesc.strTexture_Blend);
    wstrPath = TEXT("../Resources/Textures/Universal/") + wstrKey;
    if (TEXT("None") != wstrKey)
        m_pMaterial->Set_TextureMap(RESOURCES.Load<Texture>(wstrKey, wstrPath), TextureMapType::TEXTURE10);
    else
        m_pMaterial->Set_TextureMap(nullptr, TextureMapType::TEXTURE10);

    wstrKey = Utils::ToWString(m_tDesc.strOverlayTexture);
    wstrPath = TEXT("../Resources/Textures/Universal/") + wstrKey;
    if (TEXT("None") != wstrKey)
        m_pMaterial->Set_TextureMap(RESOURCES.Load<Texture>(wstrKey, wstrPath), TextureMapType::TEXTURE11);
    else
        m_pMaterial->Set_TextureMap(nullptr, TextureMapType::TEXTURE11);

    wstrKey = Utils::ToWString(m_tDesc.strDissolveTexture);
    wstrPath = TEXT("../Resources/Textures/Universal/") + wstrKey;
    if (TEXT("None") != wstrKey)
        m_pMaterial->Set_TextureMap(RESOURCES.Load<Texture>(wstrKey, wstrPath), TextureMapType::DISSOLVE);
    else
        m_pMaterial->Set_TextureMap(nullptr, TextureMapType::DISSOLVE);

    wstrKey = Utils::ToWString(m_tDesc.strDistortionTexture);
    wstrPath = TEXT("../Resources/Textures/Universal/") + wstrKey;
    if (TEXT("None") != wstrKey)
        m_pMaterial->Set_TextureMap(RESOURCES.Load<Texture>(wstrKey, wstrPath), TextureMapType::DISTORTION);
    else
        m_pMaterial->Set_TextureMap(nullptr, TextureMapType::DISTORTION);

    wstrKey = Utils::ToWString(m_tDesc.strNormalTexture);
    wstrPath = TEXT("../Resources/Textures/Universal/") + wstrKey;
    if (TEXT("None") != wstrKey)
        m_pMaterial->Set_TextureMap(RESOURCES.Load<Texture>(wstrKey, wstrPath), TextureMapType::NORMAL);
    else
        m_pMaterial->Set_TextureMap(nullptr, TextureMapType::NORMAL);
}

void MeshEffect::Translate()
{
    switch (m_iTranslateOption)
    {
    case 0:
        break;
    case 1: // Move to direction 
    case 2: // Move to random direction 
    {
        _float fSpeed = Calc_Spline(m_tTransform_Desc.iSpeedType, m_SplineInput_Force);
        if (m_bToolMode_On)
        {
            _float4 vCurrPos = Get_Transform()->Get_State(Transform_State::POS);

            _float3 vDir = m_vEndPos;
            vDir.Normalize();

            vCurrPos += _float4(vDir * fSpeed * fDT, 0.f);

            Get_Transform()->Set_State(Transform_State::POS, vCurrPos);
        }
        else
        {
            _float3 vDir = m_vEndPos;
            vDir.Normalize();

            m_vLocalPos += vDir * fSpeed * fDT;
        }

        break;
    }
    case 3:
    {
        Get_Transform()->Set_Speed(Calc_Spline(m_tTransform_Desc.iSpeedType, m_SplineInput_Force));

        if (m_bToolMode_On)
        {
            Get_Transform()->Go_Straight();
        }
        else
        {
            _float3 vDir = Get_LocalMatrix().Backward();
            vDir.Normalize();
            m_vLocalPos += vDir * Get_Transform()->Get_Speed() * fDT;
        }
        break;
    }
    case 4:
    {
        Get_Transform()->Set_Speed(Calc_Spline(m_tTransform_Desc.iSpeedType, m_SplineInput_Force));

        if (m_bToolMode_On)
        {
            Get_Transform()->Go_Backward();
        }
        else
        {
            _float3 vDir = Get_LocalMatrix().Backward();
            vDir.Normalize();
            m_vLocalPos -= vDir * Get_Transform()->Get_Speed() * fDT;
        }
        break;
    }
    case 5:
    {
        Get_Transform()->Set_Speed(Calc_Spline(m_tTransform_Desc.iSpeedType, m_SplineInput_Force));

        if (m_bToolMode_On)
        {
            Get_Transform()->Go_Left();
        }
        else
        {
            _float3 vDir = Get_LocalMatrix().Right();
            vDir.Normalize();
            m_vLocalPos -= vDir * Get_Transform()->Get_Speed() * fDT;
        }
        break;
    }
    case 6:
    {
        Get_Transform()->Set_Speed(Calc_Spline(m_tTransform_Desc.iSpeedType, m_SplineInput_Force));

        if (m_bToolMode_On)
        {
            Get_Transform()->Go_Right();
        }
        else
        {
            _float3 vDir = Get_LocalMatrix().Right();
            vDir.Normalize();
            m_vLocalPos += vDir * Get_Transform()->Get_Speed() * fDT;
        }
        break;
    }
    case 7:
    {
        Get_Transform()->Set_Speed(Calc_Spline(m_tTransform_Desc.iSpeedType, m_SplineInput_Force));

        if (m_bToolMode_On)
        {
            Get_Transform()->Go_Up();
        }
        else
        {
            _float3 vDir = Get_LocalMatrix().Up();
            vDir.Normalize();
            m_vLocalPos += vDir * Get_Transform()->Get_Speed() * fDT;

        }
        break;
    }
    case 8:
    {
        Get_Transform()->Set_Speed(Calc_Spline(m_tTransform_Desc.iSpeedType, m_SplineInput_Force));

        if (m_bToolMode_On)
        {
            Get_Transform()->Go_Down();
        }
        else
        {
            _float3 vDir = Get_LocalMatrix().Up();
            vDir.Normalize();
            m_vLocalPos -= vDir * Get_Transform()->Get_Speed() * fDT;

        }
        break;
    }
    case 9: // fountain 
    {
        if (m_bToolMode_On)
        {
            _float4 vPos = Get_Transform()->Get_State(Transform_State::POS);
            _float4 vDir = Get_Transform()->Get_State(Transform_State::LOOK);
            vDir.Normalize();
            vDir.y = 0.f;

            vPos += vDir * (m_tTransform_Desc.vCurvePoint_Force[0].x * fDT);
            m_fCurrYspeed += 10.0f * fDT;
            vPos.y -= m_fCurrYspeed * fDT;

            Get_Transform()->Set_State(Transform_State::POS, vPos);
        }
        else
        {
            _float3 vDir = Get_LocalMatrix().Backward();
            vDir.Normalize();

            m_vLocalPos += vDir * (m_tTransform_Desc.vCurvePoint_Force[0].x * fDT);
            m_fCurrYspeed += 9.8f * fDT;
            m_vLocalPos.y -= m_fCurrYspeed * fDT;
        }

        break;
    }
    case 10: // Move to target position 
    {
        Get_Transform()->Set_Speed(Calc_Spline(m_tTransform_Desc.iSpeedType, m_SplineInput_Force));

        if (m_bToolMode_On)
            Get_Transform()->Set_State(Transform_State::POS, _float4(_float3::Lerp(m_vStartPos, m_vEndPos, m_fLifeTimeRatio), 1.f));
        else
        {
            m_vLocalPos = _float3::Lerp(m_vStartPos, m_vEndPos, m_fLifeTimeRatio);
        }
        break;

    }
    }
}

void MeshEffect::Scaling()
{
    _float3 vScale = Get_Transform()->Get_Scale();
    if (m_bToolMode_On)
    {
        // lerp 
        if (2 == m_tTransform_Desc.iScaleSpeedType)
        {
            vScale = XMVectorLerp(m_vStartScale, m_vEndScale, m_fLifeTimeRatio);
            if (m_tDesc.bIsSSD)
                vScale.y = m_vStartScale.y;

            if (vScale.x < 0) vScale.x = 0.f;
            if (vScale.y < 0) vScale.y = 0.f;
            if (vScale.z < 0) vScale.z = 0.f;

        }
        // scaling by scale speed
        else
        {
            _float fScaleSpeed = Calc_Spline(m_tTransform_Desc.iScaleSpeedType, m_SplineInput_ScaleSpeed);
            vScale = Get_Transform()->Get_Scale() + _float3(fScaleSpeed * fDT);
            if (m_tDesc.bIsSSD)
                vScale.y = m_vStartScale.y;
        }

        Get_Transform()->Scaled(vScale);
    }
    else
    {
        // lerp 
        if (2 == m_tTransform_Desc.iScaleSpeedType)
        {
            m_vLocalScale = _float3::Lerp(m_vStartScale, m_vEndScale, m_fLifeTimeRatio);
            if (m_tDesc.bIsSSD)
                m_vLocalScale.y = m_vStartScale.y;
            if (m_vLocalScale.x < 0) m_vLocalScale.x = 0.f;
            if (m_vLocalScale.y < 0) m_vLocalScale.y = 0.f;
            if (m_vLocalScale.z < 0) m_vLocalScale.z = 0.f;
        }
        // scaling by scale speed
        else
        {
            _float fScaleSpeed = Calc_Spline(m_tTransform_Desc.iScaleSpeedType, m_SplineInput_ScaleSpeed);
            m_vLocalScale = Get_Transform()->Get_Scale() + _float3(fScaleSpeed * fDT);
            if (m_tDesc.bIsSSD)
                m_vLocalScale.y = m_vStartScale.y;
        }
    }
}

void MeshEffect::Turn()
{
    // Billbord 
    if (m_tTransform_Desc.iTurnOption == 3)
    {
        //
        BillBoard();
    }
    else
    {
        if (m_vRandomAxis.x == 0.f && m_vRandomAxis.y == 0.f && m_vRandomAxis.z == 0.f)
            return;
        
        if(m_bToolMode_On)
            Get_Transform()->Turn(m_vRandomAxis, m_fTurnSpeed);
        else
        {
            m_qRotation = Quaternion::CreateFromRotationMatrix(_float4x4::CreateFromQuaternion(m_qRotation)* _float4x4::CreateFromAxisAngle(m_vRandomAxis, m_fTurnSpeed * fDT));
        }
    }
}

void MeshEffect::Run_SpriteAnimation()
{
    if (m_fTimeAcc_SpriteAnimation > m_tDesc.fAnimSpeed)
    {
        m_iCurrSpriteIndex++;

        if (m_iCurrSpriteIndex >= m_iNumSprite)
            m_iCurrSpriteIndex = 0;

        _int iIndexX = m_iCurrSpriteIndex % m_tDesc.iNumSprite_Col;
        _int iIndexY = m_iCurrSpriteIndex / m_tDesc.iNumSprite_Col;

        m_UVTexRangeX.x = (_float)iIndexX / (_float)m_tDesc.iNumSprite_Col;
        m_UVTexRangeX.y = ((_float)iIndexX + 1) / (_float)m_tDesc.iNumSprite_Col;
        m_UVTexRangeY.x = (_float)iIndexY / (_float)m_tDesc.iNumSprite_Row;
        m_UVTexRangeY.y = ((_float)iIndexY + 1) / (_float)m_tDesc.iNumSprite_Row;

        m_fTimeAcc_SpriteAnimation = 0.f;
    }
}

void MeshEffect::Init_RenderParams()
{
    _float2 vTemp2;
    _float4 vTemp4;
    _float4x4 vTemp4x4;
    
    /* Int */
    m_RenderParams.SetInt(0, (_int)m_tDesc.bUseFadeOut);
    m_RenderParams.SetInt(1, (_int)m_tDesc.bIsRimLightOn);
    m_RenderParams.SetInt(2, (_int)m_tDesc.bUseSpriteAnim);
    m_RenderParams.SetInt(3, (_int)m_tDesc.bLightOn);

    /* Float */
    m_RenderParams.SetFloat(0, m_fCurrAge / m_fDuration);
    m_RenderParams.SetFloat(1, m_fCurrDissolveWeight);
    m_RenderParams.SetFloat(2, m_fCurrRimLightIntensity);
    m_RenderParams.SetFloat(3, m_tDesc.fLightIntensity);

    /* Float2 */
    vTemp2 = _float2(m_tDesc.fContrast_Op1, m_tDesc.fAlphaOffset_Op1);
    m_RenderParams.SetVec2(0, vTemp2);
    vTemp2 = _float2(m_tDesc.fContrast_Op2, m_tDesc.fAlphaOffset_Op2);
    m_RenderParams.SetVec2(1, vTemp2);
    vTemp2 = _float2(m_tDesc.fContrast_Op3, m_tDesc.fAlphaOffset_Op3);
    m_RenderParams.SetVec2(2, vTemp2);
    vTemp2 = _float2(m_vCurrTexUVOffset_Distortion.x, m_vCurrTexUVOffset_Distortion.y);
    m_RenderParams.SetVec2(3, vTemp2);

    /* Float4 */
    vTemp4 = _float4(m_vCurrTexUVOffset_Op1.x, m_vCurrTexUVOffset_Op1.y, m_vCurrTexUVOffset_Op2.x, m_vCurrTexUVOffset_Op2.y);
    m_RenderParams.SetVec4(0, vTemp4);
    vTemp4 = _float4(m_vCurrTexUVOffset_Op3.x, m_vCurrTexUVOffset_Op3.y, m_vCurrTexUVOffset_Blend.x, m_vCurrTexUVOffset_Blend.y);
    m_RenderParams.SetVec4(1, vTemp4);
    vTemp4 = _float4(m_vCurrTexUVOffset_Overlay.x, m_vCurrTexUVOffset_Overlay.y, m_vCurrTexUVOffset_Dissolve.x, m_vCurrTexUVOffset_Dissolve.y);
    m_RenderParams.SetVec4(2, vTemp4);
    vTemp4 = _float4(m_UVTexRangeX.x, m_UVTexRangeX.y, m_UVTexRangeY.x, m_UVTexRangeY.y);
    m_RenderParams.SetVec4(3, vTemp4);

    /* Mat4x4 */
    vTemp4x4 = _float4x4(
        m_tDesc.vBaseColor1_Op1, 
        m_tDesc.vBaseColor2_Op1,
        m_tDesc.vBaseColor1_Op2,
        m_tDesc.vBaseColor2_Op2
    );
    m_RenderParams.SetMatrix(0, vTemp4x4);
    vTemp4x4 = _float4x4(
        m_tDesc.vBaseColor1_Op3,
        m_tDesc.vBaseColor2_Op3,
        m_tDesc.vBaseColor_Overlay,
        m_tDesc.vBaseColor_RimLight
    );
    m_RenderParams.SetMatrix(1, vTemp4x4);
    vTemp4x4 = _float4x4(
        _float4((_float)m_tDesc.bIsUseTextureColor_Op1, (_float)m_tDesc.bIsUseTextureColor_Op2, (_float)m_tDesc.bIsUseTextureColor_Op3, 0.f),
        _float4((_float)m_tDesc.iFlipOption_Op1, (_float)m_tDesc.iFlipOption_Op2, (_float)m_tDesc.iFlipOption_Op3, m_tDesc.fAlphaOffset_Blend),
        _float4((_float)m_tDesc.bIsSSD, 0.f, 0.f, 0.f),
        _float4(0.f, 0.f, 0.f, 0.f)
    );
    m_RenderParams.SetMatrix(2, vTemp4x4);
}

void MeshEffect::Bind_UpdatedColor_ToShader()
{
    _float4x4 vTemp4x4;

    Color vFinalColor1_Op1 = XMVectorLerp(m_tDesc.vBaseColor1_Op1, m_tDesc.vDestColor1_Op1, m_fLifeTimeRatio);
    Color vFinalColor2_Op1 = XMVectorLerp(m_tDesc.vBaseColor2_Op1, m_tDesc.vDestColor2_Op1, m_fLifeTimeRatio);
    Color vFinalColor1_Op2 = XMVectorLerp(m_tDesc.vBaseColor1_Op2, m_tDesc.vDestColor1_Op2, m_fLifeTimeRatio);
    Color vFinalColor2_Op2 = XMVectorLerp(m_tDesc.vBaseColor2_Op2, m_tDesc.vDestColor2_Op2, m_fLifeTimeRatio);
    Color vFinalColor1_Op3 = XMVectorLerp(m_tDesc.vBaseColor1_Op3, m_tDesc.vDestColor1_Op3, m_fLifeTimeRatio);
    Color vFinalColor2_Op3 = XMVectorLerp(m_tDesc.vBaseColor2_Op3, m_tDesc.vDestColor2_Op3, m_fLifeTimeRatio);
    Color vFinalColor_Overlay = XMVectorLerp(m_tDesc.vBaseColor_Overlay, m_tDesc.vDestColor_Overlay, m_fLifeTimeRatio);
    Color vFinalColor_RimLight = XMVectorLerp(m_tDesc.vBaseColor_RimLight, m_tDesc.vBaseColor_RimLight, m_fLifeTimeRatio);
    vTemp4x4 = _float4x4(
        vFinalColor1_Op1,
        vFinalColor2_Op1,
        vFinalColor1_Op2,
        vFinalColor2_Op2
    );
    m_RenderParams.SetMatrix(0, vTemp4x4);
    vTemp4x4 = _float4x4(
        vFinalColor1_Op3,
        vFinalColor2_Op3,
        vFinalColor_Overlay,
        vFinalColor_RimLight
    );
    m_RenderParams.SetMatrix(1, vTemp4x4);
}

void MeshEffect::Bind_UpdatedTexUVOffset_ToShader()
{
    _float2 vTemp2;
    _float4 vTemp4;

    // For. Update UV Offset
    if (m_tDesc.bUseSpriteAnim)
        Run_SpriteAnimation();
    else
    {
        m_vCurrTexUVOffset_Op1 += m_tDesc.vUVSpeed_Op1 * fDT;
        m_vCurrTexUVOffset_Op2 += m_tDesc.vUVSpeed_Op2 * fDT;
        m_vCurrTexUVOffset_Op3 += m_tDesc.vUVSpeed_Op3 * fDT;
        m_vCurrTexUVOffset_Blend += m_tDesc.vUVSpeed_Blend * fDT;
    }
    m_vCurrTexUVOffset_Overlay += m_tDesc.vUVSpeed_Overlay * fDT;
    m_vCurrTexUVOffset_Dissolve += m_tDesc.vUVSpeed_Dissolve * fDT;
    m_vCurrTexUVOffset_Distortion += m_tDesc.vUVSpeed_Distortion * fDT;

    vTemp2 = _float2(m_vCurrTexUVOffset_Distortion.x, m_vCurrTexUVOffset_Distortion.y);
    m_RenderParams.SetVec2(3, vTemp2);
    vTemp4 = _float4(m_vCurrTexUVOffset_Op1.x, m_vCurrTexUVOffset_Op1.y, m_vCurrTexUVOffset_Op2.x, m_vCurrTexUVOffset_Op2.y);
    m_RenderParams.SetVec4(0, vTemp4);
    vTemp4 = _float4(m_vCurrTexUVOffset_Op3.x, m_vCurrTexUVOffset_Op3.y, m_vCurrTexUVOffset_Blend.x, m_vCurrTexUVOffset_Blend.y);
    m_RenderParams.SetVec4(1, vTemp4);
    vTemp4 = _float4(m_vCurrTexUVOffset_Overlay.x, m_vCurrTexUVOffset_Overlay.y, m_vCurrTexUVOffset_Dissolve.x, m_vCurrTexUVOffset_Dissolve.y);
    m_RenderParams.SetVec4(2, vTemp4);
    vTemp4 = _float4(m_UVTexRangeX.x, m_UVTexRangeX.y, m_UVTexRangeY.x, m_UVTexRangeY.y);
    m_RenderParams.SetVec4(3, vTemp4);
}

void MeshEffect::Bind_RenderParams_ToShader()
{
    if (m_bIsLocked)
        return;

    m_RenderParams.SetFloat(0, m_fLifeTimeRatio);    
    m_RenderParams.SetFloat(1, m_fCurrDissolveWeight);
    m_RenderParams.SetFloat(2, m_fCurrRimLightIntensity);

    Bind_UpdatedColor_ToShader();
    Bind_UpdatedTexUVOffset_ToShader();

    // For. Bind Data 
    m_pShader->Push_RenderParamData(m_RenderParams);
}

_float MeshEffect::Calc_Spline(_int iType, _float* vSplineInput)
{
    _float fSpeed = 0.f;
    if (0 == iType)
    {
        _float output[4];
        Utils::Spline(vSplineInput, 4, 1, m_fLifeTimeRatio, output);
        fSpeed = output[0];
    }
    else if (1 == iType)
    {
        if (0.f <= m_fLifeTimeRatio && m_fLifeTimeRatio < vSplineInput[0])
            fSpeed = 0.f;
        else if (vSplineInput[0] <= m_fLifeTimeRatio && m_fLifeTimeRatio < vSplineInput[2])
        {
            _float fRatio = (m_fLifeTimeRatio - vSplineInput[0]) / (vSplineInput[2] - vSplineInput[0]);
            _float2 vTemp2 = XMVectorLerp(_float2(vSplineInput[1], 0.f), _float2(vSplineInput[3], 0.f), fRatio);
            fSpeed = vTemp2.x;
        }
        else if (vSplineInput[2] <= m_fLifeTimeRatio && m_fLifeTimeRatio < vSplineInput[4])
        {
            _float fRatio = (m_fLifeTimeRatio - vSplineInput[2]) / (vSplineInput[4] - vSplineInput[2]);
            _float2 vTemp2 = XMVectorLerp(_float2(vSplineInput[3], 0.f), _float2(vSplineInput[5], 0.f), fRatio);
            fSpeed = vTemp2.x;
        }
        else if (vSplineInput[4] <= m_fLifeTimeRatio && m_fLifeTimeRatio < vSplineInput[6])
        {
            _float fRatio = (m_fLifeTimeRatio - vSplineInput[4]) / (vSplineInput[6] - vSplineInput[4]);
            _float2 vTemp2 = XMVectorLerp(_float2(vSplineInput[5], 0.f), _float2(vSplineInput[7], 0.f), fRatio);
            fSpeed = vTemp2.x;
        }
        else
            fSpeed = 0.f;
    }

    return fSpeed;
}

void MeshEffect::BillBoard()
{
	if (m_tTransform_Desc.iTurnOption == 3)
	{
		_float3 vCurrDir = Get_Transform()->Get_State(Transform_State::LOOK).xyz();
		vCurrDir.Normalize();

		_float3 vTargetDir = (CUR_SCENE->Get_MainCamera()->Get_Transform()->Get_State(Transform_State::POS) - Get_Transform()->Get_State(Transform_State::POS)).xyz();
		vTargetDir.Normalize();

		if (m_tTransform_Desc.bBillbordAxes[0])
		{
            vTargetDir.y = 0.f;
		}
		if (m_tTransform_Desc.bBillbordAxes[1])
			vTargetDir.y = 0.f;

		Get_Transform()->Set_LookDir(vTargetDir);
	}
}

_float4x4 MeshEffect::Get_LocalMatrix()
{
    
    return _float4x4::CreateScale(m_vLocalScale) * _float4x4::CreateFromQuaternion(m_qRotation) * _float4x4::CreateTranslation(m_vLocalPos);
}

InstanceID MeshEffect::Get_InstanceID()
{
	return make_pair(_ulonglong(m_iRenderPriority), _ulonglong(m_pMaterial.get()));
}

static _float3 ToEulerAngles(Quaternion q)
{
    _float3 angles;

    float sqw = q.w * q.w;
    float sqx = q.x * q.x;
    float sqy = q.y * q.y;
    float sqz = q.z * q.z;
    angles.x = asinf(2.0f * (q.w * q.x - q.y * q.z)); // rotation about x-axis
    angles.y = atan2f(2.0f * (q.x * q.z + q.w * q.y), (-sqx - sqy + sqz + sqw)); // rotation about y-axis
    angles.z = atan2f(2.0f * (q.x * q.y + q.w * q.z), (-sqx + sqy - sqz + sqw)); // rotation about z-axis

    if (isnan(angles.x) || isnan(angles.y) || isnan(angles.z))
    {
        int a = 0;
    }

    return angles;
}