#include "pch.h"
#include "Material.h"

Material::Material()
	:ResourceBase(ResourceType::Material)
{
}

Material::~Material()
{
}

void Material::Set_Shader(shared_ptr<Shader> shader)
{
	m_pShader = shader;

	m_pTextureMapEffectBuffers[static_cast<_uint>(TextureMapType::DIFFUSE)] = m_pShader->GetSRV("DiffuseMap");
	m_pTextureMapEffectBuffers[static_cast<_uint>(TextureMapType::NORMAL)] = m_pShader->GetSRV("NormalMap");
	m_pTextureMapEffectBuffers[static_cast<_uint>(TextureMapType::SPECULAR)] = m_pShader->GetSRV("SpecularMap");
	m_pTextureMapEffectBuffers[static_cast<_uint>(TextureMapType::OPACITY)] = m_pShader->GetSRV("OpacityMap");
	m_pTextureMapEffectBuffers[static_cast<_uint>(TextureMapType::EMISSIVE)] = m_pShader->GetSRV("EmissiveMap");
	m_pTextureMapEffectBuffers[static_cast<_uint>(TextureMapType::DISSOLVE)] = m_pShader->GetSRV("DissolveMap");
	m_pTextureMapEffectBuffers[static_cast<_uint>(TextureMapType::DISTORTION)] = m_pShader->GetSRV("DistortionMap");

	for (_uint i = static_cast<_uint>(TextureMapType::TEXTURE7); i < MAX_TEXTURE_MAP_COUONT; ++i)
	{
		m_pTextureMapEffectBuffers[i] = m_pShader->GetSRV("TextureMap" + to_string(i));
	}

	for (_uint i = 0; i < MAX_SUB_SRV_COUNT; ++i)
	{
		m_pSubMapEffectBuffers[i] = m_pShader->GetSRV("SubMap" + to_string(i));
	}
}

shared_ptr<ResourceBase> Material::Clone()
{
	shared_ptr<Material> material = make_shared<Material>();
	material->Set_Name(Get_Name());
	material->m_Desc = m_Desc;
	material->m_pShader = m_pShader;
	for(_uint i = 0; i<MAX_TEXTURE_MAP_COUONT;++i)
		material->m_TextureMaps[i] = m_TextureMaps[i];
	for (_uint i = 0; i < MAX_SUB_SRV_COUNT; ++i)
		material->m_pSubMaps[i] = m_pSubMaps[i];

	return material;
}

void Material::Tick()
{
	if (!m_pShader)
		return;

	m_pShader->Push_MaterialData(m_Desc);
}

void Material::Push_TextureMapData()
{
	TextureMapDesc desc{};
	for (_uint i = 0; i < MAX_TEXTURE_MAP_COUONT; ++i)
	{
		if (m_TextureMaps[i])
		{
			desc.bHasTextureMap[i] = 1;
			m_pTextureMapEffectBuffers[i]->SetResource(m_TextureMaps[i]->Get_SRV().Get());
		}
		else
			desc.bHasTextureMap[i] = 0;
	}
	m_pShader->Push_TextureMapData(desc);
}

void Material::Push_SubMapData()
{
	SubMapDesc desc{};
	for (_uint i = 0; i < MAX_SUB_SRV_COUNT; ++i)
	{

		if (m_pSubMaps[i])
		{
			desc.bHasSubMap[i] = 1;
			m_pSubMapEffectBuffers[i]->SetResource(m_pSubMaps[i]->Get_SRV().Get());
		}
		else
			desc.bHasSubMap[i] = 0;

	}
	m_pShader->Push_SubmapData(desc);
}