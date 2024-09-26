#include "pch.h"
#include "ImguiMgr.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Utils.h"
#include "BaseUI.h"
#include "FileUtils.h"
#include "Camera.h"
#include "Texture.h"
#include "FontRenderer.h"
#include "CustomFont.h"

ImguiMgr::~ImguiMgr()
{
   ImGui_ImplDX11_Shutdown();
   ImGui_ImplWin32_Shutdown();
   ImGui::DestroyContext();
}

HRESULT ImguiMgr::Initialize(HWND& hWnd)
{
   IMGUI_CHECKVERSION();
   ImGui::CreateContext();
   ImGuiIO& io = ImGui::GetIO();
   io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
   io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 15.0f, NULL, io.Fonts->GetGlyphRangesKorean());
   
   ImGui_ImplWin32_Init(hWnd);
   ImGui_ImplDX11_Init(DEVICE.Get(), CONTEXT.Get());

   ImGuizmo::Enable(true);
   ImGuizmo::SetRect(0.f, 0.f, g_iWinSizeX, g_iWinSizeY);

   m_pSampleObj = make_shared<GameObject>();
   m_pSampleObj->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(1.f, 1.f, 1.f, 1.f));
   m_pSampleObj->GetOrAddTransform()->Scaled(_float3(1.f, 1.f, 1.f));

   m_tagParamDesc.vec4Params[0] = _float4(1.f, 1.f, 1.f, 1.f);

   // 폰트 태그 wstring 추가
   m_vecFontType.push_back(L"136ex");
   m_vecFontType.push_back(L"Ori_136ex");
   m_vecFontType.push_back(L"overwatch");
   m_vecFontType.push_back(L"Soyo_Maple");



   return S_OK;
}

void ImguiMgr::Tick()
{
   ImGui_ImplDX11_NewFrame();
   ImGui_ImplWin32_NewFrame();
   ImGui::NewFrame();
   ImGuizmo::BeginFrame();

   //ImGui::ShowDemoWindow(); // Show demo window! :)


   Key_Input();

   Create_Ui();

   Manapulate_Ui();

   Parameter_Ui();

   Font_Ui();


}

HRESULT ImguiMgr::Render()
{
   ImGui::Render();
   ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

   return S_OK;
}

void ImguiMgr::Key_Input()
{
   // F1 : on/off Create Tool
   if (KEYTAP(KEY_TYPE::F1))
      m_bRender_Create_Ui = !m_bRender_Create_Ui;

   // F2 : on/off Manapulate Tool
   if (KEYTAP(KEY_TYPE::F2))
      m_bRender_Manapulate_Ui = !m_bRender_Manapulate_Ui;

   // F3 : on/off Parameter Tool
   if (KEYTAP(KEY_TYPE::F3))
      m_bRender_Param_Ui = !m_bRender_Param_Ui;

   // F4 : on/off Font Tool
   if (KEYTAP(KEY_TYPE::F4))
      m_bRender_Font_Ui = !m_bRender_Font_Ui;


}

void ImguiMgr::Create_Ui()
{
   if (false == m_bRender_Create_Ui)
      return;

   ImGui::Begin("Create Ui");


   Create_Object();
   File_Dialog();
   Select_Texture();
   Add_Texture();
   // Decide_Transform_Value();

   // 경로 추출, 저장, 로드 다 포함
   Save_Ui_Desc();


   ImGui::End();
}

void ImguiMgr::Manapulate_Ui()
{
   if (false == m_bRender_Manapulate_Ui)
      return;

   ImGui::Begin("Manapulate Ui");


   Select_Object();
   Change_Object_Value();
   Add_Picking_Zone();
   Record_Two_Point();


   Delete_Object();


   ImGui::End();
}

void ImguiMgr::Parameter_Ui()
{
   if (false == m_bRender_Param_Ui)
      return;

   ImGui::Begin("Parameter Ui");


   Change_ParamDesc();


   ImGui::End();
}

void ImguiMgr::Font_Ui()
{
   if (false == m_bRender_Font_Ui)
      return;

   ImGui::Begin("Font Ui");

   Input_Words();
   Select_Font_Value();
   Set_Font();

   ImGui::End();
}

void ImguiMgr::File_Dialog()
{
   ImGui::NewLine();
   ImGui::SeparatorText("File Dialog");

   if (ImGui::Button("Open File Dialog"))
      ImGuiFileDialog::Instance()->OpenDialog("ddsFileDlgKey", "Add File Dialog", ".dds,.tga,.png", ".");

   ImGui::BulletText("You can add dds, tag, png");

   if (ImGuiFileDialog::Instance()->Display("ddsFileDlgKey"))
   {
      if (ImGuiFileDialog::Instance()->IsOk())
      {
         string strPath = ImGuiFileDialog::Instance()->GetFilePathName();
         string strFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();

         wstring path = Utils::ToWString(strPath);
         wstring FileName = Utils::ToWString(strFileName);

         size_t pos = path.find(L"Resources");
         if (pos != std::wstring::npos) {
            path.erase(0, pos);
         }
         path = L"..\\" + path;

         size_t dotPos = FileName.rfind(L'.');
         if (dotPos != std::wstring::npos) {
            FileName.erase(dotPos);
         }

         RESOURCES.Load<Texture>(FileName, path);
      }

      ImGuiFileDialog::Instance()->Close();
   }
}

