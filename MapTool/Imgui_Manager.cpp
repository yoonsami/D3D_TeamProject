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
#include "ModelMesh.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "Geometry.h"
#include "Terrain.h"

#include "Camera.h"
#include "DemoScene.h"

#include "SphereCollider.h"
#include "AABBBoxCollider.h"
#include "OBBBoxCollider.h"
#include "MeshCollider.h"

#include "GeometryHelper.h"
#include "MeshRenderer.h"
#include "RigidBody.h"

#include "PointLightScript.h"
#include "ModelAnimation.h"
#include "WaterUVSliding.h"
//#include "TerrainRenderer.h"

#include "WeedScript.h"
using namespace ImGui;

ImGuizmo::OPERATION m_eGuizmoType = { ImGuizmo::TRANSLATE };
namespace fs = std::filesystem;

bool VectorOfStringGetter(void* data, int n, const char** out_text)
{
	const vector<string>* v = (vector<string>*)data;
	*out_text = (v[0][n]).c_str();
	return true;
}

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

    Load_SkyBoxTexture();
    Load_MapObjectBase();
    Load_TerrainTile();
    Load_Water();
    Load_WeedNames();

    Create_SampleObjects();
}

void ImGui_Manager::ImGui_Tick()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::ShowDemoWindow();

    Frame_Light();
    Frame_ObjectBase();
    Frame_ObjectBaseManager();
    Frame_Objects();
    Frame_SelcetObjectManager();
    Frame_Wall();
    Frame_ShaderOption();
    Picking_Object();
    LookAtSampleObject();
    Frame_ModelObj();
    Frame_Terrain();
    Frame_PickingManager();
    Frame_Weed();
    Frame_WeedManager();

    Show_Gizmo();
}

void ImGui_Manager::ImGui_Render()
{
    ImGui::Render();
   
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGui_Manager::Show_Gizmo()
{
    // 기즈모 출력
    ImGuizmo::BeginFrame();
    if (m_strObjectName.size() != 0 && m_GizmoTarget == GizmoTMapObj)
    {
        // 기즈모를 위해 카메라의 뷰,투영을 가져옴.
        shared_ptr<Camera> CurCam = CUR_SCENE->Get_Camera(L"Default")->Get_Camera();
        _float4x4 matView = CurCam->Get_ViewMat();
        _float4x4 matProj = CurCam->Get_ProjMat();

        // 기즈모 상태 적용
        _float4x4 matGuizmo = m_pMapObjects[m_iObjects]->Get_Transform()->Get_WorldMatrix();

        ImGuizmo::Manipulate((float*)&matView,
            (float*)&matProj,
            m_eGuizmoType,
            ImGuizmo::LOCAL,
            (float*)&matGuizmo);

        m_pMapObjects[m_iObjects].get()->Get_Transform()->Set_WorldMat(matGuizmo);
    }
    else if (m_strPointLightList.size() != 0 && m_GizmoTarget == GizmoTPointLight)
    {
        // 기즈모를 위해 카메라의 뷰,투영을 가져옴.
        shared_ptr<Camera> CurCam = CUR_SCENE->Get_Camera(L"Default")->Get_Camera();
        _float4x4 matView = CurCam->Get_ViewMat();
        _float4x4 matProj = CurCam->Get_ProjMat();

        // 기즈모 상태 적용
        _float4x4 matGuizmo = m_pPointLightObjects[m_iPointLightIndex]->Get_Transform()->Get_WorldMatrix();

        ImGuizmo::Manipulate((float*)&matView,
            (float*)&matProj,
            ImGuizmo::TRANSLATE,
            ImGuizmo::LOCAL,
            (float*)&matGuizmo);

        m_pPointLightObjects[m_iPointLightIndex]->Get_Transform()->Set_WorldMat(matGuizmo);
    }
}

void ImGui_Manager::Frame_ObjectBase()
{
    ImGui::Begin("Frame_ObjectBase"); // 글자 맨윗줄

    if (ImGui::ListBox("##ObjectBase", &m_iObjectBaseIndex, m_strObjectBaseNameList.data(), (int)m_strObjectBaseNameList.size(), (_int)m_strObjectBaseNameList.size()))
        Set_SampleObject(); // 리스트박스 선택대상이 달라지면 샘플모델변경
    // 높이 재설정
    if (m_bBaseObjectListResetHeight)
    {
        m_bBaseObjectListResetHeight = false;
        ImGui::SetScrollY(m_iObjectBaseIndex * ImGui::GetTextLineHeightWithSpacing());
    }

    ImGui::End();
}

void ImGui_Manager::Frame_ObjectBaseManager()
{
    ImGui::Begin("Frame_ObjectBaseManager"); // 글자 맨윗줄

    if (ImGui::InputText("Filter##ObjectBase", m_szBaseObjectFilter, sizeof(m_szBaseObjectFilter)))
    {
        // 필터내용 string화
        string strFilter = m_szBaseObjectFilter;
        m_FilteredBaseObjectNames.clear();
        for (size_t i = 0; i < m_strObjectBaseNameList.size(); ++i)
        {
            // 검사당할 대상 텍스트
            string strFilterTarget = m_strObjectBaseNameList[i];
            if (strFilterTarget.find(strFilter) != std::string::npos)
            {
                // 동적생성
                shared_ptr<char[]> pChar = shared_ptr<char[]>(new char[MAX_PATH]);
                strncpy_s(pChar.get(), MAX_PATH, strFilterTarget.c_str(), _TRUNCATE);

                // 필터된 이름을 포인터 벡터에 추가
                m_strFilteredNamePtr.push_back(pChar);
                // 필터된 이름을 리스트UI에 추가
                m_FilteredBaseObjectNames.push_back(pChar.get());
            }
        }
    }

    if (ImGui::ListBox("FilteredObjectBases##ObjectBase", &m_iFilteredBaseObjectsIndex, m_FilteredBaseObjectNames.data(), (_int)m_FilteredBaseObjectNames.size()))
    {
        for (size_t i = 0; i < m_strObjectBaseNameList.size(); ++i)
        {
            string strBaseObjName = m_strObjectBaseNameList[i];
            string strFilteredObjName = m_FilteredBaseObjectNames[m_iFilteredBaseObjectsIndex];
            if (strBaseObjName == strFilteredObjName)
            {
                m_iObjectBaseIndex = (_int)i;
                break;
            }
        }
        Set_SampleObject(); // 리스트박스 선택대상이 달라지면 샘플모델변경
        m_bBaseObjectListResetHeight = true; // 베이스오브젝트리스트를 해당 높이에 맞게 변경
    }



    // 맵오브젝트 정보
    ImGui::SeparatorText("BaseObjectDesc");
    // 노란색 저장
    ImVec4 YellowColor(1.f, 1.f, 0.f, 1.f);
    string strCreateObjectName = m_strObjectBaseNameList[m_iObjectBaseIndex];
    ImGui::Text(("Name - " + strCreateObjectName/* + "-" + to_string(m_iObjectBaseIndexList[m_iObjectBaseIndex])*/).data());
    m_CreateObjectDesc.strName = strCreateObjectName /*+ "-" + to_string(m_iObjectBaseIndexList[m_iObjectBaseIndex])*/;
    ImGui::InputFloat("UVWeight", &m_CreateObjectDesc.fUVWeight);
    ImGui::TextColored(YellowColor, "Options");
    // 그림자
    ImGui::Text("Shadow - ");
    ImGui::SameLine();
    ImGui::Checkbox("##bShadow", &m_CreateObjectDesc.bShadow);
    // 블러
    ImGui::Text("Blur - ");
    ImGui::SameLine();
    ImGui::Checkbox("##bBlur", &m_CreateObjectDesc.bBlur);
    ImGui::TextColored(YellowColor, "Components");
    // 트랜스폼컴포넌트
    //ImGui::Text("Transform - ");
    //ImGui::SameLine();
    //ImGui::Checkbox("##bTransform", &m_CreateObjectDesc.bTransform);
    //if (m_CreateObjectDesc.bTransform)
    //{
    ImGui::InputFloat3("CreatePos", (_float*)&m_PickingPos);
    memcpy(&m_CreateObjectDesc.WorldMatrix._41, &m_PickingPos, sizeof(_float3));
    //}
    // 콜라이더 컴포넌트
    ImGui::Text("Collider - ");
    ImGui::SameLine();
    ImGui::Checkbox("##bCollider", &m_CreateObjectDesc.bCollider);
    string ModelName = m_strObjectBaseNameList[m_iObjectBaseIndex];
    m_CreateObjectDesc.ColModelName = ModelName;
    if (m_CreateObjectDesc.bCollider)
    {
        ImGui::Combo("ColliderType", &m_CreateObjectDesc.ColliderType, m_szColliderTypes, IM_ARRAYSIZE(m_szColliderTypes));
        ImGui::DragFloat3("ColliderOffset", (_float*)&m_CreateObjectDesc.ColliderOffset);
        switch (m_CreateObjectDesc.ColliderType)
        {
        case 0: //Sphere
            ImGui::DragFloat("Radius", &m_CreateObjectDesc.ColRadius);
            break;
        case 1: // AABB
            ImGui::DragFloat3("BoundingSize", (_float*)&m_CreateObjectDesc.ColBoundingSize);
            break;
        case 2: // OBB
            ImGui::DragFloat3("BoundingSize", (_float*)&m_CreateObjectDesc.ColBoundingSize);
            break;
        case 3: // Mesh
            ImGui::Text(("ModelName : " + ModelName).data());
            break;
        default:
            break;
        }
    }
    ImGui::SeparatorText("##CreateLine"); // 선긋기
    if (ImGui::Button("Create") || KEYTAP(KEY_TYPE::Z))
        if (FAILED(Create_SelectObject()))
            MSG_BOX("Fail : Create_MapObject");

    ImGui::SameLine();
    ImGui::Text("Press Z");

    // 현재 맵에있는 오브젝트 이름들을 텍스트파일로 저장.
    if (ImGui::Button("CreateNames"))
        Save_ModelNames();

    ImGui::End();
}

void ImGui_Manager::Frame_Objects()
{
    ImGui::Begin("Frame_SelectObjects"); // 글자 맨윗줄

    if(ImGui::ListBox("##Objects", &m_iObjects, m_strObjectName.data(), (_int)m_strObjectName.size(), (_int)m_strObjectName.size()))
    {
        strcpy_s(m_szBaseObjectFilter, m_strObjectName[m_iObjects]);
    }
    // 높이재설정
    if(m_bObjectListResetHeight)
    {
        m_bObjectListResetHeight = false;
        ImGui::SetScrollY(m_iObjects * ImGui::GetTextLineHeightWithSpacing());
    }

    ImGui::End();
}

void ImGui_Manager::Frame_SelcetObjectManager()
{
    ImGui::Begin("Frame_SelectObjectManager"); // 글자 맨윗줄

    if (m_strObjectName.size() > 0)
        if (ImGui::Button("Delete"))
            if (FAILED(Delete_MapObject()))
                MSG_BOX("Fail : Delete_MapObject");

    // 현재 오브젝트의 정보띄우기
    if (m_strObjectName.size() > 0)
    {
        if (ImGui::Button("GizmoTargetChange"))
            m_GizmoTarget = GizmoTMapObj;

        ImGui::SeparatorText("ObjectDesc");
        // 노란색 저장
        ImVec4 YellowColor(1.f, 1.f, 0.f, 1.f);

        // 맵오브젝트 정보
        ImGui::TextColored(YellowColor, "Default");
        MapObjectScript::MAPOBJDESC& CurObjectDesc = m_pMapObjects[m_iObjects]->Get_Script<MapObjectScript>()->Get_DESC();
        ImGui::Text(("Name - " + CurObjectDesc.strName).data());
        // 컬링관련
        ImGui::Text(("CullPos - X:" + to_string(CurObjectDesc.CullPos.x)
            + " Y:" + to_string(CurObjectDesc.CullPos.y)
            + " Z:" + to_string(CurObjectDesc.CullPos.z)).data());
        ImGui::Text(("CullRadius - " + to_string(CurObjectDesc.CullRadius)).data());
        ImGui::TextColored(YellowColor, "Options");
        // 그림자
        ImGui::Text("Shadow - ");
        ImGui::SameLine();
        ImGui::Checkbox("##bShadow", &CurObjectDesc.bShadow);
        // 블러
        ImGui::Text("Blur - ");
        ImGui::SameLine();
        ImGui::Checkbox("##bBlur", &CurObjectDesc.bBlur);
        // 오브젝트에 그림자와 블러 적용
        if (ImGui::Button("Bake"))
            Bake(m_pMapObjects[m_iObjects]);
        ImGui::SameLine();
        if (ImGui::Button("Bake All"))
            BakeAll();
        ImGui::TextColored(YellowColor, "Components");
        // 트랜스폼컴포넌트
        ImGui::Text("Transform - ");
        ImGui::SameLine();
        ImGui::Checkbox("##bTransform", &CurObjectDesc.bTransform);
        if (CurObjectDesc.bTransform && m_pMapObjects[m_iObjects].get()->Get_Transform() != nullptr && m_GizmoTarget == GizmoTMapObj)
        {
            // 기즈모의 선택옵션(TR or RT or SC) 업데이트
            // 전체옵션 false로 초기화
            for (size_t i = 0; i < GizmoEND; i++)
            {
                m_bGizmoOp[i] = false;
                // 현재옵션이랑 같으면 true
                if (m_eGizmoOp == i)
                    m_bGizmoOp[i] = true;
            }
            // 포지션, 회전, 스케일 선택
            if (ImGui::RadioButton("Tr", m_bGizmoOp[GizmoTR]))
                m_eGizmoOp = GizmoTR;
            ImGui::SameLine();
            if (ImGui::RadioButton("Rt", m_bGizmoOp[GizmoRT]))
                m_eGizmoOp = GizmoRT;
            ImGui::SameLine();
            if (ImGui::RadioButton("Sc", m_bGizmoOp[GizmoSC]))
                m_eGizmoOp = GizmoSC;
            // 현재 기즈모 옵션 적용
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
            // 포지션, 회전, 스케일 조정 및 출력
            _float4x4 tempFloat4x4 = m_pMapObjects[m_iObjects].get()->Get_Transform()->Get_WorldMatrix();
            _float4x4* pTempObj = &tempFloat4x4;
            float matrixTranslation[3], matrixRotation[3], matrixScale[3];
            ImGuizmo::DecomposeMatrixToComponents((float*)pTempObj, matrixTranslation, matrixRotation, matrixScale);
            ImGui::InputFloat3("Tr", matrixTranslation);
            ImGui::InputFloat3("Rt", matrixRotation);
            ImGui::InputFloat3("Sc", matrixScale);
            ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, (float*)pTempObj);
            m_pMapObjects[m_iObjects]->Get_Transform()->Set_WorldMat(*(_float4x4*)pTempObj);
        }
        // 콜라이더 컴포넌트
        ImGui::Text("Collider - ");
        ImGui::SameLine();
        if (ImGui::Checkbox("##bCollider", &CurObjectDesc.bCollider))
        {
            if (CurObjectDesc.bCollider)
            {
                // 콜라이더 타입에 따라 콜라이더 생성
                switch (CurObjectDesc.ColliderType)
                {
                case static_cast<_int>(ColliderType::Sphere):
                {
                    shared_ptr<SphereCollider> pCollider = make_shared<SphereCollider>(CurObjectDesc.ColRadius);
                    pCollider->Set_Offset(CurObjectDesc.ColliderOffset);
                    m_pMapObjects[m_iObjects]->Add_Component(pCollider);
                    pCollider->Set_Activate(true);
                    break;
                }
                case static_cast<_int>(ColliderType::AABB):
                {
                    shared_ptr<AABBBoxCollider> pCollider = make_shared<AABBBoxCollider>(CurObjectDesc.ColBoundingSize);
                    pCollider->Set_Offset(CurObjectDesc.ColliderOffset);
                    m_pMapObjects[m_iObjects]->Add_Component(pCollider);
                    pCollider->Set_Activate(true);
                    break;
                }
                case static_cast<_int>(ColliderType::OBB):
                {
                    shared_ptr<OBBBoxCollider> pCollider = make_shared<OBBBoxCollider>(CurObjectDesc.ColBoundingSize);
                    pCollider->Set_Offset(CurObjectDesc.ColliderOffset);
                    m_pMapObjects[m_iObjects]->Add_Component(pCollider);
                    pCollider->Set_Activate(true);
                    break;
                }
                case static_cast<_int>(ColliderType::Mesh):
                {
                    shared_ptr<MeshCollider> pCollider = make_shared<MeshCollider>(Utils::ToWString(CurObjectDesc.ColModelName));
                    pCollider->Set_Offset(CurObjectDesc.ColliderOffset);
                    m_pMapObjects[m_iObjects]->Add_Component(pCollider);
                    pCollider->Set_Activate(true);
                    //auto rigidBody = make_shared<RigidBody>();
                    //rigidBody->Create_RigidBody(pCollider, m_pMapObjects[m_iObjects]->GetOrAddTransform()->Get_WorldMatrix());
                    //m_pMapObjects[m_iObjects]->Add_Component(rigidBody);
                    break;
                }
                default:
                    break;
                }
            }
            else
                m_pMapObjects[m_iObjects]->DeleteComponent(COMPONENT_TYPE::Collider);
        }
        if (CurObjectDesc.bCollider)
        {
            ImGui::Combo("ColliderType", &CurObjectDesc.ColliderType, m_szColliderTypes, IM_ARRAYSIZE(m_szColliderTypes));
            ImGui::DragFloat3("ColliderOffset", (_float*)&CurObjectDesc.ColliderOffset);
            switch (CurObjectDesc.ColliderType)
            {
            case 0: //Sphere
                ImGui::DragFloat("Radius", &CurObjectDesc.ColRadius);
                break;
            case 1: // AABB
                ImGui::DragFloat3("BoundingSize", (_float*)&CurObjectDesc.ColBoundingSize);
                break;
            case 2: // OBB
                ImGui::DragFloat3("BoundingSize", (_float*)&CurObjectDesc.ColBoundingSize);
                break;
            case 3: // Mesh
                ImGui::Text(("ModelName : " + CurObjectDesc.ColModelName).data());
                break;
            default:
                break;
            }
        }
        // 애니메이션여부체크
        if(m_pMapObjects[m_iObjects]->Get_ModelRenderer() != nullptr)
        {
            // CullNone여부 판단
            _int passType = _int(CurObjectDesc.bCullNone);
            if (ImGui::InputInt("##bCullNone", &passType))
            {
                if (passType < 0) passType = 0;
                if (passType > ModelRenderer::INSTANCE_PASSTYPE::PASS_DEFAULT) passType = ModelRenderer::INSTANCE_PASSTYPE::PASS_DEFAULT;
                CurObjectDesc.bCullNone = _char(passType);
                m_pMapObjects[m_iObjects]->Get_ModelRenderer()->Set_PassType((ModelRenderer::INSTANCE_PASSTYPE)CurObjectDesc.bCullNone);
				/* for (_int i = 0; i < m_pMapObjects.size(); ++i)
				 {
					 MapObjectScript::MAPOBJDESC& tempDesc = m_pMapObjects[i]->Get_Script<MapObjectScript>()->Get_DESC();
					 if (CurObjectDesc.strName == tempDesc.strName)
					 {
						 tempDesc.bCullNone = CurObjectDesc.bCullNone;
						 m_pMapObjects[i]->Get_ModelRenderer()->Set_PassType((ModelRenderer::INSTANCE_PASSTYPE)CurObjectDesc.bCullNone);
					 }
				 }*/
                for (auto& obj : m_pMapObjects)
                {
                    if(!obj->Get_Model())
                        continue;

                    if (obj->Get_Model()->Get_ModelTag() == m_pMapObjects[m_iObjects]->Get_Model()->Get_ModelTag())
                    {
                        MapObjectScript::MAPOBJDESC& tempDesc = obj->Get_Script<MapObjectScript>()->Get_DESC();
                        tempDesc.bCullNone = CurObjectDesc.bCullNone;
                        obj->Get_ModelRenderer()->Set_PassType((ModelRenderer::INSTANCE_PASSTYPE)CurObjectDesc.bCullNone);
                    }
                }
            }
        }
    }
    // 세이브로드
    ImGui::SeparatorText("Save&Load");
    ImGui::Text("SaveFileName");
    ImGui::InputText("##SaveFileName", m_szSaveFileName, sizeof(m_szSaveFileName));
    if (ImGui::Button("Save Object"))
        if (FAILED(Save_MapObject()))
            MSG_BOX("Fail : Save_MapObject");
        else
            MSG_BOX("Complete_Save");

    m_MapNames.clear();
    wstring partsPath = L"..\\Resources\\MapData\\";
    for (auto& entry : fs::recursive_directory_iterator(partsPath))
    {

		if (entry.is_directory())
			continue;

		if (entry.path().extension().wstring() != L".dat" && entry.path().extension().wstring() != L".DAT")
			continue;

        string fileName = entry.path().filename().string();
        Utils::DetachExt(fileName);

        m_MapNames.push_back(fileName);

    }
    curMapIndex %= m_MapNames.size();
    ImGui::Combo("Map List", &curMapIndex, VectorOfStringGetter, &m_MapNames, int(m_MapNames.size()));

    if (ImGui::Button("Load Object"))
        if (FAILED(Load_MapObject()))
            MSG_BOX("Fail : Load_MapObject");
        else
            MSG_BOX("Complete_Load");

    ImGui::End();
}

