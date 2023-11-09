#pragma once

#include "pch.h"
#include "MapObjectScript.h"

class ImGui_Manager
{
	DECLARE_SINGLETON(ImGui_Manager)

private:
	enum GizmoOp { GizmoTR, GizmoRT, GizmoSC, GizmoEND };
	// ����� ��� ��� ( �ʿ�����Ʈ or ������ )
	enum GizmoTarget { GizmoTMapObj, GizmoTPointLight, GizmoTEnd };

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
	// ��ī�̹ڽ� ���� ������.
	void Frame_SkyBox();
	// ��ġ�� �� �ִ� ������Ʈ ��ϰ� ����� �����ְ� �����ϴ� ������.
	void Frame_ObjectBase();
	// ���� ��ġ�Ǿ��ִ� ������Ʈ ��� ������.
	void Frame_Objects();
	// ������ ���� ����.
	void Frame_Light();
	// ��ŷ
	void Picking_Object();

	// ��ī�̹ڽ���� �ҷ�����
	HRESULT Load_SkyBoxTexture();
	// �ʿ�����Ʈ���̽� ��� �ҷ�����
	HRESULT Load_MapObjectBase();
	// ������ �� ������Ʈ ����
	HRESULT Create_SelectObject();
	// �� ������Ʈ ���������� �������� ������Ʈ�� �����Ͽ� ��ȯ�ϴ� �Լ�
	shared_ptr<GameObject>& Create_MapObject(MapObjectScript::MapObjectDesc _CreateDesc);
	// ���� ������ ������ ����
	HRESULT Create_SelectPointLight();
	// �������� �������� ���� �� ���� �߰� �� ������ ��ȯ
	shared_ptr<GameObject>& Create_PointLight(LightInfo _ptltInfo);

	// ������ ����
	HRESULT Delete_PointLight();
	// �ʿ�����Ʈ ����
	HRESULT Delete_MapObject();
	// �ʿ�����Ʈ ����
	HRESULT Save_MapObject();
	// �ʿ�����Ʈ �ҷ�����
	HRESULT Load_MapObject();

	// ������Ʈ�� �޾ƿͼ� �ø������ǰ� ���̸� ����Ͽ� �ݿ�
	void Compute_CullingData(shared_ptr<GameObject>& _pGameObject);
	
	// �׸���, ��, �ø����
	void Burn(shared_ptr<GameObject>& _pGameObject);
	void BurnAll();

private:
// ��ġ ������ ������Ʈ ���
	vector<const char*> m_strObjectBaseNameList;
	// �����̸��� ���� �ߺ�����(�𵨺� ��ȣ)
	vector<_uint> m_iObjectBaseIndexList;
	// �ε�� ���ڿ� �����Ҵ��� ���� ����
	vector<shared_ptr<char[]>> m_strObjectBaseNamePtr;
	// ���� ���õ� ���̽� ������Ʈ�� ��ȣ
	_int m_iObjectBaseIndex = { 0 };
	// ������Ʈ ������ ����
	MapObjectScript::MAPOBJDESC m_CreateObjectDesc;

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
	// �ݶ��̴����
	const char* m_szColliderTypes[4] = { "Sphere","AABB", "OBB", "Mesh" };

// ��������
	GizmoOp m_eGizmoOp = { GizmoTR };
	_bool	m_bGizmoOp[GizmoEND] = { false, };
	GizmoTarget m_GizmoTarget = GizmoTMapObj;

// ��ī�̹ڽ� ����
	vector<const char*> m_strSkyboxList;
	// �ε�� ���ڿ� �����Ҵ��� ���� ����
	vector<shared_ptr<char[]>> m_strSkyBoxNamePtr;
	_int m_iCurrentSkyBoxIndex = { 0 };

// ������
	_float4 m_DirectionalLightPos = _float4{ 0.f, 0.f, 0.f, 1.f };
	_float3 m_DirectionalLightLookDir = _float3{ 0.f, 0.f, 0.f};
	LightInfo m_DirectionalLightInfo; // �� ��������
	vector<shared_ptr<GameObject>> m_pPointLightObjects;
	vector<const char*> m_strPointLightList;
	_int m_iPointLightIndex = { 0 };
	// ������ ��������
	LightInfo m_CreatePointLightInfo;
};