void ImguiMgr::Select_Texture()
{
   ImGui::NewLine();
   ImGui::SeparatorText("Select Texture");

   auto& textures = RESOURCES.Get_Resources(0);
   auto& texture = textures[static_cast<_uint>(ResourceType::Texture)];

   _uint iSize = static_cast<_uint>(texture.size());
   _uint iIndex = 0;

   char inputBuffer[256] = {};
   if (ImGui::InputText("##Search Texture Name", inputBuffer, sizeof(inputBuffer))) {
      string str(inputBuffer);
      m_strSearchTexture = Utils::ToWString(str);
   }

   if (ImGui::BeginListBox("##Texture Name"))
   {
      for (const auto& pair : texture) {

         if (pair.first.find(m_strSearchTexture) == std::wstring::npos) {
            continue;
         }

         string key = Utils::ToString(pair.first);

         const bool is_selected = (iIndex == iSize);

         if (ImGui::Selectable(key.c_str(), iIndex == m_iTextureCursor))
         {
            m_iTextureCursor = iIndex;
            m_strKeyTexture = pair.first;
            m_strSearchTexture = L"";
         }

         ++iIndex;
      }

      ImGui::EndListBox();
   }

   if (0 != m_strKeyTexture.length())
   {
      ImGui::Begin("Texture Ui");
      auto pTexture = static_pointer_cast<Texture>(texture.find(m_strKeyTexture)->second);
      ImGui::Text("Texture Size X : %0.f\t Texture Size Y : %0.f", pTexture->Get_Size().x, pTexture->Get_Size().y);
      ImGui::Image((pTexture->Get_SRV().Get()), ImVec2(300, 300));
      ImGui::End();
   }

}

void ImguiMgr::Add_Texture()
{
   ImGui::NewLine();
   ImGui::SeparatorText("Selected Texture Name");

   string strName = Utils::ToString(m_strKeyTexture);
   if (ImGui::InputText("##Selected Texture Name", strName.data(), 256, ImGuiInputTextFlags_ReadOnly)) {
   }

   ImGui::Combo("##Texture Type Combo", &m_iTextureType, m_arrTextureType, IM_ARRAYSIZE(m_arrTextureType));

   if (ImGui::Button("Add Texture", ImVec2(80.f, 20.f)))
   {
      if (!ImGui::IsItemActive())
      {
         if (0 == m_strSelectObjName.length())
         {
            return;
         }

         auto pGameobject = CUR_SCENE->Get_GameObject(m_strSelectObjName);
         pGameobject->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(m_strKeyTexture), static_cast<TextureMapType>(m_iTextureType));
      }
   }

   ImGui::SameLine();
   if (ImGui::Button("Add Submap", ImVec2(80.f, 20.f)))
   {
      if (!ImGui::IsItemActive())
      {
         if (0 == m_strSelectObjName.length())
         {
            return;
         }

         auto pGameobject = CUR_SCENE->Get_GameObject(m_strSelectObjName);

         for (_uint i = 0; i < MAX_SUB_SRV_COUNT; ++i)
         {
            if (nullptr == pGameobject->Get_MeshRenderer()->Get_Material()->Get_SubMap(i))
            {
               pGameobject->Get_MeshRenderer()->Get_Material()->Set_SubMap(i, RESOURCES.Get<Texture>(m_strKeyTexture));
               break;
            }
         }

      }
   }
}

void ImguiMgr::Decide_Transform_Value()
{
   ImGui::NewLine();
   ImGui::SeparatorText("Position");
   ImGuiInputTextFlags_ eFlag = ImGuiInputTextFlags_CharsDecimal;

   ImGui::Text("X");
   ImGui::SameLine();
   ImGui::SetNextItemWidth(200);
   ImGui::InputFloat("##PosX1", &m_vecPos.x, 0.01f, 1.0f, "%.f", eFlag);

   ImGui::Text("Y");
   ImGui::SameLine();
   ImGui::SetNextItemWidth(200);
   ImGui::InputFloat("##PosY1", &m_vecPos.y, 0.01f, 1.0f, "%.f", eFlag);

   ImGui::Text("Z");
   ImGui::SameLine();
   ImGui::SetNextItemWidth(200);
   ImGui::InputFloat("##PosZ1", &m_vecPos.z, 0.01f, 1.0f, "%.f", eFlag);

   ImGui::NewLine();
   ImGui::SeparatorText("Scale");

   ImGui::Text("X");
   ImGui::SameLine();
   ImGui::SetNextItemWidth(200);
   ImGui::InputFloat("##ScaleX1", &m_vecScale.x, 0.01f, 1.0f, "%.3f", eFlag);

   ImGui::Text("Y");
   ImGui::SameLine();
   ImGui::SetNextItemWidth(200);
   ImGui::InputFloat("##ScaleY1", &m_vecScale.y, 0.01f, 1.0f, "%.3f", eFlag);

   ImGui::Text("Z");
   ImGui::SameLine();
   ImGui::SetNextItemWidth(200);
   ImGui::InputFloat("##ScaleZ1", &m_vecScale.z, 0.01f, 1.0f, "%.3f", eFlag);

   if (0.0001f > m_vecScale.x)
      m_vecScale.x = 0.0001f;
   if (0.0001f > m_vecScale.y)
      m_vecScale.y = 0.0001f;
   if (0.0001f > m_vecScale.z)
      m_vecScale.z = 0.0001f;

   ImGui::NewLine();
   ImGui::SetNextItemWidth(200);
}