void ImGui_Manager::Frame_Light()
{
    ImGui::Begin("SkyBox&Light"); // 글자 맨윗줄

    ImGui::Text("CameraPos");
    m_CamPos = CUR_SCENE->Get_MainCamera()->Get_Transform()->Get_State(Transform_State::POS);
    if (ImGui::DragFloat3("##CamPos", (_float*)&m_CamPos))
    {
        CUR_SCENE->Get_MainCamera()->Get_Transform()->Set_State(Transform_State::POS, m_CamPos);
    }

    // 플레이어 생성위치
    ImGui::Text("PlayerCreatePosition");
    if(ImGui::DragFloat3("##PlayerCreatePosition", (_float*)&m_PlayerCreatePosition, 0.1f))
    {
        m_PlayerLookAtPosition.y = m_PlayerCreatePosition.y;
    }
    if (ImGui::Button("Set Player Pos"))
        SetPlayerPosByCameraPos();
    ImGui::Text("PlayerLookAtPosition");
    if (ImGui::DragFloat3("##PlayerLookAtPosition", (_float*)&m_PlayerLookAtPosition, 0.1f))
    {
        m_PlayerLookAtPosition.y = m_PlayerCreatePosition.y;
    }
    if (ImGui::Button("Set Player LookAt Pos"))
        SetPlayerLookAtPosByPickingPos();

    // 스카이박스 변경
    ImGui::SeparatorText("SkyBox##SkyBoxChange");
    if (ImGui::ListBox("##SkyBoxName", &m_iCurrentSkyBoxIndex, m_strSkyboxList.data(), (int)m_strSkyboxList.size()))
    {
        string strCurSkybox = m_strSkyboxList[m_iCurrentSkyBoxIndex];
        Utils::DetachExt(strCurSkybox);

        vector<shared_ptr<Material>>& Mats = CUR_SCENE->Get_GameObject(L"SkyBase")->Get_ModelRenderer()->Get_Model()->Get_Materials();
        Mats[0]->Set_TextureMap(RESOURCES.Get<Texture>(Utils::ToWString(strCurSkybox)), TextureMapType::DIFFUSE);
    }

    // 방향성광원 통제
    ImGui::SeparatorText("DirectionalLight");
    // 포지션
    _float4& Pos = (_float4&)CUR_SCENE->Get_Light()->Get_Transform()->Get_WorldMatrix().m[3];
    ImGui::DragFloat3("Position##DirLight", (_float*)&Pos);

    //_float3& LookDir = CUR_SCENE->Get_Light()->Get_Transform()->Get_CurrentDir();
    //CUR_SCENE->Get_Light()->Get_Transform()->LookAt(XMVectorSetW(m_DirectionalLightLookDir, 1.f));
    ImGui::DragFloat3("LookAtPos##DirLight", (_float*)&m_DirectionalLightLookAtPos);
    if(ImGui::Button("LookAt##DirLightLookAt"))
    {
        CUR_SCENE->Get_Light()->Get_Transform()->LookAt(XMVectorSetW(m_DirectionalLightLookAtPos, 1.f));
    }
    LightInfo& DirectionalLightInfo = CUR_SCENE->Get_Light()->Get_Light()->Get_LightInfo();
    Color& Ambient = DirectionalLightInfo.color.ambient;
    ImGui::ColorEdit4("Ambient##DirLight", (_float*)&Ambient);
    Color& Diffuse = DirectionalLightInfo.color.diffuse;
    ImGui::ColorEdit4("Diffuse##DirLight", (_float*)&Diffuse,ImGuiColorEditFlags_HDR);
    Color& Specular = DirectionalLightInfo.color.specular;
    ImGui::ColorEdit4("Specular##DirLight", (_float*)&Specular);
    Color& Emissive = DirectionalLightInfo.color.emissive;
    ImGui::ColorEdit4("Emissive##DirLight", (_float*)&Emissive);

    m_DirectionalLightPos = Pos;
    //m_DirectionalLightLookDir = LookDir;
    m_DirectionalLightInfo = DirectionalLightInfo;

    // 점광원생성정보
    ImGui::SeparatorText("CreatePointLight");
    ImGui::DragFloat3("CreatePos##CreatePtLt", (_float*)&m_PickingPos, 0.1f);
    ImGui::ColorEdit4("Ambient##CreatePtLt", (_float*)&m_CreatePointLightInfo.color.ambient);
    ImGui::ColorEdit4("Diffuse##CreatePtLt", (_float*)&m_CreatePointLightInfo.color.diffuse);
    ImGui::ColorEdit4("Specular##CreatePtLt", (_float*)&m_CreatePointLightInfo.color.specular);
    ImGui::ColorEdit4("Emissive##CreatePtLt", (_float*)&m_CreatePointLightInfo.color.emissive);
    ImGui::DragFloat("Range##CreatePtLt", &m_CreatePointLightInfo.range, 0.1f);
    
    ImGui::Checkbox("PointLightEffect", &m_bPointLightEffectHas);
    if (m_bPointLightEffectHas)
    {
        ImGui::Text("TargetAmbient");
        ImGui::ColorEdit4("Ambient##PtLtEffectAmbient", (_float*)&m_CreateTargetAmbient);
        ImGui::Text("TargetDiffuse");
        ImGui::ColorEdit4("Diffuse##PtLtEffectDiffuse", (_float*)&m_CreateTargetDiffuse);
        ImGui::DragFloat("Speed##PointLightEffectSpeed", &m_fCreatePointLightSpeed, 0.1f);
    }

    if (ImGui::Button("Create##CreatePtLt"))
        Create_SelectPointLight();

    ImGui::SameLine();

    if (ImGui::Button("Delete##DeletePtLt"))
        Delete_PointLight();

    // 현재 점광원의 정보
    if(m_strPointLightList.size() > 0)
    {
        ImGui::SeparatorText("PointLights");

        if (ImGui::Button("GizmoTargetChange"))
            m_GizmoTarget = GizmoTPointLight;

        ImGui::ListBox("##PointLights", &m_iPointLightIndex, m_strPointLightList.data(), (_int)m_strPointLightList.size());

        LightInfo& CurPtLightInfo = m_pPointLightObjects[m_iPointLightIndex]->Get_Light()->Get_LightInfo();
        _float4 PtLightPos = m_pPointLightObjects[m_iPointLightIndex]->Get_Transform()->Get_State(Transform_State::POS);
        ImGui::DragFloat3("Pos##CurrentPtLt", (_float*)&PtLightPos, 0.1f);
        m_pPointLightObjects[m_iPointLightIndex]->Get_Transform()->Set_State(Transform_State::POS, PtLightPos);
        ImGui::ColorEdit4("Ambient##CurrentPtLt", (_float*)&CurPtLightInfo.color.ambient);
        ImGui::ColorEdit4("Diffuse##CurrentPtLt", (_float*)&CurPtLightInfo.color.diffuse);
        ImGui::ColorEdit4("Specular##CurrentPtLt", (_float*)&CurPtLightInfo.color.specular);
        ImGui::ColorEdit4("Emissive##CurrentPtLt", (_float*)&CurPtLightInfo.color.emissive);
        ImGui::DragFloat("Range##CurrentPtLt", &CurPtLightInfo.range, 0.1f);

        _float4 PointLightPosition = m_pPointLightObjects[m_iPointLightIndex]->Get_Transform()->Get_State(Transform_State::POS);
        ImGui::DragFloat3("PointLightPosition", (_float*)&PointLightPosition, 0.1f);
        m_pPointLightObjects[m_iPointLightIndex]->Get_Transform()->Set_State(Transform_State::POS, PointLightPosition);

        // 선택한 점광원의 변화효과 설정
        if(m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>() != nullptr)
        {
            _bool bEffectUse = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Get_bUseEffect();
            if (ImGui::Checkbox("##CurrentPointLightEffectUse", &bEffectUse))
                // 사용여부 체크박스 반영하여 다시 넣기
            {
                m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Set_bUseEffect(bEffectUse);
                // 이펙트를 사용하기로 하면 그때색깔을 시작색깔로 지정
                if (bEffectUse)
                    m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Clear();
                // 이펙트를 사용 안하기로 하면 시작색을 반대로 대입
                else
                    m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Init_Reverse();
            }
            if (bEffectUse)
            {
                // 스타트앰비언트 가져오고, 변경하면 다시대입
                _float4 StartAmbient = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Get_AmbientStart();
                if (ImGui::ColorEdit4("SAmbient##PtLtEffectAmbient", (_float*)&StartAmbient))
                    m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Set_AmbientStart(StartAmbient);
                // 타겟앰비언트 가져오고, 변경하면 다시대입
                _float4 TargetAmbient = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Get_AmbientTarget();
                if (ImGui::ColorEdit4("EAmbient##PtLtEffectAmbient", (_float*)&TargetAmbient))
                    m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Set_AmbientTarget(TargetAmbient);

                // 스타트디퓨즈 가져오고, 변경하면 다시대입
                _float4 StartDiffuse = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Get_DiffuseStart();
                if (ImGui::ColorEdit4("SDiffuse##PtLtEffectDiffuse", (_float*)&StartDiffuse))
                    m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Set_DiffuseStart(StartDiffuse);
                // 타겟디퓨즈 가져오고, 변경하면 다시대입
                _float4 TargetDiffuse = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Get_DiffuseTarget();
                if (ImGui::ColorEdit4("Diffuse##PtLtEffectDiffuse", (_float*)&TargetDiffuse))
                    m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Set_DiffuseTarget(TargetDiffuse);

                // 스타트레인지 가져오고, 변경하면 다시대입
                _float fStartRange = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Get_StartRange();
                ImGui::DragFloat("SRange#PtLtTargetRange", &fStartRange, 0.1f);
                m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Set_StartRange(fStartRange);
                // 타겟레인지 가져오고, 변경하면 다시대입
                _float fTargetRange = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Get_TargetRange();
                ImGui::DragFloat("TRange#PtLtTargetRange", &fTargetRange, 0.1f);
                m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Set_TargetRange(fTargetRange);

                // 이펙트 스피드 가져오고, 변경하면 다시대입
                _float fEffectSpeed = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Get_Speed();
                ImGui::DragFloat("EffectSpeed#PtLtEffectSpeed", &fEffectSpeed, 0.1f);
                m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightScript>()->Set_Speed(fEffectSpeed);
            }
        }
    }

    ImGui::SeparatorText("##LightFrame");
    
    ImGui::End();
}

void ImGui_Manager::Frame_Wall()
{
    ImGui::Begin("Frame_Wall"); // 글자 맨윗줄
    // 벽피킹관련
    ImGui::Text("Wall");
    if (ImGui::Checkbox("WallPickingMode", &m_bWallPickingMod))
    {
        m_bGroundPickingMod = !m_bWallPickingMod;
    }
    if (ImGui::Button("CreateWall"))
    {
        Create_WallMesh();
    }
    if (ImGui::Button("ClearWall"))
    {
        Clear_WallMesh();
    }
    if (ImGui::Button("DeleteWall"))
    {
        Delete_WallMesh();
    }
    // 바닥피킹관련
    ImGui::Text("Ground");
    if (ImGui::Checkbox("GroundPickingMode", &m_bGroundPickingMod))
    {
        m_bWallPickingMod = !m_bGroundPickingMod;
    }
    if (ImGui::Button("CreateGround"))
    {
        Create_GroundMesh();
    }
    if (ImGui::Button("DeleteGround"))
    {
        Delete_GroundMesh();
    }

    if (ImGui::Button("MagicButton"))
    {
        m_iObjects = 0;
        for (; m_iObjects<m_pMapObjects.size();)
        {
            wstring ObjName = m_pMapObjects[m_iObjects]->Get_Name();
            if(ObjName.find(L"Weed") != std::string::npos)
            {
                Delete_MapObject();
            }
            if (m_iObjects >= m_pMapObjects.size() - 1)
                break;
            ++m_iObjects;
        }
    }

    ImGui::End();
}

