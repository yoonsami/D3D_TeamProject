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

	// �ʿ�����Ʈ���̽� ��� �ҷ�����
	HRESULT Load_MapObjectBase();
	// �ʿ�����Ʈ ����
	HRESULT Create_MapObject();

private:
// ��ġ ������ ������Ʈ ���
	vector<const char*> m_strObjectBaseName;
	_int m_iObjectBaseIndex = { 0 };
// ��ġ�� ������Ʈ ���
	vector<const char*> m_strObjectName;
	_int m_iObjects = { 0 };
	// ���� ��ġ�� ������Ʈ���� ������ ��������
	vector<weak_ptr<GameObject>> m_pMapObjects;
};