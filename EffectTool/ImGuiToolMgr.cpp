#include "pch.h"
#include "ImGuiToolMgr.h"

/* Widgets */
#include "Widget_Model_Controller.h"
#include "Widget_EffectMaker_Mesh.h"
#include "Widget_ParticleMaker_Instancing.h"
#include "Widget_GroupEffectMaker.h"

ImGuiToolMgr::~ImGuiToolMgr()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiToolMgr::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(DEVICE.Get(), CONTEXT.Get());

    ImGuizmo::Enable(true);
    ImGuizmo::SetRect(0.f, 0.f, g_iWinSizeX, g_iWinSizeY);

    // For. Setting Widgets
    m_pWidget_ParticleMaker_Instancing = make_shared<Widget_ParticleMaker_Instancing>();
    m_pWidget_EffectMaker_Mesh = make_shared<Widget_EffectMaker_Mesh>();
    m_pWidget_Model_Controller = make_shared<Widget_Model_Controller>();
    m_pWidget_GroupEffectMaker = make_shared<Widget_GroupEffectMaker>();

    m_pWidget_ParticleMaker_Instancing->Initialize();
    m_pWidget_EffectMaker_Mesh->Initialize();
    m_pWidget_Model_Controller->Initialize();
    m_pWidget_GroupEffectMaker->Initialize();
}

void ImGuiToolMgr::Tick()
{
    // For. Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    ImGuizmo::BeginFrame();

    // Start guizmo
    ImGuizmo::BeginFrame();

    // For. Basic Widget
    ImGui::SetNextWindowPos(ImVec2(0, g_iWinSizeY - 60.f));
    ImGui::SetNextWindowSize(ImVec2(600.f, 100.f));
    ImGui::Begin("Basic Widget");
    ImGui_BasicWidget();
    ImGui::End();

    if (m_bIsParticleMaker_Instancing_On)
        m_pWidget_ParticleMaker_Instancing->Tick();
    if (m_bIsEffectMaker_Mesh_On)
        m_pWidget_EffectMaker_Mesh->Tick();
    if (m_bIsGroupEffectMaker_On)
        m_pWidget_GroupEffectMaker->Tick();

    m_pWidget_Model_Controller->Tick();
}

void ImGuiToolMgr::Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiToolMgr::ImGui_BasicWidget()
{
    Option_ToolSelector();    
}

void ImGuiToolMgr::Option_ToolSelector()
{
    ImGui::Checkbox("EffectMaker(Mesh)", &m_bIsEffectMaker_Mesh_On);
    ImGui::SameLine();
    ImGui::Checkbox("ParticleMaker(Instancing)", &m_bIsParticleMaker_Instancing_On);
    ImGui::SameLine();
    ImGui::Checkbox("GroupEffectMaker", &m_bIsGroupEffectMaker_On);
    ImGui::SameLine();
    string strFPS = "FPS: " + to_string(TIME.GetFPS());
    ImGui::Text(strFPS.c_str());
}