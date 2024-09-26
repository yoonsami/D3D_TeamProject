#include "pch.h"
#include "SphereCollider.h"

#include "Camera.h"
#include "MeshCollider.h"
#include "OBBBoxCollider.h"
#include "AABBBoxCollider.h"

SphereCollider::SphereCollider(_float radius)
	:BaseCollider(ColliderType::Sphere)
{
	m_fRadius = radius;
	m_BoundingSphere.Radius = radius;

	 shared_ptr<Mesh> mesh = make_shared<Mesh>();
	 mesh->CreateSphere();

	 m_Meshes.push_back(mesh);


	

}

SphereCollider::~SphereCollider()
{

}

HRESULT SphereCollider::Init()
{

	return S_OK;
}

void SphereCollider::Tick()
{
	Add_ColliderGroup();
	_float4x4 matWorld = Get_Owner()->Get_Transform()->Get_WorldMatrix();

	_float3 trans, scale;
	Quaternion q;

	matWorld.Decompose(scale, q, trans);

	m_BoundingSphere.Center = _float3::Transform(m_vOffset,matWorld);
	m_BoundingSphere.Radius = m_fRadius * Get_Transform()->Get_Scale().x;
}

_bool SphereCollider::Intersects(Ray& worldRay, OUT _float& distance)
{
	return m_BoundingSphere.Intersects(worldRay.position, worldRay.direction,OUT distance);
}

_bool SphereCollider::Intersects(shared_ptr<BaseCollider>& others)
{
	ColliderType type = others->Get_ColliderType();

	switch (type)
	{
	case ColliderType::Sphere:
		return m_BoundingSphere.Intersects(dynamic_pointer_cast<SphereCollider>(others)->Get_BoundingSphere());

		break;
	case ColliderType::AABB:
		return m_BoundingSphere.Intersects(dynamic_pointer_cast<AABBBoxCollider>(others)->Get_BoundingBox());
		break;
	case ColliderType::OBB:
		return m_BoundingSphere.Intersects(dynamic_pointer_cast<OBBBoxCollider>(others)->Get_BoundingBox());
		break;
	default:
		break;
	}
	return false;
}

TransformDesc SphereCollider::Get_TransformDesc()
{
	return TransformDesc{ _float4x4::CreateScale(m_BoundingSphere.Radius)  * _float4x4::CreateTranslation(m_BoundingSphere.Center) };
}

_bool SphereCollider::Intersects_With_Bottom(shared_ptr<MeshCollider>& others, OUT _float3& collidePos, OUT _float& outDist)
{
	_float3 vRayDir = -Get_Transform()->Get_State(Transform_State::UP).xyz();
	vRayDir.Normalize();
	_float3 vRayOrigin = m_BoundingSphere.Center - vRayDir * m_BoundingSphere.Radius;

	_float dist = FLT_MAX;

	Ray ray(vRayOrigin, vRayDir);
	_bool collided = others->Intersects(ray, dist);
	if (!collided)
		return false;

	collidePos = vRayOrigin + vRayDir * dist;
	outDist = dist;

	return true;
}

_float3 SphereCollider::Get_ClosestPoint(shared_ptr<BaseCollider> pCollider)
{
	return _float3();
}
