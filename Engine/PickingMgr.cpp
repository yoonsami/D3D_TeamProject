#include "pch.h"
#include "PickingMgr.h"

#include "Model.h"
#include "Utils.h"
#include "Camera.h"
#include "Geometry.h"
#include "Material.h"
#include "ModelMesh.h"
#include "GameObject.h"
#include "BaseCollider.h"
#include "MeshRenderer.h"

shared_ptr<GameObject> PickingMgr::Pick_Collider(_float2 screenPos, shared_ptr<Camera> camera, const vector<shared_ptr<GameObject>>& gameObjects, OUT _float3& outPos)
{
	_float width = GRAPHICS.Get_ViewPort().Get_Width();
	_float height = GRAPHICS.Get_ViewPort().Get_Height();

	_float4x4 matView = camera->Get_ViewMat();
	_float4x4 matViewInv = matView.Invert();
	_float4x4 matProj = camera->Get_ProjMat();

	_float viewX = (2.f * screenPos.x / width - 1.f) / matProj(0, 0);
	_float viewY = (-2.f * screenPos.y / height + 1.f) / matProj(1, 1);

	_float4 rayOriginPos = _float4(0, 0, 0, 1.f);
	_float4 rayDir = _float4(viewX, viewY, 1.f, 0.f);

	_float3 rayOriginPos_World = XMVector3TransformCoord(rayOriginPos, matViewInv);
	_float3 rayDir_World = XMVector3TransformNormal(rayDir, matViewInv);

	rayDir_World.Normalize();

	Ray ray = Ray(rayOriginPos_World, rayDir_World);

	_float minDist = FLT_MAX;
	shared_ptr<GameObject> picked = nullptr;

	for (auto& gameObject : gameObjects)
	{
		_float distance = 0.f;

		if(camera->IsCulled(gameObject->Get_LayerIndex()))
			continue;

		if(!gameObject->Get_Collider())
			continue;

		if(!gameObject->Get_Collider()->Intersects(ray, OUT distance))
			continue;

		if (distance < minDist)
		{
			minDist = distance;
			picked = gameObject;
			outPos = rayOriginPos_World + rayDir_World * minDist;
		}


	}


	return picked;
}

shared_ptr<GameObject> PickingMgr::Pick_Mesh(_float2 screenPos, shared_ptr<Camera> camera, const list<shared_ptr<GameObject>>& gameObjects, OUT _float3& outPos)
{
	_float width = GRAPHICS.Get_ViewPort().Get_Width();
	_float height = GRAPHICS.Get_ViewPort().Get_Height();

	_float4x4 matView = camera->Get_ViewMat();
	_float4x4 matViewInv = matView.Invert();
	_float4x4 matProj = camera->Get_ProjMat();

	_float viewX = (2.f * screenPos.x / width - 1.f) / matProj(0, 0);
	_float viewY = (-2.f * screenPos.y / height + 1.f) / matProj(1, 1);

	_float4 rayOriginPos = _float4(0, 0, 0, 1.f);
	_float4 rayDir = _float4(viewX, viewY, 1.f, 0.f);

	_float3 rayOriginPos_World = XMVector3TransformCoord(rayOriginPos, matViewInv);
	_float3 rayDir_World = XMVector3TransformNormal(rayDir, matViewInv);

	rayDir_World.Normalize();

	Ray ray = Ray(rayOriginPos_World, rayDir_World);

	_float minDist = FLT_MAX;
	shared_ptr<GameObject> picked = nullptr;
	_uint tmp = 0;
	for (auto& gameObject : gameObjects)
	{

		_float distance = 0.f;

		if (camera->IsCulled(gameObject->Get_LayerIndex()))
			continue;

		if (gameObject->Is_FrustumCulled())
		{
			if (camera->Get_Frustum().Contain_Sphere(gameObject->Get_CullPos(), gameObject->Get_CullRadius()) == false)
				continue;
		}

		if (!gameObject->Get_Model())
			continue;

		const _float4x4& matWorld = gameObject->Get_Transform()->Get_WorldMatrix();
		{
			auto meshes = gameObject->Get_Model()->Get_Meshes();

			shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();

			const _uint boneCount = gameObject->Get_Model()->Get_BoneCount();

			for (_uint i = 0; i < boneCount; ++i)
			{
				shared_ptr<ModelBone> bone = gameObject->Get_Model()->Get_BoneByIndex(i);
				boneDesc->transform[i] = bone->transform * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI);
			}

			for (auto& mesh : meshes)
			{
				_int boneIndex = mesh->boneIndex;
				auto& vertices = mesh->geometry->Get_Vertices();
				auto& indices = mesh->geometry->Get_Indices();

				for (_uint i = 0; i < _uint(indices.size());)
				{
					_float fDistance = 0.f;
					_float3 vVtxPos[3] = {
					   _float3::Transform(vertices[indices[i++]].vPosition,boneDesc->transform[boneIndex] * matWorld),
					   _float3::Transform(vertices[indices[i++]].vPosition,boneDesc->transform[boneIndex] * matWorld),
					   _float3::Transform(vertices[indices[i++]].vPosition,boneDesc->transform[boneIndex] * matWorld)
					};

					tmp++;

					if (!ray.Intersects(vVtxPos[0], vVtxPos[1], vVtxPos[2], fDistance))
						continue;

					if (fDistance < minDist)
					{
						minDist = fDistance;
						distance = minDist;
						picked = gameObject;
						outPos = rayOriginPos_World + rayDir_World * minDist;
					}
				}
			}
		}
	}

	return picked;
}

