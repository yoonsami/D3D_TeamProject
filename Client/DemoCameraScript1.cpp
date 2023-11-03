#include "pch.h"
#include "DemoCameraScript1.h"

void DemoCameraScript1::Tick()
{
	// weak_ptr expired() -> �� �����Ͱ� �̹� �Ҹ��ؼ� ����� �� ���� ���¸� true ��ȯ
	if (m_pOwner.expired())
		return;

	if (KEYPUSH(KEY_TYPE::W))
		Get_Transform()->Go_Straight();

	if (KEYPUSH(KEY_TYPE::S))
		Get_Transform()->Go_Backward();

	if(KEYPUSH(KEY_TYPE::A))
		Get_Transform()->Go_Left();

	if(KEYPUSH(KEY_TYPE::D))
		Get_Transform()->Go_Right();

	if(KEYPUSH(KEY_TYPE::Q))
		Get_Transform()->Go_Dir(_float3(0.f, 5.f, 0.f));

	if(KEYPUSH(KEY_TYPE::E))
		Get_Transform()->Go_Dir(_float3(0.f, -5.f, 0.f));
}
