#include "pch.h"

#include "Imgui_Manager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ImGuizmo.h"

#include "FileUtils.h"
#include "Utils.h"
#include "PickingMgr.h"
#include "Light.h"

#include "Model.h"
#include "ModelRenderer.h"

#include "Camera.h"
#include "MapObjectScript.h"

ImGuizmo::OPERATION m_eGuizmoType = { ImGuizmo::TRANSLATE };
namespace fs = std::filesystem;

ImGui_Manager::~ImGui_Manager()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGui_Manager::ImGui_SetUp()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(DEVICE.Get(), CONTEXT.Get());

    ImGuizmo::Enable(true);
    ImGuizmo::SetRect(0.f, 0.f, g_iWinSizeX, g_iWinSizeY);

    Load_MapObjectBase();
}

void ImGui_Manager::ImGui_Tick()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    Frame_DirectionalLight();
    Frame_ObjectBase();
    Frame_Objects();
    
    Picking_Object();

    Show_Gizmo();
}

void ImGui_Manager::ImGui_Render()
{
    ImGui::Render();
   
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGui_Manager::Show_Gizmo()
{
    // ����� ���
    ImGuizmo::BeginFrame();
    if (m_strObjectName.size() != 0)
    {
        // ����� ���� ī�޶��� ��,������ ������.
        shared_ptr<Camera> CurCam = CUR_SCENE->Get_Camera(L"Default")->Get_Camera();
        _float4x4 matView = CurCam->Get_ViewMat();
        _float4x4 matProj = CurCam->Get_ProjMat();

        // ����� ���� ����
        _float4x4 matGuizmo = m_pMapObjects[m_iObjects].get()->Get_Transform()->Get_WorldMatrix();

        ImGuizmo::Manipulate((float*)&matView,
            (float*)&matProj,
            m_eGuizmoType,
            ImGuizmo::LOCAL,
            (float*)&matGuizmo);

        m_pMapObjects[m_iObjects].get()->Get_Transform()->Set_WorldMat(matGuizmo);
    }
}

void ImGui_Manager::Frame_ObjectBase()
{
    ImGui::Begin("Frame_ObjectBase"); // ���� ������

    ImGui::ListBox("##ObjectBase", &m_iObjectBaseIndex, m_strObjectBaseNameList.data(), (int)m_strObjectBaseNameList.size(), 10);

    ImGui::InputFloat("UVWeight", &m_fUVWeight);

    ImGui::InputFloat3("PickingPos", (_float*)&m_PickingPos);

    if (ImGui::Button("Create") || KEYTAP(KEY_TYPE::Z))
        if (FAILED(Create_MapObject()))
            MSG_BOX("Fail : Create_MapObject");

    ImGui::SameLine();
    ImGui::Text("Press Z");

    ImGui::End();
}

void ImGui_Manager::Frame_Objects()
{
    ImGui::Begin("Frame_Objects"); // ���� ������

    ImGui::ListBox("##Objects", &m_iObjects, m_strObjectName.data(), (_int)m_strObjectName.size(), 10);

    if (m_strObjectName.size() > 0)
    {
        // ��ü�ɼ� false�� �ʱ�ȭ
        for (size_t i = 0; i < GizmoEND; i++)
        {
            m_bGizmoOp[i] = false;
            // ����ɼ��̶� ������ true
            if (m_eGizmoOp == i)
                m_bGizmoOp[i] = true;
        }
        // ������, ȸ��, ������ ����
        if (ImGui::RadioButton("Tr", m_bGizmoOp[GizmoTR]))
            m_eGizmoOp = GizmoTR;
        ImGui::SameLine();
        if (ImGui::RadioButton("Rt", m_bGizmoOp[GizmoRT]))
            m_eGizmoOp = GizmoRT;
        ImGui::SameLine();
        if (ImGui::RadioButton("Sc", m_bGizmoOp[GizmoSC]))
            m_eGizmoOp = GizmoSC;
        // ���� ����� �ɼ� ����
        switch (m_eGizmoOp)
        {
        case GizmoTR:
            m_eGuizmoType = ImGuizmo::TRANSLATE;
            break;
        case GizmoRT:
            m_eGuizmoType = ImGuizmo::ROTATE;
            break;
        case GizmoSC:
            m_eGuizmoType = ImGuizmo::SCALE;
            break;
        default:
            break;
        }

        // ������, ȸ��, ������ ���� �� ���
        _float4x4 tempFloat4x4 = m_pMapObjects[m_iObjects].get()->Get_Transform()->Get_WorldMatrix();
        _float4x4* pTempObj = &tempFloat4x4;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents((float*)pTempObj, matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation);
        ImGui::InputFloat3("Rt", matrixRotation);
        ImGui::InputFloat3("Sc", matrixScale);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (float*)pTempObj);
        m_pMapObjects[m_iObjects]->Get_Transform()->Set_WorldMat(*(_float4x4*)pTempObj);

        if (ImGui::Button("Delete"))
            if (FAILED(Delete_MapObject()))
                MSG_BOX("Fail : Delete_MapObject");
    }

    // ���̺�ε�
    ImGui::NewLine();
    ImGui::InputText("##SaveFileName", m_szSaveFileName, sizeof(m_szSaveFileName));
    if (ImGui::Button("Save Object"))
        if (FAILED(Save_MapObject()))
            MSG_BOX("Fail : Save_MapObject");
        else
            MSG_BOX("Complete_Save");
    ImGui::SameLine();
    if (ImGui::Button("Load Object"))
        if (FAILED(Load_MapObject()))
            MSG_BOX("Fail : Load_MapObject");
        else
            MSG_BOX("Complete_Load");

    ImGui::End();
}

