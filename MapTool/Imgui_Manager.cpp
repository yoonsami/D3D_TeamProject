#include "pch.h"

#include "Imgui_Manager.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ImGuizmo.h"

#include "Utils.h"

#include "Model.h"
#include "ModelRenderer.h"

#include "Camera.h"

ImGuizmo::OPERATION m_eGuizmoType = { ImGuizmo::TRANSLATE };

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

    Frame_ObjectBase();
    Frame_Objects();
    
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
        _float4x4 matGuizmo = m_pMapObjects[m_iObjects].lock()->Get_Transform()->Get_WorldMatrix();

        ImGuizmo::Manipulate((float*)&matView,
            (float*)&matProj,
            m_eGuizmoType,
            ImGuizmo::LOCAL,
            (float*)&matGuizmo);

        m_pMapObjects[m_iObjects].lock()->Get_Transform()->Set_WorldMat(matGuizmo);
    }
}

void ImGui_Manager::Frame_ObjectBase()
{
    ImGui::Begin("Frame_ObjectBase"); // ���� ������

    ImGui::ListBox("ObjectBase", &m_iObjectBaseIndex, m_strObjectBaseName.data(), m_strObjectBaseName.size(), 10);

    if (ImGui::Button("Create"))
        if (FAILED(Create_MapObject()))
            MSG_BOX("Fail : Create_MapObject");

    ImGui::End();
}

void ImGui_Manager::Frame_Objects()
{
    ImGui::Begin("Frame_Objects"); // ���� ������

    ImGui::ListBox("Objects", &m_iObjects, m_strObjectName.data(), m_strObjectName.size(), 10);

    ImGui::End();
}

HRESULT ImGui_Manager::Load_MapObjectBase()
{
    // �ϴ� �ӽ÷� �ϳ� �־����. 
    m_strObjectBaseName.push_back("Catacomb");

    return S_OK;
}

HRESULT ImGui_Manager::Create_MapObject()
{
    // 1. ���� ���õ� ���̽�������Ʈ�̸� ��������
    wstring strCurObjBase = Utils::ToWString(m_strObjectBaseName[m_iObjectBaseIndex]);

    // 2. ���ӿ�����Ʈ �����Ҵ��Ͽ� ������� �߰�
    shared_ptr<GameObject> testObj = make_shared<GameObject>();
    testObj->Add_Component(make_shared<Transform>());
    testObj->Get_Transform()->Set_State(Transform_State::POS, _float4(0.f, 0.f, 0.f, 1.f));
    {
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
        shared_ptr<ModelRenderer> animator = make_shared<ModelRenderer>(shader);
        {
            // ���̽� ������Ʈ�� �̸��� ����Ͽ� ������Ʈ ����
            shared_ptr<Model> model = RESOURCES.Get<Model>(strCurObjBase);
            animator->Set_Model(model);
        }
        //�ִϸ����� ������Ʈ
        testObj->Add_Component(animator);
    }
    CUR_SCENE->Add_GameObject(testObj);
    //weak_ptr<GameObject> tempObject = testObj;
    m_pMapObjects.push_back(testObj);

    // 3. ���� ��ġ�� ������Ʈ ��Ͽ� �߰�
    m_strObjectName.push_back(m_strObjectBaseName[m_iObjectBaseIndex]);

    return S_OK;
}
