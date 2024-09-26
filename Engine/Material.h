#pragma once
#include "ResourceBase.h"


enum
{
    MAX_SUB_SRV_COUNT = SRV_MAP_COUNT,
};

class Material :
    public ResourceBase
{
public:
    Material();
    ~Material();

public:
    auto Get_Shader() { return m_pShader; }
    auto& Get_MaterialDesc() { return m_Desc; }
    void Set_Shader(shared_ptr<Shader> shader);
    void Set_Desc(const MaterialDesc& desc) { m_Desc = desc; }
    virtual shared_ptr<ResourceBase> Clone() override;
public:
    void Tick();


    void Push_TextureMapData();
    void Push_SubMapData();

    shared_ptr<Texture> Get_TextureMap(TextureMapType eType){ if (static_cast<_uint>(eType) < MAX_TEXTURE_MAP_COUONT && static_cast<_uint>(eType) >= 0) return m_TextureMaps[static_cast<_uint>(eType)]; else return nullptr; }
    void Set_TextureMap(shared_ptr<Texture> pTextureMap, TextureMapType eType) { 
        if (static_cast<_uint>(eType) < MAX_TEXTURE_MAP_COUONT && static_cast<_uint>(eType) >= 0) m_TextureMaps[static_cast<_uint>(eType)] = pTextureMap;    }
   
    shared_ptr<Texture> Get_SubMap(int index) { if (index < MAX_SUB_SRV_COUNT && index >=0) return m_pSubMaps[index]; else return nullptr; }
	void Set_SubMap(int index, shared_ptr<Texture> pTexture) {
        if (index < MAX_SUB_SRV_COUNT && index >= 0) m_pSubMaps[index] = pTexture;	}


private:
    friend class MeshRenderer;

    MaterialDesc m_Desc;
  
    shared_ptr<Shader>  m_pShader = nullptr;

    array< shared_ptr<Texture>, MAX_TEXTURE_MAP_COUONT> m_TextureMaps;
    array< ComPtr<ID3DX11EffectShaderResourceVariable>, MAX_TEXTURE_MAP_COUONT> m_pTextureMapEffectBuffers;


    array<shared_ptr<Texture>, MAX_SUB_SRV_COUNT> m_pSubMaps = { nullptr };
    array<ComPtr<ID3DX11EffectShaderResourceVariable>, MAX_SUB_SRV_COUNT> m_pSubMapEffectBuffers = { nullptr };

};

