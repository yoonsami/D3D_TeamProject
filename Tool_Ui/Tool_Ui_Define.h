#pragma once
enum LEVEL { LEVEL_STATIC, LEVEL_LOADING, LEVEL_LOGO, LEVEL_GAMEPLAY, LEVEL_END };


const unsigned int	g_iWinSizeX = 1920;
const unsigned int	g_iWinSizeY = 1080;

extern HWND		g_hWnd;

enum class CollisionGroup
{
	Player,

	Monster,

	Script,




	END = MAX_COLLIDER_GROUP
};