void ImguiMgr::Create_Object()
{
   ImGui::SeparatorText("Input Object Name");

   char inputBuffer[256] = {};
   if (ImGui::InputText("##Object Name", inputBuffer, sizeof(inputBuffer))) {
      string str(inputBuffer);
      m_strName = Utils::ToWString(str);
   }

   if (0 != m_strSelectObjName.length())
   {
      auto pGameobject = CUR_SCENE->Get_GameObject(m_strSelectObjName);
      if (nullptr != pGameobject)
      {
         if (true == CUR_SCENE->Is_Static(pGameobject))
         {
            m_iSetStaticValue = 0;
         }
         else
         {
            m_iSetStaticValue = 1;
         }
      }
   }

   ImGui::RadioButton("Static", &m_iSetStaticValue, 0);
   ImGui::SameLine();
   ImGui::RadioButton("None", &m_iSetStaticValue, 1);
   ImGui::SameLine();

   if (ImGui::Button("Create Ui", ImVec2(80.f, 20.f)))
   {
      if (!ImGui::IsItemActive())
      {
         auto UiObject = make_shared<GameObject>();
         UiObject->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(0.f, 0.f, 0.f, 1));
         UiObject->GetOrAddTransform()->Scaled(_float3(0.001f, 0.001f, 0.001f));
         shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Mesh.fx"));

         auto mesh = RESOURCES.Get<Mesh>(L"Point");
         renderer->Set_Mesh(mesh);

         auto material = make_shared<Material>();
         renderer->Get_RenderParamDesc().SetVec4(0, _float4(1));
         renderer->Set_Material(material);
         renderer->Set_Pass(MeshRenderer::PASS_INFO::Default_UI);
         UiObject->Add_Component(renderer);

         UiObject->Set_Name(m_strName);
         m_GameobjectName.push_back(m_strName);

         UiObject->Set_LayerIndex(Layer_UI);
         UiObject->Set_Instancing(false);

         if(0 == m_iSetStaticValue)
            EVENTMGR.Create_Object(UiObject, true);
         else
            EVENTMGR.Create_Object(UiObject, false);
      }
   }
}

void ImguiMgr::Select_Object()
{
   ImGui::SeparatorText("Select Object");

   _uint iSize = static_cast<_uint>(m_GameobjectName.size());
   _uint iIndex = 0;

   if (ImGui::BeginListBox("##Object Name"))
   {
      for (const auto& Name : m_GameobjectName) {
         string key = Utils::ToString(Name);

         const bool is_selected = (iIndex == iSize);

         if (ImGui::Selectable(key.c_str(), iIndex == m_iObjNameCursor))
         {
            m_iObjNameCursor = iIndex;
            m_strSelectObjName = Name;

            if (0 == m_strSelectObjName.length())
               break;

            auto pGameobject = CUR_SCENE->Get_GameObject(m_strSelectObjName);
            if (nullptr == pGameobject)
               break;

            m_iPass = pGameobject->Get_MeshRenderer()->Get_Pass();
            m_tagParamDesc = pGameobject->Get_MeshRenderer()->Get_RenderParamDesc();
            m_pSampleObj->GetOrAddTransform()->Set_WorldMat(pGameobject->GetOrAddTransform()->Get_WorldMatrix());

            if(nullptr != pGameobject->Get_FontRenderer())
            {
               m_fFontSize = pGameobject->Get_FontRenderer()->Get_Size();

               _float4 vColor = pGameobject->Get_FontRenderer()->Get_Color();

               m_arrColors[0] = vColor.x;
               m_arrColors[1] = vColor.y;
               m_arrColors[2] = vColor.z;
               m_arrColors[3] = vColor.w;
            }
         }

         ++iIndex;
      }

      ImGui::EndListBox();
   }

   if (ImGui::TreeNode("Textures"))
   {
      if (0 != m_strSelectObjName.length())
      {
         auto pGameobject = CUR_SCENE->Get_GameObject(m_strSelectObjName);

         for (_uint i = 0; i < MAX_TEXTURE_MAP_COUONT; ++i)
         {
            string strButtonName = "Delete##" + to_string(i);
            if (ImGui::Button(strButtonName.c_str(), ImVec2(50.f, 20.f)))
            {
               if (!ImGui::IsItemActive())
               {
                  pGameobject->Get_MeshRenderer()->Get_Material()->Set_TextureMap(nullptr, static_cast<TextureMapType>(i));
               }
            }

            ImGui::SameLine();
            ImGui::BulletText("%s : ", m_arrTextureType[i]);

            ImGui::SameLine();
            ImVec2 CursorPos = ImGui::GetCursorPos();
            CursorPos.x = 195.f;
            ImGui::SetCursorPos(CursorPos);

            if (nullptr != pGameobject->Get_MeshRenderer()->Get_Material()->Get_TextureMap(static_cast<TextureMapType>(i)))
               ImGui::Text("%s", Utils::ToString(pGameobject->Get_MeshRenderer()->Get_Material()->Get_TextureMap(static_cast<TextureMapType>(i))->Get_Name()).c_str());
            else
               ImGui::Text("None");
         }
      }

      ImGui::TreePop();
   }

   if (ImGui::TreeNode("Submaps"))
   {
      if (0 != m_strSelectObjName.length())
      {
         auto pGameobject = CUR_SCENE->Get_GameObject(m_strSelectObjName);

         for (_uint i = 0; i < MAX_SUB_SRV_COUNT; ++i)
         {
            string strButtonName = "Delete##" + to_string(i);
            if (ImGui::Button(strButtonName.c_str(), ImVec2(50.f, 20.f)))
            {
               if (!ImGui::IsItemActive())
               {
                  pGameobject->Get_MeshRenderer()->Get_Material()->Set_SubMap(i, nullptr);
               }
            }

            ImGui::SameLine();
            ImGui::BulletText("Submap%d : ", i);

            ImGui::SameLine();
            ImVec2 CursorPos = ImGui::GetCursorPos();
            CursorPos.x = 180.f;
            ImGui::SetCursorPos(CursorPos);

            if (nullptr != pGameobject->Get_MeshRenderer()->Get_Material()->Get_SubMap(i))
               ImGui::Text("%s", Utils::ToString(pGameobject->Get_MeshRenderer()->Get_Material()->Get_SubMap(i)->Get_Name()).c_str());
            else
               ImGui::Text("None");
         }
      }
      ImGui::TreePop();
   }
}

