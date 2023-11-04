#include "pch.h"
#include "Particle.h"

#include "Camera.h"
#include "StructuredBuffer.h"

Particle::Particle(shared_ptr<Shader> shader)
	: Component(COMPONENT_TYPE::ParticleSystem)
	, m_pShader(shader)
{
}

Particle::~Particle()
{
}

HRESULT Particle::Init(void* tDesc)
{
	// For. Setting basic info  
	memcpy(&m_tDesc, tDesc, sizeof(tDesc));

	if (!m_pModel) 
		m_eType = TYPE_Mesh;
	else 
		m_eType = TYPE_Model;

	m_eComputePass = (COMPUTE_PASS)m_tDesc.iMovementOption;
	m_eRenderPass = (RENDERMESH_PASS)m_tDesc.iBillbordOption;

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
	_int iAddCnt = 0; // �̹� ƽ�� ���� �������� ��ƼŬ���� 
	if (m_fTimeAcc_CreatCoolTime > m_tDesc.fCreateInterval)
	{
		// min ~ max ������ ������ ���� ��� �׸�ŭ ���� �����Ѵ�. 
		iAddCnt = (rand() % (m_tDesc.iMaxCnt + 1 - m_tDesc.iMinCnt)) + m_tDesc.iMinCnt;
		m_fTimeAcc_CreatCoolTime = 0.f;
	}
	
	// For. Bind Data in shader ( ���̴��� ���� ������ �� ���� ������ �� �����Ӹ��� �Ѱ�����Ѵ�.)
	auto& world = Get_Transform()->Get_WorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world });

	Bind_ComputeShaderData_ToShader();

	// For. Bind UAV Buffer in shader ( ���̴��� ���� ������ �� ���� ������ �� �����Ӹ��� �Ѱ�����Ѵ�.)
	m_pShader->GetUAV("g_ParticleInfo_UAVBuffer")->SetUnorderedAccessView(m_pParticleInfo_UAVBuffer->Get_UAV().Get());
	m_pShader->GetUAV("g_ComputeShared_UAVBuffer")->SetUnorderedAccessView(m_pComputeShared_UAVBuffer->Get_UAV().Get());	 
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
		Bind_RenderShaderData_ToShader();
		
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
		Bind_RenderShaderData_ToShader();

		// TODO 

		break;
	}
}

void Particle::Bind_BasicData_ToShader()
{
	/* Particle �� mesh�� model�̵� ������� ������ ���̴��� ���ε��ؾ��ϴ� �����͵� */

	m_pShader->GetSRV("g_Data")->SetResource(m_pParticleInfo_UAVBuffer->Get_SRV().Get());

	auto world = Get_Transform()->Get_WorldMatrix();
	m_pShader->Push_TransformData(TransformDesc{ world });

	m_pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());
}

void Particle::Bind_ComputeShaderData_ToShader()
{

}

void Particle::Bind_RenderShaderData_ToShader()
{

}
