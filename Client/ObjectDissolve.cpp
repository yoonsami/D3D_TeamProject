#include "pch.h"
#include "Model.h"
#include "ObjectDissolve.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "CharacterController.h"

ObjectDissolve::ObjectDissolve(_float fDissolveSpeed, shared_ptr<Texture> dissolveTexture)
    : m_fDissolveSpeed(fDissolveSpeed)
{
    if (!dissolveTexture)
        m_pTexture = RESOURCES.GetOrAddTexture(L"Dissolve_Default", L"..\\Resources\\Textures\\Universal\\T_glow_ring_001.tga");

}

HRESULT ObjectDissolve::Init()
{
    if (!m_pOwner.expired())
    {
        for (auto& material : Get_Owner()->Get_Model()->Get_Materials())
            material->Set_TextureMap(m_pTexture.lock(), TextureMapType::DISSOLVE);
    }

    if (Get_CharacterController() && Get_CharacterController()->Get_Actor())
        Get_CharacterController()->Release_Controller();
    return S_OK;
}

void ObjectDissolve::Tick()
{
    if (m_pOwner.expired())
        return;

    if (Get_Owner()->Get_ModelRenderer())
    {
        Get_Owner()->Get_ModelRenderer()->Get_RenderParamDesc().vec4Params[0].w += m_fDissolveSpeed * fDT;
        if (Get_Owner()->Get_ModelRenderer()->Get_RenderParamDesc().vec4Params[0].w > 1.f)
            EVENTMGR.Delete_Object(Get_Owner());
    }

    if (Get_Owner()->Get_Animator())
    {
		Get_Owner()->Get_Animator()->Get_RenderParamDesc().vec4Params[0].w += m_fDissolveSpeed * fDT;
		if (Get_Owner()->Get_Animator()->Get_RenderParamDesc().vec4Params[0].w > 1.f)
            EVENTMGR.Delete_Object(Get_Owner());
    }
}