void ImguiMgr::Delete_Object()
{
   ImGui::NewLine();
   ImGui::SeparatorText("Delete Object");

   if (ImGui::Button("Delete", ImVec2(80.f, 20.f)))
   {
      if (!ImGui::IsItemActive())
      {
         if (0 == m_strSelectObjName.length())
         {
            return;
         }

         wstring strName = m_strSelectObjName;
         for (auto iter = m_GameobjectName.begin();
            iter != m_GameobjectName.end();
            )
         {
            if (*iter == m_strSelectObjName)
            {
               iter = m_GameobjectName.erase(iter);
               break;
            }
            else
            {
               ++iter;
            }
         }

         auto pGameobject = CUR_SCENE->Get_GameObject(strName);
         if (nullptr == pGameobject)
         {
            return;
         }

         EVENTMGR.Delete_Object(pGameobject);

         m_strSelectObjName = L"";
      }
   }

   ImGui::SameLine();
   if (ImGui::Button("Delete All", ImVec2(80.f, 20.f)))
   {
      if (!ImGui::IsItemActive())
      {
         if (0 == m_GameobjectName.size())
            return;

         wstring strName = m_strSelectObjName;
         for (auto iter = m_GameobjectName.begin();
            iter != m_GameobjectName.end();
            )
         {
            wstring strTemp = *iter;
            if (0 == strTemp.length())
            {
               ++iter;
               continue;
            }
               
            iter = m_GameobjectName.erase(iter);
            auto pGameobject = CUR_SCENE->Get_GameObject(strTemp);
            if (nullptr == pGameobject)
            {
               return;
            }

            EVENTMGR.Delete_Object(pGameobject);
         }

         

         m_strSelectObjName = L"";
      }
   }
}

void ImguiMgr::Change_Object_Value()
{
   ImGuiInputTextFlags_ eFlag = ImGuiInputTextFlags_CharsDecimal;

   ImGui::NewLine();
   ImGui::SeparatorText("Control");

   ImGui::RadioButton("Trans", &m_iTypeZmoOper, 0);
   ImGui::RadioButton("Rotate", &m_iTypeZmoOper, 1);
   ImGui::RadioButton("Scale", &m_iTypeZmoOper, 2);
   ImGui::RadioButton("Off", &m_iTypeZmoOper, 3);

   if (0 == m_iTypeZmoOper)
      m_eZmoOper = ImGuizmo::OPERATION::TRANSLATE;
   else if (1 == m_iTypeZmoOper)
      m_eZmoOper = ImGuizmo::OPERATION::ROTATE;
   else if (2 == m_iTypeZmoOper)
      m_eZmoOper = ImGuizmo::OPERATION::SCALE;

   ImGui::NewLine();
   ImGui::SeparatorText("Position");

   _float4 vecPos = m_pSampleObj->GetOrAddTransform()->Get_State(Transform_State::POS);
   _float3 vecScale = m_pSampleObj->GetOrAddTransform()->Get_Scale();

   ImGui::Text("X");
   ImGui::SameLine();
   ImGui::SetNextItemWidth(200);
   ImGui::InputFloat("##PosX2", &vecPos.x, 0.01f, 1.0f, "%.3f", eFlag);

   ImGui::Text("Y");
   ImGui::SameLine();
   ImGui::SetNextItemWidth(200);
   ImGui::InputFloat("##PosY2", &vecPos.y, 0.01f, 1.0f, "%.3f", eFlag);

   ImGui::Text("Z");
   ImGui::SameLine();
   ImGui::SetNextItemWidth(200);
   ImGui::InputFloat("##PosZ2", &vecPos.z, 1.f, 1.0f, "%.3f", eFlag);

   ImGui::NewLine();
   ImGui::SeparatorText("Scale");

   ImGui::Text("X");
   ImGui::SameLine();
   ImGui::SetNextItemWidth(200);
   ImGui::InputFloat("##ScaleX2", &vecScale.x, 0.01f, 1.0f, "%.3f", eFlag);

   ImGui::Text("Y");
   ImGui::SameLine();
   ImGui::SetNextItemWidth(200);
   ImGui::InputFloat("##ScaleY2", &vecScale.y, 0.01f, 1.0f, "%.3f", eFlag);

   ImGui::Text("Z");
   ImGui::SameLine();
   ImGui::SetNextItemWidth(200);
   ImGui::InputFloat("##ScaleZ2", &vecScale.z, 0.01f, 1.0f, "%.3f", eFlag);

   if (0.001f > m_vecScale.x)
      m_vecScale.x = 0.001f;
   if (0.001f > m_vecScale.y)
      m_vecScale.y = 0.001f;
   if (0.001f > m_vecScale.z)
      m_vecScale.z = 0.001f;

   m_pSampleObj->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
   m_pSampleObj->GetOrAddTransform()->Scaled(vecScale);

   _bool bIsSelected = true;
   if (0 == m_strSelectObjName.length())
   {
      bIsSelected = false;
      m_bIsChangeCheck = false;
   }

   if (ImGui::Button("Change##Pos_Scale", ImVec2(80.f, 20.f)) || m_bIsChangeCheck)
   {
      if (!ImGui::IsItemActive() || m_bIsChangeCheck)
      {
         if (false == bIsSelected)
         {
            return;
         }

         auto pGameobject = CUR_SCENE->Get_GameObject(m_strSelectObjName);
         if (nullptr == pGameobject)
         {
            return;
         }

         auto transform = pGameobject->Get_Transform();

         transform->Set_WorldMat(m_pSampleObj->GetOrAddTransform()->Get_WorldMatrix());
      }
   }

   ImGui::SameLine();
   ImGui::Checkbox("Immediately", &m_bIsChangeCheck);

   if (true == bIsSelected && 3 != m_iTypeZmoOper)
   {
      _float4x4 matWorld = {};
      memcpy(&matWorld, &m_pSampleObj->GetOrAddTransform()->Get_WorldMatrix(), sizeof(_float4x4));

      auto pUiCamera = CUR_SCENE->Get_UICamera()->Get_Camera();
      _float4x4 matView = pUiCamera->Get_ViewMat();
      _float4x4 matProj = pUiCamera->Get_ProjMat();

      ImGuizmo::Manipulate(
         matView.m[0],
         matProj.m[0],
         m_eZmoOper,
         ImGuizmo::WORLD,
         matWorld.m[0]
      );

      m_pSampleObj->GetOrAddTransform()->Set_WorldMat(matWorld);
   }

}

