#include "pch.h"
#include "EffectGoUp.h"
#include "MeshEffect.h"

EffectGoUp::EffectGoUp(_float fUpSpeed, shared_ptr<GameObject> owner)
	:m_fSpeed(fUpSpeed)
	, m_pFollowObj(owner)
{
}

void EffectGoUp::Final_Tick()
{
	if (m_pFollowObj.expired())
		return;
	m_fAcc += m_fSpeed * fDT;

	_float4x4 mWorldMatrix = m_pFollowObj.lock()->Get_Transform()->Get_WorldMatrix();
	Get_Transform()->Set_WorldMat(mWorldMatrix * _float4x4::CreateTranslation(_float3(0.f, m_fAcc, 0.f)));
}
