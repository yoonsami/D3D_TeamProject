#include "pch.h"
#include "Particle.h"

#include "Utils.h"
#include "Camera.h"
#include "StructuredBuffer.h"

Particle::Particle(shared_ptr<Shader> shader)
	: Component(COMPONENT_TYPE::Particle)
	, m_pShader(shader)
{
}

Particle::~Particle()
{
}

HRESULT Particle::Init(void* pArg)
{
	// For. Setting basic info  
	DESC* pDesc = (DESC*)pArg;
	m_tDesc = *pDesc;

	if (!m_pModel)
	{
		m_eType = TYPE_InstanceParticle;
		m_pMesh = RESOURCES.Get<Mesh>(L"Point");
	}
	else 
		m_eType = TYPE_RectParticle;

	m_eComputePass = (COMPUTE_PASS)m_tDesc.iMovementOption;
	m_eRenderPass = (RENDER_INSTANCEPARTICLE_PASS)m_tDesc.iBillbordOption;

	// For. Texture
	m_pMaterial = make_shared<Material>();
	m_pMaterial->Set_Shader(m_pShader);
	if ("../Resources/Textures/Universal/None" != m_tDesc.strSelected_Texture_Shape)
		m_pMaterial->Set_TextureMap(RESOURCES.Load<Texture>(L"ParticleTexture_shape", Utils::ToWString(m_tDesc.strSelected_Texture_Shape)), TextureMapType::DIFFUSE);
	if("../Resources/Textures/Universal/None" != m_tDesc.strSelected_Texture_Dissolve)
		m_pMaterial->Set_TextureMap(RESOURCES.Load<Texture>(L"ParticleTexture_dissolve", Utils::ToWString(m_tDesc.strSelected_Texture_Dissolve)), TextureMapType::DISSOLVE);

	// For. Initialize Shader Params
	Init_ComputeParams();
	Init_RenderParams();
	Init_CreateParticleParams();

	// For. Create ParticleInfo UAV Buffers  
	vector<ParticleInfo_UAV> tmp(m_tDesc.iMaxInstanceCnt);
	if (!m_pParticleInfo_UAVBuffer)
		m_pParticleInfo_UAVBuffer = make_shared<StructuredBuffer>(tmp.data(), static_cast<_uint>(sizeof ParticleInfo_UAV), m_tDesc.iMaxInstanceCnt);

	// For. Create ComputeShader UAV Buffers  
	if (!m_pComputeShared_UAVBuffer)
		m_pComputeShared_UAVBuffer = make_shared<StructuredBuffer>(nullptr, static_cast <_uint>(sizeof m_pComputeShared_UAVBuffer), 1);

	return S_OK;
}

void Particle::Tick()
{
}

void Particle::Final_Tick()
{
	// For. Update TimeAcc 
	m_fCurrAge += fDT;
	m_fTimeAcc_CreatCoolTime += fDT;

	// For. Check End(Delete) this particle 
	if (m_fCurrAge >= m_tDesc.fDuration)
	{
		EVENTMGR.Delete_Object(Get_Owner());
		return;
	}

	// For. Decide whether to create a new Particle and Cnt
	m_CreateParticleParams.iNewlyAddCnt = 0;
	if (m_tDesc.bIsbCreateOnce)
	{
		if (!m_bIsFirstCreateParticleDone)
		{
			// min ~ max 사이의 랜덤한 수를 골라 그만큼 새로 생성한다. 
			m_CreateParticleParams.iNewlyAddCnt = (rand() % (m_tDesc.iMaxCnt + 1 - m_tDesc.iMinCnt)) + m_tDesc.iMinCnt;
			m_fTimeAcc_CreatCoolTime = 0.f;
			m_bIsFirstCreateParticleDone = true;
		}
	}
	else 
	{
		if (m_fTimeAcc_CreatCoolTime > m_tDesc.fCreateInterval)
		{
			// min ~ max 사이의 랜덤한 수를 골라 그만큼 새로 생성한다. 
			m_CreateParticleParams.iNewlyAddCnt = (rand() % (m_tDesc.iMaxCnt + 1 - m_tDesc.iMinCnt)) + m_tDesc.iMinCnt;
			m_fTimeAcc_CreatCoolTime = 0.f;
		}
	}
	
	// For. Bind Params and data to Shader 
	auto& world = Get_Transform()->Get_WorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world });

	Bind_ComputeParams_ToShader();
	Bind_CreateParticleParams_ToShader();

	// For. Bind UAV Buffer in shader ( 쉐이더가 값을 보관할 수 없기 때문에 매 프레임마다 넘겨줘야한다.)
	m_pShader->GetUAV("g_ParticleInfo_UAVBuffer")->SetUnorderedAccessView(m_pParticleInfo_UAVBuffer->Get_UAV().Get());
	m_pShader->GetUAV("g_ComputeShared_UAVBuffer")->SetUnorderedAccessView(m_pComputeShared_UAVBuffer->Get_UAV().Get());	 

	// For. Dispatch ( Get UAV data from shader )
	m_pShader->Dispatch(TECHNIQUE_Compute, m_eComputePass, 1, 1, 1);
	vector<ParticleInfo_UAV> tmp(m_tDesc.iMaxInstanceCnt);
	m_pParticleInfo_UAVBuffer->Copy_FromOutput(tmp.data());
	m_pParticleInfo_UAVBuffer->Copy_ToInput(tmp.data());
}