void ImguiMgr::Change_ParamDesc()
{
   ImGuiInputTextFlags_ eFlag = ImGuiInputTextFlags_CharsDecimal;

   ImGui::SeparatorText("Pass Index");
   ImGui::SetNextItemWidth(200);
   ImGui::InputInt("##int pass", &m_iPass, 1, 1, eFlag);

   ImGui::NewLine();
   ImGui::SeparatorText("Set Param");

   ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
   if (ImGui::BeginTabBar("##Set Param", tab_bar_flags))
   {
      if (ImGui::BeginTabItem("Param1"))
      {
         ImGui::BulletText("g_int_0");
         ImGui::SetNextItemWidth(200);
         ImGui::InputInt("##int param1", &m_iParam1, 1, 1, eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderInt("##int param1 slider", &m_iParam1, 0, 100, "Value = %d");

         ImGui::BulletText("g_int_1");
         ImGui::SetNextItemWidth(200);
         ImGui::InputInt("##int param2", &m_iParam2, 1, 1, eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderInt("##int param2 slider", &m_iParam2, 0, 100, "Value = %d");

         ImGui::BulletText("g_int_2");
         ImGui::SetNextItemWidth(200);
         ImGui::InputInt("##int param3", &m_iParam3, 1, 1, eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderInt("##int param3 slider", &m_iParam3, 0, 100, "Value = %d");

         ImGui::BulletText("g_int_3");
         ImGui::SetNextItemWidth(200);
         ImGui::InputInt("##int param4", &m_iParam4, 1, 1, eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderInt("##int param4 slider", &m_iParam4, 0, 100, "Value = %d");

         m_tagParamDesc.intParams[0] = m_iParam1;
         m_tagParamDesc.intParams[1] = m_iParam2;
         m_tagParamDesc.intParams[2] = m_iParam3;
         m_tagParamDesc.intParams[3] = m_iParam4;

         ImGui::BulletText("g_float_0");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##float param1", &m_tagParamDesc.floatParams[0], 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##float param1 slider", &m_tagParamDesc.floatParams[0], 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_float_1");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##float param2", &m_tagParamDesc.floatParams[1], 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##float param2 slider", &m_tagParamDesc.floatParams[1], 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_float_2");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##float param3", &m_tagParamDesc.floatParams[2], 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##float param3 slider", &m_tagParamDesc.floatParams[2], 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_float_3");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##float param4", &m_tagParamDesc.floatParams[3], 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##float param4 slider", &m_tagParamDesc.floatParams[3], 0.0f, 100.0f, "Value = %.3f");

         ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Param2"))
      {
         ImGui::BulletText("g_vec2_0 x");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec2_0 x", &m_tagParamDesc.vec2Params[0].x, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec2_0 x slider", &m_tagParamDesc.vec2Params[0].x, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec2_0 y");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec2_0 y", &m_tagParamDesc.vec2Params[0].y, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec2_0 y slider", &m_tagParamDesc.vec2Params[0].y, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec2_1 x");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec2_1 x", &m_tagParamDesc.vec2Params[1].x, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec2_1 x slider", &m_tagParamDesc.vec2Params[1].x, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec2_1 y");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec2_1 y", &m_tagParamDesc.vec2Params[1].y, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec2_1 y slider", &m_tagParamDesc.vec2Params[1].y, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec2_2 x");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec2_2 x", &m_tagParamDesc.vec2Params[2].x, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec2_2 x slider", &m_tagParamDesc.vec2Params[2].x, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec2_2 y");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec2_2 y", &m_tagParamDesc.vec2Params[2].y, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec2_2 y slider", &m_tagParamDesc.vec2Params[2].y, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec2_3 x");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec2_3 x", &m_tagParamDesc.vec2Params[3].x, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec2_3 x slider", &m_tagParamDesc.vec2Params[3].x, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec2_3 y");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec2_3 y", &m_tagParamDesc.vec2Params[3].y, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec2_3 y slider", &m_tagParamDesc.vec2Params[3].y, 0.0f, 100.0f, "Value = %.3f");

         ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Param3"))
      {
         ImGui::BulletText("g_vec4_0 x");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_0 x", &m_tagParamDesc.vec4Params[0].x, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_0 x slider", &m_tagParamDesc.vec4Params[0].x, 0.0f, 1.f, "Value = %.3f");

         ImGui::BulletText("g_vec4_0 y");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_0 y", &m_tagParamDesc.vec4Params[0].y, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_0 y slider", &m_tagParamDesc.vec4Params[0].y, 0.0f, 1.f, "Value = %.3f");

         ImGui::BulletText("g_vec4_0 z");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_0 z", &m_tagParamDesc.vec4Params[0].z, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_0 z slider", &m_tagParamDesc.vec4Params[0].z, 0.0f, 1.f, "Value = %.3f");

         ImGui::BulletText("g_vec4_0 w");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_0 w", &m_tagParamDesc.vec4Params[0].w, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_0 w slider", &m_tagParamDesc.vec4Params[0].w, 0.0f, 1.f, "Value = %.3f");

         ImGui::BulletText("g_vec4_1 x");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_1 x", &m_tagParamDesc.vec4Params[1].x, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_1 x slider", &m_tagParamDesc.vec4Params[1].x, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec4_1 y");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_1 y", &m_tagParamDesc.vec4Params[1].y, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_1 y slider", &m_tagParamDesc.vec4Params[1].y, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec4_1 z");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_1 z", &m_tagParamDesc.vec4Params[1].z, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_1 z slider", &m_tagParamDesc.vec4Params[1].z, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec4_1 w");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_1 w", &m_tagParamDesc.vec4Params[1].w, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_1 w slider", &m_tagParamDesc.vec4Params[1].w, 0.0f, 100.0f, "Value = %.3f");

         ImGui::EndTabItem();
      }
      if (ImGui::BeginTabItem("Param4"))
      {
         ImGui::BulletText("g_vec4_3 x");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_3 x", &m_tagParamDesc.vec4Params[2].x, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_3 x slider", &m_tagParamDesc.vec4Params[2].x, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec4_3 y");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_3 y", &m_tagParamDesc.vec4Params[2].y, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_3 y slider", &m_tagParamDesc.vec4Params[2].y, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec4_3 z");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_3 z", &m_tagParamDesc.vec4Params[2].z, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_3 z slider", &m_tagParamDesc.vec4Params[2].z, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec4_3 w");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_3 w", &m_tagParamDesc.vec4Params[2].w, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_3 w slider", &m_tagParamDesc.vec4Params[2].w, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec4_4 x");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_4 x", &m_tagParamDesc.vec4Params[3].x, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_4 x slider", &m_tagParamDesc.vec4Params[3].x, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec4_4 y");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_4 y", &m_tagParamDesc.vec4Params[3].y, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_4 y slider", &m_tagParamDesc.vec4Params[3].y, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec4_4 z");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_4 z", &m_tagParamDesc.vec4Params[3].z, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_4 z slider", &m_tagParamDesc.vec4Params[3].z, 0.0f, 100.0f, "Value = %.3f");

         ImGui::BulletText("g_vec4_4 w");
         ImGui::SetNextItemWidth(200);
         ImGui::InputFloat("##g_vec4_4 w", &m_tagParamDesc.vec4Params[3].w, 0.01f, 1.0f, "%.3f", eFlag);
         ImGui::SetNextItemWidth(200);
         ImGui::SliderFloat("##g_vec4_4 w slider", &m_tagParamDesc.vec4Params[3].w, 0.0f, 100.0f, "Value = %.3f");

         ImGui::EndTabItem();
      }
      ImGui::EndTabBar();
   }

   if (0 == m_strSelectObjName.length())
   {
      return;
   }

   auto pGameobject = CUR_SCENE->Get_GameObject(m_strSelectObjName);
   if (nullptr == pGameobject)
   {
      return;
   }

   auto pRenderer = pGameobject->Get_MeshRenderer();
   pRenderer->Set_Pass(m_iPass);
   pRenderer->Get_RenderParamDesc() = m_tagParamDesc;
}

