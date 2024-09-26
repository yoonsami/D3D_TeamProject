#pragma once
#include "Component.h"

class MeshCollider;


class RigidBody : public Component
{
public:
	RigidBody();
	virtual ~RigidBody();

	virtual void Late_Tick() override;

	_float3& Get_Dimension() { return m_vDimension; }
	PxMaterial* Get_Material() { return m_pMaterial; }
	void Create_RigidBody(shared_ptr<MeshCollider> meshCollider, const _float4x4& matWorld);
	PxRigidActor* Get_RigidBody() { return m_pRigidBody; }
	void Create_CapsuleRigidBody(_float3 centerPos, _float radius, _float Height);
	void AddOrRemoveRigidBody_FromScene(_bool flag);


	void RemoveRigidBody();

private:
	 _float3 m_vDimension =_float3(1.f);
	 PxMaterial* m_pMaterial = nullptr;
	 PxRigidStatic* m_pRigidBody = nullptr;
};

