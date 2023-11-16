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
#include "Geometry.h"

#include "Camera.h"
#include "DemoScene.h"

#include "SphereCollider.h"
#include "AABBBoxCollider.h"
#include "OBBBoxCollider.h"
#include "MeshCollider.h"

#include "GeometryHelper.h"
#include "MeshRenderer.h"
#include "RigidBody.h"

#include "PointLightEffect.h"

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

    Picking_Object();
    LookAtSampleObject();

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
    if (m_strObjectName.size() != 0 && m_GizmoTarget == GizmoTMapObj)
    {
        // ����� ���� ī�޶��� ��,������ ������.
        shared_ptr<Camera> CurCam = CUR_SCENE->Get_Camera(L"Default")->Get_Camera();
        _float4x4 matView = CurCam->Get_ViewMat();
        _float4x4 matProj = CurCam->Get_ProjMat();

        // ����� ���� ����
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
        // ����� ���� ī�޶��� ��,������ ������.
        shared_ptr<Camera> CurCam = CUR_SCENE->Get_Camera(L"Default")->Get_Camera();
        _float4x4 matView = CurCam->Get_ViewMat();
        _float4x4 matProj = CurCam->Get_ProjMat();

        // ����� ���� ����
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
    ImGui::Begin("Frame_ObjectBase"); // ���� ������

    if (ImGui::ListBox("##ObjectBase", &m_iObjectBaseIndex, m_strObjectBaseNameList.data(), (int)m_strObjectBaseNameList.size(), (_int)m_strObjectBaseNameList.size()))
        Set_SampleObject(); // ����Ʈ�ڽ� ���ô���� �޶����� ���ø𵨺���
    // ���� �缳��
    if (m_bBaseObjectListResetHeight)
    {
        m_bBaseObjectListResetHeight = false;
        ImGui::SetScrollY(m_iObjectBaseIndex * ImGui::GetTextLineHeightWithSpacing());
    }

    ImGui::End();
}

