#include "pch.h"
#include "Get_Gravity.h"
#include "CharacterController.h"

Get_Gravity::Get_Gravity(_float fFinalDistY)
	: m_fFinalDistY(fFinalDistY)
{
}

void Get_Gravity::Final_Tick()
{
	if (Get_Transform()->Get_State(Transform_State::POS).y > m_fFinalDistY)
	{
		m_fVelY += -9.8f * fDT;
		
		Get_Transform()->Go_Dir(_float3(0.f, m_fVelY,0.f));

		_float4 vPos = Get_Transform()->Get_State(Transform_State::POS);
		if (vPos.y < m_fFinalDistY)
		{
			if (Get_CharacterController() && Get_CharacterController()->Get_Actor())
				Get_CharacterController()->Get_Actor()->setFootPosition({ vPos.x,m_fFinalDistY, vPos.z });

			Get_Transform()->Set_State(Transform_State::POS, _float4(vPos.x, m_fFinalDistY, vPos.z, 1.f));
		}
	}

}
