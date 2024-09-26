#include "pch.h"
#include "Camera.h"

#include "Scene.h"
#include "Particle.h"
#include "TerrainRenderer.h"
#include "MeshEffect.h"
#include "MeshRenderer.h"
#include "GroupEffect.h"
#include "FontRenderer.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "TrailRenderer.h"
#include "DistortionRenderer.h"
#include "MotionTrailRenderer.h"

Camera::Camera(CameraDesc desc)
	:Component(COMPONENT_TYPE::Camera)
{
	m_Desc = desc;
}

Camera::~Camera()
{

}
HRESULT Camera::Init()
{
	return S_OK;
}

void Camera::Late_Tick()
{
	

	//RENDER->Push_GlobalData(m_matView, m_matProj);
}

void Camera::Final_Tick()
{
	Update_Matrix();
	frustum.Final_Tick(m_matView,m_matProj);
}

void Camera::Update_Matrix()
{
	m_matView = Get_Transform()->Get_WorldMatrix().Invert();

	if (m_eType == ProjectionType::Perspective)
		m_matProj =  XMMatrixPerspectiveFovLH(m_Desc.fFOV, m_Desc.fSizeX / m_Desc.fSizeY, m_Desc.fNear, m_Desc.fFar);

	else
		m_matProj = XMMatrixOrthographicLH(m_Desc.fSizeX, m_Desc.fSizeY, m_Desc.fNear, m_Desc.fFar);

	
}

void Camera::Sort_GameObject(shared_ptr<Scene> scene)
{
	auto& gameObjects = scene->Get_Objects();

	// Todo 
	//Frustum Check

	m_Sky.clear();
	m_Water.clear();
	m_Forward.clear();
	m_Deferred.clear();
	m_Particle.clear();
	m_DistortionEffects.clear();
	m_Trails.clear();
	m_VelocityMapObj.clear();
	m_AfterUI.clear();
	m_Decal.clear();

	for (auto& gameObject : gameObjects)
	{
		if (false == gameObject->Is_Render())
			continue;

		if (gameObject->Get_MeshRenderer() == nullptr
			&& gameObject->Get_ModelRenderer() == nullptr
			&& gameObject->Get_Animator() == nullptr
			&& gameObject->Get_DistortionRenderer() == nullptr
			&& gameObject->Get_Particle() == nullptr
			&& gameObject->Get_TrailRenderer() == nullptr
			&& gameObject->Get_MotionTrailRenderer() == nullptr
			&& gameObject->Get_FontRenderer() == nullptr
			&& gameObject->Get_TerrainRenderer() == nullptr
			&& ((m_bEffectToolMode_On && gameObject->Get_MeshEffect() == nullptr) ||
			    (!m_bEffectToolMode_On && gameObject->Get_GroupEffect() == nullptr))
			)
			continue;

		if (gameObject->Get_LayerIndex() == Layer_AfterUI)
			int a = 0;

		if (IsCulled(gameObject->Get_LayerIndex()))
			continue;

		if (gameObject->Is_FrustumCulled())
		{
			if (frustum.Contain_Sphere(gameObject->Get_CullPos(), gameObject->Get_CullRadius()) == false)
				continue;
		}

		if (gameObject->Has_VelocityMap())
			m_VelocityMapObj.push_back(gameObject);
		
		if (gameObject->Get_ShaderType() == SHADER_TYPE::SKYBOX)
			m_Sky.push_back(gameObject);
		else if (gameObject->Get_TerrainRenderer())
			m_Terrain = gameObject;
		else if (gameObject->Get_ShaderType() == SHADER_TYPE::WATER)
			m_Water.push_back(gameObject);
		else if (gameObject->Get_TrailRenderer())
			m_Trails.push_back(gameObject);
		else if (gameObject->Get_MotionTrailRenderer())
			m_Trails.push_back(gameObject);
		else if (gameObject->Get_FontRenderer())
			m_Forward.push_back(gameObject);
		else if (gameObject->Get_ShaderType() == SHADER_TYPE::DEFERRED)
			m_Deferred.push_back(gameObject);
		else if (m_bEffectToolMode_On && gameObject->Get_MeshEffect() && !gameObject->Get_MeshEffect()->Get_Desc().bIsFDistortion && !gameObject->Get_MeshEffect()->Get_Desc().bIsSSD)
			m_Forward.push_back(gameObject);
		else if (!m_bEffectToolMode_On && gameObject->Get_GroupEffect())
			m_Forward.push_back(gameObject);
		else if (gameObject->Get_ShaderType() == SHADER_TYPE::FORWARD)
			m_Forward.push_back(gameObject);
		if (gameObject->Get_ShaderType() == SHADER_TYPE::DISTORTION)
			m_DistortionEffects.push_back(gameObject);
		
		if (m_bEffectToolMode_On && gameObject->Get_MeshEffect()&& gameObject->Get_MeshEffect()->Get_Desc().bIsSSD)
			m_Decal.push_back(gameObject);
		else if (!m_bEffectToolMode_On && gameObject->Get_GroupEffect())
			m_Decal.push_back(gameObject);

		//if (gameObject->Get_ParticleSystem())
		//	m_Particle.push_back(gameObject);
		if (gameObject->Get_Particle())
			m_Particle.push_back(gameObject);
	}
}

