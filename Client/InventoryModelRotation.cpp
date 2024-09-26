#include "pch.h"
#include "InventoryModelRotation.h"

void InventoryModelRotation::Tick()
{
	if (g_bIsCanMouseMove)
		return;

	
	if (KEYPUSH(KEY_TYPE::RBUTTON))
	{
		_float2 vMouseMove = INPUT.GetMouseDir();

		Get_Transform()->Turn(_float3::Up, vMouseMove.x * XM_PI * 2.f * fDT);

	}

}