void ImGui_Manager::Frame_ShaderOption()
{
    ImGui::Begin("Debug");
    if (CUR_SCENE)
    {
        if (BeginTabBar("##ShaderOptionTabBar"))
        {
            RenderOptionTab();
            //ModelOptionTab();


			if (BeginTabItem("NormalDepth"))
			{

				DragFloat("g_DepthRange", &CUR_SCENE->g_DepthRange, 1.f, 0.1f,1000.f);
				DragFloat("g_ClosestDepth", &CUR_SCENE->g_ClosestDepth, 1.f, 0.1f, 200.f);
				DragFloat2("MinMaxTessellation Dist", (_float*)&GAMEINSTANCE.m_vMinMaxTessellationDistance, 1.f, 0.01f, 1000.f);

				EndTabItem();
			}


            EndTabBar();
        }
    }

    ImGui::End();
}

void ImGui_Manager::Frame_Terrain()
{
    ImGui::Begin("Frame_Terrain");

    // 바다 색깔변ㄱ경
    ImGui::Text("OceanData");
    ImGui::ColorEdit4("Color##Ocean1", (_float*)&m_WaterColor1, ImGuiColorEditFlags_HDR);
    ImGui::ColorEdit4("Color##Ocean2", (_float*)&m_WaterColor2, ImGuiColorEditFlags_HDR);
    ImGui::DragInt("##OceanInt0", &m_WaterInt0, 0.1f);
    ImGui::DragInt("##OceanInt1", &m_WaterInt1, 0.1f);
    
    auto OceanObj = CUR_SCENE->Get_GameObject(L"Water_Plane");
    if(OceanObj != nullptr)
    {
        auto WaterScript = OceanObj->Get_Script<WaterUVSliding>();
        WaterScript->Set_Color1(m_WaterColor1);
        WaterScript->Set_Color2(m_WaterColor2);
        WaterScript->Set_Int0(m_WaterInt0);
        WaterScript->Set_Int1(m_WaterInt1);
    }

    ImGui::Text("TerrainSize");
    ImGui::DragInt2("##TerrainSize", m_iTerrainSize);

    if (ImGui::Button("CreateTerrain"))
    {
        Create_Terrain();
    }
    //// 선택한 텍스쳐로 터레인의 디퓨즈 바꾸기 - 임시코드
    //if(ImGui::ListBox("Tile Texture List", &m_iCurrentTile, VectorOfStringGetter, &m_TileNames, int(m_TileNames.size())))
    //{
    //    CUR_SCENE->Get_GameObject(L"Terrain")->Get_TerrainRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(Utils::ToWString(m_TileNames[m_iCurrentTile])), TextureMapType::DIFFUSE);
    //}

    ImGui::DragFloat("BrushRadius", &m_fTerrainBrushRadius, 0.1f);
    ImGui::DragFloat("TilePressForce", &m_fTilePressForce, 0.001f);

    // 범위안의 정점을 해당높이로 변환
    ImGui::InputFloat("##TerrainSetHeight", &m_fTerrainSetHeight);
    ImGui::SameLine();
    if(ImGui::Button("TerrainSetHeight"))
    {
        Set_TerrainHeight();
    }

    if (ImGui::Button("Save##TerrainSave"))
    {
        Save_TerrainData();
    }
    ImGui::SameLine();
    if (ImGui::Button("Load##TerrainLoad"))
    {
        LoadAndCreateTerrain();
    }
	ImGui::SameLine();
	if (ImGui::Button("CalNormalTangent##TerrainCalNormalTangent"))
	{
        Cal_NormalTangent();
	}

    ImGui::End();
}

void ImGui_Manager::Frame_Weed()
{
    ListBox("WeedName", &m_iCurrentWeedIndex, VectorOfStringGetter, &m_strWeedCatalogue, int(m_strWeedCatalogue.size()));
}

void ImGui_Manager::Frame_WeedManager()
{
    if (Button("CreateWeed")) {
        Create_WeedRegion();
    }
}

void ImGui_Manager::Frame_PickingManager()
{
    ImGui::Begin("Frame_PickingManager");

    // 우클릭 맵오브젝트or터레인 피킹
    ImGui::Text("RightButton");
    if (ImGui::Checkbox("MapObjectPickingMode", &m_bMapObjectPickingMode))
    {
        m_bTerrainPickingMode = !m_bMapObjectPickingMode;
    }
    if (ImGui::Checkbox("TerrainPickingMode", &m_bTerrainPickingMode))
    {
        m_bMapObjectPickingMode = !m_bTerrainPickingMode;
    }
    // 휠버튼 벽or바닥 피킹
    ImGui::Text("MiddleButton");
    if (ImGui::Checkbox("WallPickingMode", &m_bWallPickingMod))
    {
        m_bGroundPickingMod = !m_bWallPickingMod;
    }
    if (ImGui::Checkbox("GroundPickingMode", &m_bGroundPickingMod))
    {
        m_bWallPickingMod = !m_bGroundPickingMod;
    }

    ImGui::End();
}

void ImGui_Manager::Picking_Object()
{
    POINT MousePos;
    ::GetCursorPos(&MousePos);
    ::ScreenToClient(g_hWnd, &MousePos);
    _float2 ScreenPos{ (_float)MousePos.x, (_float)MousePos.y };

    if (m_bTerrainPickingMode && m_pTerrain != nullptr)
    {
        // 브러시위치를 기준으로 y제외 BrushRadius 거리안에있는 범위를 상승(y증가)시킴
        if (KEYPUSH(KEY_TYPE::E))
        {
            auto& vtx = m_pTerrain->Get_MeshRenderer()->Get_Mesh()->Get_Geometry()->Get_Vertices();
            auto tempVTX = vtx;
            for (auto& VB : tempVTX)
            {
                m_vTerrainBrushPosition.y = VB.vPosition.y; // 같은높이라고 가정.
                if ((VB.vPosition - m_vTerrainBrushPosition).Length() <= m_fTerrainBrushRadius)
                {
                    VB.vPosition.y += m_fTilePressForce;
                }
            }
            m_pTerrain->Get_MeshRenderer()->Get_Mesh()->Get_Geometry()->Set_Vertices(tempVTX);
            m_pTerrain->Get_MeshRenderer()->Get_Mesh()->Create_Buffer();
            //D3D11_MAPPED_SUBRESOURCE tempDesc;
            //CONTEXT->Map(m_pTerrain->Get_MeshRenderer()->Get_Mesh()->Get_VertexBuffer()->Get_ComPtr().Get(),
            //    0,
            //    D3D11_MAP_WRITE_DISCARD,
            //    0,
            //    &tempDesc);

            //CONTEXT->Unmap(m_pTerrain->Get_MeshRenderer()->Get_Mesh()->Get_VertexBuffer()->Get_ComPtr().Get(), 0);
        }
        // 브러시위치를 기준으로 y제외 BrushRadius 거리안에있는 범위를 상승(y증가)시킴
        else if (KEYPUSH(KEY_TYPE::Q))
        {
            auto& vtx = m_pTerrain->Get_MeshRenderer()->Get_Mesh()->Get_Geometry()->Get_Vertices();
            auto tempVTX = vtx;
            for (auto& VB : tempVTX)
            {
                m_vTerrainBrushPosition.y = VB.vPosition.y; // 같은높이라고 가정.
                if ((VB.vPosition - m_vTerrainBrushPosition).Length() <= m_fTerrainBrushRadius)
                {
                    VB.vPosition.y -= m_fTilePressForce;
                }
            }
            m_pTerrain->Get_MeshRenderer()->Get_Mesh()->Get_Geometry()->Set_Vertices(tempVTX);
            m_pTerrain->Get_MeshRenderer()->Get_Mesh()->Create_Buffer();
        }
    }

    // 마우스 우클릭 시 맵오브젝트피킹
    if (KEYTAP(KEY_TYPE::RBUTTON))
    {
        // 맵오브젝트피킹
        if(m_bMapObjectPickingMode)
        {
            shared_ptr<GameObject> PickObject = PickingMgr::GetInstance().Pick_Mesh(ScreenPos, CUR_SCENE->Get_Camera(L"Default")->Get_Camera(), m_pMapObjects, m_PickingPos);

            if (m_GizmoTarget == GizmoTMapObj)
                // 선택된 오브젝트 번호 바꾸기 현재기즈모타겟이 맵오브젝트일때만 사용.
            {
                for (_int i = 0; i < m_pMapObjects.size(); ++i)
                {
                    if (PickObject == m_pMapObjects[i])
                    {
                        m_iObjects = i;
                        // 오브젝트 리스트에서 선택한 높이로 변경
                        m_bObjectListResetHeight = true;
                    }
                }
            }
        }
    }
    // 터레인피킹
    if (KEYPUSH(KEY_TYPE::RBUTTON))
    {
        if (m_bTerrainPickingMode && m_pTerrain != nullptr)
        {
            // 터레인 피킹위치갱신
            PickingMgr::GetInstance().Pick_Mesh(ScreenPos, CUR_SCENE->Get_Camera(L"Default")->Get_Camera(), m_pTerrain, m_vTerrainBrushPosition);
            // 오브젝트 생성 피킹위치도 변경
            m_PickingPos = m_vTerrainBrushPosition;

            m_pTerrain->Get_MeshRenderer()->SetVec4(0, _float4(m_vTerrainBrushPosition, 1.f));
            m_pTerrain->Get_MeshRenderer()->SetFloat(0, m_fTerrainBrushRadius);
        }
    }
    // 마우스 휠클릭 시 WallPickingPoint 저장
    if (KEYTAP(KEY_TYPE::MBUTTON))
    {
        // 맵오브젝트피킹 - 벽으로사용
        if (m_bWallPickingMod)
        {
            if (m_bFirstWallPick)
            {
                m_bFirstWallPick = false;
                shared_ptr<GameObject> PickObject = PickingMgr::GetInstance().Pick_Mesh(ScreenPos, CUR_SCENE->Get_Camera(L"Default")->Get_Camera(), CUR_SCENE->Get_Objects(), m_WallPickingPos[0]);
            }
            else
            {
                shared_ptr<GameObject> PickObject = PickingMgr::GetInstance().Pick_Mesh(ScreenPos, CUR_SCENE->Get_Camera(L"Default")->Get_Camera(), CUR_SCENE->Get_Objects(), m_WallPickingPos[1]);
                // 사각형 하나 추가
                m_WallRectPosLDRU.push_back(pair<_float3, _float3>(m_WallPickingPos[0], _float3{ m_WallPickingPos[1].x, m_fWallHeight, m_WallPickingPos[1].z }));
                // 좌하단 점을 우하단 점으로 변경
                m_WallPickingPos[0] = m_WallPickingPos[1];
            }
        }
        // 맵오브젝트피킹 - 바닥으로사용
        else if (m_bGroundPickingMod)
        {
            if (m_bFirstGroundPick)
            {
                m_bFirstGroundPick = !m_bFirstGroundPick;
                PickingMgr::GetInstance().Pick_Mesh(ScreenPos, CUR_SCENE->Get_Camera(L"Default")->Get_Camera(), CUR_SCENE->Get_Objects(), m_GroundPickingPos[0]);
            }
            else
            {
                m_bFirstGroundPick = !m_bFirstGroundPick;
                PickingMgr::GetInstance().Pick_Mesh(ScreenPos, CUR_SCENE->Get_Camera(L"Default")->Get_Camera(), CUR_SCENE->Get_Objects(), m_GroundPickingPos[1]); // 사각형 하나 추가
                m_GroundRectPosLURD.push_back(pair<_float3, _float3>(m_GroundPickingPos[0], m_GroundPickingPos[1]));
            }
        }
    }
}

void ImGui_Manager::LookAtSampleObject()
{
    if (KEYTAP(KEY_TYPE::TAB))
    {
        // 현재 카메라 위치저장
        m_matPreCamera = CUR_SCENE->Get_MainCamera()->Get_Transform()->Get_WorldMatrix();
    }
    if (KEYHOLD(KEY_TYPE::TAB))
    {
        // 카메라 이동
        _float4 SampleObjPos = m_SampleObject->Get_Transform()->Get_State(Transform_State::POS);
        shared_ptr<Transform> pMainCameraTransform = CUR_SCENE->Get_MainCamera()->Get_Transform();
        pMainCameraTransform->Set_State(Transform_State::POS, XMVectorAdd(SampleObjPos, _float4{1.f, 1.f, 1.f, 0.f}));
        pMainCameraTransform->LookAt(SampleObjPos);
        pMainCameraTransform->Set_State(Transform_State::POS, pMainCameraTransform->Get_State(Transform_State::POS) - pMainCameraTransform->Get_State(Transform_State::LOOK) * m_fSampleModelCullSize * 2.f);
    }
    if (KEYAWAY(KEY_TYPE::TAB))
    {
        // 카메라 원래대로 돌아가기
        CUR_SCENE->Get_MainCamera()->Get_Transform()->Set_WorldMat(m_matPreCamera);
    }
}

void ImGui_Manager::GeometryTest()
{
    for(int i=0; i<8; ++i)
	{
		shared_ptr<Mesh> WeedMesh = RESOURCES.Get<Mesh>(L"Point");

		// 메시를 기반으로 벽오브젝트 생성
		shared_ptr<GameObject> WeedObj = make_shared<GameObject>();
		WeedObj->Set_Name(L"WeedObj");
		WeedObj->GetOrAddTransform();
        WeedObj->Get_Transform()->Set_State(Transform_State::POS, _float4{ (_float)i + 1.f, 0.f, 0.f, 1.f });

		// 메시렌더러
		shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Grass.fx"));
		renderer->Set_Mesh(WeedMesh);

        wstring WeedTextureName = L"Weed" + to_wstring(i);
        shared_ptr<Material> material = RESOURCES.Get<Material>(WeedTextureName);
        renderer->Set_Material(material);

		WeedObj->Add_Component(renderer);

        shared_ptr<WeedScript> WeedSc = make_shared<WeedScript>();
        WeedObj->Add_Component(WeedSc);

		EVENTMGR.Create_Object(WeedObj);
	}
}

HRESULT ImGui_Manager::Load_SkyBoxTexture()
{
    wstring path = L"..\\Resources\\Textures\\MapObject\\SkyBox\\";

    for (auto& entry : fs::recursive_directory_iterator(path))
    {
        // 파일의 이름을 가져옴
        wstring fileName = entry.path().filename().wstring();
        WCHAR szTempName[MAX_PATH];
        lstrcpy(szTempName, fileName.c_str());

        wstring noExtName = szTempName;
        Utils::DetachExt(noExtName);
        RESOURCES.Load<Texture>(noExtName, path+szTempName);

        // char 형식으로 변환
        shared_ptr<char[]> pChar = shared_ptr<char[]>(new char[MAX_PATH]);
        WideCharToMultiByte(CP_ACP, 0, noExtName.c_str(), -1, pChar.get(), MAX_PATH, 0, 0);

        // 스카이박스 이름을 포인터 벡터에 추가
        m_strSkyBoxNamePtr.push_back(pChar);
        // 스카이박스 이름을 리스트UI에 추가
        m_strSkyboxList.push_back(pChar.get());
    }
    return S_OK;
}

HRESULT ImGui_Manager::Load_MapObjectBase()
{
    wstring path = L"..\\Resources\\Models\\MapObject\\";
    wstring folderName = L"";
    for (auto& entry : fs::recursive_directory_iterator(path))
    {
        // 맵오브젝트 폴더내부의 폴더이름들을 순회하며 베이스오브젝트 리스트를 만듦.
        if (entry.is_directory())
        {
            //folderName = entry.path().filename().wstring();
            continue;
        }

        // 확장자가 모델이 아니면 컨티뉴우우우
        wstring Ext = entry.path().extension().wstring();
        if (Ext != L".Model")
            continue;

        // 파일의 이름을 가져옴
        wstring fileName = entry.path().filename().wstring();
        Utils::DetachExt(fileName);
        // 뒤에 폴더명을 붙임
        //fileName = fileName + L"-" + folderName;
        WCHAR szTempName[MAX_PATH];
        lstrcpy(szTempName, fileName.c_str());

        // char 형식으로 변환
        shared_ptr<char[]> pChar = shared_ptr<char[]>(new char[MAX_PATH]);
        WideCharToMultiByte(CP_ACP, 0, szTempName, -1, pChar.get(), MAX_PATH, 0, 0);

        // 베이스오브젝트 이름을 포인터 벡터에 추가
        m_strObjectBaseNamePtr.push_back(pChar);
        // 베이스오브젝트 이름을 리스트UI에 추가
        m_strObjectBaseNameList.push_back(pChar.get());
        // 베이스 오브젝트 중복개수 인덱스 추가
        //m_iObjectBaseIndexList.push_back(0);
    }
    return S_OK;
}