void Camera::Sort_ShadowObject(shared_ptr<Scene> scene)
{
	auto& gameObjects = scene->Get_Objects();

	m_Shadow.clear();
	for (auto& gameObject : gameObjects)
	{
		if (gameObject->Get_ModelRenderer() == nullptr
			&& gameObject->Get_Animator() == nullptr)
			continue;

		if(!gameObject->Is_DrawShadow())
			continue;

		if (IsCulled(gameObject->Get_LayerIndex()))
			continue;

		if (gameObject->Is_FrustumCulled())
		{
			if (frustum.Contain_Sphere(gameObject->Get_CullPos(), gameObject->Get_CullRadius()) == false)
				continue;
		}


		m_Shadow.push_back(gameObject);
	}
}

void Camera::Render_DistrotionEffects()
{
	for (auto& obj : m_DistortionEffects)
	{
		if (m_bEffectToolMode_On && obj->Get_MeshEffect())
			obj->Get_MeshEffect()->Render();
		else if (!m_bEffectToolMode_On && obj->Get_GroupEffect())
			obj->Get_GroupEffect()->Render_Distortion();
	}
}

void Camera::Render_BlurForward()
{
	S_View = m_matView;
	S_Proj = m_matProj;

	for (auto& obj : m_Sky)
	{
		if(obj->Is_Blured())
			obj->Get_ModelRenderer()->Render_Skybox();
	}


	for (auto& trail : m_Trails)
	{
		if (trail->Is_Blured())
		{
			if(trail->Get_TrailRenderer())
			trail->Get_TrailRenderer()->Render();
		}
	}

	sort(m_Forward.begin(), m_Forward.end(), [this](shared_ptr<GameObject>& a, shared_ptr<GameObject>& b)
		{
			_float zDepthA = a->Get_Transform()->Get_State(Transform_State::POS).z;
			_float zDepthB = b->Get_Transform()->Get_State(Transform_State::POS).z;

			if (m_eType == ProjectionType::Orthographic)
			{

				return zDepthA > zDepthB;
			}

			_float3 vCameraPos = Get_Transform()->Get_State(Transform_State::POS).xyz();
			_float3 vPosA = a->Get_Transform()->Get_State(Transform_State::POS).xyz();
			_float3 vPosB = b->Get_Transform()->Get_State(Transform_State::POS).xyz();

			if (a->Get_MeshRenderer() && b->Get_MeshRenderer())
			{
				_float viewDepthA = _float3::Transform(vPosA, m_matView).z;
				_float viewDepthB = _float3::Transform(vPosB, m_matView).z;
				if (vPosA == vPosB)
					return false;
				else
					return viewDepthA > viewDepthB;
			}

			_float distanceASQ = (vCameraPos - vPosA).LengthSquared();
			_float distanceBSQ = (vCameraPos - vPosB).LengthSquared();
			/*if (CMP(distanceASQ, distanceBSQ))
			{
				if (a->Get_MeshRenderer() && b->Get_EffectRenderer())
					return false;
				else if (a->Get_EffectRenderer() && b->Get_MeshRenderer())
					return true;

			}*/

			return distanceASQ > distanceBSQ; });

	for (auto& obj : m_Forward)
	{
		if(!obj->Is_Blured())
			continue;

		if (obj->Get_MeshRenderer())
			obj->Get_MeshRenderer()->Render();
		else if (obj->Get_ModelRenderer())
			obj->Get_ModelRenderer()->Render();
		//else if (obj->Get_EffectRenderer())
		//	obj->Get_EffectRenderer()->Render();
		else if (obj->Get_Animator())
			obj->Get_Animator()->Render();

	}


	for (auto& particle : m_Particle)
	{
		if(particle->Is_Blured())
			particle->Get_Particle()->Render();
	}
}