void ImGui_Manager::Frame_DirectionalLight()
{
    ImGui::Begin("Frame_Directionalight"); // ���� ������

    // ������
    _float4& Pos = (_float4&)CUR_SCENE->Get_Light()->Get_Transform()->Get_WorldMatrix().m[3];
    ImGui::DragFloat3("Position", (_float*)&Pos);
    _float3& LookDir = CUR_SCENE->Get_Light()->Get_Transform()->Get_CurrentDir();
    ImGui::DragFloat3("LookDir", (_float*)&LookDir);
    CUR_SCENE->Get_Light()->Get_Transform()->Set_LookDir(LookDir);
    LightInfo& DirectionalLightInfo = CUR_SCENE->Get_Light()->Get_Light()->Get_LightInfo();
    _float4& Diffuse = DirectionalLightInfo.color.diffuse;
    ImGui::ColorEdit4("Diffuse", (_float*)&Diffuse);
    _float4& Ambient = DirectionalLightInfo.color.ambient;
    ImGui::ColorEdit4("Ambient", (_float*)&Ambient);
    _float4& Specular = DirectionalLightInfo.color.specular;
    ImGui::ColorEdit4("Specular", (_float*)&Specular);
    _float4& Emissive = DirectionalLightInfo.color.emissive;
    ImGui::ColorEdit4("Emissive", (_float*)&Emissive);

    ImGui::End();
}

void ImGui_Manager::Picking_Object()
{
    // ���콺 ��Ŭ�� �� �޽���ŷ
    if (KEYTAP(KEY_TYPE::RBUTTON))
    {
        POINT MousePos;
        ::GetCursorPos(&MousePos);
        ::ScreenToClient(g_hWnd, &MousePos);
        _float2 ScreenPos{ (_float)MousePos.x, (_float)MousePos.y };

        shared_ptr<GameObject> PickObject = PickingMgr::GetInstance().Pick_Mesh(ScreenPos, CUR_SCENE->Get_Camera(L"Default")->Get_Camera(), CUR_SCENE->Get_Objects(), m_PickingPos);

        // ���õ� ������Ʈ ��ȣ �ٲٱ�
        for (_int i = 0; i < m_pMapObjects.size(); ++i)
        {
            if (PickObject == m_pMapObjects[i])
                m_iObjects = i;
        }
    }
}

