#include "pch.h"
#include "CharacterController.h"

CharacterController::CharacterController() : Component(COMPONENT_TYPE::CharacterController)
{
	
	
}

CharacterController::~CharacterController()
{
	if (PHYSX.Get_PxScene())
	{
		if(m_pController)
		{
			m_pController->release();
			m_pController = nullptr;
		}

	}
}

void CharacterController::Create_Controller(_bool IsBoxCollider)
{
	m_pMaterial = PHYSX.Get_PxPhysics()->createMaterial(0.f, 0.f, 1.f);
	if (IsBoxCollider)
	{
		m_BoxControllerDesc.material = m_pMaterial;


		m_pController = PHYSX.Get_ControllManager()->createController(m_BoxControllerDesc);

	}
	else
	{
		m_CapsuleControllerDesc.material = m_pMaterial;


		m_pController = PHYSX.Get_ControllManager()->createController(m_CapsuleControllerDesc);
	}
	assert(m_pController);

	m_pController->getActor()->setMass(1.f);
	PHYSX.Add_GameObject(Get_Owner());
	//PHYSX.Get_PxScene()->addActor(*(m_pController->getActor()));
}

HRESULT CharacterController::Init()
{
	m_pController->setFootPosition({ Get_Owner()->Get_Transform()->Get_State(Transform_State::POS).x, Get_Owner()->Get_Transform()->Get_State(Transform_State::POS).y, Get_Owner()->Get_Transform()->Get_State(Transform_State::POS).z });
	
	
	return S_OK;
}

void CharacterController::Tick()
{
	/*if (m_bGravityOn)
	{
		_float3 vDir = _float3(0.f, -9.8f, 0.f);

		if (!m_pController)
			return;
		auto result = m_pController->move({ vDir.x ,vDir.y  ,vDir.z }, 0.0f, fDT, PxControllerFilters());
	}*/
	if(m_pController)
	{
		if (!m_bJump)
		{

			auto result = m_pController->move({ 0.f ,-30.f * fDT  ,0.f }, 0.0f, fDT, PxControllerFilters());

			auto controllerPos = m_pController->getFootPosition();
			Get_Transform()->Set_State(Transform_State::POS, _float4(_float(controllerPos.x), _float(controllerPos.y), _float(controllerPos.z), 1.f));

		}
		else
		{
			m_fYVelocity -= 9.8f * fDT;

			auto result = m_pController->move({ 0.f ,m_fYVelocity * fDT  ,0.f }, 0.0f, fDT, PxControllerFilters());
			auto controllerPos = m_pController->getFootPosition();
			Get_Transform()->Set_State(Transform_State::POS, _float4(_float(controllerPos.x), _float(controllerPos.y), _float(controllerPos.z), 1.f));

			if (result == PxControllerCollisionFlag::eCOLLISION_DOWN && m_fYVelocity < 0.f)
			{
				m_fYVelocity = 0.f;
				m_bOnGround = true;
				m_bJump = false;
			}
		}
	}


}

void CharacterController::Release_Controller()
{
	if (PHYSX.Get_PxScene())
	{
		if (m_pController)
		{
			m_pController->release();
			m_pController = nullptr;
		}

	}
}

void CharacterController::Add_Velocity(const _float vel)
{
	m_fYVelocity += vel;
	m_bOnGround = false;
	m_bJump = true;
}