shared_ptr<GameObject> PickingMgr::Pick_Mesh(_float2 screenPos, shared_ptr<Camera> camera, vector<shared_ptr<GameObject>>& gameObjects, OUT _float3& outPos)
{
	_float width = GRAPHICS.Get_ViewPort().Get_Width();
	_float height = GRAPHICS.Get_ViewPort().Get_Height();

	_float4x4 matView = camera->Get_ViewMat();
	_float4x4 matViewInv = matView.Invert();
	_float4x4 matProj = camera->Get_ProjMat();

	_float viewX = (2.f * screenPos.x / width - 1.f) / matProj(0, 0);
	_float viewY = (-2.f * screenPos.y / height + 1.f) / matProj(1, 1);

	_float4 rayOriginPos = _float4(0, 0, 0, 1.f);
	_float4 rayDir = _float4(viewX, viewY, 1.f, 0.f);

	_float3 rayOriginPos_World = XMVector3TransformCoord(rayOriginPos, matViewInv);
	_float3 rayDir_World = XMVector3TransformNormal(rayDir, matViewInv);

	rayDir_World.Normalize();

	Ray ray = Ray(rayOriginPos_World, rayDir_World);

	_float minDist = FLT_MAX;
	shared_ptr<GameObject> picked = nullptr;
	_uint tmp = 0;

	// 피킹렉방지 - 특정 오브젝트 이후로만 검색하기
	_float minObjectIndex = gameObjects.size() * 0.9f;
	_int CurrentObjIndex = 0;
	for (auto& gameObject : gameObjects)
	{
		// 피킹렉방지 - 특정 오브젝트 이후로만 검색하기
		++CurrentObjIndex;
		if (CurrentObjIndex <= minObjectIndex)
			continue;

		_float distance = 0.f;

		if (camera->IsCulled(gameObject->Get_LayerIndex()))
			continue;

		if (gameObject->Is_FrustumCulled())
		{
			if (camera->Get_Frustum().Contain_Sphere(gameObject->Get_CullPos(), gameObject->Get_CullRadius()) == false)
				continue;
		}

		if (!gameObject->Get_Model())
			continue;

		const _float4x4& matWorld = gameObject->Get_Transform()->Get_WorldMatrix();
		{
			auto meshes = gameObject->Get_Model()->Get_Meshes();

			shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();

			const _uint boneCount = gameObject->Get_Model()->Get_BoneCount();

			for (_uint i = 0; i < boneCount; ++i)
			{
				shared_ptr<ModelBone> bone = gameObject->Get_Model()->Get_BoneByIndex(i);
				boneDesc->transform[i] = bone->transform * _float4x4::CreateScale(0.01f) * _float4x4::CreateRotationY(XM_PI);
			}

			for (auto& mesh : meshes)
			{
				_int boneIndex = mesh->boneIndex;
				auto& vertices = mesh->geometry->Get_Vertices();
				auto& indices = mesh->geometry->Get_Indices();

				for (_uint i = 0; i < _uint(indices.size());)
				{
					_float fDistance = 0.f;
					_float3 vVtxPos[3] = {
					   _float3::Transform(vertices[indices[i++]].vPosition,boneDesc->transform[boneIndex] * matWorld),
					   _float3::Transform(vertices[indices[i++]].vPosition,boneDesc->transform[boneIndex] * matWorld),
					   _float3::Transform(vertices[indices[i++]].vPosition,boneDesc->transform[boneIndex] * matWorld)
					};

					tmp++;

					if (!ray.Intersects(vVtxPos[0], vVtxPos[1], vVtxPos[2], fDistance))
						continue;

					if (fDistance < minDist)
					{
						minDist = fDistance;
						distance = minDist;
						picked = gameObject;
						outPos = rayOriginPos_World + rayDir_World * minDist;
					}
				}
			}
		}
	}

	return picked;
}