void ImGui_Manager::Load_TerrainTile()
{
    wstring partsPath = L"..\\Resources\\Textures\\MapObject\\TerrainTile\\";
    for (auto& entry : fs::recursive_directory_iterator(partsPath))
    {

        if (entry.is_directory())
            continue;

        if (entry.path().extension().wstring() != L".tga" && entry.path().extension().wstring() != L".TGA")
            continue;

        string fileName = entry.path().filename().string();
        Utils::DetachExt(fileName);

        // 타일의 텍스쳐이름을 리소스에 로드
        wstring TileTexture = L"..\\Resources\\Textures\\MapObject\\TerrainTile\\";
        TileTexture += Utils::ToWString(fileName) + L".tga";
        RESOURCES.Load<Texture>(Utils::ToWString(fileName), TileTexture);
        // 리스트에도 추가
        m_TileNames.push_back(fileName);
    }
}

HRESULT ImGui_Manager::Create_SelectObject()
{
    // 현재 선택된 베이스오브젝트이름 가져오기
    wstring strCurObjBase = Utils::ToWString(m_strObjectBaseNameList[m_iObjectBaseIndex]);
    // 맵오브젝트 생성
    shared_ptr<GameObject> pCreateObject = Create_MapObject(m_CreateObjectDesc);
    m_pMapObjects.push_back(pCreateObject);
    // 현재 설치된 오브젝트 목록에 추가
    m_strObjectName.push_back(m_strObjectBaseNameList[m_iObjectBaseIndex]);
    // 새로 생성된 오브젝트를 타겟팅(기즈모)하기
    m_iObjects = (_int)(m_pMapObjects.size() - 1);

    return S_OK;
}

shared_ptr<GameObject> ImGui_Manager::Create_MapObject(MapObjectScript::MapObjectDesc _CreateDesc)
{
    // 오브젝트 틀 생성
    shared_ptr<GameObject> CreateObject = make_shared<GameObject>();
    // 맵오브젝트 정보 생성 및 반영
    shared_ptr<MapObjectScript> MapObjSc = make_shared<MapObjectScript>(_CreateDesc);
    CreateObject->Add_Component(MapObjSc);
    MapObjectScript::MapObjectDesc& CreateDesc = MapObjSc->Get_DESC();
    CreateObject->Set_Name(Utils::ToWString(CreateDesc.strName));
    // 이름을 사용하여 모델생성
    // 고유번호를 제거하여 모델명을 얻어옴
    _int iPureNameSize = 0;
    while (CreateDesc.strName[iPureNameSize] != '-' && iPureNameSize < CreateDesc.strName.size())
    {
        ++iPureNameSize;
    }
    string strModelName = CreateDesc.strName.substr(0, iPureNameSize);
    CreateDesc.ColModelName = strModelName;
    // 모델생성 
    shared_ptr<Model> model = RESOURCES.Get<Model>(Utils::ToWString(strModelName));
    shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

    // 애님모델
    if(strModelName.find("Anim_") != std::string::npos)
    {
        shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
        CreateObject->Add_Component(animator);
        animator->Set_Model(model);
        animator->Set_CurrentAnim(0, true, 1.f);
    }
    // 논애님모델
    else
    {
        shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
        CreateObject->Add_Component(renderer);
        renderer->Set_Model(model);

        renderer->Set_PassType((ModelRenderer::INSTANCE_PASSTYPE)CreateDesc.bCullNone);

         renderer->SetVec4(0, _float4(CreateDesc.fUVWeight));
    }

    if (CreateDesc.bTransform)
    {
        CreateObject->Add_Component(make_shared<Transform>());
        CreateObject->Get_Transform()->Set_WorldMat(CreateDesc.WorldMatrix);
    }

    if (CreateDesc.bCollider)
    {
        switch (CreateDesc.ColliderType)
        {
        case static_cast<_int>(ColliderType::Sphere):
        {
            shared_ptr<SphereCollider> pCollider = make_shared<SphereCollider>(CreateDesc.ColRadius);
            pCollider->Set_Offset(CreateDesc.ColliderOffset);
            CreateObject->Add_Component(pCollider);
            pCollider->Set_Activate(true);
            break;
        }
        case static_cast<_int>(ColliderType::AABB):
        {
            shared_ptr<AABBBoxCollider> pCollider = make_shared<AABBBoxCollider>(CreateDesc.ColBoundingSize);
            pCollider->Set_Offset(CreateDesc.ColliderOffset);
            CreateObject->Add_Component(pCollider);
            pCollider->Set_Activate(true);
            break;
        }
        case static_cast<_int>(ColliderType::OBB):
        {
            shared_ptr<OBBBoxCollider> pCollider = make_shared<OBBBoxCollider>(CreateDesc.ColBoundingSize);
            pCollider->Set_Offset(CreateDesc.ColliderOffset);
            CreateObject->Add_Component(pCollider);
            pCollider->Set_Activate(true);
            break;
        }
        case static_cast<_int>(ColliderType::Mesh):
        {
            shared_ptr<MeshCollider> pCollider = make_shared<MeshCollider>(Utils::ToWString(CreateDesc.ColModelName));
            pCollider->Set_Offset(CreateDesc.ColliderOffset);
            CreateObject->Add_Component(pCollider);
            pCollider->Set_Activate(true);
            break;
        }
        default:
            break;
        }
    }
    // 그림자, 블러, 컬링정보계산
    Bake(CreateObject);

    EVENTMGR.Create_Object(CreateObject);

    return CreateObject;
}

HRESULT ImGui_Manager::Create_SelectPointLight()
{
    // 선택한 점광원 생성
    LightInfo CreateLightInfo;
    m_CreatePointLightInfo.vPosition = XMVectorSetW(m_PickingPos, 1.f);
    shared_ptr<GameObject> LightObject = Create_PointLight(m_CreatePointLightInfo);
    m_pPointLightObjects.push_back(LightObject);

    {
        // 포인트 라이트라면 무조건 추가
        // 기본초기화
        shared_ptr<PointLightScript> pPLightScript = make_shared<PointLightScript>();
        m_pPointLightObjects.back()->Add_Component(pPLightScript);
        pPLightScript->Clear();
    }

    // 이름리스트에 하나 넣기
    m_strPointLightList.push_back("PointLight");
    
    return S_OK;
}

shared_ptr<GameObject> ImGui_Manager::Create_PointLight(LightInfo _ptltInfo)
{
    shared_ptr<GameObject> PointLight = make_shared<GameObject>();
    PointLight->Set_Name(L"PointLight");
    // 포지션
    PointLight->GetOrAddTransform()->Set_State(Transform_State::POS, _ptltInfo.vPosition);
    {
        // LightComponent 생성 후 세팅
        shared_ptr<Light> lightCom = make_shared<Light>();
        lightCom->Set_Ambient(_ptltInfo.color.ambient);
        lightCom->Set_Diffuse(_ptltInfo.color.diffuse);
        lightCom->Set_Specular(_ptltInfo.color.specular);
        lightCom->Set_Emissive(_ptltInfo.color.emissive);
        lightCom->Set_LightType(LIGHT_TYPE::POINT_LIGHT);
        lightCom->Set_LightRange(_ptltInfo.range);

        PointLight->Add_Component(lightCom);
    }
    EVENTMGR.Create_Object(PointLight);


    return PointLight;
}

void ImGui_Manager::Create_WallMesh()
{
    // 벽정보를 기반으로 벽메시 생성
    shared_ptr<Mesh> WallMesh = make_shared<Mesh>();
    WallMesh->Create3DRect(m_WallRectPosLDRU);

    // 메시를 기반으로 벽오브젝트 생성
    shared_ptr<GameObject> WallObject = make_shared<GameObject>();
    WallObject->Set_Name(L"Wall");
    WallObject->GetOrAddTransform();

    // 메시렌더러
    shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Mesh.fx"));
    renderer->Set_Mesh(WallMesh);

    // 메시를 통해 메시콜라이더 생성
    shared_ptr<MeshCollider> pCollider = make_shared<MeshCollider>(*WallMesh.get());
    WallObject->Add_Component(pCollider);
    pCollider->Set_Activate(true);
    //auto rigidBody = make_shared<RigidBody>();
    //rigidBody->Create_RigidBody(pCollider, WallObject->GetOrAddTransform()->Get_WorldMatrix());
    //WallObject->Add_Component(rigidBody);

    shared_ptr<GameObject> PreWall = CUR_SCENE->Get_GameObject(L"Wall");
    if (PreWall != nullptr)
        EVENTMGR.Delete_Object(PreWall);
    EVENTMGR.Create_Object(WallObject);
}

void ImGui_Manager::Create_GroundMesh()
{
    // 정보를 기반으로 바닥메시 생성
    shared_ptr<Mesh> GroundMesh = make_shared<Mesh>();
    GroundMesh->CreateGround(m_GroundRectPosLURD);

    // 메시를 기반으로 바닥오브젝트 생성
    shared_ptr<GameObject> GroundObject = make_shared<GameObject>();
    GroundObject->Set_Name(L"Ground");
    GroundObject->GetOrAddTransform();

    // 메시렌더러
    shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Mesh.fx"));
    renderer->Set_Mesh(GroundMesh);

    // 메시를 통해 메시콜라이더 생성
    shared_ptr<MeshCollider> pCollider = make_shared<MeshCollider>(*GroundMesh.get());
    GroundObject->Add_Component(pCollider);
    pCollider->Set_Activate(true);

    // 바닥 콜라이더 중복체크
    shared_ptr<GameObject> PrGround = CUR_SCENE->Get_GameObject(L"Ground");
    if (PrGround != nullptr)
        EVENTMGR.Delete_Object(PrGround);
    EVENTMGR.Create_Object(GroundObject);
}

void ImGui_Manager::Clear_WallMesh()
{
    m_WallRectPosLDRU.clear();
    m_bFirstWallPick = true;
}

void ImGui_Manager::Delete_WallMesh()
{
    m_WallRectPosLDRU.pop_back();
}

void ImGui_Manager::Delete_GroundMesh()
{
    m_GroundRectPosLURD.pop_back();
}

void ImGui_Manager::SetPlayerPosByCameraPos()
{
    m_PlayerCreatePosition = m_CamPos;
}

void ImGui_Manager::SetPlayerLookAtPosByPickingPos()
{
    m_PlayerLookAtPosition = m_PickingPos;
    m_PlayerLookAtPosition.y = m_PlayerCreatePosition.y;
}

void ImGui_Manager::Create_Terrain()
{
    // 터레인생성
    shared_ptr<Terrain> terrain = make_shared<Terrain>();
    terrain->CreateGrid(m_iTerrainSize[0], m_iTerrainSize[1]);

    Create_Terrain(terrain, m_iTerrainSize[0], m_iTerrainSize[1]);
}

void ImGui_Manager::Create_Terrain(shared_ptr<Terrain> _pTerrainMesh, _int _iTerrainSizeX, _int _iTerrainSizeY)
{
    /// 지형오브젝트 생성
    shared_ptr<GameObject> TerrainObject = make_shared<GameObject>();
    TerrainObject->Set_Name(L"Terrain");
    TerrainObject->GetOrAddTransform();

    // 하이트맵 계산해서 메시에 적용하기
    // TGA(HeightMap)텍스쳐를 가져와서 RGB값 모두 저장.
    //{
    //    // TGA 파일 경로 설정
    //    const wchar_t* filePath = L"..\\Resources\\Textures\\MapObject\\TerrainTile\\HeightMap.tga";
    //    // TGA 파일 로드
    //    TexMetadata metadata;
    //    ScratchImage image;
    //    LoadFromTGAFile(filePath, &metadata, image);

    //    // 픽셀 데이터 얻기
    //    const auto* img = image.GetImage(0, 0, 0); // 첫 이미지 데이터 가져오기
    //    // 픽셀 데이터에 접근하여 출력
    //    const uint8_t* pixels = static_cast<const uint8_t*>(img->pixels);
    //    size_t pixelSize = metadata.format == DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM ? 4 : 3; // RGBA 또는 RGB 픽셀 크기
    //    size_t rowPitch = img->rowPitch;

    //    // 모든픽셀의 색을 저장할 벡터
    //    vector<_float4> pixelColors;
    //    for (size_t i = 0; i < img->height ; ++i) {
    //        for (size_t j = 0; j < img->width; ++j) {
    //            size_t pixelIndex = i * rowPitch + j * pixelSize;
    //            _float4 pixelColor = _float4{ static_cast<_float>(pixels[pixelIndex + 0]) , static_cast<_float>(pixels[pixelIndex + 1]), static_cast<_float>(pixels[pixelIndex + 2]), static_cast<_float>(pixels[pixelIndex + 3]) };
    //            pixelColors.push_back(pixelColor);
    //        }
    //    }

    //    // 정점들의 w
    //    auto& vertices = _pTerrainMesh->Get_Geometry()->Get_Vertices();
    //    auto tempVertices = vertices;
    //    for (size_t i = 0; i < vertices.size(); ++i)
    //    {
    //        tempVertices[i].vPosition.y = pixelColors[i].z * 0.04;
    //    }
    //    _pTerrainMesh->Get_Geometry()->Set_Vertices(tempVertices);
    //    _pTerrainMesh->Create_Buffer();
    //}

    // 메시렌더러
    shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Terrain.fx"));
    renderer->Set_Mesh(_pTerrainMesh);
    //renderer->Set_Pass(18); // 터레인패스
    renderer->SetInt(0, _iTerrainSizeX);
    renderer->SetInt(1, _iTerrainSizeY);

	//shared_ptr<TerrainRenderer> renderer = make_shared<TerrainRenderer>(RESOURCES.Get<Shader>(L"Shader_Terrain.fx"));
	//renderer->CreateGrid(_iTerrainSizeX, _iTerrainSizeY);

    // 디퓨즈텍스쳐
    shared_ptr<Material> material = make_shared<Material>();
    auto Grasstexture = RESOURCES.Get<Texture>(L"TileGrass");
    if (Grasstexture == nullptr)
    {
        MSG_BOX("NoDiffuseTexture");
        return;
    }
    material->Set_TextureMap(Grasstexture, TextureMapType::DIFFUSE);

    // 노말텍스쳐
    auto Normaltexture = RESOURCES.Get<Texture>(L"ForestGrass_01_N_ASB");
    if (Normaltexture == nullptr)
    {
        MSG_BOX("NoNormalTexture");
        return;
    }
    material->Set_TextureMap(Normaltexture, TextureMapType::NORMAL);

    // Mask텍스쳐
    auto Masktexture = RESOURCES.Get<Texture>(L"TileMask");
    if (Masktexture == nullptr)
    {
        MSG_BOX("NoMaskTexture");
        return;
    }
    material->Set_TextureMap(Masktexture, TextureMapType::TEXTURE7);

    // Road텍스쳐
    auto Roadtexture = RESOURCES.Get<Texture>(L"TileRoad");
    if (Roadtexture == nullptr)
    {
        MSG_BOX("NoSubTexture");
        return;
    }
    material->Set_TextureMap(Roadtexture, TextureMapType::TEXTURE8);

	{
		auto HeightMap = RESOURCES.GetOrAddTexture(L"HeightMap1", L"..\\Resources\\Textures\\MapObject\\TerrainTile\\HeightMap.png");
		if (HeightMap == nullptr)
		{
			MSG_BOX("NoHeightTexture");
			return;
		}
		material->Set_TextureMap(HeightMap, TextureMapType::TEXTURE9);
	}

    renderer->Set_Material(material);

    // 메시를 통해 메시콜라이더 생성
    shared_ptr<MeshCollider> pCollider = make_shared<MeshCollider>(*_pTerrainMesh.get());
    TerrainObject->Add_Component(pCollider);
    pCollider->Set_Activate(true);

    TerrainObject->Add_Component(renderer);

    // 터레인오브젝트 중복체크
    shared_ptr<GameObject> PrTerrain = CUR_SCENE->Get_GameObject(L"Terrain");
    if (PrTerrain != nullptr)
        EVENTMGR.Delete_Object(PrTerrain);
    EVENTMGR.Create_Object(TerrainObject);

    m_pTerrain = TerrainObject;
    // 터레인의 가로세로 정보 셰이더에 떤져주기
    //m_pTerrain->Get_MeshRenderer()->SetVec2(0, _float2{ (_float)_iTerrainSizeX, (_float)_iTerrainSizeY });
}

