#include "pch.h"
#include "Particle.h"

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
		m_eType = TYPE_Mesh;
		m_pMesh = RESOURCES.Get<Mesh>(L"Point");
	}
	else 
		m_eType = TYPE_Model;

	m_eComputePass = (COMPUTE_PASS)m_tDesc.iMovementOption;
	m_eRenderPass = (RENDERMESH_PASS)m_tDesc.iBillbordOption;

	// For. Initialize Shader Params
	Init_ComputeParams();
	Init_RenderParams();
	Init_CreateParticleParams();

	// For. Create ParticleInfo UAV Buffers  
	vector<ParticleInfo_UAV> tmp(m_tDesc.iMaxParticleNum);
	if (!m_pParticleInfo_UAVBuffer)
		m_pParticleInfo_UAVBuffer = make_shared<StructuredBuffer>(tmp.data(), static_cast<_uint>(sizeof ParticleInfo_UAV), m_tDesc.iMaxParticleNum);

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
	m_fCurrLifeTime += fDT;
	m_fTimeAcc_CreatCoolTime += fDT;

	// For. Check End(Delete) this particle 
	if (m_fCurrLifeTime >= m_tDesc.fDuration)
	{
		CUR_SCENE->Remove_GameObject(Get_Owner());
		return;
	}

	// For. Decide whether to create a new Particle and Cnt
	m_CreateParticleParams.iNewlyAddCnt = 0;
	if (m_tDesc.bIsbCreateOnce)
	{
		if (!m_bIsFirstCreateParticleDone)
		{
			// min ~ max ������ ������ ���� ��� �׸�ŭ ���� �����Ѵ�. 
			m_CreateParticleParams.iNewlyAddCnt = (rand() % (m_tDesc.iMaxCnt + 1 - m_tDesc.iMinCnt)) + m_tDesc.iMinCnt;
			m_fTimeAcc_CreatCoolTime = 0.f;
		}
	}
	else 
	{
		if (m_fTimeAcc_CreatCoolTime > m_tDesc.fCreateInterval)
		{
			// min ~ max ������ ������ ���� ��� �׸�ŭ ���� �����Ѵ�. 
			m_CreateParticleParams.iNewlyAddCnt = (rand() % (m_tDesc.iMaxCnt + 1 - m_tDesc.iMinCnt)) + m_tDesc.iMinCnt;
			m_fTimeAcc_CreatCoolTime = 0.f;
		}
	}
	
	// For. Bind Params and data to Shader 
	auto& world = Get_Transform()->Get_WorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world });

	Bind_ComputeParams_ToShader();
	Bind_CreateParticleParams_ToShader();

	// For. Bind UAV Buffer in shader ( ���̴��� ���� ������ �� ���� ������ �� �����Ӹ��� �Ѱ�����Ѵ�.)
	m_pShader->GetUAV("g_ParticleInfo_UAVBuffer")->SetUnorderedAccessView(m_pParticleInfo_UAVBuffer->Get_UAV().Get());
	m_pShader->GetUAV("g_ComputeShared_UAVBuffer")->SetUnorderedAccessView(m_pComputeShared_UAVBuffer->Get_UAV().Get());	 

	// For. Dispatch
	m_pShader->Dispatch(TECHNIQUE_Compute, 0, 1, 1, 1);

	vector<ParticleInfo_UAV> tmp(m_tDesc.iMaxParticleNum);
	m_pParticleInfo_UAVBuffer->Copy_FromOutput(tmp.data());
	m_pParticleInfo_UAVBuffer->Copy_ToInput(tmp.data());
}