void ImguiMgr::Add_Picking_Zone()
{
   ImGui::NewLine();
   ImGui::SeparatorText("Add Picking Zone");
   ImGuiInputTextFlags_ eFlag = ImGuiInputTextFlags_CharsDecimal;

   ImGui::RadioButton("Rect", &m_iPickTypeIndex, 0);
   ImGui::SameLine();
   ImGui::RadioButton("Circle", &m_iPickTypeIndex, 1);

   ImGui::SameLine();
   ImGui::Spacing();
   ImGui::SameLine();
   if (ImGui::Button("Record", ImVec2(60.f, 20.f)))
   {
      if (!ImGui::IsItemActive())
      {
         m_bIsRecordStart = true;
         m_ptPos1 = {};
         m_ptPos2 = {};
         m_iRecordOrder = 0;
      }
   }

   ImGui::BulletText("Press the record button and follow the instructions");
   ImGui::BulletText("Rect : Click on the top left corner first. \n\t    Then, click on the bottom right corner.");
   ImGui::BulletText("Circle : Click on the center first. \n\t\t  Then, click on a point anywhere in the circle");

   ImGui::NewLine();
   ImGui::SeparatorText("Mouse Position");

   POINT ptPos = INPUT.GetMousePosToPoint();
   ImGui::Text("Mouse pos: (%d, %d)", ptPos.x, ptPos.y);
   ImGui::Text("First Pick Pos: (%d, %d)", m_ptPos1.x, m_ptPos1.y);
   ImGui::Text("Second Pick Pos: (%d, %d)", m_ptPos2.x, m_ptPos2.y);


   if (ImGui::Button("Add Picking Zone##add base ui", ImVec2(100.f, 20.f)))
   {
      if (!ImGui::IsItemActive())
      {
         if (0 == m_strSelectObjName.length())
         {
            return;
         }

         auto pGameobject = CUR_SCENE->Get_GameObject(m_strSelectObjName);
         if (nullptr == pGameobject)
         {
            return;
         }

         shared_ptr<BaseUI> pBaseUi;
         if (nullptr != pGameobject->Get_Button())
         {
            pBaseUi = pGameobject->Get_Button();

            switch (m_iPickTypeIndex)
            {
            case 0:
               pBaseUi->Create(BaseUI::PICK_TYPE::RECT, m_ptPos1, m_ptPos2);
               break;
            case 1:
               pBaseUi->Create(BaseUI::PICK_TYPE::CIRCLE, m_ptPos1, m_ptPos2);
               break;
            }
         }
         else
         {
            pBaseUi = make_shared<BaseUI>();

            switch (m_iPickTypeIndex)
            {
            case 0:
               pBaseUi->Create(BaseUI::PICK_TYPE::RECT, m_ptPos1, m_ptPos2);
               break;
            case 1:
               pBaseUi->Create(BaseUI::PICK_TYPE::CIRCLE, m_ptPos1, m_ptPos2);
               break;
            }

            pGameobject->Add_Component(pBaseUi);
         }

         m_ptPos1 = {};
         m_ptPos2 = {};
      }
   }


}

