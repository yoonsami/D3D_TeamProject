#pragma once

#include "Client_Enum.h"
#include "Client_Macro.h"
#include "Client_Struct.h"

const unsigned int	g_iWinSizeX = 1920;
const unsigned int	g_iWinSizeY = 1080;

extern HWND		g_hWnd;
extern nanoseconds g_preSecondTick;
extern nanoseconds g_secondTick;
extern nanoseconds g_preSecondRender;
extern nanoseconds g_secondRender;
extern nanoseconds timeTick;
extern nanoseconds timeRender;

extern _bool    g_bIsCanMouseMove;
extern _bool    g_bIsCanRotation;
extern _bool	g_bCutScene;
extern _bool	g_bWitcherSenseOn;