void Particle::Render()
{
	// For. Render Standard Mesh
	switch (m_eType)
	{
	case TYPE_InstanceParticle:
		if (nullptr == m_pMesh || nullptr == m_pMaterial)
			return;

		// For. Material Tick ( for Bind Texture to Shader )
		m_pMaterial->Tick();

		// For. Bind data to shader
		Bind_BasicData_ToShader();
		Bind_RenderParams_ToShader(); 

		if (0 == m_tDesc.iColorOption)
			m_pShader->GetScalar("g_bUseShapeTextureColor")->SetBool(true);
		else
			m_pShader->GetScalar("g_bUseShapeTextureColor")->SetBool(false);
		
		m_pMesh->Get_VertexBuffer()->Push_Data();
		m_pMesh->Get_IndexBuffer()->Push_Data();
		
		// For. Setting Context Topology
		CONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		
		// For. Draw call
		m_pShader->DrawIndexedInstanced(TECHNIQUE_InstanceParticle, m_eRenderPass, m_pMesh->Get_IndexBuffer()->Get_IndicesNum(), m_tDesc.iMaxInstanceCnt);
		break;

	case TYPE_RectParticle:
		if (nullptr == m_pModel)
			return;
		
		// For. Bind data to shader
		Bind_BasicData_ToShader();
		Bind_RenderParams_ToShader();

		// TODO 

		break;
	}
}

void Particle::Init_RenderParams()
{
	// For. Diffuse Color
	m_RenderParams.SetFloat(0, m_tDesc.fGradationByAlpha_Brighter);
	m_RenderParams.SetFloat(1, m_tDesc.fGradationByAlpha_Darker);
	m_RenderParams.SetVec4(0, m_tDesc.vDestColor);

	// Duration and Current t7ime 
	m_RenderParams.SetFloat(2, m_tDesc.fDuration);
	m_RenderParams.SetFloat(3, m_fCurrAge);
}

void Particle::Init_CreateParticleParams()
{
	// For. Scale 
	m_CreateParticleParams.iEndScaleOption = m_tDesc.iEndScaleOption;
	m_CreateParticleParams.vMinMaxStartScale = m_tDesc.vStartScale;
	m_CreateParticleParams.vMinMaxEndScale = m_tDesc.vEndScale;

	// For. Create Position 
	m_CreateParticleParams.vCreateRange = _float4(m_tDesc.vCreateRange, 0.f);
	m_CreateParticleParams.vCreateOffsets = m_tDesc.vCreateOffsets;

	// For. Rotation 
	m_CreateParticleParams.vMinMaxRotationSpeed_X = m_tDesc.vRotationSpeed_X;
	m_CreateParticleParams.vMinMaxRotationSpeed_Y = m_tDesc.vRotationSpeed_Y;
	m_CreateParticleParams.vMinMaxRotationSpeed_Z = m_tDesc.vRotationSpeed_Z;
	m_CreateParticleParams.vMinMaxRotationAngle_X = m_tDesc.vRotationAngle_X;
	m_CreateParticleParams.vMinMaxRotationAngle_Y = m_tDesc.vRotationAngle_Y;
	m_CreateParticleParams.vMinMaxRotationAngle_Z = m_tDesc.vRotationAngle_Z;
	m_CreateParticleParams.iRandomRotationOn = m_tDesc.bRandomRotationOn;
	
	// For. Speed
	m_CreateParticleParams.iEndSpeedOption = m_tDesc.iEndSpeedOption;
	m_CreateParticleParams.vMinMaxStartSpeed = m_tDesc.vStartSpeed;
	m_CreateParticleParams.vMinMaxEndSpeed = m_tDesc.vEndSpeed;

	// For. LifeTime
	m_CreateParticleParams.vMinMaxLifeTime = m_tDesc.vLifeTime;

	// For. Diffuse Color 
	m_CreateParticleParams.vStartColor = m_tDesc.vStartColor;
	m_CreateParticleParams.vEndColor = m_tDesc.vEndColor;
}

void Particle::Init_ComputeParams()
{
	// For. Max instance cnt
	m_ComputeParams.SetInt(0, m_tDesc.iMaxInstanceCnt);

	// For. Particle Object's Duration
	m_ComputeParams.SetFloat(0, m_tDesc.fDuration);
	m_ComputeParams.SetFloat(1, m_fCurrAge);

	// For. Center Position
	m_ComputeParams.SetVec4(0, _float4(m_tDesc.vCenterPosition, 0.f));

	// For. Movement 
	m_ComputeParams.SetVec4(1, m_tDesc.vMovementOffsets);

	// Random Rotation 
	m_RenderParams.SetInt(1, m_tDesc.bRandomRotationOn);
}

void Particle::Bind_BasicData_ToShader()
{
	/* Particle 이 mesh든 model이든 상관없이 무조건 쉐이더에 바인딩해야하는 데이터들 */

	m_pShader->GetSRV("g_Data")->SetResource(m_pParticleInfo_UAVBuffer->Get_SRV().Get());

	auto world = Get_Transform()->Get_WorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world });

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

	m_pShader->GetScalar("g_fTimeDelta")->SetFloat(fDT);
}

void Particle::Bind_ComputeParams_ToShader()
{
	m_ComputeParams.SetFloat(1, m_fCurrAge);

	m_pShader->Push_RenderParamData(m_ComputeParams);
}

void Particle::Bind_RenderParams_ToShader()
{
	m_RenderParams.SetFloat(3, m_fCurrAge);

	m_pShader->Push_RenderParamData(m_RenderParams);
}

void Particle::Bind_CreateParticleParams_ToShader()
{
	m_pShader->Push_CreateParticleData(m_CreateParticleParams);
}