void ImguiMgr::Decide_FilePath()
{
   if (ImGui::Button("File Path Dialog##save load file"))
      ImGuiFileDialog::Instance()->OpenDialog("File Path", "Get File", ".dat", ".");

   if (ImGuiFileDialog::Instance()->Display("File Path"))
   {
      if (ImGuiFileDialog::Instance()->IsOk())
      {
         string strPath = ImGuiFileDialog::Instance()->GetFilePathName();
         string strFileName = ImGuiFileDialog::Instance()->GetCurrentFileName();

         m_strSaveLoadPath = Utils::ToWString(strPath);
         m_strSaveFileName = Utils::ToWString(strFileName);

         size_t pos = m_strSaveLoadPath.find(L"Resources");
         if (pos != std::wstring::npos) {
            m_strSaveLoadPath.erase(0, pos);
         }
         m_strSaveLoadPath = L"..\\" + m_strSaveLoadPath;

         pos = m_strSaveLoadPath.rfind(m_strSaveFileName);
         if (pos != std::wstring::npos) {
            m_strSaveLoadPath = m_strSaveLoadPath.substr(0, pos);
         }

      }

      ImGuiFileDialog::Instance()->Close();
   }
}

void ImguiMgr::Save_Ui_Desc()
{
   ImGui::NewLine();
   ImGui::SeparatorText("Save and Load");

   Decide_FilePath();

   ImGui::SameLine();
   if (ImGui::Button("Save##Save button", ImVec2(60.f, 20.f)))
   {
      if (!ImGui::IsItemActive())
      {
         if (0 == m_strSaveLoadPath.length() && 0 == m_strSaveFileName.length())
         {
            return;
         }

         shared_ptr<FileUtils> file = make_shared<FileUtils>();
         file->Open(m_strSaveLoadPath + m_strSaveFileName, FileMode::Write);

         // create ojbect count
         file->Write<_uint>(static_cast<_uint>(m_GameobjectName.size()));

         for (auto iter = m_GameobjectName.begin();
            iter != m_GameobjectName.end();
            ++iter)
         {
            auto pGameobject = CUR_SCENE->Get_GameObject(*iter);

            // object name
            file->Write<string>(Utils::ToString(*iter));

            // texture name
            for (_uint i = 0; i < MAX_TEXTURE_MAP_COUONT; ++i)
            {
               if (nullptr == pGameobject->Get_MeshRenderer()->Get_Material()->Get_TextureMap(static_cast<TextureMapType>(i)))
               {
                  file->Write<_bool>(false);
               }
               else
               {
                  file->Write<_bool>(true);
                  file->Write<string>(Utils::ToString(pGameobject->Get_MeshRenderer()->Get_Material()->Get_TextureMap(static_cast<TextureMapType>(i))->Get_Name()));
               }
            }

            // submap name
            for (_uint i = 0; i < MAX_SUB_SRV_COUNT; ++i)
            {
               if (nullptr == pGameobject->Get_MeshRenderer()->Get_Material()->Get_SubMap(i))
               {
                  file->Write<_bool>(false);
               }
               else
               {
                  file->Write<_bool>(true);
                  file->Write<string>(Utils::ToString(pGameobject->Get_MeshRenderer()->Get_Material()->Get_SubMap(i)->Get_Name()));
               }
            }

            // worldmatrix
            file->Write<_float4x4>(pGameobject->GetOrAddTransform()->Get_WorldMatrix());

            // pass index
            file->Write<_uint>(pGameobject->Get_MeshRenderer()->Get_Pass());

            // renderer param
            file->Write<RenderParams>(pGameobject->Get_MeshRenderer()->Get_RenderParamDesc());

            // base ui type, value
            if (nullptr == pGameobject->Get_Button())
            {
               file->Write<_bool>(false);
            }
            else
            {
               file->Write<_bool>(true);
               BaseUI::BASEUIDESC tagDesc = pGameobject->Get_Button()->Get_Desc();
               file->Write<BaseUI::BASEUIDESC>(tagDesc);
            }

            // Font Renderer
            if (nullptr == pGameobject->Get_FontRenderer())
            {
               file->Write<_bool>(false);
            }
            else
            {
               file->Write<_bool>(true);

               auto pFontRenderer = pGameobject->Get_FontRenderer();
               file->Write<string>(Utils::TostringUtf8(pFontRenderer->Get_Text()));
               string strTemp = Utils::ToString(pFontRenderer->Get_Font()->Get_Name());
               file->Write<string>(strTemp);
               Color vecColor = pFontRenderer->Get_Color();
               file->Write<Color>(vecColor);
               _float fSize = pFontRenderer->Get_Size();
               file->Write<_float>(fSize);
            }

            // is static
            _bool bIsStatic = CUR_SCENE->Is_Static(pGameobject);
            file->Write<_bool>(bIsStatic);
         }
      }
   }

   Load_Ui_Desc();

   ImGui::BulletText("Data File Path");
   string strName = Utils::ToString(m_strSaveLoadPath);
   if (ImGui::InputText("##Save Load Path", strName.data(), 256, ImGuiInputTextFlags_ReadOnly)) {
   }

   strName = Utils::ToString(m_strSaveFileName);
   char inputBuffer[256] = {};
   strncpy_s(inputBuffer, strName.c_str(), sizeof(inputBuffer) - 1);

   ImGui::BulletText("Data File Name");
   if (ImGui::InputText("##Save File Name", inputBuffer, sizeof(inputBuffer))) {
      string str(inputBuffer);
      m_strSaveFileName = Utils::ToWString(str);
   }
}