void Camera::Render_SkyBox()
{
	for (auto& obj : m_Sky)
		obj->Get_ModelRenderer()->Render_Skybox();
}

void Camera::Render_Forward()
{
	S_View = m_matView;
	S_Proj = m_matProj;

	for (auto& obj : m_Water)
	{
		if (obj->Get_MeshRenderer())
			obj->Get_MeshRenderer()->Render_Water();
	}

	for (auto& trail : m_Trails)
	{
		if (trail->Get_TrailRenderer())
			trail->Get_TrailRenderer()->Render();
		else if (trail->Get_MotionTrailRenderer())
			trail->Get_MotionTrailRenderer()->Render();
	}

	sort(m_Forward.begin(), m_Forward.end(), [this](shared_ptr<GameObject>& a, shared_ptr<GameObject>& b)
		{
			_float zDepthA = a->Get_Transform()->Get_State(Transform_State::POS).z;
			_float zDepthB = b->Get_Transform()->Get_State(Transform_State::POS).z;

			if (m_eType == ProjectionType::Orthographic)
			{
				return zDepthA > zDepthB;
			}

			_float3 vCameraPos = Get_Transform()->Get_State(Transform_State::POS).xyz();
			_float3 vPosA = a->Get_Transform()->Get_State(Transform_State::POS).xyz();
			_float3 vPosB = b->Get_Transform()->Get_State(Transform_State::POS).xyz();

			if (a->Get_MeshRenderer() && b->Get_MeshRenderer())
			{
				_float viewDepthA = _float3::Transform(vPosA, m_matView).z;
				_float viewDepthB = _float3::Transform(vPosB, m_matView).z;
				if (vPosA == vPosB)
					return false;
				else
					return viewDepthA > viewDepthB;
			}

			_float distanceASQ = (vCameraPos - vPosA).LengthSquared();
			_float distanceBSQ = (vCameraPos - vPosB).LengthSquared();
			

			return distanceASQ > distanceBSQ; });

	for (auto& obj : m_Decal)
	{
		if (!m_bEffectToolMode_On && obj->Get_GroupEffect())
			obj->Get_GroupEffect()->Render_Decal();
		else if (m_bEffectToolMode_On && obj->Get_MeshEffect() && obj->Get_MeshEffect()->Get_Desc().bIsSSD)
			obj->Get_MeshEffect()->Render();


	}

	for(auto& obj : m_Forward)
	{
		if (obj->Get_MeshRenderer())
			obj->Get_MeshRenderer()->Render();
		else if (obj->Get_ModelRenderer())
			obj->Get_ModelRenderer()->Render();
		else if (obj->Get_Animator())
			obj->Get_Animator()->Render();

		// Effect
		else if (m_bEffectToolMode_On && obj->Get_MeshEffect())
			obj->Get_MeshEffect()->Render();
		else if(!m_bEffectToolMode_On && obj->Get_GroupEffect())
			obj->Get_GroupEffect()->Render();

		// Font
		if (obj->Get_FontRenderer())
			obj->Get_FontRenderer()->Render();
	}

	for (auto& particle : m_Particle)
	{
		particle->Get_Particle()->Render();
	}
}

void Camera::Render_Deferred()
{

	S_View = m_matView;
	S_Proj = m_matProj;
	if(m_Terrain)
		m_Terrain->Get_TerrainRenderer()->Render();

	INSTANCING.Render(m_Deferred);
}

void Camera::Render_Shadow()
{
	S_View = m_matView;
	S_Proj = m_matProj;

	INSTANCING.Render_Shadow(m_Shadow);

}

void Camera::Render_MotionBlur()
{
	S_View = m_matView;
	S_Proj = m_matProj;
	for (auto& obj : m_VelocityMapObj)
	{
		if (obj->Get_ModelRenderer())
			obj->Get_ModelRenderer()->Render_MotionBlur();
		if (obj->Get_Animator())
			obj->Get_Animator()->Render_MotionBlur();
	}
}

void Camera::Render_AfterUI()
{
	S_View = m_matView;
	S_Proj = m_matProj;
	for (auto& obj : m_Deferred)
	{
		if (obj->Get_Animator())
			obj->Get_Animator()->Render_Forward();
		if (obj->Get_ModelRenderer())
			obj->Get_ModelRenderer()->Render_Forward();
	}
}

vector<shared_ptr<MonoBehaviour>>& Camera::Get_Scripts()
{
	return Get_Owner()->Get_Scripts();
}

_float4x4 Camera::S_View = _float4x4::Identity;

_float4x4 Camera::S_Proj = _float4x4::Identity;
