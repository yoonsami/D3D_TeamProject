#include "pch.h"
#include "Transform.h"

#include "BaseCollider.h"
#include "CharacterController.h"
#include "MathUtils.h"

//#include "Physics.h"
//#include "MeshCollider.h"

Transform::Transform()
	:Component(COMPONENT_TYPE::Transform)
{
}

Transform::~Transform()
{
}

HRESULT Transform::Init()
{
	return S_OK;
}




void Transform::Tick()
{
	m_matPreWorld = m_matWorld;
}

void Transform::Late_Tick()
{
	//if (!Get_Owner()->Get_CharacterController())
	//	return;
	//PxTransform transform = Get_Owner()->Get_CharacterController()->Get_Actor()->get();
	//Set_State(Transform_State::POS, _float4(transform.p.x, transform.p.y, transform.p.z, 1.f));
	//Set_Quaternion(Quaternion(transform.q.x, transform.q.y, transform.q.z, transform.q.w));
}

void Transform::Fixed_Tick()
{
}

_float3 Transform::Get_Scale()
{
	return _float3(Get_State(Transform_State::RIGHT).Length(),
		Get_State(Transform_State::UP).Length(),
		Get_State(Transform_State::LOOK).Length());
}

Quaternion Transform::Get_Rotation()
{
	_float3 vScale, vTranslation;
	Quaternion quaternion;
	m_matWorld.Decompose(vScale, quaternion, vTranslation);
	return quaternion;
}

_float3 Transform::Get_RollPitchYaw()
{
	_float3 vScale, vTranslation;
	Quaternion quaternion;
	m_matWorld.Decompose(vScale, quaternion, vTranslation);

	return MathUtils::ToEulerAngles(quaternion);
}

void Transform::Set_State(Transform_State eState, const _float4& vState)
{
	memcpy(m_matWorld.m[_uint(eState)], &vState, sizeof(_float4));
	if (eState == Transform_State::POS)
	{
		if (Get_Owner()->Get_CharacterController() && Get_Owner()->Get_CharacterController()->Get_Actor())
		{
			Get_Owner()->Get_CharacterController()->Get_Actor()->setFootPosition({ vState.x,vState.y,vState.z });
		}
	}
}

_bool Transform::Go_Dir(const _float3& vVel)
{
	_bool bResult = false;

	auto controller = Get_CharacterController();
	if(controller && controller->Get_Actor())
	{
		auto actor = Get_CharacterController()->Get_Actor();


		_float3 vDir = vVel;
		
		auto result = actor->move({ vDir.x  ,vDir.y   ,vDir.z  }, 0.0f, fDT, PxControllerFilters());
	
		auto controllerPos = actor->getFootPosition();
		Set_State(Transform_State::POS, _float4(_float(controllerPos.x), _float(controllerPos.y), _float(controllerPos.z), 1.f));
		if (result == PxControllerCollisionFlag::eCOLLISION_SIDES)
			bResult = true;
	}
	else
		Set_State(Transform_State::POS,Get_State(Transform_State::POS) + vVel);

	return bResult;

}

_bool Transform::Go_Straight()
{
	_bool bResult = false;
	_float4 vDir = Get_State(Transform_State::LOOK);
	vDir.Normalize();

	auto controller = Get_CharacterController();
	if (controller && controller->Get_Actor())
	{
		auto actor = Get_CharacterController()->Get_Actor();

		auto result = actor->move({ vDir.x * m_fMoveSpeed * fDT,vDir.y * m_fMoveSpeed * fDT ,vDir.z * m_fMoveSpeed * fDT }, 0.0f, fDT, PxControllerFilters());
		auto controllerPos = actor->getFootPosition();

		Set_State(Transform_State::POS, _float4(_float(controllerPos.x), _float(controllerPos.y), _float(controllerPos.z), 1.f));
		if (result == PxControllerCollisionFlag::eCOLLISION_SIDES)
			bResult = true;
	}
	else
		Set_State(Transform_State::POS, Get_State(Transform_State::POS) + vDir * m_fMoveSpeed * fDT);

	return bResult;
}

_bool Transform::Go_Backward()
{
	_bool bResult = false;
	_float4 vDir = -Get_State(Transform_State::LOOK);
	vDir.Normalize();

	auto controller = Get_CharacterController();
	if (controller && controller->Get_Actor())
	{
		auto actor = Get_CharacterController()->Get_Actor();

		auto result = actor->move({ vDir.x * m_fMoveSpeed * fDT,vDir.y * m_fMoveSpeed * fDT ,vDir.z * m_fMoveSpeed * fDT }, 0.0f, fDT, PxControllerFilters());
		auto controllerPos = actor->getFootPosition();

		Set_State(Transform_State::POS, _float4(_float(controllerPos.x), _float(controllerPos.y), _float(controllerPos.z), 1.f));
		if (result == PxControllerCollisionFlag::eCOLLISION_SIDES)
			bResult = true;
	}
	else
		Set_State(Transform_State::POS, Get_State(Transform_State::POS) + vDir * m_fMoveSpeed * fDT);

	return bResult;
}

