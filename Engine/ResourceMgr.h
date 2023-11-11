#pragma once
#include "ResourceBase.h"

class Shader;
class Texture;
class Mesh;
class Material;
class Model;
class CustomFont;
class MeshEffectData;
class ParticleData;
class GroupEffectData;
class ResourceMgr
{
	DECLARE_SINGLETON(ResourceMgr)

public:
	~ResourceMgr();

public:
	void Initialize();

	template<typename T>
	shared_ptr<T> Load(const wstring& key, const wstring& path, _uint iLevelIndex = 0);

	template<typename T>
	_bool Add(const wstring& key, shared_ptr<T> resource, _uint iLevelIndex = 0);

	template<typename T>
	shared_ptr<T> Get(const wstring& key, _uint iLevelIndex = 0);

	template<typename T>
	shared_ptr<T> Clone(const wstring& key, _uint iLevelIndex = 0);

	template<typename T>
	ResourceType Get_ResourceType();
	shared_ptr<Texture> GetOrAddTexture(const wstring& key, const wstring& path);

	shared_ptr<GroupEffectData> GetOrAddGroupEffectData(const wstring& key, const wstring& path);

	//shared_ptr<Parts> Get_Part(const wstring& key);
	//map<wstring, shared_ptr<Parts>>& Get_Parts(PARTS_INFO type) { return m_PrototypeParts[(_uint)type]; }
	auto& Get_Resources(_uint index) { return m_Resources[index]; }
    //void LoadBase();
	//void LoadParts();
	void CreateDefaultMesh();
	void CreateDefaultShader();
	void CreateModel(const wstring& path);

	void CreateDefaultMaterial();
	void CreateMapModel(const wstring& mapName);
	void CreateDefaultFont();

	void CreateParticleData();
	void CreateMeshEffectData();
	void CreateGroupEffectData();


	void Reset_LevelModel(_uint iLevelIndex);
	
	shared_ptr<Texture> CreateTexture(const wstring& name, DXGI_FORMAT format, _uint width, _uint height, _uint BindFlags, _float4 clearColor);
	shared_ptr<Texture> CreateTextureFromResource(const wstring& path, ComPtr<ID3D11Texture2D> texture);
	shared_ptr<Texture> CreateShadowTexture(const wstring& name, _float width, _float height);

private:

private:
	wstring m_strTexturePath = L"../Resources/Textures/";
	wstring m_strModelPath = L"../Resources/Models/";

	using KeyResouceMap = map<wstring, shared_ptr<ResourceBase>>;
	array<array<KeyResouceMap, RESOURCE_TYPE_COUNT>,MAX_LEVEL_COUNT> m_Resources;

	//map<wstring, shared_ptr<Parts>> m_PrototypeParts[PARTS_MAX_COUNT];
};


template<typename T>
inline shared_ptr<T> ResourceMgr::Load(const wstring& key, const wstring& path, _uint iLevelIndex)
{
	if (iLevelIndex >= MAX_LEVEL_COUNT)
	{
		MSG_BOX("Level Index Bigger than MaxCount");
		return nullptr;
	}

	auto eType = Get_ResourceType<T>();
	KeyResouceMap& keyObjMap = m_Resources[iLevelIndex][static_cast<_uchar>(eType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	shared_ptr<T> resource = make_shared<T>();
	resource->Load(path);
	resource->Set_Name(key);
	keyObjMap[key] = resource;

	return resource;
}

template<typename T>
inline _bool ResourceMgr::Add(const wstring& key, shared_ptr<T> resource, _uint iLevelIndex)
{
	ResourceType eType = Get_ResourceType<T>();
	KeyResouceMap& keyObjMap = m_Resources[iLevelIndex][static_cast<_uchar>(eType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return false;

	resource->Set_Name(key);
	keyObjMap[key] = resource;
	return true;
}

template<typename T>
inline shared_ptr<T> ResourceMgr::Get(const wstring& key, _uint iLevelIndex)
{
	ResourceType eType = Get_ResourceType<T>();
	KeyResouceMap& keyObjMap = m_Resources[iLevelIndex][static_cast<_uchar>(eType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second);

	return nullptr;
}

template<typename T>
inline shared_ptr<T> ResourceMgr::Clone(const wstring& key, _uint iLevelIndex)
{
	ResourceType eType = Get_ResourceType<T>();
	KeyResouceMap& keyObjMap = m_Resources[iLevelIndex][static_cast<_uchar>(eType)];

	auto findIt = keyObjMap.find(key);
	if (findIt != keyObjMap.end())
		return static_pointer_cast<T>(findIt->second->Clone());

	return nullptr;
}

template<typename T>
inline ResourceType ResourceMgr::Get_ResourceType()
{
	if (is_same_v<T, Texture>)
		return ResourceType::Texture;

	if (is_same_v<T, Mesh>)
		return ResourceType::Mesh;

	if (is_same_v<T, Shader>)
		return ResourceType::Shader;

	if (is_same_v<T, Material>)
		return ResourceType::Material;

	if (is_same_v<T, Model>)
		return ResourceType::Model;

	if (is_same_v<T, CustomFont>)
		return ResourceType::Font;

	if (is_same_v<T, MeshEffectData>)
		return ResourceType::MeshEffectData;

	if (is_same_v<T, ParticleData>)
		return ResourceType::ParticleData;

	if (is_same_v<T, GroupEffectData>)
		return ResourceType::GroupEffectData;



	assert(false);
	return ResourceType::None;
}