void ImguiMgr::Load_Ui_Desc()
{
   ImGui::SameLine();

   if (ImGui::Button("Load##Load button", ImVec2(60.f, 20.f)))
   {
      if (!ImGui::IsItemActive())
      {
         if (0 == m_strSaveLoadPath.length() && 0 == m_strSaveFileName.length())
         {
            return;
         }

         shared_ptr<FileUtils> file = make_shared<FileUtils>();
         file->Open(m_strSaveLoadPath + m_strSaveFileName, FileMode::Read);

         _uint iSize = file->Read<_uint>();

         for (_uint i = 0; i < iSize; ++i)
         {
            auto UiObject = make_shared<GameObject>();

            wstring strObjectName = Utils::ToWString(file->Read<string>());
            UiObject->Set_Name(strObjectName);
            m_GameobjectName.push_back(strObjectName);

            shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Mesh.fx"));
            auto mesh = RESOURCES.Get<Mesh>(L"Point");
            renderer->Set_Mesh(mesh);
            auto material = make_shared<Material>();

            for (_uint i = 0; i < MAX_TEXTURE_MAP_COUONT; ++i)
            {
               _bool bIsUseTexture = file->Read<_bool>();

               if (true == bIsUseTexture)
               {
                  material->Set_TextureMap(RESOURCES.Get<Texture>(Utils::ToWString(file->Read<string>())), static_cast<TextureMapType>(i));
               }
            }

            for (_uint i = 0; i < MAX_SUB_SRV_COUNT; ++i)
            {
               _bool bIsUseSubmap = file->Read<_bool>();

               if (true == bIsUseSubmap)
               {
                  material->Set_SubMap(i, RESOURCES.Get<Texture>(Utils::ToWString(file->Read<string>())));
               }
            }
            renderer->Set_Material(material);

            UiObject->GetOrAddTransform()->Set_WorldMat(file->Read<_float4x4>());
            renderer->Set_Pass(file->Read<_uint>());
            renderer->Get_RenderParamDesc() = file->Read<RenderParams>();
            UiObject->Add_Component(renderer);

            _bool bIsUseBaseUi = file->Read<_bool>();
            if (true == bIsUseBaseUi)
            {
               auto BaseUi = make_shared<BaseUI>();
               BaseUI::BASEUIDESC tagDesc = file->Read<BaseUI::BASEUIDESC>();
               BaseUi->Get_Desc() = tagDesc;
               UiObject->Add_Component(BaseUi);
            }

            _bool bIsUseFont = file->Read<_bool>();
            if (true == bIsUseFont)
            {
               UiObject->Add_Component(make_shared<FontRenderer>(Utils::ToWstringUtf8(file->Read<string>())));

               auto pFontRenderer = UiObject->Get_FontRenderer();
               wstring strTemp = Utils::ToWString(file->Read<string>());
               Color vecColor = file->Read<Color>();
               _float fSize = file->Read<_float>();
               pFontRenderer->Set_Font(RESOURCES.Get<CustomFont>(strTemp), vecColor, fSize);
            }

            UiObject->Set_LayerIndex(Layer_UI);
            UiObject->Set_Instancing(false);
            _bool bIsStatic = file->Read<_bool>();
            EVENTMGR.Create_Object(UiObject, bIsStatic);

         }
      }
   }

}

void ImguiMgr::Record_Two_Point()
{
   if (false == m_bIsRecordStart)
   {
      return;
   }

   if (0 == m_iRecordOrder)
   {
      if (true == KEYTAP(KEY_TYPE::LBUTTON))
      {
         ++m_iRecordOrder;

         m_ptPos1 = INPUT.GetMousePosToPoint();
      }
   }
   else if (1 == m_iRecordOrder)
   {
      if (true == KEYTAP(KEY_TYPE::LBUTTON))
      {
         ++m_iRecordOrder;

         m_ptPos2 = INPUT.GetMousePosToPoint();
      }
   }

}

void ImguiMgr::Input_Words()
{
    ImGui::SeparatorText("Input Words");

   char inputBuffer[256] = u8"";
   if (ImGui::InputText("##Input Words", inputBuffer, sizeof(inputBuffer))) {
      string str(inputBuffer);
      m_wstrFont = Utils::ToWstringUtf8(str);
   }
}

void ImguiMgr::Select_Font_Value()
{
    ImGui::NewLine();
    ImGui::SeparatorText("Font Type");
    ImGui::Combo("##Font Type Combo", &m_iFontType, m_arrFontType, IM_ARRAYSIZE(m_arrFontType));

    ImGui::NewLine();
    ImGui::SeparatorText("Font Color");
    ImGui::ColorEdit4("##Font Color", m_arrColors);

    ImGui::NewLine();
    ImGui::SeparatorText("Font Size");
    ImGuiInputTextFlags_ eFlag = ImGuiInputTextFlags_CharsDecimal;
    ImGui::Text("Size");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(100);
    ImGui::InputFloat("##PosX2", &m_fFontSize, 0.01f, 1.0f, "%.2f", eFlag);
}

void ImguiMgr::Set_Font()
{
    ImGui::NewLine();
    ImGui::SeparatorText("Set");

    if (ImGui::Button("Set Font", ImVec2(80.f, 20.f)))
    {
        if (0 == m_strSelectObjName.length())
        {
            return;
        }

        auto pGameobject = CUR_SCENE->Get_GameObject(m_strSelectObjName);
        auto pFontRenderer = pGameobject->Get_FontRenderer();

        if (nullptr == pFontRenderer)
        {
            pGameobject->Add_Component(make_shared<FontRenderer>(L""));
            pGameobject->Get_FontRenderer()->Set_Font(RESOURCES.Get<CustomFont>(L"136ex"), Color(1.f, 1.f, 1.f, 1.f), 1.f);
            return;
        }

        pFontRenderer->Set_Font(RESOURCES.Get<CustomFont>(m_vecFontType[m_iFontType]), Color(m_arrColors[0], m_arrColors[1], m_arrColors[2], m_arrColors[3]), m_fFontSize);
        pFontRenderer->Get_Text() = m_wstrFont;
    }

}