HRESULT ImGui_Manager::Load_MapObjectBase()
{
    wstring path = L"..\\Resources\\Models\\MapObject\\";

    for (auto& entry : fs::recursive_directory_iterator(path))
    {
        // �ʿ�����Ʈ ���������� �����̸����� ��ȸ�ϸ� ���̽�������Ʈ ����Ʈ�� ����.
        if (!entry.is_directory())
            continue;

        // ������ �̸��� ������
        wstring fileName = entry.path().filename().wstring();
        WCHAR szTempName[MAX_PATH];
        lstrcpy(szTempName, fileName.c_str());

        // char �������� ��ȯ
        shared_ptr<char[]> pChar = shared_ptr<char[]>(new char[MAX_PATH]);
        WideCharToMultiByte(CP_ACP, 0, szTempName, -1, pChar.get(), MAX_PATH, 0, 0);

        // ���̽�������Ʈ �̸��� ������ ���Ϳ� �߰�
        m_strObjectBaseNamePtr.push_back(pChar);
        // ���̽�������Ʈ �̸��� ����ƮUI�� �߰�
        m_strObjectBaseNameList.push_back(pChar.get());
    }
    return S_OK;
}

HRESULT ImGui_Manager::Create_MapObject()
{
    // 1. ���� ���õ� ���̽�������Ʈ�̸� ��������
    wstring strCurObjBase = Utils::ToWString(m_strObjectBaseNameList[m_iObjectBaseIndex]);

    // 2. ���ӿ�����Ʈ �����Ҵ��Ͽ� ������� �߰�
    shared_ptr<GameObject> CreateObject = make_shared<GameObject>();
    CreateObject->Add_Component(make_shared<Transform>());
    CreateObject->Get_Transform()->Set_State(Transform_State::POS, _float4(m_PickingPos.x, m_PickingPos.y, m_PickingPos.z, 1.f));
    {
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
        shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
        {
            // �ʿ�����Ʈ �н����� + UV����ġ����
            shared_ptr<MapObjectScript> MapObjSc = make_shared<MapObjectScript>(renderer, ModelRenderer::PASS_MAPOBJECT, m_fUVWeight);
            CreateObject->Add_Component(MapObjSc);
        }
        {
            // ���̽� ������Ʈ�� �̸��� ����Ͽ� ������Ʈ ����
            shared_ptr<Model> model = RESOURCES.Get<Model>(strCurObjBase);
            renderer->Set_Model(model);
        }
        //�𵨷����� ������Ʈ
        CreateObject->Add_Component(renderer);
    }
    CUR_SCENE->Add_GameObject(CreateObject);
    m_pMapObjects.push_back(CreateObject);

    // 3. ���� ��ġ�� ������Ʈ ��Ͽ� �߰�
    m_strObjectName.push_back(m_strObjectBaseNameList[m_iObjectBaseIndex]);

    // 4. ���� ������ ������Ʈ�� Ÿ����(�����)�ϱ�
    m_iObjects = (_int)(m_pMapObjects.size() - 1);

    return S_OK;
}

HRESULT ImGui_Manager::Delete_MapObject()
{
    // 1. ���� ���õ� ������Ʈ�� ������ ����
    CUR_SCENE->Remove_GameObject(m_pMapObjects[m_iObjects]);
    // 2. ���� ���õ� ������Ʈ�� ���Ϳ��� ����
    {
        auto iter = m_pMapObjects.begin();
        for (_int i = 0; i < m_iObjects; ++i)
            ++iter;
        // ����Ʈ���Ϳ��� �ش系�����
        if (m_iObjects > 0)
            iter = m_pMapObjects.erase(iter);
        else
            m_pMapObjects.erase(m_pMapObjects.begin());
    }
    // 3. ���� ��ġ�� ������Ʈ List �̸� ����
    {
        auto iter = m_strObjectName.begin();
        for (_int i = 0; i < m_iObjects; ++i)
            ++iter;
        // ����Ʈ���Ϳ��� �ش系�����
        if (m_iObjects > 0)
        {
            iter = m_strObjectName.erase(iter);
            // ����Ʈ�� ������ġ ����
            --m_iObjects;
        }
        else
        {
            m_strObjectName.erase(m_strObjectName.begin());
        }
    }

    return S_OK;
}

