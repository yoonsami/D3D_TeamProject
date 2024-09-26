#include "pch.h"
#include "Light.h"

#include "Camera.h"
#include "Material.h"
#include "Transform.h"

Light::Light()
	: Component(COMPONENT_TYPE::Light)
{
	m_pShadowCamera = make_shared<GameObject>();
	m_pShadowCamera->GetOrAddTransform();
	m_pShadowCamera->Add_Component(make_shared<Camera>());
	_uchar layerIndex = Layer_UI;
	m_pShadowCamera->Get_Camera()->Set_CullingMaskLayerOnOff(layerIndex, true);

}

Light::~Light()
{
}

void Light::Tick()
{

}

void Light::Final_Tick()
{
	_float4 dir = Get_Transform()->Get_State(Transform_State::LOOK);
	dir.Normalize();
	m_LightInfo.vPosition = Get_Transform()->Get_State(Transform_State::POS);
	m_LightInfo.vDirection = dir;
	if (m_LightInfo.lightType == static_cast<_uint>(LIGHT_TYPE::POINT_LIGHT))
		Get_Transform()->Scaled(_float3(m_LightInfo.range));

	_float4x4 matWorld = Get_Transform()->Get_WorldMatrix();
	_float3 trans, scale;
	Quaternion rot;

	matWorld.Decompose(scale, rot, trans);

	m_pShadowCamera->Get_Transform()->Set_Quaternion(rot);
	m_pShadowCamera->Get_Transform()->Set_State(Transform_State::POS, _float4(trans, 1.f));

	m_pShadowCamera->Final_Tick();
}

void Light::Render()
{
	assert(m_iLightIndex >= 0);
	auto pShader = m_pLightMaterial->Get_Shader();
	pShader->GetScalar("g_SSAO_On")->SetBool(CUR_SCENE->g_SSAOData.g_bSSAO_On);

	_float4x4 matScale = _float4x4::CreateScale(2.f);
	
	if(m_LightInfo.lightType != _int(LIGHT_TYPE::DIRECTIONAL_LIGHT))
		pShader->Push_TransformData(TransformDesc{ matScale*Get_Transform()->Get_WorldMatrix() });

	_float4x4 ShadowCameraVP = m_pShadowCamera->Get_Camera()->Get_ViewMat() * m_pShadowCamera->Get_Camera()->Get_ProjMat();

	pShader->GetMatrix("ShadowCameraVP")->SetMatrix((_float*)&ShadowCameraVP);

	pShader->Push_GlobalData(Camera::Get_View(), Camera::Get_Proj());

	auto& LightParam = CUR_SCENE->Get_LightParams();
	pShader->Push_LightData(LightParam);

	m_pLightMaterial->Push_SubMapData();

	pShader->GetScalar("lightIndex")->SetInt(m_iLightIndex);
	_float2 RTSize = { GRAPHICS.Get_ViewPort().Get_Width(), GRAPHICS.Get_ViewPort().Get_Height() };

	pShader->GetVector("RenderTargetResolution")->SetFloatVector((_float*)&RTSize);
	pShader->GetScalar("g_ShadowBias")->SetFloat(CUR_SCENE->g_fShadowBias);
	pShader->GetScalar("g_lightAttenuation")->SetFloat(CUR_SCENE->g_lightAttenuation);


	m_pVolumeMesh->Get_VertexBuffer()->Push_Data();
	m_pVolumeMesh->Get_IndexBuffer()->Push_Data();
	int techniqueIndex = CUR_SCENE->g_bPBR_On ? 1 : 0;
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pShader->DrawIndexed(techniqueIndex, m_LightInfo.lightType, m_pVolumeMesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

}

void Light::Render_Shadow(shared_ptr<Scene> scene)
{
	m_pShadowCamera->Get_Camera()->Sort_ShadowObject(scene);
	m_pShadowCamera->Get_Camera()->Render_Shadow();
}

void Light::Set_LightType(LIGHT_TYPE type)
{
	m_LightInfo.lightType = static_cast<_int>(type);
	
	switch (type)
	{
	case LIGHT_TYPE::DIRECTIONAL_LIGHT:
		m_pVolumeMesh = RESOURCES.Get<Mesh>(L"Quad");
		m_pLightMaterial = RESOURCES.Get<Material>(L"LightMaterial");

		{
			auto& desc = m_pShadowCamera->Get_Camera()->Get_CameraDesc();
			desc.eType = ProjectionType::Orthographic;
			desc.fFOV = XM_PI / 2.f;
			desc.fNear = 10.f;
			desc.fFar = 1000.f;
			desc.fSizeX = 4096.f;
			desc.fSizeY = 4096.f;
		}


		break;
	case LIGHT_TYPE::POINT_LIGHT:
		m_pVolumeMesh = RESOURCES.Get<Mesh>(L"Sphere");
		m_pLightMaterial = RESOURCES.Get<Material>(L"LightMaterial");
		break;
	case LIGHT_TYPE::SPOT_LIGHT:
		m_pVolumeMesh = RESOURCES.Get<Mesh>(L"Sphere");
		m_pLightMaterial = RESOURCES.Get<Material>(L"LightMaterial");
		break;
	default:
		break;
	}

}