// 특정 오브젝트만 피킹
shared_ptr<GameObject> PickingMgr::Pick_Mesh(_float2 screenPos, shared_ptr<Camera> camera, shared_ptr<GameObject> gameObject, OUT _float3& outPos)
{
	_float width = GRAPHICS.Get_ViewPort().Get_Width();
	_float height = GRAPHICS.Get_ViewPort().Get_Height();

	_float4x4 matView = camera->Get_ViewMat();
	_float4x4 matViewInv = matView.Invert();
	_float4x4 matProj = camera->Get_ProjMat();

	_float viewX = (2.f * screenPos.x / width - 1.f) / matProj(0, 0);
	_float viewY = (-2.f * screenPos.y / height + 1.f) / matProj(1, 1);

	_float4 rayOriginPos = _float4(0, 0, 0, 1.f);
	_float4 rayDir = _float4(viewX, viewY, 1.f, 0.f);

	_float3 rayOriginPos_World = XMVector3TransformCoord(rayOriginPos, matViewInv);
	_float3 rayDir_World = XMVector3TransformNormal(rayDir, matViewInv);

	rayDir_World.Normalize();

	Ray ray = Ray(rayOriginPos_World, rayDir_World);

	_float minDist = FLT_MAX;
	shared_ptr<GameObject> picked = nullptr;
	{
		_float distance = 0.f;

		if (camera->IsCulled(gameObject->Get_LayerIndex()))
			return nullptr;

		if (gameObject->Is_FrustumCulled())
		{
			if (camera->Get_Frustum().Contain_Sphere(gameObject->Get_CullPos(), gameObject->Get_CullRadius()) == false)
				return nullptr;
		}

		const _float4x4& matWorld = gameObject->Get_Transform()->Get_WorldMatrix();
		{
			auto mesh = gameObject->Get_MeshRenderer()->Get_Mesh();

			auto& vertices = mesh->Get_Geometry()->Get_Vertices();
			auto& indices = mesh->Get_Geometry()->Get_Indices();

			// 필요한 사이즈만 사용
			for (_uint i = /*_uint(indices.size() * 0.3f)*/0; i < _uint(indices.size() /** 0.7f*/);)
			{
				_float fDistance = 0.f;
				_float3 vVtxPos[3] = {
				   _float3::Transform(vertices[indices[i++]].vPosition,matWorld),
				   _float3::Transform(vertices[indices[i++]].vPosition,matWorld),
				   _float3::Transform(vertices[indices[i++]].vPosition,matWorld)
				};

				if (!ray.Intersects(vVtxPos[0], vVtxPos[1], vVtxPos[2], fDistance))
					continue;

				if (fDistance < minDist)
				{
					minDist = fDistance;
					distance = minDist;
					picked = gameObject;
					outPos = rayOriginPos_World + rayDir_World * minDist;
				}
				////최소거리 계산X, 피킹성공하면 리턴
				//outPos = rayOriginPos_World + rayDir_World * fDistance;
				//return picked;
			}
		}
	}

	return picked;
}

