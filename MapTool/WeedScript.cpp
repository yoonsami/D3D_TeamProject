#include "pch.h"
#include "WeedScript.h"
#include "ModelRenderer.h"
#include "MeshRenderer.h"

void WeedScript::Tick()
{
	if (Get_Owner()->Get_MeshRenderer())
	{
		m_fWindWeight += fDT * m_fWindWeightPM * m_fWindSpeed;
		if (m_fWindWeight >= 1.f)
		{
			m_fWindWeight = 1.f;
			m_fWindWeightPM = -1.f;
		}
		else if (m_fWindWeight <= 0.f)
		{
			m_fWindWeight = 0.f;
			m_fWindWeightPM = 1.f;
			m_Wind.x = Utils::Random_In_Range(-1.f, 1.f);
			m_Wind.y = Utils::Random_In_Range(-1.f, 1.f);
			m_Wind.z = Utils::Random_In_Range(-1.f, 1.f);
		}

		Get_Owner()->Get_MeshRenderer()->SetVec4(0, _float4{  m_Wind.x, m_Wind.y, m_Wind.z, m_fWindWeight });
	}
}
