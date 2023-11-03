#include "pch.h"
#include "DemoCameraScript2.h"

void DemoCameraScript2::Tick()
{
	// ������Ʈ��Ű�� ������ Ŀ���߾Ӱ���+ȭ��ȸ������
	if (KEYPUSH(KEY_TYPE::LCTRL))
	{
		_float2 vMouseDir = INPUT.GetMouseDir();

		RECT tmp{};
		GetClientRect(g_hWnd, &tmp);
		POINT center = { (tmp.right - tmp.left) / 2, (tmp.bottom - tmp.top) / 2 };
		SetCursorPos(center.x, center.y);

		{
			_float3 axis = _float3::Up;

			Get_Transform()->Turn(axis, vMouseDir.x * m_fRotationSpeed);
		}
		{
			_float3 axis = Get_Transform()->Get_State(Transform_State::RIGHT).xyz();

			Get_Transform()->Turn(axis, vMouseDir.y * m_fRotationSpeed);
		}
	}
}