HRESULT ImGui_Manager::Save_MapObject()
{
    // ���̺� ���� �̸����� �����ϱ�
    string strFileName = m_szSaveFileName;
    string strFilePath = "..\\Resources\\Data\\";
    strFilePath += strFileName + ".dat";

    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(Utils::ToWString(strFilePath), FileMode::Write);

    // 1. ������Ʈ ���� ����
    file->Write<_int>((_int)m_pMapObjects.size());
    
    // 2. ������Ʈ�� �̸��� UVWeight, �����Ʈ���� ����
    for (_uint i = 0; i < m_pMapObjects.size(); ++i)
    {
        // �̸�����
        string ObjectName(m_strObjectName[i]);
        file->Write<string>(ObjectName);
        // UVWeight����
        file->Write<_float>(m_pMapObjects[i]->Get_Script<MapObjectScript>()->Get_UVWeight());
        // ������� ����
        file->Write<_float4x4>(m_pMapObjects[i].get()->Get_Transform()->Get_WorldMatrix());
    }

    return S_OK;
}

HRESULT ImGui_Manager::Load_MapObject()
{
    // ���簮���ִ� ������Ʈ����
    _uint iSize = (_int)m_pMapObjects.size();
    // 1. ������ ��������Ʈ ���� �� Clear
    for (_uint i = 0; i< iSize; ++i)
    {
        // ��� ������Ʈ�� ������ ���� ( �ǵں��� )
        CUR_SCENE->Remove_GameObject(m_pMapObjects.back());
        m_pMapObjects.pop_back();
        m_strObjectName.pop_back();
    }
    m_pMapObjects.clear();
    m_strObjectName.clear();
    m_strObjectNamePtr.clear();
    m_iObjects = 0;

    // ���̺� ���� �̸����� �����ϱ�
    string strFileName = m_szSaveFileName;
    string strFilePath = "..\\Resources\\Data\\";
    strFilePath += strFileName + ".dat";
    // 2. ������Ʈ ���� �ҷ�����
    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(Utils::ToWString(strFilePath), FileMode::Read);
    _int iNumObjects = file->Read<_int>();
    
    for (_int i = 0; i < iNumObjects; ++i)
    {
        // 3. �̸��� ��Ʈ������ �о����
        wstring strObjectName = Utils::ToWString(file->Read<string>());
        _float fUVWeight = file->Read<_float>();
        _float4x4 matWorld = file->Read<_float4x4>();

        // 4. ������ ������ �������� �ʿ�����Ʈ�� �����Ͽ� ������� �߰�
        shared_ptr<GameObject> CreateObject = make_shared<GameObject>();
        CreateObject->Add_Component(make_shared<Transform>());
        CreateObject->Get_Transform()->Set_WorldMat(matWorld);
        {
            shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
            shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
            {
                // �ʿ�����Ʈ �н����� + UV����ġ����
                shared_ptr<MapObjectScript> MapObjSc = make_shared<MapObjectScript>(renderer, ModelRenderer::PASS_MAPOBJECT, fUVWeight);
                CreateObject->Add_Component(MapObjSc);
            }
            {
                // ���̽� ������Ʈ�� �̸��� ����Ͽ� ������Ʈ ����
                shared_ptr<Model> model = RESOURCES.Get<Model>(strObjectName);
                renderer->Set_Model(model);
            }
            //�ִϸ����� ������Ʈ
            CreateObject->Add_Component(renderer);
        }
        // 4-1 ����� + ������Ʈ���Ϳ� �߰�
        CUR_SCENE->Add_GameObject(CreateObject);
        m_pMapObjects.push_back(CreateObject);
        // 4-2. ���� ��ġ�� ������Ʈ �̸� UI�� �߰�
        WCHAR szTempName[MAX_PATH];
        lstrcpy(szTempName, strObjectName.c_str());
        // char �������� ��ȯ
        shared_ptr<char[]> pChar = shared_ptr<char[]>(new char[MAX_PATH]);
        WideCharToMultiByte(CP_ACP, 0, szTempName, -1, pChar.get(), MAX_PATH, 0, 0);
        m_strObjectNamePtr.push_back(pChar);
        m_strObjectName.push_back(pChar.get());
    }

    return S_OK;
}