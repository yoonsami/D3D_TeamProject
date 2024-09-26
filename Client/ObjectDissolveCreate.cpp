#include "pch.h"
#include "Model.h"
#include "ObjectDissolveCreate.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "CharacterController.h"

ObjectDissolveCreate::ObjectDissolveCreate(_float fDissolveSpeed, shared_ptr<Texture> dissolveTexture)
    : m_fDissolveSpeed(fDissolveSpeed)
{
    if (!dissolveTexture)
        m_pTexture = RESOURCES.GetOrAddTexture(L"Dissolve_Default", L"..\\Resources\\Textures\\Universal\\T_glow_ring_001.tga");
}

HRESULT ObjectDissolveCreate::Init()
{
    if (!m_pOwner.expired())
    {
        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
            material->Set_TextureMap(m_pTexture.lock(), TextureMapType::DISSOLVE);
    }

    if (Get_Owner()->Get_ModelRenderer())
        Get_Owner()->Get_ModelRenderer()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
    
    if (Get_Owner()->Get_Animator())    
        Get_Owner()->Get_Animator()->Get_RenderParamDesc().vec4Params[0].w = 1.f;
    

    return S_OK;
}

void ObjectDissolveCreate::Tick()
{
    if (m_pOwner.expired())
        return;

    if (Get_Owner()->Get_ModelRenderer())
    {
        if (Get_Owner()->Get_ModelRenderer()->Get_RenderParamDesc().vec4Params[0].w <= 0.f)
            m_bCreate = true;
            
        if (!m_bCreate)
            Get_Owner()->Get_ModelRenderer()->Get_RenderParamDesc().vec4Params[0].w -= m_fDissolveSpeed * fDT;
    }

    if (Get_Owner()->Get_Animator())
    {
        if (Get_Owner()->Get_Animator()->Get_RenderParamDesc().vec4Params[0].w <= 0.f)
            m_bCreate = true;

        if (!m_bCreate)
            Get_Owner()->Get_Animator()->Get_RenderParamDesc().vec4Params[0].w -= m_fDissolveSpeed * fDT;
    }
}