void ImGui_Manager::Create_MaskTexture()
{
    ID3D11Texture2D* pTexture2D = { nullptr };

    D3D11_TEXTURE2D_DESC TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    TextureDesc.Width = 256;
    TextureDesc.Height = 256;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_R16G16B16A16_UNORM;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    // 맵언맵을 사용하기위해 다이나믹.
    TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
    // 용도 - 쉐이더리소스
    TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    TextureDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    TextureDesc.MiscFlags = 0;

    uint16_t* pPixel = new uint16_t[TextureDesc.Width * TextureDesc.Height * 4];

    D3D11_SUBRESOURCE_DATA SubResourceData;
    ZeroMemory(&SubResourceData, sizeof SubResourceData);

    SubResourceData.pSysMem = pPixel;
    // 한줄짜리 데이터가 아니기 때문에 한줄의 사이즈를 지정해줌.
    SubResourceData.SysMemPitch = TextureDesc.Width * 4;

    if (FAILED(DEVICE->CreateTexture2D(&TextureDesc, &SubResourceData, &pTexture2D)))
    {
        delete[] pPixel;
        return;
    }

    auto& terrainVertices = CUR_SCENE->Get_GameObject(L"Terrain")->Get_Collider()->Get_Meshes().front()->Get_Geometry()->Get_Vertices();
   
    for (_uint i = 0; i < TextureDesc.Height; ++i)
    {
        for (_uint j = 0; j < TextureDesc.Width; j++)
        {
            _uint iIndex = i * TextureDesc.Width + j;

            _float terrainHeight = terrainVertices[(TextureDesc.Width - i - 1) * TextureDesc.Width + j].vPosition.y + 50.f;
            terrainHeight *= 0.01f;
           pPixel[iIndex * 4 +0] = static_cast<std::uint16_t>(terrainHeight * 65535.0f);
           pPixel[iIndex * 4 +1] = static_cast<std::uint16_t>(terrainHeight * 65535.0f);
           pPixel[iIndex * 4 +2] = static_cast<std::uint16_t>(terrainHeight * 65535.0f);
           pPixel[iIndex * 4 +3] =65535.0f;
        }
    }

    D3D11_MAPPED_SUBRESOURCE MappedSubResource;

    CONTEXT->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedSubResource);

    memcpy(MappedSubResource.pData, pPixel, sizeof(uint16_t) * TextureDesc.Width * TextureDesc.Height * 4);

    CONTEXT->Unmap(pTexture2D, 0);

    ScratchImage scratchImage;
    CaptureTexture(DEVICE.Get(), CONTEXT.Get(), pTexture2D, scratchImage);

    wstring wstrName = L"..\\Resources\\Textures\\MapObject\\TerrainTile\\HeightMap.png";
    auto result = SaveToWICFile(scratchImage.GetImages(),scratchImage.GetImageCount(),WIC_FLAGS_NONE, GetWICCodec(WIC_CODEC_PNG), wstrName.data());

    delete[] pPixel;
}

void ImGui_Manager::Set_TerrainHeight()
{
    auto& vtx = m_pTerrain->Get_MeshRenderer()->Get_Mesh()->Get_Geometry()->Get_Vertices();
    auto tempVTX = vtx;
    for (auto& VB : tempVTX)
    {
        m_vTerrainBrushPosition.y = VB.vPosition.y; // 같은높이라고 가정.
        if ((VB.vPosition - m_vTerrainBrushPosition).Length() <= m_fTerrainBrushRadius)
        {
            VB.vPosition.y = m_fTerrainSetHeight;
        }
    }
    m_pTerrain->Get_MeshRenderer()->Get_Mesh()->Get_Geometry()->Set_Vertices(tempVTX);
    m_pTerrain->Get_MeshRenderer()->Get_Mesh()->Create_Buffer();
}

HRESULT ImGui_Manager::Delete_PointLight()
{
    // 1. 현재씬에서 제거
    EVENTMGR.Delete_Object(m_pPointLightObjects[m_iPointLightIndex]);
    // 2. 현재 선택된 오브젝트를 벡터에서 삭제
    {
        auto iter = m_pPointLightObjects.begin();
        for (_int i = 0; i < m_iPointLightIndex; ++i)
            ++iter;
        // 리스트벡터에서 해당내용삭제
        if (m_iPointLightIndex > 0)
            iter = m_pPointLightObjects.erase(iter);
        else
            m_pPointLightObjects.erase(m_pPointLightObjects.begin());
    }
    // 3. 현재 배치된 오브젝트 List 이름 제거
    {
        auto iter = m_strPointLightList.begin();
        for (_int i = 0; i < m_iPointLightIndex; ++i)
            ++iter;
        // 리스트벡터에서 해당내용삭제
        if (m_iPointLightIndex > 0)
        {
            iter = m_strPointLightList.erase(iter);
            // 리스트의 지정위치 변경
            --m_iPointLightIndex;
        }
        else
        {
            m_strPointLightList.erase(m_strPointLightList.begin());
        }
    }

    return S_OK;
}