_bool Transform::Go_Left()
{
	_bool bResult = false;
	_float4 vDir = -Get_State(Transform_State::RIGHT);
	vDir.Normalize();

	auto controller = Get_CharacterController();
	if (controller && controller->Get_Actor())
	{
		auto actor = Get_CharacterController()->Get_Actor();




		auto result = actor->move({ vDir.x * m_fMoveSpeed * fDT,vDir.y * m_fMoveSpeed * fDT ,vDir.z * m_fMoveSpeed * fDT }, 0.0f, fDT, PxControllerFilters());
		auto controllerPos = actor->getFootPosition();

		Set_State(Transform_State::POS, _float4(_float(controllerPos.x), _float(controllerPos.y), _float(controllerPos.z), 1.f));
		if (result == PxControllerCollisionFlag::eCOLLISION_SIDES)
			bResult = true;
	}
	else
		Set_State(Transform_State::POS, Get_State(Transform_State::POS) + vDir * m_fMoveSpeed * fDT);
	return bResult;
}

_bool Transform::Go_Right()
{
	_bool bResult = false;
	_float4 vDir = Get_State(Transform_State::RIGHT);
	vDir.Normalize();

	auto controller = Get_CharacterController();
	if (controller && controller->Get_Actor())
	{
		auto actor = Get_CharacterController()->Get_Actor();




		auto result = actor->move({ vDir.x * m_fMoveSpeed * fDT,vDir.y * m_fMoveSpeed * fDT ,vDir.z * m_fMoveSpeed * fDT }, 0.0f, fDT, PxControllerFilters());
		auto controllerPos = actor->getFootPosition();

		Set_State(Transform_State::POS, _float4(_float(controllerPos.x), _float(controllerPos.y), _float(controllerPos.z), 1.f));
		if (result == PxControllerCollisionFlag::eCOLLISION_SIDES)
			bResult = true;

	}
	else
		Set_State(Transform_State::POS, Get_State(Transform_State::POS) + vDir * m_fMoveSpeed * fDT);
	
	return bResult;

}

_bool Transform::Go_Up()
{
	_bool bResult = false;
	_float4 vDir = Get_State(Transform_State::UP);
	vDir.Normalize();

	auto controller = Get_CharacterController();
	if (controller && controller->Get_Actor())
	{
		auto actor = Get_CharacterController()->Get_Actor();




		auto result = actor->move({ vDir.x * m_fMoveSpeed * fDT,vDir.y * m_fMoveSpeed * fDT ,vDir.z * m_fMoveSpeed * fDT }, 0.0f, fDT, PxControllerFilters());
		auto controllerPos = actor->getFootPosition();

		Set_State(Transform_State::POS, _float4(_float(controllerPos.x), _float(controllerPos.y), _float(controllerPos.z), 1.f));
		if (result == PxControllerCollisionFlag::eCOLLISION_SIDES)
			bResult = true;

	}
	else
		Set_State(Transform_State::POS, Get_State(Transform_State::POS) + vDir * m_fMoveSpeed * fDT);

	return bResult;
}

_bool Transform::Go_Down()
{
	_bool bResult = false;
	_float4 vDir = -Get_State(Transform_State::UP);
	vDir.Normalize();

	auto controller = Get_CharacterController();
	if (controller && controller->Get_Actor())
	{
		auto actor = Get_CharacterController()->Get_Actor();




		auto result = actor->move({ vDir.x * m_fMoveSpeed * fDT,vDir.y * m_fMoveSpeed * fDT ,vDir.z * m_fMoveSpeed * fDT }, 0.0f, fDT, PxControllerFilters());
		auto controllerPos = actor->getFootPosition();

		Set_State(Transform_State::POS, _float4(_float(controllerPos.x), _float(controllerPos.y), _float(controllerPos.z), 1.f));
		if (result == PxControllerCollisionFlag::eCOLLISION_SIDES)
			bResult = true;

	}
	else
		Set_State(Transform_State::POS, Get_State(Transform_State::POS) + vDir * m_fMoveSpeed * fDT);

	return bResult;
}

void Transform::Go_Horizontally_Up()
{
	_float4 vDir = Get_State(Transform_State::UP);
	vDir.Normalize();

	auto controller = Get_CharacterController();
	if (controller && controller->Get_Actor())
	{
		auto actor = Get_CharacterController()->Get_Actor();




		auto result = actor->move({ vDir.x * m_fMoveSpeed * fDT,vDir.y * m_fMoveSpeed * fDT ,vDir.z * m_fMoveSpeed * fDT }, 0.0f, fDT, PxControllerFilters());
		auto controllerPos = actor->getFootPosition();

		Set_State(Transform_State::POS, _float4(_float(controllerPos.x), _float(controllerPos.y), _float(controllerPos.z), 1.f));
	}
	else
		Set_State(Transform_State::POS, Get_State(Transform_State::POS) + vDir * m_fMoveSpeed * fDT);
}