void PickingMgr::Brushing(_float2 screenPos, shared_ptr<Camera> camera, shared_ptr<GameObject> gameObject, _float radius, ID3D11Texture2D** texture,vector<_ulong>& pixels)
{
	/*_float width = GRAPHICS.Get_ViewPort().Get_Width();
	_float height = GRAPHICS.Get_ViewPort().Get_Height();

	_float4x4 matView = camera->Get_ViewMat();
	_float4x4 matViewInv = matView.Invert();
	_float4x4 matProj = camera->Get_ProjMat();

	_float viewX = (2.f * screenPos.x / width - 1.f) / matProj(0, 0);
	_float viewY = (-2.f * screenPos.y / height + 1.f) / matProj(1, 1);

	_float4 rayOriginPos = _float4(0, 0, 0, 1.f);
	_float4 rayDir = _float4(viewX, viewY, 1.f, 0.f);

	_float3 rayOriginPos_World = XMVector3TransformCoord(rayOriginPos, matViewInv);
	_float3 rayDir_World = XMVector3TransformNormal(rayDir, matViewInv);

	rayDir_World.Normalize();

	Ray ray = Ray(rayOriginPos_World, rayDir_World);

	_float minDist = FLT_MAX;

	_float distance = 0.f;
	_bool bCollided = false;
	auto model = gameObject->Get_Model();
	_float4x4 matWorld = gameObject->Get_Transform()->Get_WorldMatrix();

	shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();

	const _uint boneCount = model->Get_BoneCount();

	shared_ptr<ModelMesh> pickedMesh = nullptr;

	for (_uint i = 0; i < boneCount; ++i)
	{
		shared_ptr<ModelBone> bone = model->Get_BoneByIndex(i);
		boneDesc->transform[i] = bone->transform;
	}
	_float3 vMax = _float3(0.f);
	_float3 vMin = _float3(FLT_MAX);
	for (auto& mesh : model->Get_Meshes())
	{
		_int boneIndex = mesh->boneIndex;
		auto& vertices = mesh->geometry->Get_Vertices();
		auto& indices = mesh->geometry->Get_Indices();

		for (_int i = 0; i < _int(indices.size());)
		{
			_float fdistance = 0;
			_float3 vVtxPos[3] = {
				(vertices[indices[i++]].vPosition),
				(vertices[indices[i++]].vPosition),
				(vertices[indices[i++]].vPosition)
			};

			for (_int j = 0; j < 3; ++j)
			{
				vVtxPos[j] = _float3::Transform(vVtxPos[j], boneDesc->transform[boneIndex]);
				vVtxPos[j] = _float3::Transform(vVtxPos[j], matWorld);
			}

			vMax.x = max(vMax.x, max(vVtxPos[0].x, max(vVtxPos[1].x, vVtxPos[2].x)));
			vMax.y = max(vMax.y, max(vVtxPos[0].y, max(vVtxPos[1].y, vVtxPos[2].y)));
			vMax.z = max(vMax.z, max(vVtxPos[0].z, max(vVtxPos[1].z, vVtxPos[2].z)));

			vMin.x = min(vMin.x, min(vVtxPos[0].x, min(vVtxPos[1].x, vVtxPos[2].x)));
			vMin.y = min(vMin.y, min(vVtxPos[0].y, min(vVtxPos[1].y, vVtxPos[2].y)));
			vMin.z = min(vMin.z, min(vVtxPos[0].z, min(vVtxPos[1].z, vVtxPos[2].z)));

			if (!ray.Intersects(vVtxPos[0], vVtxPos[1], vVtxPos[2], fdistance))
				continue;



			if (fdistance < minDist)
			{
				pickedMesh = mesh;
				bCollided = true;
				minDist = fdistance;
				distance = minDist;
			}
		}
	}

	if (!bCollided)
		return;

	_float3 vCollidedPos = rayOriginPos_World + rayDir_World * distance;
	_float2 vPickedXZ = _float2(vCollidedPos.x, vCollidedPos.z);
	D3D11_TEXTURE2D_DESC desc;
	(*texture)->GetDesc(&desc);


	D3D11_MAPPED_SUBRESOURCE subResource;
	HRESULT hr = CONTEXT->Map(*texture, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);


	for (_uint x = 0; x < desc.Width; ++x)
	{
		for (_uint z = 0; z < desc.Height; ++z)
		{
			_float2 vPos;

			vPos.x = (x - desc.Width /2.f) ;
			vPos.y = desc.Width / 2.f - z;
			_uint		iIndex = z * desc.Width + x;

			if ((vPos - vPickedXZ).Length() < radius)
			{
				pixels[iIndex] = D3DCOLOR_ARGB(255, 0, 0, 0);
			}

		}
	}

	::memcpy(subResource.pData, pixels.data(), desc.Width * desc.Height * sizeof _ulong);

	CONTEXT->Unmap(*texture, 0);

	ScratchImage img;
	hr = CaptureTexture(DEVICE.Get(), CONTEXT.Get(), *texture, img);


	SaveToDDSFile(img.GetImages(), img.GetImageCount(), img.GetMetadata(), DDS_FLAGS_NONE, (L"..\\Resources\\Textures\\" + gameObject->Get_Name() + L"_Mask.dds").c_str());
	shared_ptr<Texture> ptexture = make_shared<Texture>();
	ptexture->Load(L"..\\Resources\\Textures\\" + gameObject->Get_Name() + L"_Mask.dds");
	pickedMesh->material.lock()->Set_TextureMap(ptexture);*/
}