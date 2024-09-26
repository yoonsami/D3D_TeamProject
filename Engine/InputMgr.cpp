#include "pch.h"
#include "InputMgr.h"

int g_arrVK[static_cast<int>(KEY_TYPE::LAST)] =
{
   VK_UP,
   VK_DOWN,
   VK_LEFT,
   VK_RIGHT,

   'W',   'A',   'S',   'D',   'Q',   'E',   'Z',   'C',   'R',   'X',   'V', 'O', 'P',
   '1',   '2',   '3',   '4',   '5',
   VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7,


   VK_SPACE,
   VK_ESCAPE,
   VK_RETURN,
   VK_LCONTROL,
   VK_LSHIFT,
   VK_MENU,
   VK_LBUTTON,
   VK_RBUTTON,
   VK_MBUTTON,
   VK_TAB,
};


void InputMgr::Initialize(HWND hWnd)
{
	m_vecKeyStates.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);
	m_hWnd = hWnd;

	//::ShowCursor(true);
}

void InputMgr::Tick()
{
	HWND hWnd = ::GetActiveWindow();
	if (m_hWnd != hWnd)
	{
		for (auto& i : m_vecKeyStates)
			i = KEY_STATE::NONE;
		m_vMouseDir = { 0.f,0.f };
		return;
	}

	for (_uint key = 0; key < KEY_TYPE_COUNT; ++key)
	{
		if (GetAsyncKeyState(g_arrVK[key]) & 0x8000)
		{
			KEY_STATE& state = m_vecKeyStates[key];

			if (state == KEY_STATE::HOLD || state == KEY_STATE::TAP)
			{
				state = KEY_STATE::HOLD;
			}
			else
			{
				state = KEY_STATE::TAP;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
			}
		}
		else
		{
			KEY_STATE& state = m_vecKeyStates[key];

			if (state == KEY_STATE::HOLD || state == KEY_STATE::TAP)
			{
				state = KEY_STATE::AWAY;
			}
			else
			{
				state = KEY_STATE::NONE;
			}
		}
	}

	POINT MousPos;

	::GetCursorPos(&MousPos);
	::ScreenToClient(m_hWnd, &MousPos);

	m_ptMousePos = { _float(MousPos.x),_float(MousPos.y) };

	if (false == m_bIsCanMove)
	{
		RECT tmp{};
		GetClientRect(hWnd, &tmp);

		POINT center = { (tmp.right - tmp.left) / 2, (tmp.bottom - tmp.top) / 2 };

		m_vMouseDir.x = _float(m_ptMousePos.x - center.x);
		m_vMouseDir.y = _float(m_ptMousePos.y - center.y);
		ClientToScreen(hWnd, &center);
		SetCursorPos(center.x, center.y);
	}
	else
	{
		m_vMouseDir = { 0.f, 0.f };
	}
}

void InputMgr::Set_Mouse_Move(_bool bValue)
{
	if (false == bValue)
	{
		HWND hWnd = ::GetActiveWindow();
		if (m_hWnd != hWnd)
		{
			for (auto& i : m_vecKeyStates)
				i = KEY_STATE::NONE;
			m_vMouseDir = { 0.f,0.f };
			return;
		}

		m_bIsCanMove = bValue;

		RECT tmp{};
		GetClientRect(hWnd, &tmp);

		POINT center = { (tmp.right - tmp.left) / 2, (tmp.bottom - tmp.top) / 2 };

		ClientToScreen(hWnd, &center);
		SetCursorPos(center.x, center.y);

		m_vMouseDir = { 0.f,0.f };
	}
	else if (true == bValue)
	{
		m_bIsCanMove = bValue;
	}
}

void InputMgr::Set_Show_Cursor(_bool bValue)
{
	::ShowCursor(bValue);
}