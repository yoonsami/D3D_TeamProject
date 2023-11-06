#pragma once

#include "pch.h"

class ImGui_Manager
{
	DECLARE_SINGLETON(ImGui_Manager)

private:
	enum GizmoOp { GizmoTR, GizmoRT, GizmoSC, GizmoEND };

private:
	virtual ~ImGui_Manager();

private:
	bool show_demo_window = true;
	bool show_another_window = false;

public:
	void ImGui_SetUp();
	void ImGui_Tick();
	void ImGui_Render();

private:
	void Show_Gizmo();
	// ��ġ�� �� �ִ� ������Ʈ ��ϰ� ����� �����ְ� �����ϴ� ������.
	void Frame_ObjectBase();
	// ���� ��ġ�Ǿ��ִ� ������Ʈ ��� ������.
	void Frame_Objects();
	// ������ ������Ʈ�� ����.
	void Frame_ObjectDesc();
	// ���⼺������ ���� ����.
	void Frame_DirectionalLight();
	// ��ŷ
	void Picking_Object();

	// �ʿ�����Ʈ���̽� ��� �ҷ�����
	HRESULT Load_MapObjectBase();
	// �ʿ�����Ʈ ����
	HRESULT Create_MapObject();
	// �ʿ�����Ʈ ����
	HRESULT Delete_MapObject();
	// �ʿ�����Ʈ ����
	HRESULT Save_MapObject();
	// �ʿ�����Ʈ �ҷ�����
	HRESULT Load_MapObject();

private:
// ��ġ ������ ������Ʈ ���
	vector<const char*> m_strObjectBaseNameList;
	// �����̸��� ���� �ߺ�����(�𵨺� ��ȣ)
	vector<_uint> m_iObjectBaseIndexList;
	vector<shared_ptr<char[]>> m_strObjectBaseNamePtr;
	_int m_iObjectBaseIndex = { 0 };
	// �����ϴ� ������Ʈ�� �ְ���� UVWeight - UV�� �������� ����
	_float m_fUVWeight = { 1.f };
// ��ġ�� ������Ʈ ���
	vector<const char*> m_strObjectName;
	// Load�� �̸����� �����Ҵ��Ͽ� �����ϰ����� �����ͺ���
	vector<shared_ptr<char[]>> m_strObjectNamePtr;
	// ���������ȣ
	_int m_iObjects = { 0 };
	// ���� ��ġ�� ������Ʈ���� ������ �ξ��������
	vector<shared_ptr<GameObject>> m_pMapObjects;
	_float3 m_PickingPos = { 0.f, 0.f, 0.f };
	// ���̺������̸�
	char m_szSaveFileName[MAX_PATH] = "";
// ��������
	GizmoOp m_eGizmoOp = { GizmoTR };
	_bool	m_bGizmoOp[GizmoEND] = { false, };
};