void Particle::Render()
{
	/* ���⼭ Shader�� ���ε��ϴ� ������ VS, PS�� ������ ���� ���Ǵ� �����̴�. */

	// For. Render Standard Mesh
	switch (m_eType)
	{
	case TYPE_Mesh: 
		if (nullptr == m_pMesh || nullptr == m_pMaterial)
			return;

		// For. Bind data to shader
		Bind_BasicData_ToShader();
		Bind_RenderParams_ToShader();
		
		m_pMesh->Get_VertexBuffer()->Push_Data();
		m_pMesh->Get_IndexBuffer()->Push_Data();
		
		// For. Setting Context Topology
		CONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		
		// For. Draw call
		m_pShader->DrawIndexedInstanced(TECHNIQUE_MeshRender, m_eRenderPass, m_pMesh->Get_IndexBuffer()->Get_IndicesNum(), m_tDesc.iMaxParticleNum);
		break;

	case TYPE_Model:
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
	/* GS, VS, PS���� ����� ���� ���� */
	
    // For. Alpha Gradation 
	m_RenderParams.SetFloat(0, m_tDesc.fGradationByAlpha_Brighter);
	m_RenderParams.SetFloat(1, m_tDesc.fGradationByAlpha_Darker);

	// For. Diffuse Color
	m_ComputeParams.SetVec4(0, m_tDesc.vDestColor);
}

void Particle::Init_CreateParticleParams()
{
	/* CS���� Particle�� ���� ������ ���� �ʿ��� �� */

	// For. LifeTime
	m_CreateParticleParams.iLifeTimeOption = m_tDesc.iLifeTimeOption;
	m_CreateParticleParams.vMinMaxLifeTime = m_tDesc.vLifeTime;

	// For. Diffuse Color 
	m_CreateParticleParams.vStartColor = m_tDesc.vStartColor;
	m_CreateParticleParams.vStartColor = m_tDesc.vEndColor;

	// For. Start Scale
	m_CreateParticleParams.vMinMaxScale = m_tDesc.vStartScale;
	
	// For. Create Position 
	m_CreateParticleParams.vCreateRange = _float4(m_tDesc.vCreateRange, 0.f);
	m_CreateParticleParams.vCreateOffsets = m_tDesc.vCreateOffsets;
}

void Particle::Init_ComputeParams()
{
	/* CS���� ����� ���� �����ϱ�. ��ǿ� ���� params�� �����ϴ� ������ �ǹ̰� �޶���              �� �ִ�. */

	// For. Particle Object's Duration, LifeTime
	m_ComputeParams.SetFloat(0, m_tDesc.fDuration);
	m_ComputeParams.SetFloat(1, m_fCurrLifeTime);

	// For. Center Position
	m_ComputeParams.SetVec4(0, _float4(m_tDesc.vCenterPosition, 0.f));

	// For. Dissolve
	m_ComputeParams.SetInt(0, m_tDesc.iDissolveOption);
	m_ComputeParams.SetVec2(0, m_tDesc.fDissolveCurveSpeed);

	// For. Update Scale
	m_ComputeParams.SetInt(1, m_tDesc.iScaleOption);
	m_ComputeParams.SetVec2(1, m_tDesc.vScaleSpeed);

	// For. Update Speed
	m_ComputeParams.SetInt(2, m_tDesc.iSpeedOption);
	m_ComputeParams.SetVec2(2, m_tDesc.vSpeed);

	// For. Max instance cnt
	m_ComputeParams.SetInt(3, m_tDesc.iMaxParticleNum);

	// For. Movement 
	m_ComputeParams.SetVec4(1, m_tDesc.vMovementOffsets);

	// For. Rotation Angle 
	m_ComputeParams.SetVec4(2, _float4(m_tDesc.iRotationAngleOption, m_tDesc.vRotationAngle.x, m_tDesc.vRotationAngle.y, m_tDesc.vRotationAngle.z));

	// For. Rotation Speed
	m_ComputeParams.SetVec4(3, _float4(m_tDesc.vRotationSpeed, 0.f));
}

void Particle::Bind_BasicData_ToShader()
{
	/* Particle �� mesh�� model�̵� ������� ������ ���̴��� ���ε��ؾ��ϴ� �����͵� */

	m_pShader->GetSRV("g_Data")->SetResource(m_pParticleInfo_UAVBuffer->Get_SRV().Get());

	auto world = Get_Transform()->Get_WorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world });

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

	m_pShader->GetScalar("g_fTimeDelta")->SetFloat(fDT);
}

void Particle::Bind_ComputeParams_ToShader()
{
	// param�߿� tick���� Particle.cpp���� �����ؾ��ϴ� ���� �ִٸ� ���⼭ �ٲ��ְ��� ���̴��� ���ε��ϸ��. 
	
	m_pShader->Push_RenderParamData(m_ComputeParams);
}

void Particle::Bind_RenderParams_ToShader()
{
	// TODO: Ȥ�� param�߿� tick���� Particle.cpp���� �����ؾ��ϴ� ���� �ִٸ� ���⼭ �ٲ��ְ��� ���̴��� ���ε��ϸ��. 

	m_pShader->Push_RenderParamData(m_RenderParams);
}

void Particle::Bind_CreateParticleParams_ToShader()
{
	m_pShader->Push_CreateParticleData(m_CreateParticleParams);
}