void ImGui_Manager::Frame_ObjectBaseManager()
{
    ImGui::Begin("Frame_ObjectBaseManager"); // ���� ������

    if (ImGui::InputText("Filter##ObjectBase", m_szBaseObjectFilter, sizeof(m_szBaseObjectFilter)))
    {
        // ���ͳ��� stringȭ
        string strFilter = m_szBaseObjectFilter;
        m_FilteredBaseObjectNames.clear();
        for (size_t i = 0; i < m_strObjectBaseNameList.size(); ++i)
        {
            // �˻���� ��� �ؽ�Ʈ
            string strFilterTarget = m_strObjectBaseNameList[i];
            if (strFilterTarget.find(strFilter) != std::string::npos)
            {
                // ��������
                shared_ptr<char[]> pChar = shared_ptr<char[]>(new char[MAX_PATH]);
                strncpy_s(pChar.get(), MAX_PATH, strFilterTarget.c_str(), _TRUNCATE);

                // ���͵� �̸��� ������ ���Ϳ� �߰�
                m_strFilteredNamePtr.push_back(pChar);
                // ���͵� �̸��� ����ƮUI�� �߰�
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
        Set_SampleObject(); // ����Ʈ�ڽ� ���ô���� �޶����� ���ø𵨺���
        m_bBaseObjectListResetHeight = true; // ���̽�������Ʈ����Ʈ�� �ش� ���̿� �°� ����
    }



    // �ʿ�����Ʈ ����
    ImGui::SeparatorText("BaseObjectDesc");
    // ����� ����
    ImVec4 YellowColor(1.f, 1.f, 0.f, 1.f);
    string strCreateObjectName = m_strObjectBaseNameList[m_iObjectBaseIndex];
    ImGui::Text(("Name - " + strCreateObjectName + "-" + to_string(m_iObjectBaseIndexList[m_iObjectBaseIndex])).data());
    m_CreateObjectDesc.strName = strCreateObjectName /*+ "-" + to_string(m_iObjectBaseIndexList[m_iObjectBaseIndex])*/;
    ImGui::InputFloat("UVWeight", &m_CreateObjectDesc.fUVWeight);
    ImGui::TextColored(YellowColor, "Options");
    // �׸���
    ImGui::Text("Shadow - ");
    ImGui::SameLine();
    ImGui::Checkbox("##bShadow", &m_CreateObjectDesc.bShadow);
    // ��
    ImGui::Text("Blur - ");
    ImGui::SameLine();
    ImGui::Checkbox("##bBlur", &m_CreateObjectDesc.bBlur);
    ImGui::TextColored(YellowColor, "Components");
    // Ʈ������������Ʈ
    //ImGui::Text("Transform - ");
    //ImGui::SameLine();
    //ImGui::Checkbox("##bTransform", &m_CreateObjectDesc.bTransform);
    //if (m_CreateObjectDesc.bTransform)
    //{
    ImGui::InputFloat3("CreatePos", (_float*)&m_PickingPos);
    memcpy(&m_CreateObjectDesc.WorldMatrix._41, &m_PickingPos, sizeof(_float3));
    //}
    // �ݶ��̴� ������Ʈ
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
    ImGui::SeparatorText("##CreateLine"); // ���߱�
    if (ImGui::Button("Create") || KEYTAP(KEY_TYPE::Z))
        if (FAILED(Create_SelectObject()))
            MSG_BOX("Fail : Create_MapObject");

    ImGui::SameLine();
    ImGui::Text("Press Z");

    ImGui::End();
}

void ImGui_Manager::Frame_Objects()
{
    ImGui::Begin("Frame_SelectObjects"); // ���� ������

    ImGui::ListBox("##Objects", &m_iObjects, m_strObjectName.data(), (_int)m_strObjectName.size(), (_int)m_strObjectName.size());
    // �����缳��
    if(m_bObjectListResetHeight)
    {
        m_bObjectListResetHeight = false;
        ImGui::SetScrollY(m_iObjects * ImGui::GetTextLineHeightWithSpacing());
    }

    ImGui::End();
}
void ImGui_Manager::Frame_SelcetObjectManager()
{
    ImGui::Begin("Frame_SelectObjectManager"); // ���� ������

    if (m_strObjectName.size() > 0)
        if (ImGui::Button("Delete"))
            if (FAILED(Delete_MapObject()))
                MSG_BOX("Fail : Delete_MapObject");

    // ���� ������Ʈ�� ��������
    if (m_strObjectName.size() > 0)
    {
        if (ImGui::Button("GizmoTargetChange"))
            m_GizmoTarget = GizmoTMapObj;

        ImGui::SeparatorText("ObjectDesc");
        // ����� ����
        ImVec4 YellowColor(1.f, 1.f, 0.f, 1.f);

        // �ʿ�����Ʈ ����
        ImGui::TextColored(YellowColor, "Default");
        MapObjectScript::MAPOBJDESC& CurObjectDesc = m_pMapObjects[m_iObjects]->Get_Script<MapObjectScript>()->Get_DESC();
        ImGui::Text(("Name - " + CurObjectDesc.strName).data());
        // �ø�����
        ImGui::Text(("CullPos - X:" + to_string(CurObjectDesc.CullPos.x)
            + " Y:" + to_string(CurObjectDesc.CullPos.y)
            + " Z:" + to_string(CurObjectDesc.CullPos.z)).data());
        ImGui::Text(("CullRadius - " + to_string(CurObjectDesc.CullRadius)).data());
        ImGui::TextColored(YellowColor, "Options");
        // �׸���
        ImGui::Text("Shadow - ");
        ImGui::SameLine();
        ImGui::Checkbox("##bShadow", &CurObjectDesc.bShadow);
        // ��
        ImGui::Text("Blur - ");
        ImGui::SameLine();
        ImGui::Checkbox("##bBlur", &CurObjectDesc.bBlur);
        // ������Ʈ�� �׸��ڿ� �� ����
        if (ImGui::Button("Bake"))
            Bake(m_pMapObjects[m_iObjects]);
        ImGui::SameLine();
        if (ImGui::Button("Bake All"))
            BakeAll();
        ImGui::TextColored(YellowColor, "Components");
        // Ʈ������������Ʈ
        ImGui::Text("Transform - ");
        ImGui::SameLine();
        ImGui::Checkbox("##bTransform", &CurObjectDesc.bTransform);
        if (CurObjectDesc.bTransform && m_pMapObjects[m_iObjects].get()->Get_Transform() != nullptr && m_GizmoTarget == GizmoTMapObj)
        {
            // ������� ���ÿɼ�(TR or RT or SC) ������Ʈ
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
        }
        // �ݶ��̴� ������Ʈ
        ImGui::Text("Collider - ");
        ImGui::SameLine();
        if (ImGui::Checkbox("##bCollider", &CurObjectDesc.bCollider))
        {
            if (CurObjectDesc.bCollider)
            {
                // �ݶ��̴� Ÿ�Կ� ���� �ݶ��̴� ����
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
            //ObjPlayer->Add_Component(make_shared<OBBBoxCollider>(_float3{ 0.5f, 0.8f, 0.5f })); //obbcollider
            //ObjPlayer->Get_Collider()->Set_CollisionGroup(Player_Body);
            //ObjPlayer->Get_Collider()->Set_Activate(true);
        }
    }
    // ���̺�ε�
    ImGui::SeparatorText("Save&Load");
    ImGui::Text("SaveFileName");
    ImGui::InputText("##SaveFileName", m_szSaveFileName, sizeof(m_szSaveFileName));
    if (ImGui::Button("Save Object"))
        if (FAILED(Save_MapObject()))
            MSG_BOX("Fail : Save_MapObject");
        else
            MSG_BOX("Complete_Save");

    m_MapNames.clear();
    wstring partsPath = L"..\\Resources\\Data\\";
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
	ImGui::Text("CameraPos");
	m_CamPos = CUR_SCENE->Get_MainCamera()->Get_Transform()->Get_State(Transform_State::POS);
	if (ImGui::DragFloat3("##CamPos", (_float*)&m_CamPos))
	{
		CUR_SCENE->Get_MainCamera()->Get_Transform()->Set_State(Transform_State::POS, m_CamPos);
	}

    ImGui::Begin("SkyBox&Light"); // ���� ������

    // ��ī�̹ڽ� ����
    ImGui::SeparatorText("SkyBox##SkyBoxChange");
    if (ImGui::ListBox("##SkyBoxName", &m_iCurrentSkyBoxIndex, m_strSkyboxList.data(), (int)m_strSkyboxList.size()))
    {
        string strCurSkybox = m_strSkyboxList[m_iCurrentSkyBoxIndex];
        Utils::DetachExt(strCurSkybox);

        vector<shared_ptr<Material>>& Mats = CUR_SCENE->Get_GameObject(L"SkyBase")->Get_ModelRenderer()->Get_Model()->Get_Materials();
        Mats[0]->Set_TextureMap(RESOURCES.Get<Texture>(Utils::ToWString(strCurSkybox)), TextureMapType::DIFFUSE);
    }

    // ���⼺���� ����
    ImGui::SeparatorText("DirectionalLight");
    // ������
    _float4& Pos = (_float4&)CUR_SCENE->Get_Light()->Get_Transform()->Get_WorldMatrix().m[3];
    ImGui::DragFloat3("Position##DirLight", (_float*)&Pos);
    _float3& LookDir = CUR_SCENE->Get_Light()->Get_Transform()->Get_CurrentDir();
    ImGui::DragFloat3("LookDir##DirLight", (_float*)&LookDir);
    CUR_SCENE->Get_Light()->Get_Transform()->Set_LookDir(LookDir);
    LightInfo& DirectionalLightInfo = CUR_SCENE->Get_Light()->Get_Light()->Get_LightInfo();
    Color& Ambient = DirectionalLightInfo.color.ambient;
    ImGui::ColorEdit4("Ambient##DirLight", (_float*)&Ambient);
    Color& Diffuse = DirectionalLightInfo.color.diffuse;
    ImGui::ColorEdit4("Diffuse##DirLight", (_float*)&Diffuse);
    Color& Specular = DirectionalLightInfo.color.specular;
    ImGui::ColorEdit4("Specular##DirLight", (_float*)&Specular);
    Color& Emissive = DirectionalLightInfo.color.emissive;
    ImGui::ColorEdit4("Emissive##DirLight", (_float*)&Emissive);

    m_DirectionalLightPos = Pos;
    m_DirectionalLightLookDir = LookDir;
    m_DirectionalLightInfo = DirectionalLightInfo;

    // ��������������
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

    // ���� �������� ����
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

        // ������ �������� ��ȭȿ�� ����
        if(m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>() != nullptr)
        {
            _bool bEffectUse = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Get_bUseEffect();
            if (ImGui::Checkbox("##CurrentPointLightEffectUse", &bEffectUse))
                // ��뿩�� üũ�ڽ� �ݿ��Ͽ� �ٽ� �ֱ�
            {
                m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Set_bUseEffect(bEffectUse);
                // ����Ʈ�� ����ϱ�� �ϸ� �׶������� ���ۻ���� ����
                if (bEffectUse)
                    m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Init();
                // ����Ʈ�� ��� ���ϱ�� �ϸ� ���ۻ��� �ݴ�� ����
                else
                    m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Init_Reverse();
            }
            if (bEffectUse)
            {
                // ��ŸƮ�ں��Ʈ ��������, �����ϸ� �ٽô���
                _float4 StartAmbient = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Get_AmbientStart();
                if (ImGui::ColorEdit4("SAmbient##PtLtEffectAmbient", (_float*)&StartAmbient))
                    m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Set_AmbientStart(StartAmbient);
                // Ÿ�پں��Ʈ ��������, �����ϸ� �ٽô���
                _float4 TargetAmbient = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Get_AmbientTarget();
                if (ImGui::ColorEdit4("EAmbient##PtLtEffectAmbient", (_float*)&TargetAmbient))
                    m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Set_AmbientTarget(TargetAmbient);

                // ��ŸƮ��ǻ�� ��������, �����ϸ� �ٽô���
                _float4 StartDiffuse = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Get_DiffuseStart();
                if (ImGui::ColorEdit4("SDiffuse##PtLtEffectDiffuse", (_float*)&StartDiffuse))
                    m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Set_DiffuseStart(StartDiffuse);
                // Ÿ�ٵ�ǻ�� ��������, �����ϸ� �ٽô���
                _float4 TargetDiffuse = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Get_DiffuseTarget();
                if (ImGui::ColorEdit4("Diffuse##PtLtEffectDiffuse", (_float*)&TargetDiffuse))
                    m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Set_DiffuseTarget(TargetDiffuse);

                // ��ŸƮ������ ��������, �����ϸ� �ٽô���
                _float fStartRange = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Get_StartRange();
                ImGui::DragFloat("SRange#PtLtTargetRange", &fStartRange, 0.1f);
                m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Set_StartRange(fStartRange);
                // Ÿ�ٷ����� ��������, �����ϸ� �ٽô���
                _float fTargetRange = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Get_TargetRange();
                ImGui::DragFloat("TRange#PtLtTargetRange", &fTargetRange, 0.1f);
                m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Set_TargetRange(fTargetRange);

                // ����Ʈ ���ǵ� ��������, �����ϸ� �ٽô���
                _float fEffectSpeed = m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Get_Speed();
                ImGui::DragFloat("EffectSpeed#PtLtEffectSpeed", &fEffectSpeed, 0.1f);
                m_pPointLightObjects[m_iPointLightIndex]->Get_Script<PointLightEffect>()->Set_Speed(fEffectSpeed);
            }
        }
    }

    ImGui::SeparatorText("##LightFrame");
    
    ImGui::End();
}

void ImGui_Manager::Frame_Wall()
{
    ImGui::Begin("Frame_Wall"); // ���� ������

    if (ImGui::Button("Create"))
    {
        Create_WallMesh();
    }
    if (ImGui::Button("Clear"))
    {
        Clear_WallMesh();
    }

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
        if(m_GizmoTarget == GizmoTMapObj)
            // ���õ� ������Ʈ ��ȣ �ٲٱ� ��������Ÿ���� �ʿ�����Ʈ�϶��� ���.
        {
            for (_int i = 0; i < m_pMapObjects.size(); ++i)
            {
                if (PickObject == m_pMapObjects[i])
                {
                    m_iObjects = i;
                    // ������Ʈ ����Ʈ���� ������ ���̷� ����
                    m_bObjectListResetHeight = true;
                }
            }
        }
    }
    // ���콺 ��Ŭ�� �� WallPickingPoint ����
    if (KEYTAP(KEY_TYPE::MBUTTON))
    {
        POINT MousePos;
        ::GetCursorPos(&MousePos);
        ::ScreenToClient(g_hWnd, &MousePos);
        _float2 ScreenPos{ (_float)MousePos.x, (_float)MousePos.y };

        if (m_bFirstWallPick)
        {
            m_bFirstWallPick = false;
            shared_ptr<GameObject> PickObject = PickingMgr::GetInstance().Pick_Mesh(ScreenPos, CUR_SCENE->Get_Camera(L"Default")->Get_Camera(), CUR_SCENE->Get_Objects(), m_WallPickingPos[0]);
        }
        else
        {
            shared_ptr<GameObject> PickObject = PickingMgr::GetInstance().Pick_Mesh(ScreenPos, CUR_SCENE->Get_Camera(L"Default")->Get_Camera(), CUR_SCENE->Get_Objects(), m_WallPickingPos[1]);
            // �簢�� �ϳ� �߰�
            m_WallRectPosLDRU.push_back(pair<_float3, _float3>(m_WallPickingPos[0], _float3{ m_WallPickingPos[1].x, m_fWallHeight, m_WallPickingPos[1].z }));
            // ���ϴ� ���� ���ϴ� ������ ����
            m_WallPickingPos[0] = m_WallPickingPos[1];
        }
    }
}

void ImGui_Manager::LookAtSampleObject()
{
    if (KEYTAP(KEY_TYPE::TAB))
    {
        // ���� ī�޶� ��ġ����
        m_matPreCamera = CUR_SCENE->Get_MainCamera()->Get_Transform()->Get_WorldMatrix();
    }
    if (KEYHOLD(KEY_TYPE::TAB))
    {
        // ī�޶� �̵�
        _float4 SampleObjPos = m_SampleObject->Get_Transform()->Get_State(Transform_State::POS);
        shared_ptr<Transform> pMainCameraTransform = CUR_SCENE->Get_MainCamera()->Get_Transform();
        pMainCameraTransform->Set_State(Transform_State::POS, XMVectorAdd(SampleObjPos, _float4{1.f, 1.f, 1.f, 0.f}));
        pMainCameraTransform->LookAt(SampleObjPos);
        pMainCameraTransform->Set_State(Transform_State::POS, pMainCameraTransform->Get_State(Transform_State::POS) - pMainCameraTransform->Get_State(Transform_State::LOOK) * m_fSampleModelCullSize * 2.f);
    }
    if (KEYAWAY(KEY_TYPE::TAB))
    {
        // ī�޶� ������� ���ư���
        CUR_SCENE->Get_MainCamera()->Get_Transform()->Set_WorldMat(m_matPreCamera);
    }
}

HRESULT ImGui_Manager::Load_SkyBoxTexture()
{
    wstring path = L"..\\Resources\\Textures\\MapObject\\SkyBox\\";

    for (auto& entry : fs::recursive_directory_iterator(path))
    {
        // ������ �̸��� ������
        wstring fileName = entry.path().filename().wstring();
        WCHAR szTempName[MAX_PATH];
        lstrcpy(szTempName, fileName.c_str());

        wstring noExtName = szTempName;
        Utils::DetachExt(noExtName);
        RESOURCES.Load<Texture>(noExtName, path+szTempName);

        // char �������� ��ȯ
        shared_ptr<char[]> pChar = shared_ptr<char[]>(new char[MAX_PATH]);
        WideCharToMultiByte(CP_ACP, 0, noExtName.c_str(), -1, pChar.get(), MAX_PATH, 0, 0);

        // ��ī�̹ڽ� �̸��� ������ ���Ϳ� �߰�
        m_strSkyBoxNamePtr.push_back(pChar);
        // ��ī�̹ڽ� �̸��� ����ƮUI�� �߰�
        m_strSkyboxList.push_back(pChar.get());
    }
    return S_OK;
}

HRESULT ImGui_Manager::Load_MapObjectBase()
{
    wstring path = L"..\\Resources\\Models\\MapObject\\";

    for (auto& entry : fs::recursive_directory_iterator(path))
    {
        // �ʿ�����Ʈ ���������� �����̸����� ��ȸ�ϸ� ���̽�������Ʈ ����Ʈ�� ����.
        if (entry.is_directory())
            continue;

        // Ȯ���ڰ� ���� �ƴϸ� ��Ƽ������
        wstring Ext = entry.path().extension().wstring();
        if (Ext != L".Model")
            continue;

        // ������ �̸��� ������
        wstring fileName = entry.path().filename().wstring();
        Utils::DetachExt(fileName);
        WCHAR szTempName[MAX_PATH];
        lstrcpy(szTempName, fileName.c_str());

        // char �������� ��ȯ
        shared_ptr<char[]> pChar = shared_ptr<char[]>(new char[MAX_PATH]);
        WideCharToMultiByte(CP_ACP, 0, szTempName, -1, pChar.get(), MAX_PATH, 0, 0);

        // ���̽�������Ʈ �̸��� ������ ���Ϳ� �߰�
        m_strObjectBaseNamePtr.push_back(pChar);
        // ���̽�������Ʈ �̸��� ����ƮUI�� �߰�
        m_strObjectBaseNameList.push_back(pChar.get());
        // ���̽� ������Ʈ �ߺ����� �ε��� �߰�
        m_iObjectBaseIndexList.push_back(0);
    }
    return S_OK;
}

HRESULT ImGui_Manager::Create_SelectObject()
{
    // ���� ���õ� ���̽�������Ʈ�̸� ��������
    wstring strCurObjBase = Utils::ToWString(m_strObjectBaseNameList[m_iObjectBaseIndex]);
    // �ʿ�����Ʈ ����
    shared_ptr<GameObject> pCreateObject = Create_MapObject(m_CreateObjectDesc);
    m_pMapObjects.push_back(pCreateObject);
    // ���� ��ġ�� ������Ʈ ��Ͽ� �߰�
    m_strObjectName.push_back(m_strObjectBaseNameList[m_iObjectBaseIndex]);
    // ���� ������ ������Ʈ�� Ÿ����(�����)�ϱ�
    m_iObjects = (_int)(m_pMapObjects.size() - 1);

    return S_OK;
}

shared_ptr<GameObject>& ImGui_Manager::Create_MapObject(MapObjectScript::MapObjectDesc _CreateDesc)
{
    // ������Ʈ Ʋ ����
    shared_ptr<GameObject> CreateObject = make_shared<GameObject>();
    // �ʿ�����Ʈ ���� ���� �� �ݿ�
    shared_ptr<MapObjectScript> MapObjSc = make_shared<MapObjectScript>(_CreateDesc);
    CreateObject->Add_Component(MapObjSc);
    MapObjectScript::MapObjectDesc& CreateDesc = MapObjSc->Get_DESC();
    //// �׸���
    //CreateObject->Set_DrawShadow(CreateDesc.bShadow);
    //// ��
    //CreateObject->Set_Blur(CreateDesc.bBlur);
    // �̸��� ����Ͽ� �𵨻���
    // ������ȣ�� �����Ͽ� �𵨸��� ����
    _int iPureNameSize = 0;
    while (CreateDesc.strName[iPureNameSize] != '-' && iPureNameSize < CreateDesc.strName.size())
    {
        ++iPureNameSize;
    }
    string strModelName = CreateDesc.strName.substr(0, iPureNameSize);
    CreateDesc.ColModelName = strModelName;
    CreateObject->Set_Name(Utils::ToWString(strModelName));
    // �𵨻���
    shared_ptr<Model> model = RESOURCES.Get<Model>(Utils::ToWString(strModelName));
    shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
    shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
    CreateObject->Add_Component(renderer);
    renderer->Set_Model(model);
    renderer->Set_PassType(ModelRenderer::PASS_MAPOBJECT);
    renderer->SetFloat(3, CreateDesc.fUVWeight);
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
    // �׸���, ��, �ø��������
    Bake(CreateObject);

    CUR_SCENE->Add_GameObject(CreateObject);

    return CreateObject;
}

HRESULT ImGui_Manager::Create_SelectPointLight()
{
    // ������ ������ ����
    LightInfo CreateLightInfo;
    m_CreatePointLightInfo.vPosition = XMVectorSetW(m_PickingPos, 1.f);
    shared_ptr<GameObject>& LightObject = Create_PointLight(m_CreatePointLightInfo);
    m_pPointLightObjects.push_back(LightObject);

    {
        // ����Ʈ ����Ʈ��� ������ �߰�
        // �⺻�ʱ�ȭ
        shared_ptr<PointLightEffect> pPLightScript = make_shared<PointLightEffect>();
        m_pPointLightObjects.back()->Add_Component(pPLightScript);
        pPLightScript->Init();
    }

    // �̸�����Ʈ�� �ϳ� �ֱ�
    m_strPointLightList.push_back("PointLight");
    
    return S_OK;
}

shared_ptr<GameObject>& ImGui_Manager::Create_PointLight(LightInfo _ptltInfo)
{
    shared_ptr<GameObject> PointLight = make_shared<GameObject>();
    PointLight->Set_Name(L"PointLight");
    // ������
    PointLight->GetOrAddTransform()->Set_State(Transform_State::POS, _ptltInfo.vPosition);
    {
        // LightComponent ���� �� ����
        shared_ptr<Light> lightCom = make_shared<Light>();
        lightCom->Set_Ambient(_ptltInfo.color.ambient);
        lightCom->Set_Diffuse(_ptltInfo.color.diffuse);
        lightCom->Set_Specular(_ptltInfo.color.specular);
        lightCom->Set_Emissive(_ptltInfo.color.emissive);
        lightCom->Set_LightType(LIGHT_TYPE::POINT_LIGHT);
        lightCom->Set_LightRange(_ptltInfo.range);

        PointLight->Add_Component(lightCom);
    }
    CUR_SCENE->Add_GameObject(PointLight);


    return PointLight;
}

void ImGui_Manager::Create_WallMesh()
{
    // �������� ������� ���޽� ����
    shared_ptr<Mesh> WallMesh = make_shared<Mesh>();
    WallMesh->Create3DRect(m_WallRectPosLDRU);

    // �޽ø� ������� ��������Ʈ ����
    shared_ptr<GameObject> WallObject = make_shared<GameObject>();
    WallObject->Set_Name(L"Wall");
    WallObject->GetOrAddTransform();

    // �޽÷�����
    shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Mesh.fx"));
    renderer->Set_Mesh(WallMesh);

    // �޽ø� ���� �޽��ݶ��̴� ����
    shared_ptr<MeshCollider> pCollider = make_shared<MeshCollider>(*WallMesh.get());
    WallObject->Add_Component(pCollider);
    pCollider->Set_Activate(true);
    //auto rigidBody = make_shared<RigidBody>();
    //rigidBody->Create_RigidBody(pCollider, WallObject->GetOrAddTransform()->Get_WorldMatrix());
    //WallObject->Add_Component(rigidBody);

    shared_ptr<GameObject> PreWall = CUR_SCENE->Get_GameObject(L"Wall");
    if (PreWall != nullptr)
        CUR_SCENE->Remove_GameObject(PreWall);
    CUR_SCENE->Add_GameObject(WallObject);
}

void ImGui_Manager::Clear_WallMesh()
{
    m_WallRectPosLDRU.clear();
    m_bFirstWallPick = true;
}

HRESULT ImGui_Manager::Delete_PointLight()
{
    // 1. ��������� ����
    CUR_SCENE->Remove_GameObject(m_pPointLightObjects[m_iPointLightIndex]);
    // 2. ���� ���õ� ������Ʈ�� ���Ϳ��� ����
    {
        auto iter = m_pPointLightObjects.begin();
        for (_int i = 0; i < m_iPointLightIndex; ++i)
            ++iter;
        // ����Ʈ���Ϳ��� �ش系�����
        if (m_iPointLightIndex > 0)
            iter = m_pPointLightObjects.erase(iter);
        else
            m_pPointLightObjects.erase(m_pPointLightObjects.begin());
    }
    // 3. ���� ��ġ�� ������Ʈ List �̸� ����
    {
        auto iter = m_strPointLightList.begin();
        for (_int i = 0; i < m_iPointLightIndex; ++i)
            ++iter;
        // ����Ʈ���Ϳ��� �ش系�����
        if (m_iPointLightIndex > 0)
        {
            iter = m_strPointLightList.erase(iter);
            // ����Ʈ�� ������ġ ����
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
    // ���̺��� �ø����, �׸���, �� ó��
    BakeAll();

    // ���̺� ���� �̸����� �����ϱ�
    string strFileName = m_szSaveFileName;
    string strFilePath = "..\\Resources\\Data\\";
    strFilePath += strFileName + ".dat";

    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(Utils::ToWString(strFilePath), FileMode::Write);

    // ��ī�̹ڽ� ���� ����
    string strSkyBoxTetureName = m_strSkyboxList[m_iCurrentSkyBoxIndex];
    file->Write<string>(strSkyBoxTetureName);

    // ���⼺���� ���� ����
    file->Write<_float4>(m_DirectionalLightPos);
    file->Write<_float3>(m_DirectionalLightLookDir);
    file->Write<LightColor>(m_DirectionalLightInfo.color);

    // ������ ���� ����
    file->Write<_int>((_int)m_pPointLightObjects.size());
    for (auto& PtLtObject : m_pPointLightObjects)
    {
        _float4 PtLtPosition = PtLtObject->Get_Transform()->Get_State(Transform_State::POS);
        file->Write<_float4>(PtLtPosition);
        LightInfo& ptltInfo = PtLtObject->Get_Light()->Get_LightInfo();
        file->Write<LightColor>(ptltInfo.color);
        file->Write<_float>(ptltInfo.range);

        // ������ �ﷷ�ﷷȿ�� ���� ����
        weak_ptr<PointLightEffect> pPTLTEffect = PtLtObject->Get_Script<PointLightEffect>();
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

    // ���޽��ݶ��̴��� �ʿ��� ���� ����
    file->Write<_int>((_int)m_WallRectPosLDRU.size());
    for (_int i = 0; i < m_WallRectPosLDRU.size(); ++i)
    {
        file->Write<pair<_float3, _float3>>(m_WallRectPosLDRU[i]);
    }

    // �ʿ�����Ʈ ���� ����
    // 1. ������Ʈ ���� ����
    file->Write<_int>((_int)m_pMapObjects.size());
    
    // 2. ������Ʈ�� MapObjectDesc ������� ����
    for (_uint i = 0; i < m_pMapObjects.size(); ++i)
    {
        MapObjectScript::MapObjectDesc& MapDesc = m_pMapObjects[i]->Get_Script<MapObjectScript>()->Get_DESC();
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
    }
    return S_OK;
}

HRESULT ImGui_Manager::Load_MapObject()
{
    // ������ ����Ʈ����Ʈ ��λ��� �� Ŭ����
    _int iPtltSize = (_int)m_pPointLightObjects.size();
    for (auto& ptltObj : m_pPointLightObjects)
        CUR_SCENE->Remove_GameObject(ptltObj);
    m_pPointLightObjects.clear();
    m_strPointLightList.clear();
    m_iPointLightIndex = 0;

    // ���簮���ִ� ������Ʈ����
    _uint iSize = (_int)m_pMapObjects.size();
    // 1. ������ ��������Ʈ ���� �� Clear
    for (auto& mapObj : m_pMapObjects)
        CUR_SCENE->Remove_GameObject(mapObj);
    m_pMapObjects.clear();
    m_strObjectName.clear();
    m_strObjectNamePtr.clear();
    m_iObjects = 0;

    // ���̺� ���� �̸����� �ε��ϱ�
    string strFileName = m_MapNames[curMapIndex];
    string strFilePath = "..\\Resources\\Data\\";
    strFilePath += strFileName + ".dat";
    shared_ptr<FileUtils> file = make_shared<FileUtils>();
    file->Open(Utils::ToWString(strFilePath), FileMode::Read);

    // ��ī�̹ڽ� �̸� �������� �����ϱ�.
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

    // ���⼺�������� �������� �����ϱ�
    // ������
    shared_ptr<GameObject> DirectionalLightObject = CUR_SCENE->Get_Light();
    _float4 DirLightPos = _float4{ 0.f, 0.f, 0.f, 1.f };
    file->Read<_float4>(DirLightPos);
    DirectionalLightObject->Get_Transform()->Set_State(Transform_State::POS, DirLightPos);
    // ���¹���
    _float3 DirLightLookDir = _float3{ 0.f, 0.f, 0.f };
    file->Read<_float3>(DirLightLookDir);
    DirectionalLightObject->Get_Transform()->Set_LookDir(DirLightLookDir);
    // ����
    LightColor DirLightColor;
    file->Read<LightColor>(DirLightColor);
    DirectionalLightObject->Get_Light()->Set_Ambient(DirLightColor.ambient);
    DirectionalLightObject->Get_Light()->Set_Diffuse(DirLightColor.diffuse);
    DirectionalLightObject->Get_Light()->Set_Specular(DirLightColor.specular);
    DirectionalLightObject->Get_Light()->Set_Emissive(DirLightColor.emissive);

    // ���������� �������� �ҷ�����
    _int iNumPointLight = file->Read<_int>();
    for (_int i = 0; i < iNumPointLight; ++i)
    {
        LightInfo loadPointLightInfo;
        file->Read<_float4>(loadPointLightInfo.vPosition);
        file->Read<LightColor>(loadPointLightInfo.color);
        file->Read<_float>(loadPointLightInfo.range);
        shared_ptr<GameObject>& CreatePtltObj = Create_PointLight(loadPointLightInfo);
        m_pPointLightObjects.push_back(CreatePtltObj);
        m_strPointLightList.push_back("PointLight");

        // ������ȿ�� �⺻�ʱ�ȭ
        shared_ptr<PointLightEffect> pPLE = make_shared<PointLightEffect>();
        m_pPointLightObjects.back()->Add_Component(pPLE);
        pPLE->Init();

        // ������ ȿ������
        _bool bUseEffect = false;
        file->Read<_bool>(bUseEffect);
        pPLE->Set_bUseEffect(bUseEffect);
        if (bUseEffect) // ������ȿ���� �ִ� �༮�̶��
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
        }
    }

    // ������ �ҷ����� �� ������
    m_WallRectPosLDRU.clear();
    _int iNumWall = 0;
    file->Read<_int>(iNumWall);
    m_WallRectPosLDRU.resize(iNumWall);
    for (_int i = 0; i < iNumWall; ++i)
        file->Read<pair<_float3, _float3>>(m_WallRectPosLDRU[i]);
    Create_WallMesh();

    // ������Ʈ ���� �ҷ�����
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
        
        shared_ptr<GameObject> CreateObject = Create_MapObject(MapDesc);
        
        m_pMapObjects.push_back(CreateObject);
        // ���� ��ġ�� ������Ʈ �̸� UI�� �߰�
        WCHAR szTempName[MAX_PATH];
        //lstrcpy(szTempName, strObjectName.c_str());
        lstrcpy(szTempName, Utils::ToWString(MapDesc.strName).c_str());
        // char �������� ��ȯ
        shared_ptr<char[]> pChar = shared_ptr<char[]>(new char[MAX_PATH]);
        WideCharToMultiByte(CP_ACP, 0, szTempName, -1, pChar.get(), MAX_PATH, 0, 0);
        m_strObjectNamePtr.push_back(pChar);
        m_strObjectName.push_back(pChar.get());
    }

    return S_OK;
}

_float4 ImGui_Manager::Compute_CullingData(shared_ptr<GameObject>& _pGameObject)
{
    // ���� �޾ƿͼ� �ø������ǰ� ���̸� ����Ͽ� ��ȯ float4(_float(Pos), _float(Radius))
    // ��� ������ ���鼭 XYZ������ min�� max�� ã�ƾ���.
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
            // ������Ĺݿ�
            _float3 vPos = _float3::Transform(VtxData.vPosition, boneDesc->transform[boneIndex] * _pGameObject->Get_Transform()->Get_WorldMatrix());

            vMaxPos.x = max(vPos.x, vMaxPos.x);
            vMaxPos.y = max(vPos.y, vMaxPos.y);
            vMaxPos.z = max(vPos.z, vMaxPos.z);
            vMinPos.x = min(vPos.x, vMinPos.x);
            vMinPos.y = min(vPos.y, vMinPos.y);
            vMinPos.z = min(vPos.z, vMinPos.z);
        }
    }

    // Min�� Max�� ���� �� 2�� ������
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
        renderer->SetFloat(3, 1.f);
        
        CUR_SCENE->Add_GameObject(m_SampleObject);

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