HRESULT ImGui_Manager::Delete_MapObject()
{
    // 1. 현재 선택된 오브젝트를 씬에서 삭제
    EVENTMGR.Delete_Object(m_pMapObjects[m_iObjects]);

    // 2. 현재 선택된 오브젝트를 벡터에서 삭제
    {
        auto iter = m_pMapObjects.begin();
        for (_int i = 0; i < m_iObjects; ++i)
            ++iter;
        // 리스트벡터에서 해당내용삭제
        if (m_iObjects > 0)
            iter = m_pMapObjects.erase(iter);
        else
            m_pMapObjects.erase(m_pMapObjects.begin());
    }

    // 3. 현재 배치된 오브젝트 List 이름 제거
    {
        auto iter = m_strObjectName.begin();
        for (_int i = 0; i < m_iObjects; ++i)
            ++iter;
        // 리스트벡터에서 해당내용삭제
        if (m_iObjects > 0)
        {
            iter = m_strObjectName.erase(iter);
            // 리스트의 지정위치 변경
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
    // 세이브전 컬링계산, 그림자, 블러 처리
    BakeAll();

    // 세이브 파일 이름으로 저장하기
    string strFileName = m_szSaveFileName;
    string strFilePath = "..\\Resources\\MapData\\";
    strFilePath += strFileName + ".dat";

    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(Utils::ToWString(strFilePath), FileMode::Write);

    // 스카이박스 정보 저장
    string strSkyBoxTextureName = m_strSkyboxList[m_iCurrentSkyBoxIndex];
    file->Write<string>(strSkyBoxTextureName);

    // 방향성광원 정보 저장
    file->Write<_float4>(m_DirectionalLightPos);
    file->Write<_float3>(m_DirectionalLightLookAtPos);
    file->Write<LightColor>(m_DirectionalLightInfo.color);

    // 점광원 정보 저장
    file->Write<_int>((_int)m_pPointLightObjects.size());
    for (auto& PtLtObject : m_pPointLightObjects)
    {
        _float4 PtLtPosition = PtLtObject->Get_Transform()->Get_State(Transform_State::POS);
        file->Write<_float4>(PtLtPosition);
        LightInfo& ptltInfo = PtLtObject->Get_Light()->Get_LightInfo();
        file->Write<LightColor>(ptltInfo.color);
        file->Write<_float>(ptltInfo.range);

        // 점광원 울렁울렁효과 관련 정보
        weak_ptr<PointLightScript> pPTLTEffect = PtLtObject->Get_Script<PointLightScript>();
        if (!pPTLTEffect.expired())
        {
            file->Write<_bool>(pPTLTEffect.lock()->Get_bUseEffect());
            if(pPTLTEffect.lock()->Get_bUseEffect())
            {
                file->Write<_float4>(pPTLTEffect.lock()->Get_AmbientStart());
                file->Write<_float4>(pPTLTEffect.lock()->Get_AmbientTarget());
                file->Write<_float4>(pPTLTEffect.lock()->Get_DiffuseStart());
                file->Write<_float4>(pPTLTEffect.lock()->Get_DiffuseTarget());
                file->Write<_float>(pPTLTEffect.lock()->Get_StartRange());
                file->Write<_float>(pPTLTEffect.lock()->Get_TargetRange());
                file->Write<_float>(pPTLTEffect.lock()->Get_Speed());
            }
        }
    }

    // 벽메시콜라이더에 필요한 정보 저장
    file->Write<_int>((_int)m_WallRectPosLDRU.size());
    for (_int i = 0; i < m_WallRectPosLDRU.size(); ++i)
    {
        file->Write<pair<_float3, _float3>>(m_WallRectPosLDRU[i]);
    }
    // 바닥메시콜라이더에 필요한 정보 저장
    file->Write<_int>((_int)m_GroundRectPosLURD.size());
    for (_int i = 0; i < m_GroundRectPosLURD.size(); ++i)
    {
        file->Write<pair<_float3, _float3>>(m_GroundRectPosLURD[i]);
    }

    // 맵오브젝트 정보 저장
    // 1. 오브젝트 개수 저장
    file->Write<_int>((_int)m_pMapObjects.size());

    // 애님오브젝트 인덱스관리
    _int iAnimObjectIndex = 0;
    // 논애님오브젝트 인덱스관리
    _int iNonAnimObjectIndex = 0;
    // 2. 오브젝트의 MapObjectDesc 모든정보 저장
    for (_uint i = 0; i < m_pMapObjects.size(); ++i)
    {
        MapObjectScript::MapObjectDesc& MapDesc = m_pMapObjects[i]->Get_Script<MapObjectScript>()->Get_DESC();
        
        // 애님모델은 이름에 숫자를 붙여서 저장
        if (MapDesc.strName.find("Anim_") != std::string::npos)
        {
            // 고유번호를 제거하여 모델명을 얻어옴
            _int iPureNameSize = 0;
            while (MapDesc.strName[iPureNameSize] != '-' && iPureNameSize < MapDesc.strName.size())
            {
                ++iPureNameSize;
            }
            string strModelName = MapDesc.strName.substr(0, iPureNameSize);

            MapDesc.strName = strModelName + "-" + to_string(iAnimObjectIndex++);
        }
        else
            // 논애님모델은 논애님숫자를 붙임
        {
            // 고유번호를 제거하여 모델명을 얻어옴
            _int iPureNameSize = 0;
            while (MapDesc.strName[iPureNameSize] != '-' && iPureNameSize < MapDesc.strName.size())
            {
                ++iPureNameSize;
            }
            string strModelName = MapDesc.strName.substr(0, iPureNameSize);
            // 비법코드 ( -가 안나왔을때 \0이 들어가서 뭔가가 뭔가한 오류 해결하는 비법코드
            if (strModelName.back() == '\0')
                strModelName.pop_back();
            MapDesc.strName = strModelName + "-" + to_string(iNonAnimObjectIndex++);
        }
        file->Write<string>(MapDesc.strName);
        file->Write<_float>(MapDesc.fUVWeight);
        file->Write<_bool>(MapDesc.bShadow);
        file->Write<_bool>(MapDesc.bBlur);
        file->Write<_bool>(MapDesc.bTransform);
        if (MapDesc.bTransform)
        {
            MapDesc.WorldMatrix = m_pMapObjects[i]->Get_Transform()->Get_WorldMatrix();
            file->Write<_float4x4>(MapDesc.WorldMatrix);
        }
        file->Write<_bool>(MapDesc.bCollider);
        if (MapDesc.bCollider)
        {
            file->Write<_int>(MapDesc.ColliderType);
            file->Write<_float3>(MapDesc.ColliderOffset);
            switch (MapDesc.ColliderType)
            {
            case static_cast<_uint>(ColliderType::Sphere):
                file->Write<_float>(MapDesc.ColRadius);
                break;
            case static_cast<_uint>(ColliderType::AABB):
                file->Write<_float3>(MapDesc.ColBoundingSize);
                break;
            case static_cast<_uint>(ColliderType::OBB):
                file->Write<_float3>(MapDesc.ColBoundingSize);
                break;
            case static_cast<_uint>(ColliderType::Mesh):
                //file->Write<string>(MapDesc.ColModelName);
                break;
            default:
                break;
            }
        }
        file->Write<_float3>(MapDesc.CullPos);
        file->Write<_float>(MapDesc.CullRadius);
        file->Write<_char>(MapDesc.bCullNone);

        file->Write<_float4x4>(MapDesc.matDummyData);
    }

    // 플레이어의 시작위치 저장.
    m_PlayerCreatePosition.w = 1.f;
    file->Write<_float4>(m_PlayerCreatePosition);
    // 플레이어 룩앳포지션 저장
    m_PlayerLookAtPosition.w = 1.f;
    file->Write<_float4>(m_PlayerLookAtPosition);

// 셰이더옵션 저장
    //RenderOption
    file->Write<_float>(CUR_SCENE->g_fBrightness);
    file->Write<_float>(CUR_SCENE->g_fContrast);
    file->Write<_float>(CUR_SCENE->g_Saturation);
    //Bloom
    file->Write<_bool>(CUR_SCENE->g_BloomData.g_BloomOn);
    file->Write<_float>(CUR_SCENE->g_BloomData.g_BloomMin);
    //TonMapping
    file->Write<_int>(CUR_SCENE->g_iTMIndex);
    file->Write<_float>(CUR_SCENE->g_fMaxWhite);
    //SSAO
    file->Write<_bool>(CUR_SCENE->g_SSAOData.g_bSSAO_On);
    file->Write<_float>(CUR_SCENE->g_SSAOData.g_fOcclusionRadius);
    file->Write<_float>(CUR_SCENE->g_SSAOData.g_OcclusionFadeStart);
    file->Write<_float>(CUR_SCENE->g_SSAOData.g_OcclusionFadeEnd);
    //MotionBlur
    file->Write<_bool>(CUR_SCENE->g_MotionBlurData.g_bMotionBlurOn);
    file->Write<_int>(CUR_SCENE->g_MotionBlurData.g_iBlurCount);
    //FogOption
    file->Write<_bool>(CUR_SCENE->g_FogData.g_FogOn);
    file->Write<_float>(CUR_SCENE->g_FogData.g_fogStart);
    file->Write<_float>(CUR_SCENE->g_FogData.g_fogEnd);
    file->Write<_float>(CUR_SCENE->g_FogData.g_fogDensity);
    file->Write<_int>(CUR_SCENE->g_FogData.g_fogMode);
    file->Write<Color>(CUR_SCENE->g_FogData.g_fogColor);
    //LensFlare
    file->Write<_bool>(CUR_SCENE->g_bLensFlare);
    //DOF
    file->Write<_bool>(CUR_SCENE->g_DOFData.g_bDOF_On);
    file->Write<_float>(CUR_SCENE->g_DOFData.g_FocusDepth);
    file->Write<_float>(CUR_SCENE->g_DOFData.g_DOFRange);
    //LightOption
    file->Write<_float>(CUR_SCENE->g_LightPowerData.g_specularPower);
    file->Write<_float>(CUR_SCENE->g_LightPowerData.g_rimPower);
    file->Write<Color>(CUR_SCENE->Get_Light()->Get_Light()->Get_LightInfo().color.diffuse);
    //OtherOption
    file->Write<_bool>(CUR_SCENE->g_bDrawOutline);
    file->Write<_bool>(CUR_SCENE->g_bFXAAOn);
    file->Write<_bool>(CUR_SCENE->g_bAberrationOn);
    file->Write<_bool>(CUR_SCENE->g_bPBR_On);
    file->Write<_float>(CUR_SCENE->g_lightAttenuation);
    file->Write<_float>(CUR_SCENE->g_ambientRatio);

    return S_OK;
}

HRESULT ImGui_Manager::Load_MapObject()
{
    // 기존의 포인트라이트 모두삭제 및 클리어
    _int iPtltSize = (_int)m_pPointLightObjects.size();
    for (auto& ptltObj : m_pPointLightObjects)
        EVENTMGR.Delete_Object(ptltObj);
    m_pPointLightObjects.clear();
    m_strPointLightList.clear();
    m_iPointLightIndex = 0;

    // 현재갖고있는 오브젝트개수
    _uint iSize = (_int)m_pMapObjects.size();
    // 1. 기존의 모든오브젝트 삭제 및 Clear
    for (auto& mapObj : m_pMapObjects)
        EVENTMGR.Delete_Object(mapObj);
    m_pMapObjects.clear();
    m_strObjectName.clear();
    m_strObjectNamePtr.clear();
    m_iObjects = 0;

    // 세이브 파일 이름으로 로드하기
    string strFileName = m_MapNames[curMapIndex];
    string strFilePath = "..\\Resources\\MapData\\";
    strFilePath += strFileName + ".dat";
    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(Utils::ToWString(strFilePath), FileMode::Read);

    // 스카이박스 이름 가져오고 적용하기.
    wstring strSkyBoxTextureName = Utils::ToWString(file->Read<string>());
    vector<shared_ptr<Material>>& Mats = CUR_SCENE->Get_GameObject(L"SkyBase")->Get_ModelRenderer()->Get_Model()->Get_Materials();
    Mats[0]->Set_TextureMap(RESOURCES.Get<Texture>(strSkyBoxTextureName), TextureMapType::DIFFUSE);
    for (size_t i = 0; i < m_strSkyboxList.size(); ++i)
    {
        string strSkyBoxName = m_strSkyboxList[i];
        wstring wstrSkyBoxName = Utils::ToWString(strSkyBoxName);
        if (strSkyBoxTextureName == wstrSkyBoxName)
        {
            m_iCurrentSkyBoxIndex = (_int)i;
            break;
        }
    }

    // 방향성광원정보 가져오고 적용하기
    // 포지션
    shared_ptr<GameObject> DirectionalLightObject = CUR_SCENE->Get_Light();
    _float4 DirLightPos = _float4{ 0.f, 0.f, 0.f, 1.f };
    file->Read<_float4>(DirLightPos);
    DirectionalLightObject->Get_Transform()->Set_State(Transform_State::POS, DirLightPos);
    // 보는방향
    _float3 DirLightLookDir = _float3{ 0.f, 0.f, 0.f };
    file->Read<_float3>(DirLightLookDir);
    m_DirectionalLightLookAtPos = DirLightLookDir;
    DirectionalLightObject->Get_Transform()->LookAt(XMVectorSetW(DirLightLookDir,1.f));
    // 색깔
    LightColor DirLightColor;
    file->Read<LightColor>(DirLightColor);
    DirectionalLightObject->Get_Light()->Set_Ambient(DirLightColor.ambient);
    DirectionalLightObject->Get_Light()->Set_Diffuse(DirLightColor.diffuse);
    DirectionalLightObject->Get_Light()->Set_Specular(DirLightColor.specular);
    DirectionalLightObject->Get_Light()->Set_Emissive(DirLightColor.emissive);

    // 점광원정보 가져오고 불러오기
    _int iNumPointLight = file->Read<_int>();
    for (_int i = 0; i < iNumPointLight; ++i)
    {
        LightInfo loadPointLightInfo;
        file->Read<_float4>(loadPointLightInfo.vPosition);
        file->Read<LightColor>(loadPointLightInfo.color);
        file->Read<_float>(loadPointLightInfo.range);
        shared_ptr<GameObject> CreatePtltObj = Create_PointLight(loadPointLightInfo);
        m_pPointLightObjects.push_back(CreatePtltObj);
        m_strPointLightList.push_back("PointLight");

        // 점광원효과 기본초기화
        shared_ptr<PointLightScript> pPLE = make_shared<PointLightScript>();
        m_pPointLightObjects.back()->Add_Component(pPLE);
        pPLE->Clear();

        // 점광원 효과정보
        _bool bUseEffect = false;
        file->Read<_bool>(bUseEffect);
        pPLE->Set_bUseEffect(bUseEffect);
        if (bUseEffect) // 점광원효과가 있는 녀석이라면
        {
            _float4 tempColor = { 1.f, 1.f, 1.f, 1.f };
            file->Read<_float4>(tempColor);
            pPLE->Set_AmbientStart(tempColor);
            file->Read<_float4>(tempColor);
            pPLE->Set_AmbientTarget(tempColor);
            file->Read<_float4>(tempColor);
            pPLE->Set_DiffuseStart(tempColor);
            file->Read<_float4>(tempColor);
            pPLE->Set_DiffuseTarget(tempColor);
            _float tempFloat = { 0.f };
            file->Read<_float>(tempFloat);
            pPLE->Set_StartRange(tempFloat);
            file->Read<_float>(tempFloat);
            pPLE->Set_TargetRange(tempFloat);
            file->Read<_float>(tempFloat);
            pPLE->Set_Speed(tempFloat);
            // 랜덤으로 델타타임과 PM세팅
            pPLE->Set_DeltaTime(Utils::Random_In_Range(0.f, 1.f));
            pPLE->Set_DeltaPM(rand() % 2 > 0 ? 1.f : -1.f);
        }
    }

    // 벽정보 불러오기 및 벽생성
    m_WallRectPosLDRU.clear();
    _int iNumWall = 0;
    file->Read<_int>(iNumWall);
    if(iNumWall != 0)
    {
        m_WallRectPosLDRU.resize(iNumWall);
        for (_int i = 0; i < iNumWall; ++i)
            file->Read<pair<_float3, _float3>>(m_WallRectPosLDRU[i]);
        Create_WallMesh();
    }
    // 바닥정보 불러오기 및 바닥생성
    m_GroundRectPosLURD.clear();
    _int iNumGround = 0;
    file->Read<_int>(iNumGround);
    if (iNumGround != 0)
    {
        m_GroundRectPosLURD.resize(iNumGround);
        for (_int i = 0; i < iNumGround; ++i)
            file->Read<pair<_float3, _float3>>(m_GroundRectPosLURD[i]);
        Create_GroundMesh();
    }

    // 오브젝트 개수 불러오기
    _int iNumObjects = file->Read<_int>();
    for (_int i = 0; i < iNumObjects; ++i)
    {
        MapObjectScript::MapObjectDesc MapDesc;
        wstring strObjectName = Utils::ToWString(file->Read<string>());
        MapDesc.strName = Utils::ToString(strObjectName);
        file->Read<_float>(MapDesc.fUVWeight);
        file->Read<_bool>(MapDesc.bShadow);
        file->Read<_bool>(MapDesc.bBlur);
        file->Read<_bool>(MapDesc.bTransform);
        if (MapDesc.bTransform)
            file->Read<_float4x4>(MapDesc.WorldMatrix);
        file->Read<_bool>(MapDesc.bCollider);
         if (MapDesc.bCollider)
        {
            file->Read<_int>(MapDesc.ColliderType);
            file->Read<_float3>(MapDesc.ColliderOffset);
            switch (static_cast<ColliderType>(MapDesc.ColliderType))
            {
            case ColliderType::Sphere:
                file->Read<_float>(MapDesc.ColRadius);
                break;
            case ColliderType::AABB:
                file->Read<_float3>(MapDesc.ColBoundingSize);
                break;
            case ColliderType::OBB:
                file->Read<_float3>(MapDesc.ColBoundingSize);
                break;
            case ColliderType::Mesh:
                //file->Read<string>(MapDesc.ColModelName);
                break;
            default:
                break;
            }
        }
        file->Read<_float3>(MapDesc.CullPos);
        file->Read<_float>(MapDesc.CullRadius);
        file->Read<_char>(MapDesc.bCullNone);
        file->Read<_float4x4>(MapDesc.matDummyData);
        
        shared_ptr<GameObject> CreateObject = Create_MapObject(MapDesc);
        
        m_pMapObjects.push_back(CreateObject);
        // 현재 설치된 오브젝트 이름 UI에 추가
        WCHAR szTempName[MAX_PATH];
        //lstrcpy(szTempName, strObjectName.c_str());
        lstrcpy(szTempName, Utils::ToWString(MapDesc.strName).c_str());
        // char 형식으로 변환
        shared_ptr<char[]> pChar = shared_ptr<char[]>(new char[MAX_PATH]);
        WideCharToMultiByte(CP_ACP, 0, szTempName, -1, pChar.get(), MAX_PATH, 0, 0);
        m_strObjectNamePtr.push_back(pChar);
        m_strObjectName.push_back(pChar.get());
    }

    // 플레이어 시작지점, 피킹포즈
    file->Read<_float4>(m_PlayerCreatePosition);
    m_PlayerCreatePosition.w = 1.f;
    file->Read<_float4>(m_PlayerLookAtPosition);
    m_PlayerLookAtPosition.w = 1.f;
    // 그 위치방향으로 카메라 세팅
    if(m_PlayerCreatePosition != m_PlayerLookAtPosition)
    {
        CUR_SCENE->Get_MainCamera()->Get_Transform()->Set_State(Transform_State::POS, m_PlayerCreatePosition);
        CUR_SCENE->Get_MainCamera()->Get_Transform()->LookAt(m_PlayerLookAtPosition);
    }

// 셰이더옵션 로드
    //RenderOption
    file->Read<_float>(CUR_SCENE->g_fBrightness);
    file->Read<_float>(CUR_SCENE->g_fContrast);
    file->Read<_float>(CUR_SCENE->g_Saturation);
    //Bloom
    file->Read<_bool>(CUR_SCENE->g_BloomData.g_BloomOn);
    file->Read<_float>(CUR_SCENE->g_BloomData.g_BloomMin);
    //TonMapping
    file->Read<_int>(CUR_SCENE->g_iTMIndex);
    file->Read<_float>(CUR_SCENE->g_fMaxWhite);
    //SSAO
    file->Read<_bool>(CUR_SCENE->g_SSAOData.g_bSSAO_On);
    file->Read<_float>(CUR_SCENE->g_SSAOData.g_fOcclusionRadius);
    file->Read<_float>(CUR_SCENE->g_SSAOData.g_OcclusionFadeStart);
    file->Read<_float>(CUR_SCENE->g_SSAOData.g_OcclusionFadeEnd);
    //MotionBlur
    file->Read<_bool>(CUR_SCENE->g_MotionBlurData.g_bMotionBlurOn);
    file->Read<_int>(CUR_SCENE->g_MotionBlurData.g_iBlurCount);
    //FogOption
    file->Read<_bool>(CUR_SCENE->g_FogData.g_FogOn);
    file->Read<_float>(CUR_SCENE->g_FogData.g_fogStart);
    file->Read<_float>(CUR_SCENE->g_FogData.g_fogEnd);
    file->Read<_float>(CUR_SCENE->g_FogData.g_fogDensity);
    file->Read<_int>(CUR_SCENE->g_FogData.g_fogMode);
    file->Read<Color>(CUR_SCENE->g_FogData.g_fogColor);
    //LensFlare
    file->Read<_bool>(CUR_SCENE->g_bLensFlare);
    //DOF
    file->Read<_bool>(CUR_SCENE->g_DOFData.g_bDOF_On);
    file->Read<_float>(CUR_SCENE->g_DOFData.g_FocusDepth);
    file->Read<_float>(CUR_SCENE->g_DOFData.g_DOFRange);
    //LightOption
    file->Read<_float>(CUR_SCENE->g_LightPowerData.g_specularPower);
    file->Read<_float>(CUR_SCENE->g_LightPowerData.g_rimPower);
    file->Read<Color>(CUR_SCENE->Get_Light()->Get_Light()->Get_LightInfo().color.diffuse);
    //OtherOption
    file->Read<_bool>(CUR_SCENE->g_bDrawOutline);
    file->Read<_bool>(CUR_SCENE->g_bFXAAOn);
    file->Read<_bool>(CUR_SCENE->g_bAberrationOn);
    file->Read<_bool>(CUR_SCENE->g_bPBR_On);
    file->Read<_float>(CUR_SCENE->g_lightAttenuation);
    file->Read<_float>(CUR_SCENE->g_ambientRatio);

    return S_OK;
}

HRESULT ImGui_Manager::Save_ModelNames()
{
    // 세이브 파일 이름으로 저장하기
    string strFilePath = "..\\Resources\\MapData\\";
    string strTextName = m_szSaveFileName;
    strFilePath = strFilePath + strTextName + ".txt";
    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(Utils::ToWString(strFilePath), FileMode::Write);

    for (_int i = 0; i < m_strObjectName.size(); ++i)
    {
        string strLastObjName = m_strObjectName[i];
        if (i == 0)
        {
            strLastObjName = strLastObjName + "\n";
            file->Write<string>(strLastObjName);
            continue;
        }
        for (_int j = i - 1; j >= 0; --j)
        {
            string strwndqhrTest = m_strObjectName[j];
            if (strwndqhrTest == strLastObjName)
                break;
            else if(j==0)
            {
                strLastObjName = strLastObjName + "\n";
                file->Write<string>(strLastObjName);
                break;
            }
        }
    }
    return S_OK;
}

HRESULT ImGui_Manager::Save_TerrainData()
{
    // 세이브 파일 이름으로 저장하기
    string strFilePath = "..\\Resources\\MapData\\TerrainData.TRdat";
    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(Utils::ToWString(strFilePath), FileMode::Write);

    shared_ptr<GameObject> TerrainObject = CUR_SCENE->Get_GameObject(L"Terrain");
    if (TerrainObject == nullptr)
    {
        MSG_BOX("NoTerrainObject");
        return E_FAIL;
    }
    auto TerrainGeometry = TerrainObject->Get_MeshRenderer()->Get_Mesh()->Get_Geometry();

    // 버텍스의 개수와 포지션,노말 저장
    auto& TerrainVertices = TerrainGeometry->Get_Vertices();
    file->Write<_int>((_int)TerrainVertices.size());
    for (size_t i = 0; i < TerrainVertices.size(); ++i)
    {
        file->Write<_float3>(TerrainVertices[i].vPosition);
        file->Write<_float2>(TerrainVertices[i].vTexCoord);
        file->Write<_float3>(TerrainVertices[i].vNormal);
        file->Write<_float3>(TerrainVertices[i].vTangent);
    }
    // 인덱스의 개수 저장
    auto& TerrainIndices = TerrainGeometry->Get_Indices();
    file->Write<_int>((_int)TerrainIndices.size());
    for (size_t i = 0; i < TerrainIndices.size(); i++)
    {
        file->Write<_uint>(TerrainIndices[i]);
    }

    // 가로세로 개수
    file->Write<_int>(m_iTerrainSize[0]);
    file->Write<_int>(m_iTerrainSize[1]);

    MSG_BOX("Save_Complete");

    return S_OK;
}

HRESULT ImGui_Manager::LoadAndCreateTerrain()
{
    shared_ptr<Terrain> loadedTerrain = make_shared<Terrain>();

    string strFilePath = "..\\Resources\\MapData\\TerrainData.TRdat";
    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(Utils::ToWString(strFilePath), FileMode::Read);

    vector<VTXTEXNORTANDATA> loadedVertices;
    // 버텍스의 개수와 포지션,노말 로드
    loadedVertices.resize(file->Read<_int>());
    for (size_t i = 0; i < loadedVertices.size(); ++i)
    {
        file->Read<_float3>(loadedVertices[i].vPosition);
        file->Read<_float2>(loadedVertices[i].vTexCoord);
        file->Read<_float3>(loadedVertices[i].vNormal);
        file->Read<_float3>(loadedVertices[i].vTangent);
    }

    // 인덱스의 개수와 정보 로드
    vector<_uint> loadedIndices;
    // 버텍스의 개수와 포지션,노말 로드
    loadedIndices.resize(file->Read<_int>());
    for (size_t i = 0; i < loadedIndices.size(); i++)
    {
        file->Read<_uint>(loadedIndices[i]);
    }

    _int sizeX = 0;
    _int sizeY = 0;
    file->Read<_int>(sizeX);
    file->Read<_int>(sizeY);
    m_iTerrainSize[0] = sizeX;
    m_iTerrainSize[1] = sizeY;

    // 터레인버퍼생성
    loadedTerrain->Get_Geometry()->Set_Vertices(loadedVertices);
    loadedTerrain->Get_Geometry()->Set_Indices(loadedIndices);
    loadedTerrain->Create_Buffer();

    Create_Terrain(loadedTerrain, sizeX, sizeY);

    return S_OK;
}

void ImGui_Manager::Cal_NormalTangent()
{
	auto terrain = CUR_SCENE->Get_GameObject(L"Terrain");

	auto mesh = terrain->Get_MeshRenderer()->Get_Mesh();
	auto& indices = mesh->Get_Geometry()->Get_Indices();
	auto& vertices = mesh->Get_Geometry()->Get_Vertices();

	for (_int i = 0; i < _int(indices.size());)
	{
		_int iIndex[3] = { indices[i++] ,indices[i++] ,indices[i++] };
		_float3 vVtxPos[3] = {
			  vertices[iIndex[0]].vPosition,
			  vertices[iIndex[1]].vPosition,
			  vertices[iIndex[2]].vPosition
		};

		_float3 vDir[3] = { vVtxPos[1] - vVtxPos[0] ,vVtxPos[2] - vVtxPos[1] ,vVtxPos[0] - vVtxPos[2] };

		_float3 vNormal = vDir[0].Cross(vDir[1]);
		vNormal.Normalize();
        _float3 vTangent = vDir[0];
        vTangent.Normalize();

		vertices[iIndex[0]].vNormal = vNormal;
		vertices[iIndex[1]].vNormal = vNormal;
		vertices[iIndex[2]].vNormal = vNormal;

        vertices[iIndex[0]].vTangent = vTangent;
        vertices[iIndex[1]].vTangent = vTangent;
        vertices[iIndex[2]].vTangent = vTangent;

	}

    mesh->Create_Buffer();

}

void ImGui_Manager::Create_WeedRegion()
{
    Create_Weed(m_PickingPos);
}

void ImGui_Manager::Create_Weed(_float3 _CreatePos)
{
    _int TerrainSize = 256;
    // 지형의 바깥이라면 return
    if (_CreatePos.x <= 0 || _CreatePos.x >= TerrainSize ||
        _CreatePos.z <= 0 || _CreatePos.z >= TerrainSize)
        return;

    _float3 TargetPos = _CreatePos;
    // 아래로 쏠거기때문에 하늘로 올려버리기
    TargetPos.y = 100.f;

    //auto& TerrainColliderMesh = CUR_SCENE->Get_GameObject(L"Terrain")->Get_Collider()->Get_Meshes();
    auto TerrainColliderMesh = CUR_SCENE->Get_GameObject(L"Terrain")->Get_MeshRenderer()->Get_Mesh();
    auto& Vertices = TerrainColliderMesh->Get_Geometry()->Get_Vertices();
    auto& Indices = TerrainColliderMesh->Get_Geometry()->Get_Indices();

    Ray ray = Ray(_CreatePos, _float3{ 0.f, -1.f, 0.f });
    
	// 연산 속도를 위해 정확한 버텍스의 위치를 바로 찾아가기
    // 왼쪽아래버텍스의 인덱스
	_int LDindex = (_int)_CreatePos.x * TerrainSize + (_int)_CreatePos.z;

    _float fDistance = 0.f;
    _float3 normal;
    // 타일과 피킹하여 최종생성되는 풀의위치
    _float3 FinalCreatePos;

    for(size_t i=0; i<2; ++i)
    {
        // 해당하는칸의 (0,1,2),(0,2,3)과 비교
        _float3 vVtxPos[3] = {
           Vertices[LDindex + TerrainSize].vPosition,
           Vertices[LDindex + TerrainSize + 1 + i].vPosition,
           Vertices[LDindex + 1 + i].vPosition
        };
        if (!ray.Intersects(vVtxPos[0], vVtxPos[1], vVtxPos[2], fDistance))
            continue;

        // 노말저장
        normal = XMVector3Normalize(XMVector3Cross(vVtxPos[1] - vVtxPos[0], vVtxPos[2] - vVtxPos[0]));
    }

    if(fDistance == 0.f)
    {
        MSG_BOX("RayIntersectsFail");
        return;
    }

    // 풀의 생성위치.
    FinalCreatePos = TargetPos + _float3{ 0.f, -1.f, 0.f } * fDistance;

    shared_ptr<Mesh> WeedMesh = RESOURCES.Get<Mesh>(L"Point");

    // 메시를 기반으로 벽오브젝트 생성
    shared_ptr<GameObject> WeedObj = make_shared<GameObject>();
    WeedObj->Set_Name(L"WeedObj");
    WeedObj->GetOrAddTransform();
    WeedObj->Get_Transform()->Set_State(Transform_State::POS, _float4{ FinalCreatePos, 1.f });

    // 메시렌더러
    shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Grass.fx"));
    renderer->Set_Mesh(WeedMesh);

    wstring WeedName = Utils::ToWString(m_strWeedCatalogue[m_iCurrentWeedIndex]);
    shared_ptr<Material> material = RESOURCES.Get<Material>(WeedName);
    renderer->Set_Material(material);

    WeedObj->Add_Component(renderer);

    shared_ptr<WeedScript> WeedSc = make_shared<WeedScript>();
    WeedObj->Add_Component(WeedSc);

    EVENTMGR.Create_Object(WeedObj);
}

_float4 ImGui_Manager::Compute_CullingData(shared_ptr<GameObject>& _pGameObject)
{
    // 모델을 받아와서 컬링포지션과 길이를 계산하여 반환 float4(_float(Pos), _float(Radius))
    // 모든 정점을 돌면서 XYZ각각의 min과 max를 찾아야함.
    _float3 vMaxPos = _float3(-FLT_MAX);
    _float3 vMinPos = _float3(FLT_MAX);

    shared_ptr<BoneDesc> boneDesc = make_shared<BoneDesc>();
    const _uint boneCount = _pGameObject->Get_Model()->Get_BoneCount();

    for (_uint i = 0; i < boneCount; ++i)
    {
        shared_ptr<ModelBone> bone = _pGameObject->Get_Model()->Get_BoneByIndex(i);
        boneDesc->transform[i] = (bone->transform) * Utils::m_matPivot;
    }

    const vector<shared_ptr<ModelMesh>>& Meshs = _pGameObject->Get_Model()->Get_Meshes();
    for (auto& pMesh : Meshs)
    {
        _uint boneIndex = pMesh->boneIndex;
        const vector<ModelVertexType>& vertices = pMesh->geometry->Get_Vertices();
        for (auto& VtxData : vertices)
        {
            // 월드행렬반영
            _float3 vPos = _float3::Transform(VtxData.vPosition, boneDesc->transform[boneIndex] * _pGameObject->Get_Transform()->Get_WorldMatrix());

            vMaxPos.x = max(vPos.x, vMaxPos.x);
            vMaxPos.y = max(vPos.y, vMaxPos.y);
            vMaxPos.z = max(vPos.z, vMaxPos.z);
            vMinPos.x = min(vPos.x, vMinPos.x);
            vMinPos.y = min(vPos.y, vMinPos.y);
            vMinPos.z = min(vPos.z, vMinPos.z);
        }
    }

    // Min과 Max를 더한 후 2로 나누기
    _float3 vCullPos = vMaxPos + vMinPos;
    vCullPos *= 0.5f;
    _float3 tempVector = vCullPos - vMinPos;
    _float vCullRadius = tempVector.Length();

    if(_pGameObject->Get_Script<MapObjectScript>() != nullptr)
    {
        _pGameObject->Get_Script<MapObjectScript>()->Get_DESC().CullPos = vCullPos;
        _pGameObject->Get_Script<MapObjectScript>()->Get_DESC().CullRadius = vCullRadius;
    }

    return XMVectorSetW(vCullPos, vCullRadius);
}

void ImGui_Manager::Bake(shared_ptr<GameObject>& _pGameObject)
{
    if (!_pGameObject->Get_Transform())
        return;
    if (!_pGameObject->Get_Script<MapObjectScript>())
        return;
    if (!_pGameObject->Get_Model())
        return;

    MapObjectScript::MapObjectDesc& MapObjDesc = _pGameObject->Get_Script<MapObjectScript>()->Get_DESC();

    _pGameObject->Set_DrawShadow(MapObjDesc.bShadow);
    _pGameObject->Set_Blur(MapObjDesc.bBlur);
    Compute_CullingData(_pGameObject);
}

void ImGui_Manager::BakeAll()
{
    const auto& gameObjects = CUR_SCENE->Get_Objects();
    for (auto gameObject : gameObjects)
    {
        Bake(gameObject);
    }
}

void ImGui_Manager::Create_SampleObjects()
{
        m_SampleObject = make_shared<GameObject>();
        m_SampleObject->GetOrAddTransform()->Set_State(Transform_State::POS, _float4{ 0.f, -100.f, 0.f, 1.f });
        string modelName = m_strObjectBaseNameList[m_iObjectBaseIndex];
        wstring strModelName = Utils::ToWString(modelName);
        m_SampleObject->Set_Name(L"Sample_" + strModelName);

        shared_ptr<Model> model = RESOURCES.Get<Model>(strModelName);
        if (model == nullptr)
            return;
        shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
        shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
        m_SampleObject->Add_Component(renderer);
        renderer->Set_Model(model);
        renderer->Set_PassType(ModelRenderer::PASS_MAPOBJECT);
        renderer->SetVec4(0, _float4(1.f));
        
        EVENTMGR.Create_Object(m_SampleObject);

        m_fSampleModelCullSize = Compute_CullingData(m_SampleObject).w;
}

void ImGui_Manager::Set_SampleObject()
{
    string modelName = m_strObjectBaseNameList[m_iObjectBaseIndex];
    wstring strModelName = Utils::ToWString(modelName);
    shared_ptr<Model> model = RESOURCES.Get<Model>(strModelName);
    if (model == nullptr)
        return;
    m_SampleObject->Set_Name(L"Sample_" + strModelName);

    m_SampleObject->Get_ModelRenderer()->Set_Model(model);

    m_fSampleModelCullSize = Compute_CullingData(m_SampleObject).w;
}

void ImGui_Manager::RenderOptionTab()
{
    if (BeginTabItem("Render Option"))
    {
        string fps = "FPS : " + to_string(TIME.GetFPS());
        Text(fps.c_str());
        if (CollapsingHeader("RenderOption"))
        {

            _float& g_fBrightness = CUR_SCENE->g_fBrightness;
            _float& g_fContrast = CUR_SCENE->g_fContrast;
            _float& g_Saturation = CUR_SCENE->g_Saturation;



            DragFloat("Brightness", &g_fBrightness, 0.01f, 0.f, 5.f);
            DragFloat("Contrast", &g_fContrast, 0.01f, 0.01f, 5.f);
            DragFloat("Saturation", &g_Saturation, 0.01f, 0.01f, 5.f);


        }
        if (CollapsingHeader("Bloom"))
        {
            Checkbox("Bloom On", &CUR_SCENE->g_BloomData.g_BloomOn);
            _float& g_fBloomMin = CUR_SCENE->g_BloomData.g_BloomMin;
            DragFloat("Bloom Min Value", &g_fBloomMin, 0.001f, 0.01f, 1.f);
        }
        if (CollapsingHeader("ToneMapping"))
        {
            _float& g_fMaxWhite = CUR_SCENE->g_fMaxWhite;
            _int& g_iTMIndex = CUR_SCENE->g_iTMIndex;

            static _int tmIndex = 0;
            InputInt("ToneMapping Mod", &tmIndex);

            if (tmIndex > 3) tmIndex %= 4;
            else if (tmIndex < 0) tmIndex += 4;

            g_iTMIndex = tmIndex;
            if (g_iTMIndex == 1)
                DragFloat("Max_White Value", &g_fMaxWhite, 0.1f, 0.01f, 5.f);
        }
        if (CollapsingHeader("SSAO"))
        {
            _bool& g_bSSAO_On = CUR_SCENE->g_SSAOData.g_bSSAO_On;
            Checkbox("SSAO On", &g_bSSAO_On);
            if (g_bSSAO_On)
            {
                _float& g_fOcclusionRadius = CUR_SCENE->g_SSAOData.g_fOcclusionRadius;
                _float& g_OcclusionFadeStart = CUR_SCENE->g_SSAOData.g_OcclusionFadeStart;
                _float& g_OcclusionFadeEnd = CUR_SCENE->g_SSAOData.g_OcclusionFadeEnd;

                DragFloat("SSAO Radius", &g_fOcclusionRadius, 0.01f, 0.0001f, 1.f);
                DragFloat("SSAO FadeStart", &g_OcclusionFadeStart, 0.01f, 0.0001f, g_OcclusionFadeEnd);
                DragFloat("SSAO FadeEnd", &g_OcclusionFadeEnd, 0.01f, g_OcclusionFadeStart, 1.f);
            }
        }
        if (CollapsingHeader("Motion Blur"))
        {
            Scene::MotionBlurData& data = CUR_SCENE->g_MotionBlurData;
            Checkbox("Motion Blur On", &data.g_bMotionBlurOn);
            InputInt("Motion Blur Count", &data.g_iBlurCount);

        }
        if (CollapsingHeader("Fog Option"))
        {
            _bool& g_FogOn = CUR_SCENE->g_FogData.g_FogOn;
            _float& gFogStart = CUR_SCENE->g_FogData.g_fogStart;
            _float& g_FogEnd = CUR_SCENE->g_FogData.g_fogEnd;
            _float& g_fogDensity = CUR_SCENE->g_FogData.g_fogDensity;
            _int& g_fogMode = CUR_SCENE->g_FogData.g_fogMode;
            Color& gColorFog = CUR_SCENE->g_FogData.g_fogColor;

            Checkbox("Fog On", &g_FogOn);
            DragFloat("Fog Start Range", &gFogStart, 1.f, 0.0001f, g_FogEnd);
            DragFloat("Fog End", &g_FogEnd, 1.f, gFogStart, 2000.f);
            DragFloat("Fog Density", &g_fogDensity, 0.001f, 0.001f, 1.f);
            InputInt("Fog Mod", &g_fogMode);
            if (g_fogMode < 0) g_fogMode = 0;
            if (g_fogMode > 2) g_fogMode = 2;
            static bool alpha_preview = true;
            static bool alpha_half_preview = false;
            static bool drag_and_drop = true;
            static bool options_menu = true;
            static bool hdr = true;

            ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
            ImGui::ColorEdit4("FogColor", (float*)&gColorFog, ImGuiColorEditFlags_DisplayHSV | misc_flags);

        }
        if (CollapsingHeader("Lens Flare Option"))
        {
            if (CUR_SCENE)
            {
                _bool& g_bLensFlare = CUR_SCENE->g_bLensFlare;
                Checkbox("LensFlare On", &g_bLensFlare);
            }
        }
        if (CollapsingHeader("DOF"))
        {
            Checkbox("DOF On", &CUR_SCENE->g_DOFData.g_bDOF_On);
            _float& g_FocusDepth = CUR_SCENE->g_DOFData.g_FocusDepth;
            _float& g_DOFRange = CUR_SCENE->g_DOFData.g_DOFRange;
            DragFloat("Focus Depth", &g_FocusDepth, 1.f, 0.1f, 1000.f);
            DragFloat("g_DOFRange", &g_DOFRange, 0.1f, 0.0001f, 1000.f);
        }
        if (CollapsingHeader("Light Option"))
        {
            _float& g_fSpecularPower = CUR_SCENE->g_LightPowerData.g_specularPower;
            _float& g_fRimPower = CUR_SCENE->g_LightPowerData.g_rimPower;
            DragFloat("Specular", &g_fSpecularPower, 0.1f, 0.01f);
            DragFloat("RimPower", &g_fRimPower, 0.1f, 0.01f);
            if (CUR_SCENE->Get_Light())
            {
                Color& g_lightDiffuse = CUR_SCENE->Get_Light()->Get_Light()->Get_LightInfo().color.diffuse;
                static bool alpha_preview = true;
                static bool alpha_half_preview = false;
                static bool drag_and_drop = true;
                static bool options_menu = true;
                static bool hdr = true;

                ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
                DragFloat4("color", (_float*)&g_lightDiffuse);

            }
        }
        if (CollapsingHeader("Other Option"))
        {
            SeparatorText("Outline");
            _bool& g_bOutline = CUR_SCENE->g_bDrawOutline;
            Checkbox("Outline On", &g_bOutline);

            SeparatorText("FXAA");
            _bool& g_bFXAAOn = CUR_SCENE->g_bFXAAOn;
            Checkbox("FXAA On", &g_bFXAAOn);

            SeparatorText("Aberration");
            _bool& g_bAberrationOn = CUR_SCENE->g_bAberrationOn;
            Checkbox("Aberration On", &g_bAberrationOn);
            if (g_bAberrationOn)
            {
                _float& g_fAberrationPower = CUR_SCENE->g_fAberrationPower;
                DragFloat("Aberration Power", &g_fAberrationPower, 1.f, -300.f, 300.f);
            }
            SeparatorText("PBR");
            _bool& g_bPBR_On = CUR_SCENE->g_bPBR_On;
            Checkbox("PBR On", &g_bPBR_On);
            _float& g_lightAttenuation = CUR_SCENE->g_lightAttenuation;
            _float& g_ambientRatio = CUR_SCENE->g_ambientRatio;
            DragFloat("g_lightAttenuation", &g_lightAttenuation);
            DragFloat("g_ambientRatio", &g_ambientRatio, 0.1f, 0.1f, 1.5f);
        }

        EndTabItem();
    }

    if (Button("ClearAllOptions"))
        ClearAllShaderOptions();
}

void ImGui_Manager::ClearAllShaderOptions()
{
    //RenderOption
    _float& g_fBrightness = CUR_SCENE->g_fBrightness = 0.f;
    _float& g_fContrast = CUR_SCENE->g_fContrast = 1.f;
    _float& g_Saturation = CUR_SCENE->g_Saturation = 1.f;
    //Bloom
    _bool& g_fBloomOn = CUR_SCENE->g_BloomData.g_BloomOn = false;
    _float& g_fBloomMin = CUR_SCENE->g_BloomData.g_BloomMin = 0.99f;
    //TonMapping
    _int& g_iTMIndex = CUR_SCENE->g_iTMIndex = 0;
    _float& g_fMaxWhite = CUR_SCENE->g_fMaxWhite = 1.f;
    //SSAO
    _bool& g_bSSAO_On = CUR_SCENE->g_SSAOData.g_bSSAO_On = true;
    _float& g_fOcclusionRadius = CUR_SCENE->g_SSAOData.g_fOcclusionRadius = 0.070f;
    _float& g_OcclusionFadeStart = CUR_SCENE->g_SSAOData.g_OcclusionFadeStart = 0.3f;
    _float& g_OcclusionFadeEnd = CUR_SCENE->g_SSAOData.g_OcclusionFadeEnd = 1.f;
    //MotionBlur
    Scene::MotionBlurData& data = CUR_SCENE->g_MotionBlurData;
    data.g_bMotionBlurOn = false;
    data.g_iBlurCount = 0;
    //FogOption
    _bool& g_FogOn = CUR_SCENE->g_FogData.g_FogOn = false;
    _float& gFogStart = CUR_SCENE->g_FogData.g_fogStart = 15.f;
    _float& g_FogEnd = CUR_SCENE->g_FogData.g_fogEnd = 150.f;
    _float& g_fogDensity = CUR_SCENE->g_FogData.g_fogDensity = 1.f;
    _int& g_fogMode = CUR_SCENE->g_FogData.g_fogMode = 0;
    Color& g_ColorFog = CUR_SCENE->g_FogData.g_fogColor = Color(1.f);
    //LensFlare
    _bool& g_bLensFlare = CUR_SCENE->g_bLensFlare = false;
    //DOF
    _bool& g_bDOF_On = CUR_SCENE->g_DOFData.g_bDOF_On = false;
    _float & g_FocusDepth = CUR_SCENE->g_DOFData.g_FocusDepth = 7.f;
    _float& g_DOFRange = CUR_SCENE->g_DOFData.g_DOFRange = 100.f;
    //LightOption
    _float& g_fSpecularPower = CUR_SCENE->g_LightPowerData.g_specularPower = 10.f;
    _float& g_fRimPower = CUR_SCENE->g_LightPowerData.g_rimPower = 10.f;
    Color& g_lightDiffuse = CUR_SCENE->Get_Light()->Get_Light()->Get_LightInfo().color.diffuse = Color(1.f);
    //OtherOption
    _bool& g_bOutline = CUR_SCENE->g_bDrawOutline = false;
    _bool& g_bFXAAOn = CUR_SCENE->g_bFXAAOn = false;
    _bool& g_bAberrationOn = CUR_SCENE->g_bAberrationOn = false;
    _bool& g_bPBR_On = CUR_SCENE->g_bPBR_On = true;
    _float& g_lightAttenuation = CUR_SCENE->g_lightAttenuation = 100.f;
    _float& g_ambientRatio = CUR_SCENE->g_ambientRatio = 0.5f;
}

void ImGui_Manager::Frame_ModelObj()
{
    Begin("ModelTool");

    if (BeginTabBar("MoTool"))
    {
        Show_Models();
        Show_ModelInfo();
        Select_ModelAnim();
        Set_Transform();

        EndTabBar();
    }




    End();
}

void ImGui_Manager::Show_Models()
{
    if(BeginTabItem("Model Select & Create"))
	{
		vector<string> ModelNames;
		if (TreeNode("Character"))
		{
			string assetsPath = "../Resources/Models/Character/";

			for (auto& entry : fs::recursive_directory_iterator(assetsPath))
			{
				if (entry.is_directory())
					continue;

				if (entry.path().extension().wstring() != L".Model" && entry.path().extension().wstring() != L".model")
					continue;

				string fileName = entry.path().filename().string();
				Utils::DetachExt(fileName);

				ModelNames.push_back(fileName);
			}
			TreePop();
		}

		ListBox("##Model List", &m_iCurrentModelIndex, VectorOfStringGetter, &ModelNames, int(ModelNames.size()));

		static char objName[MAX_PATH] = "";
		InputText("Object Name", objName, sizeof(objName));
		if (Button("Create Model"))
		{
			shared_ptr<GameObject> obj = make_shared<GameObject>();
			obj->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(m_PickingPos, 1.f));
			{
				shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
				shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
				animator->Set_Model(RESOURCES.Get<Model>(Utils::ToWString(ModelNames[m_iCurrentModelIndex])));
				obj->Add_Component(animator);
			}
			string name = objName;
			obj->Set_Name(Utils::ToWString(name));
            EVENTMGR.Create_Object(obj);
			m_pAnimModels.push_back(make_pair(obj, ObjectMoveInfo()));
		}

		vector<string> objNames;

		for (auto& obj : m_pAnimModels)
			objNames.push_back(Utils::ToString(obj.first->Get_Name()));

		ListBox("##Object List", &m_iCurrentObjectIndex, VectorOfStringGetter, &objNames, int(objNames.size()));

		if (Button("Select Obj"))
			m_pControlObjects = m_pAnimModels[m_iCurrentObjectIndex].first;

        SameLine();
        if (Button("Delete Obj"))
        {
            for (auto iter = m_pAnimModels.begin(); iter != m_pAnimModels.end();)
            {
                if (iter->first->Get_Name() == Utils::ToWString(objNames[m_iCurrentObjectIndex]))
                {
                    EVENTMGR.Delete_Object(iter->first);
                    iter = m_pAnimModels.erase(iter);
                }
                else iter++;
             }

        }


		Separator();
		Save_Files();
        SameLine();
        Load_Files();
        EndTabItem();
	}




}

void ImGui_Manager::Show_ModelInfo()
{
    if (m_pControlObjects.expired())
        return;
    
    if (BeginTabItem("Obj Info"))
    {
        
        vector<string> fsmList;
        fsmList.push_back("POTION_FSM");
        fsmList.push_back("GUARD1_FSM");
        fsmList.push_back("GUARD2_FSM");
        fsmList.push_back("TRAVELER_FSM");
        fsmList.push_back("CHILDREN_FSM");

        static _int tmp =0;
        RadioButton("minMoveArrayPos",&tmp,0);
        SameLine();    RadioButton("maxMoveArrayPos", &tmp, 1);

        _float3 vPickPos = m_PickingPos;
        InputFloat3("Picked Pos", (_float*)&vPickPos);

        if (Button("Set ArrayPos"))
        {
            if (tmp == 0)
                m_pAnimModels[m_iCurrentObjectIndex].second.minMoveArrayPos = vPickPos;
            else
                m_pAnimModels[m_iCurrentObjectIndex].second.maxMoveArrayPos = vPickPos;
            
        }

		ListBox("##FSM List", &m_iCurrentFSMIndex, VectorOfStringGetter, &fsmList, int(fsmList.size()));
        Text(("FSM : " + fsmList[m_iCurrentFSMIndex]).c_str());
        if (Button("Set FSM"))
            m_pAnimModels[m_iCurrentObjectIndex].second.eFSMIndex = m_iCurrentFSMIndex;

        SeparatorText("Cur Info");
        Text((" FSM State : " + fsmList[m_pAnimModels[m_iCurrentObjectIndex].second.eFSMIndex]).c_str());
        Text(("min Array Pos : " + to_string(m_pAnimModels[m_iCurrentObjectIndex].second.minMoveArrayPos.x) + ", " + to_string(m_pAnimModels[m_iCurrentObjectIndex].second.minMoveArrayPos.y) + ", " + to_string(m_pAnimModels[m_iCurrentObjectIndex].second.minMoveArrayPos.z)).c_str());
        Text(("max Array Pos : " + to_string(m_pAnimModels[m_iCurrentObjectIndex].second.maxMoveArrayPos.x) + ", " + to_string(m_pAnimModels[m_iCurrentObjectIndex].second.maxMoveArrayPos.y) + ", " + to_string(m_pAnimModels[m_iCurrentObjectIndex].second.maxMoveArrayPos.z)).c_str());
       
        Checkbox("Is Moving", &m_pAnimModels[m_iCurrentObjectIndex].second.bMoving);
        string strTmp = m_pAnimModels[m_iCurrentObjectIndex].second.bMoving ? "Move" : "NonMove";
        Text(("Move State : " + strTmp).c_str());
        if (Button("Look At"))
        {
            _float3 vPos = m_PickingPos;
            vPos.y = m_pControlObjects.lock()->Get_Transform()->Get_State(Transform_State::POS).y;

            m_pControlObjects.lock()->Get_Transform()->LookAt(_float4(vPos,1.f));
        }


        



        EndTabItem();
    }

}

void ImGui_Manager::Select_ModelAnim()
{
	if (m_pControlObjects.expired())
		return;

    if (BeginTabItem("Anim Info"))
    {
        vector<string> animList;

        for (auto& anim : m_pControlObjects.lock()->Get_Model()->Get_Animations())
            animList.push_back(Utils::ToString(anim->name));

        ListBox("##Anim List", &m_iCurrentAnimIndex, VectorOfStringGetter, &animList, int(animList.size()));

        if (Button("Set Anim"))
        {
            m_pControlObjects.lock()->Get_Animator()->Set_CurrentAnim(Utils::ToWString(animList[m_iCurrentAnimIndex]), true, 1.f);
        }

        EndTabItem();
    }
}

void ImGui_Manager::Set_Transform()
{
	if (m_pControlObjects.expired())
		return;
    if (BeginTabItem("Transform Info"))
    {
        _float3 vPos = m_pControlObjects.lock()->Get_Transform()->Get_State(Transform_State::POS).xyz();
        DragFloat3("Pos", (_float*)&vPos, 0.05f);
        m_pControlObjects.lock()->Get_Transform()->Set_State(Transform_State::POS, _float4(vPos, 1.f));

        EndTabItem();
    }
}

void ImGui_Manager::Save_Files()
{
	ImGui::Text("SaveFileName");
	ImGui::InputText("##SaveFileName", m_szSaveFileName, sizeof(m_szSaveFileName));

    if (Button(" Save Model Info "))
    {
		string strFileName = m_szSaveFileName;
		string strFilePath = "..\\Resources\\MapData\\";
		strFilePath += strFileName + ".subdata";

		shared_ptr<FileUtils> file = make_shared<FileUtils>();
		file->Open(Utils::ToWString(strFilePath), FileMode::Write);

        file->Write<_int>(_int(m_pAnimModels.size()));// count
        for (_int i = 0; i < _int(m_pAnimModels.size()); ++i)
        {
            auto& gameObject = m_pAnimModels[i].first;
            auto& gameObjectInfo = m_pAnimModels[i].second;
            file->Write<string>(Utils::ToString(gameObject->Get_Name()));
            file->Write<string>(Utils::ToString(gameObject->Get_Model()->Get_ModelTag()));
            ;

            file->Write<_uint>(gameObject->Get_Animator()->Get_TweenDesc().curr.animIndex);
            file->Write<_float3>(gameObject->Get_Transform()->Get_State(Transform_State::POS).xyz());
            file->Write<Quaternion>(gameObject->Get_Transform()->Get_Rotation());
            file->Write<_bool>(gameObjectInfo.bMoving);
            file->Write<_int>(gameObjectInfo.eFSMIndex);

            if (gameObjectInfo.minMoveArrayPos.x > gameObjectInfo.maxMoveArrayPos.x)
                swap(gameObjectInfo.minMoveArrayPos.x, gameObjectInfo.maxMoveArrayPos.x);

			if (gameObjectInfo.minMoveArrayPos.y > gameObjectInfo.maxMoveArrayPos.y)
				swap(gameObjectInfo.minMoveArrayPos.y, gameObjectInfo.maxMoveArrayPos.y);
			if (gameObjectInfo.minMoveArrayPos.z > gameObjectInfo.maxMoveArrayPos.x)
				swap(gameObjectInfo.minMoveArrayPos.z, gameObjectInfo.maxMoveArrayPos.z);

            file->Write<_float3>(gameObjectInfo.minMoveArrayPos);
            file->Write<_float3>(gameObjectInfo.maxMoveArrayPos);
        }
    }
}

void ImGui_Manager::Load_Files()
{
    vector<string> files;
    string path = "../Resources/MapData/";
    for (auto& entry : fs::recursive_directory_iterator(path))
    {
		if (entry.is_directory())
			continue;

		if (entry.path().extension().wstring() != L".subdata")
			continue;

        string filename = entry.path().filename().string();
        Utils::DetachExt(filename);
        
        files.push_back(filename);

    }
    static _int index = 0;

    ListBox("##File List", &index, VectorOfStringGetter, &files, int(files.size()));

    if (Button("Load"))
    {
        wstring finalPath = L"../Resources/MapData/" + Utils::ToWString(files[index]) + L".subdata";
		shared_ptr<FileUtils> file = make_shared<FileUtils>();
		file->Open(finalPath, FileMode::Read);

		_int count = file->Read<_int>();
		for (_int i = 0; i < count; ++i)
		{
			shared_ptr<GameObject> obj = make_shared<GameObject>();
			obj->GetOrAddTransform();
			obj->Set_Name(Utils::ToWString(file->Read<string>()));

			string modelTag = file->Read<string>();
			shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
			shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
			animator->Set_Model(RESOURCES.Get<Model>(Utils::ToWString(modelTag)));
			obj->Add_Component(animator);

			animator->Set_CurrentAnim(file->Read<_uint>(), true, 1.f);
			obj->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(file->Read<_float3>(), 1.f));
			obj->GetOrAddTransform()->Set_Quaternion(file->Read<Quaternion>());

			_bool isMoving = file->Read<_bool>();
			_int iFSMIndex = file->Read<_int>();
			_float3 vMinPos = file->Read<_float3>();
			_float3 vMaxPos = file->Read<_float3>();

            EVENTMGR.Create_Object(obj);

			m_pAnimModels.push_back(make_pair(obj, ObjectMoveInfo{ isMoving,iFSMIndex,vMinPos,vMaxPos }));

		}
    }
}

