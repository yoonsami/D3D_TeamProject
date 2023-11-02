#pragma once
#include "Component.h"
class FSM abstract : public Component
{
public:
	FSM();
	virtual void OnCollision(shared_ptr<BaseCollider> pCollider, _float fGap) = 0;
	virtual void OnCollisionEnter(shared_ptr<BaseCollider> pCollider, _float fGap) = 0;
	virtual void OnCollisionExit(shared_ptr<BaseCollider> pCollider, _float fGap) = 0;

protected:
	virtual void State_Tick() = 0;
	virtual void State_Init() = 0;

	_uint Get_CurFrame();
	_uint Get_FinalFrame();
	_bool Is_AnimFinished();

	_float3 Get_InputDirVector(); // Ű���� �Է¿� ���� ī�޶� ���� �չ��� : +x, ������ ���� : +y�� ũ�� 1¥�� ���� ��ȯ
	void Soft_Turn_ToInputDir(const _float3& vInputDir, _float turnSpeed);
	_float3 Soft_Turn_ToTarget(const _float3& vTargetPos, _float turnSpeed);
	_float3 Soft_Turn_ToTarget(const _float4& vTargetPos, _float turnSpeed);
};