void Transform::Go_Horizontally_Down()
{
	_float4 vDir = -Get_State(Transform_State::UP);
	vDir.Normalize();

	auto controller = Get_CharacterController();
	if (controller && controller->Get_Actor())
	{
		auto actor = Get_CharacterController()->Get_Actor();




		auto result = actor->move({ vDir.x * m_fMoveSpeed * fDT,vDir.y * m_fMoveSpeed * fDT ,vDir.z * m_fMoveSpeed * fDT }, 0.0f, fDT, PxControllerFilters());
		auto controllerPos = actor->getFootPosition();

		Set_State(Transform_State::POS, _float4(_float(controllerPos.x), _float(controllerPos.y), _float(controllerPos.z), 1.f));
	}
	else
		Set_State(Transform_State::POS, Get_State(Transform_State::POS) + vDir * m_fMoveSpeed * fDT);
}

void Transform::Rotation(const _float3& vAxis, _float fRadian)
{
	_float4x4 matScale = _float4x4::CreateScale(Get_Scale());
	_float4x4 matRotation = _float4x4::CreateFromAxisAngle(vAxis, fRadian);
	_float4x4 matTranslation = _float4x4::CreateTranslation(Get_State(Transform_State::POS).x, Get_State(Transform_State::POS).y, Get_State(Transform_State::POS).z);

	m_matWorld = matScale * matRotation * matTranslation;
}

void Transform::Turn(const _float3& vAxis, _float fRadianPerSec)
{
	_float4x4 matRotation = _float4x4::CreateFromAxisAngle(vAxis, fRadianPerSec * fDT);
	_float4 vPosition = Get_State(Transform_State::POS);

	m_matWorld *= matRotation;

	Set_State(Transform_State::POS, vPosition);
}

void Transform::Scaled(const _float3& vScale)
{
	Set_State(Transform_State::RIGHT,(Get_State(Transform_State::RIGHT).Normalize())* vScale.x);
	Set_State(Transform_State::UP,(Get_State(Transform_State::UP).Normalize())* vScale.y);
	Set_State(Transform_State::LOOK,(Get_State(Transform_State::LOOK).Normalize())* vScale.z);
}

void Transform::LookAt(const _float4& vTargetPos)
{
	_float4 vPosition = Get_State(Transform_State::POS);
	_float3 vScale = Get_Scale();
	_float4 vLook = vTargetPos - vPosition;
	vLook.Normalize();

	Set_State(Transform_State::RIGHT, (DirectX::XMVector3Cross(_float4::UnitY, vLook)));
	Set_State(Transform_State::LOOK, vLook);
	Set_State(Transform_State::UP, DirectX::XMVector3Cross(vLook, Get_State(Transform_State::RIGHT)));

	Scaled(vScale);
}

void Transform::Set_Rotation(const _float3& vRollPitchYaw)
{
	_float4x4 matScale = _float4x4::CreateScale(Get_Scale());

	Quaternion qRotation = Quaternion::CreateFromYawPitchRoll(vRollPitchYaw.y,vRollPitchYaw.x, vRollPitchYaw.z);
	qRotation.Normalize();

	_float4x4 matRotation = _float4x4::CreateFromQuaternion(qRotation);

	_float4x4 matTranslation = _float4x4::CreateTranslation(Get_State(Transform_State::POS).xyz());

	m_matWorld = matScale * matRotation * matTranslation;
}

void Transform::Set_LookDir(const _float3& vDir)
{

	LookAt(Get_State(Transform_State::POS) + vDir);
}

void Transform::Set_Quaternion(const Quaternion& q)
{
	m_matWorld = _float4x4::CreateScale(Get_Scale())*_float4x4::CreateFromQuaternion(q)* _float4x4::CreateTranslation(Get_State(Transform_State::POS).xyz());
}

_float4x4 Transform::Get_WorldFromLook(const _float3& vLook, const _float3& vPos)
{
	_float3 vRight = XMVector3Cross(_float3::Up, vLook);
	_float3 vUp = XMVector3Cross(vLook, vRight);

	return _float4x4(_float4(vRight, 0.f), _float4(vUp, 0.f), _float4(vLook, 0.f), _float4(vPos, 1.f));
}

Quaternion Transform::Get_QuatFromLook(const _float3& vLook)
{
	_float4x4 InputWorld = Transform::Get_WorldFromLook(vLook, _float3(0.f));
	_float3 vScale, vTranslation;
	Quaternion  Inputquaternion;
	InputWorld.Decompose(vScale, Inputquaternion, vTranslation);

	return Inputquaternion;
}

_float4x4 Transform::SLerpMatrix(_float4x4& m0, _float4x4& m1, _float ratio)
{
	_float3 curTrans, curScale;
	Quaternion curRot;
	_float3 nextTrans, nextScale;
	Quaternion nextRot;

	m0.Decompose(curScale, curRot, curTrans);
	m1.Decompose(nextScale, nextRot, nextTrans); 

	return _float4x4::CreateScale(_float3::Lerp(curScale, nextScale,ratio))*
		_float4x4::CreateFromQuaternion(Quaternion::Slerp(curRot, nextRot, ratio))*
		_float4x4::CreateTranslation(_float3::Lerp(curTrans, nextTrans, ratio));
}


