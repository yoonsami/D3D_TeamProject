#pragma once

enum class KEY_TYPE
{
	UP,
	DOWN,
	LEFT,
	RIGHT,

	W,	A,	S,	D,	Q,	E,	Z,	C,	R, X ,V, O, P,
	KEY_1,	KEY_2,	KEY_3,	KEY_4,	KEY_5,
	F1, F2, F3, F4, F5, F6, F7,

	SPACE,
	ESC,
	ENTER,
	LCTRL,
	LSHIFT,
	LALT,
	LBUTTON,
	RBUTTON,
	MBUTTON,
	TAB,
	LAST,
};

enum class KEY_STATE
{
	NONE,
	HOLD,
	TAP,
	AWAY,
	KEYSTATE_END
};

enum
{
	KEY_TYPE_COUNT = static_cast<_uint>(VK_MAX + 1),
	KEY_STATE_COUNT = static_cast<_uint>(KEY_STATE::KEYSTATE_END)

};

class InputMgr 
{
	DECLARE_SINGLETON(InputMgr)

public:
	void Initialize(HWND hWnd);
	void Tick();

public:
	bool GetButtonHold(KEY_TYPE key)	{ return GetState(key) == KEY_STATE::HOLD; }
	bool GetButtonTap(KEY_TYPE key)		{ return GetState(key) == KEY_STATE::TAP; }
	bool GetButtonAway(KEY_TYPE key)	{ return GetState(key) == KEY_STATE::AWAY; }

	const _float2& GetMousePos()		{ return m_ptMousePos; }
	const _float2& GetMouseDir()		{ return m_vMouseDir; }
	POINT GetMousePosToPoint()			{ return { LONG(m_ptMousePos.x), LONG(m_ptMousePos.y) }; }
	_bool Get_IsCanMove()				{ return m_bIsCanMove; }

	void Set_Mouse_Move(_bool bValue);
	void Set_Show_Cursor(_bool bValue);

private:
	inline KEY_STATE GetState(KEY_TYPE key) { return m_vecKeyStates[static_cast<_uchar>(key)]; }

private:
	vector<KEY_STATE>	m_vecKeyStates;
	HWND				m_hWnd				= { nullptr };
	_float2				m_ptMousePos		= {};
	_float2				m_vMouseDir			= {};

	_bool				m_bIsCanMove		= { true };
};

