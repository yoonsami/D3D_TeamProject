#include "pch.h"
#include "RigidBody.h"

#include "Geometry.h"
#include "MeshCollider.h"

RigidBody::RigidBody() :Component(COMPONENT_TYPE::RigidBody)
{
}

RigidBody::~RigidBody()
{
	if (PHYSX.Get_PxScene())
	{
		if (m_pRigidBody)
		{
			m_pRigidBody->release();
			m_pRigidBody = nullptr;
		}

	}
}

void RigidBody::Late_Tick()
{
	if (!m_pRigidBody)
		return;
	auto scene = PHYSX.Get_PxScene();
	
	_float3 vCenterPos = Get_Owner()->Get_CullPos();
	_float fRadius = Get_Owner()->Get_CullRadius();
	if(CUR_SCENE && CUR_SCENE->Get_Player())
	{
		_float3 vPlayerPos = CUR_SCENE->Get_Player()->Get_Transform()->Get_State(Transform_State::POS).xyz();

		if (fRadius == 0.f)
			m_pRigidBody->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
		else if ((vPlayerPos - vCenterPos).LengthSquared() > (fRadius + 10.f) * (fRadius + 10.f))
		{
			m_pRigidBody->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
		}
		else
			m_pRigidBody->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
	}
}

void RigidBody::Create_RigidBody(shared_ptr<MeshCollider> meshCollider, const _float4x4& matWorld)
{
	m_pMaterial = PHYSX.Get_PxPhysics()->createMaterial(0.5f, 0.5f, 1.f);
	vector<PxVec3> vertices;
	vector<PxU32> indices;

	auto& meshes = meshCollider->Get_Meshes();

	for (auto& mesh : meshes)
	{
		auto& ver = mesh->Get_Geometry()->Get_Vertices();
		_uint vertexCount = _uint(vertices.size());
		for (auto& vertex : ver)
		{
			_float3 vPos = _float3::Transform(vertex.vPosition, matWorld);
			PxVec3 tmp = { vPos.x,vPos.y,vPos.z };
			vertices.push_back(tmp);
		}

		auto& ind = mesh->Get_Geometry()->Get_Indices();

		for (auto& index : ind)
		{
			indices.push_back(index + vertexCount);
		}
	}

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = PxU32(vertices.size()); // 정점 수
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = vertices.data();

	meshDesc.triangles.count = PxU32(indices.size() / 3); // 삼각형 수
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = indices.data();

	PxDefaultMemoryOutputStream writeBuffer;
	auto param = PHYSX.Get_Cooking();
	_bool status = PxCookTriangleMesh(param, meshDesc, writeBuffer);
	assert(status);

	PxDefaultMemoryInputData meshData(writeBuffer.getData(), writeBuffer.getSize());

	PxTriangleMesh* triangleMesh = PHYSX.Get_PxPhysics()->createTriangleMesh(meshData);

	assert(triangleMesh);

	PxTriangleMeshGeometry triangleMeshGeometry(triangleMesh);

	PxTransform transform(PxVec3(0.0f, 0.0f, 0.0f));
	m_pRigidBody = PHYSX.Get_PxPhysics()->createRigidStatic(transform);

	PxShape* shape = PHYSX.Get_PxPhysics()->createShape(triangleMeshGeometry, *m_pMaterial);


	m_pRigidBody->attachShape(*shape);

	triangleMesh->release();
	shape->release();

	PHYSX.Get_PxScene()->addActor(*m_pRigidBody);
}

void RigidBody::Create_CapsuleRigidBody(_float3 centerPos, _float radius, _float Height)
{
	m_pMaterial = PHYSX.Get_PxPhysics()->createMaterial(0.5f, 0.5f, 1.f);
	PxTransform pose;
	pose.p.x = centerPos.x;
	pose.p.y = centerPos.y;
	pose.p.z = centerPos.z;
	_float4 a = Quaternion::CreateFromRotationMatrix(_float4x4::CreateRotationZ(XMConvertToRadians(90.f)));
	pose.q = { a.x,a.y,a.z,a.w };
	;
	;

	PxCapsuleGeometry geometry(radius, Height / 2.f);

	m_pRigidBody = PHYSX.Get_PxPhysics()->createRigidStatic(pose);
	assert(m_pRigidBody);

	PxShape* shape = PHYSX.Get_PxPhysics()->createShape(geometry, *m_pMaterial);

	m_pRigidBody->attachShape(*shape);

	shape->release();

	PHYSX.Get_PxScene()->addActor(*m_pRigidBody);
}

void RigidBody::AddOrRemoveRigidBody_FromScene(_bool flag)
{
	PxActor** actors = nullptr;
	PxU32 numActors = PHYSX.Get_PxScene()->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC);
	_bool alreadyHas = false;
	if (numActors > 0)
	{
		actors = new PxActor * [numActors];
		PHYSX.Get_PxScene()->getActors(PxActorTypeFlag::eRIGID_DYNAMIC | PxActorTypeFlag::eRIGID_STATIC, actors, numActors);
		for (PxU32 i = 0; i < numActors; ++i)
		{
			if (actors[i] == m_pRigidBody)
			{
				alreadyHas = true;
			}
		}
		delete[] actors;
	}

	if (flag)
	{
		if(alreadyHas == false)
			PHYSX.Get_PxScene()->addActor(*m_pRigidBody);
	}
	else
	{
		if (alreadyHas == true)
		PHYSX.Get_PxScene()->removeActor(*m_pRigidBody);
	}
}

void RigidBody::RemoveRigidBody()
{
	if (PHYSX.Get_PxScene())
	{
		if (m_pRigidBody)
		{
			m_pRigidBody->release();
			m_pRigidBody = nullptr;
		}

	}
}




