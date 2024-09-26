#include "pch.h"
#include "WaterUVSliding.h"
#include "ModelRenderer.h"
#include "MeshRenderer.h"

void WaterUVSliding::Tick()
{
	if (Get_Owner()->Get_MeshRenderer())
	{
		Get_Owner()->Get_MeshRenderer()->Get_RenderParamDesc().vec2Params[0] -= _float2(0.01f, 0.03f) * fDT;
		Get_Owner()->Get_MeshRenderer()->Get_RenderParamDesc().vec2Params[1] += _float2(0.03f) * fDT;
		//Get_Owner()->Get_ModelRenderer()->Get_RenderParamDesc().vec4Params[0].x = 20.f;
		Get_Owner()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[1] = m_Color1;
		Get_Owner()->Get_MeshRenderer()->Get_RenderParamDesc().vec4Params[2] = m_Color2;
		Get_Owner()->Get_MeshRenderer()->Get_RenderParamDesc().floatParams[1] += (3.f) * fDT;
		Get_Owner()->Get_MeshRenderer()->Get_RenderParamDesc().intParams[0] = m_int0;
		Get_Owner()->Get_MeshRenderer()->Get_RenderParamDesc().intParams[1] = m_int1;
	}
}