void ImGui_Manager::Load_Water()
{
    //shared_ptr<GameObject> obj = make_shared<GameObject>();
    //obj->GetOrAddTransform()->Set_State(Transform_State::POS, _float4{ 128.f, -20.f, 128.f, 1.f });
    //obj->GetOrAddTransform()->Scaled(_float3{ 3.f, 1.f, 3.f });
    //shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
    //shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
    //shared_ptr<Model> model = RESOURCES.Get<Model>(L"Water_Plane");
    //model->Get_Materials().front()->Set_TextureMap(RESOURCES.GetOrAddTexture(L"WaterDiffuse", L"..\\Resources\\Textures\\MapObject\\Field\\T_Boom_000_a.tga"), TextureMapType::DIFFUSE);
    //model->Get_Materials().front()->Set_TextureMap(RESOURCES.GetOrAddTexture(L"WaterNormal", L"..\\Resources\\Textures\\MapObject\\Field\\T_chicken_meet_001.tga"), TextureMapType::NORMAL);
    //model->Get_Materials().front()->Set_TextureMap(RESOURCES.GetOrAddTexture(L"WaterDistortion", L"..\\Resources\\Textures\\MapObject\\Field\\T_Perlin_Noise_M.tga"), TextureMapType::DISTORTION);
    //renderer->Set_PassType(ModelRenderer::PASS_WATER);
    //renderer->Set_Model(model);
    //obj->Add_Component(renderer);
    //obj->Add_Component(make_shared<WaterUVSliding>());
    //obj->Set_Name(L"Ocean");
    //CUR_SCENE->Add_GameObject_Front(obj);


    shared_ptr<GameObject> obj = make_shared<GameObject>();
    obj->GetOrAddTransform()->Scaled(_float3(120.f));
    obj->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(-450.f, -20.f, -450.f, 1.f));
    shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Water.fx");
    shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(shader);
    {
        shared_ptr<Mesh> mesh = make_shared<Mesh>();
        mesh->CreateGrid(40, 40);
        renderer->Set_Mesh(mesh);
    }
    {
        shared_ptr<Material> material = make_shared<Material>();
        material->Set_TextureMap(RESOURCES.GetOrAddTexture(L"WaterDiffuse", L"..\\Resources\\Textures\\MapObject\\Field\\T_Boom_000_a.tga"), TextureMapType::DIFFUSE);
        material->Set_TextureMap(RESOURCES.GetOrAddTexture(L"WaterNormal", L"..\\Resources\\Textures\\MapObject\\Field\\T_chicken_meet_001.png"), TextureMapType::NORMAL);
        material->Set_TextureMap(RESOURCES.GetOrAddTexture(L"WaterDistortion", L"..\\Resources\\Textures\\MapObject\\Field\\T_Perlin_Noise_M.tga"), TextureMapType::DISTORTION);

        renderer->Set_Material(material);
    }
    obj->Set_Name(L"Water_Plane");
    obj->Add_Component(renderer);
    obj->Add_Component(make_shared<WaterUVSliding>());
    CUR_SCENE->Add_GameObject_Front(obj);
}

void ImGui_Manager::Load_WeedNames()
{
    for (size_t i = 0; i < 8; i++)
    {
        string WeedName = "Weed" + to_string(i);
        m_strWeedCatalogue.push_back(WeedName);
    }
}
