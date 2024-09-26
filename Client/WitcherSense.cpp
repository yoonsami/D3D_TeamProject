#include "pch.h"
#include "WitcherSense.h"

_bool g_bWitcherSenseOn = false;

void WitcherSense::Tick()
{
	auto& saturation = CUR_SCENE->g_Saturation;

	if (KEYTAP(KEY_TYPE::Z))
		g_bWitcherSenseOn = !g_bWitcherSenseOn;


	if (g_bWitcherSenseOn)
	{
		if (saturation >= 0.4f)
			saturation -= fDT;

		if (saturation < 0.8f)
			saturation = 0.4f;
	}
	else
	{
		if (saturation <= 1.f)
			saturation += fDT;

		if (saturation > 1.f)
			saturation = 1.f;

	}


}
