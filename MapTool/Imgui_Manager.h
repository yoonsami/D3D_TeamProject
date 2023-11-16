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
	// ��ġ�� �� �ִ� ������Ʈ ��ϰ� ����� �����ְ� �����ϴ� ������.
	void Frame_ObjectBase();
	// ��ġ�� �� �ִ� ������Ʈ ��ϰ� ����� �����ְ� �����ϴ� ������.
	void Frame_ObjectBaseManager();
	// ���� ��ġ�Ǿ��ִ� ������Ʈ ��� ������.
	void Frame_Objects();
	// ���� ��ġ�Ǿ��ִ� ������Ʈ ���� ������
	void Frame_SelcetObjectManager();
	// ������ ���� ����.
	void Frame_Light();
	// ���� ���� ����
	void Frame_Wall();
	// ��ŷ
	void Picking_Object();
	// ���ÿ�����Ʈ�� �þ��̵�
	void LookAtSampleObject();

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
	// �� �޽� ���� �� ��������� ����
	void Create_WallMesh();
	// ���޽� �����迭 �ʱ�ȭ
	void Clear_WallMesh();

	// ������ ����
	HRESULT Delete_PointLight();
	// �ʿ�����Ʈ ����
	HRESULT Delete_MapObject();
	// �ʿ�����Ʈ ����
	HRESULT Save_MapObject();
	// �ʿ�����Ʈ �ҷ�����
	HRESULT Load_MapObject();

	// ������Ʈ�� �޾ƿͼ� �ø������ǰ� ���̸� ����Ͽ� �ݿ�, �ø������ǰ� ���̸� float4�� ��ȯ
	_float4 Compute_CullingData(shared_ptr<GameObject>& _pGameObject);
	
	// �׸���, ��, �ø����
	void Bake(shared_ptr<GameObject>& _pGameObject);
	void BakeAll();
	
	// ���̽�������Ʈ�̸��� ������ ���� ������Ʈ ����
	void Create_SampleObjects();
	// ���� ������ ���̽�������Ʈ�� ���� �ʿ�����Ʈ �𵨺��� �� ī�޶� ����
	void Set_SampleObject();

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
	// ���̽�������Ʈ ����
	char m_szBaseObjectFilter[MAX_PATH] = "";
	_int m_iFilteredBaseObjectsIndex = { 0 };
	// ���͸��� �Ϸ�� �̸�����
	vector<const char*> m_FilteredBaseObjectNames;
	// ���͸��� �Ϸ�� �̸������ּҰ���
	vector<shared_ptr<char[]>> m_strFilteredNamePtr;
	// ���̽�������Ʈ����Ʈ�� �ٲ������ ���̸� ã�ư����� ����, true�� ƽ���� 1ȸ������ false�� �ٽùٲ�
	_bool m_bBaseObjectListResetHeight = { false };

// ���ÿ�����Ʈ
	shared_ptr<GameObject> m_SampleObject;
	// �����̵��� ī�޶��� ����
	_float4x4 m_matPreCamera = { XMMatrixIdentity() };
	_float m_fSampleModelCullSize = { 0.f };

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
	// ��ġ�� ������Ʈ����Ʈ�� �ٲ������ ���̸� ã�ư����� ����, true�� ƽ���� 1ȸ������ false�� �ٽùٲ�
	_bool m_bObjectListResetHeight = { false };

// ��������
	GizmoOp m_eGizmoOp = { GizmoTR };
	_bool	m_bGizmoOp[GizmoEND] = { false, };
	GizmoTarget m_GizmoTarget = GizmoTMapObj;

// ��ī�̹ڽ� ����
	_float4 m_CamPos = { 0.f, 0.f, 0.f, 0.f };
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
	// ������ �������� ����Ʈ ��������
	_bool m_bPointLightEffectHas = { false };
	_float4 m_CreateTargetAmbient = { 1.f, 1.f, 1.f, 1.f };
	_float4 m_CreateTargetDiffuse = { 1.f, 1.f, 1.f, 1.f };
	_float m_fCreatePointLightSpeed = { 1.f };

// ����������ŷ����
	_float3 m_WallPickingPos[2] = { _float3{0.f, 0.f, 0.f}, _float3{0.f, 0.f, 0.f} };
	_bool m_bFirstWallPick = { true };
	_float m_fWallHeight = { 10.f };
	vector<pair<_float3, _float3>> m_WallRectPosLDRU;


	_int curMapIndex = 0;
	vector<string> m_MapNames;
};