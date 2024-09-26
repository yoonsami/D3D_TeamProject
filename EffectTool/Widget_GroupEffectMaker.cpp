#include "pch.h"
#include "Widget_GroupEffectMaker.h"

#include "GameObject.h"
#include "GroupEffect.h"
#include "Camera.h"

bool VectorOfStringGetter2(void* data, int n, const char** out_text)
{
	const vector<string>* v = (vector<string>*)data;
	*out_text = (v[0][n]).c_str();
	return true;
}

Widget_GroupEffectMaker::Widget_GroupEffectMaker()
{
}

Widget_GroupEffectMaker::~Widget_GroupEffectMaker()
{
}

void Widget_GroupEffectMaker::Initialize()
{
	Set_GroupList();
	Set_MemberEffectList();
	Set_MeshEffectList();
	Set_ParticleList();
}

void Widget_GroupEffectMaker::Tick()
{
	ImGui::Begin("Group Effect Maker");
	Widget_GroupMaker();
	ImGui::End();


	if (m_bWidgetOn_GetTag)
	{
		ImGui::SetNextWindowPos(ImVec2(g_iWinSizeX/2.f, g_iWinSizeY/2.f));
		ImGui::Begin("Create New Group");
		Widget_GetTag();
		ImGui::End();
	}
	if (m_bWidgetOn_AddMeshEffect)
	{
		ImGui::SetNextWindowPos(ImVec2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f));
		ImGui::Begin("Add member mesh Effect ");
		Widget_AddMeshEffect();
		ImGui::End();
	}
	if (m_bWidgetOn_AddParticle)
	{
		ImGui::SetNextWindowPos(ImVec2(g_iWinSizeX / 2.f, g_iWinSizeY / 2.f));
		ImGui::Begin("Add member particle");
		Widget_AddParticle();
		ImGui::End();
	}
}

void Widget_GroupEffectMaker::Set_GroupList()
{
	// For. Clear prev Group List
	if (nullptr != m_pszGroups)
		delete[] m_pszGroups;
	m_vecGroups.clear();
	m_iNumGroups = 0;
	m_iGroup = { 0 };
	m_strGroup = { "None" };

	// For. Fill Group List 
	m_vecGroups.push_back("None");

	wstring assetPath = L"..\\Resources\\EffectData\\GroupEffectData\\";
	for (auto& entry : fs::recursive_directory_iterator(assetPath))
	{
		if (entry.is_directory())
			continue;

		if (entry.path().extension().wstring() != L".dat")
			continue;

		string tag = entry.path().filename().string();
		Utils::DetachExt(tag);
		m_vecGroups.push_back(tag);
	}

	m_iNumGroups = (_uint)m_vecGroups.size();
	m_pszGroups = new const char* [m_iNumGroups];

	int iIndex = 0;
	for (auto iter : m_vecGroups)
	{
		m_pszGroups[iIndex] = m_vecGroups[iIndex].c_str();
		iIndex++;
	}
}

void Widget_GroupEffectMaker::Set_MemberEffectList()
{	
	if (nullptr == m_pCurrentGroup)
		return;

	// For. Clear Prev List 
	m_vecMemberEffects.clear();
	if (nullptr != m_pszMemberEffects)
		delete[] m_pszMemberEffects;

	// For. Fill List 
	vector<GroupEffectData::MemberEffect_Desc> vMembers = m_pCurrentGroup->Get_GroupEffect()->Get_MemberEffectData();

	m_iNumMemberEffects = (_uint)vMembers.size();
	m_pszMemberEffects = new const char* [m_iNumMemberEffects];

	_int iIndex = 0;
	for (auto& iter : vMembers)
	{
		m_vecMemberEffects.push_back(Utils::ToString(iter.wstrEffectTag));
		iIndex++;
	}

	// For. Clear Member Effect Property 
	if (nullptr != m_tCurrMemberProperty)
	{
		delete m_tCurrMemberProperty;
	}

	// For. Fill Member Effect Property
	m_tCurrMemberProperty = new MemberEffectProperty_DESC[m_iNumMemberEffects];
	vector <GroupEffectData::MemberEffect_Desc> vMemberEffectData = m_pCurrentGroup->Get_GroupEffect()->Get_MemberEffectData();
	
	_int iIndex2 = 0;
	for (auto& iter : vMemberEffectData)
	{
		m_tCurrMemberProperty[iIndex2].m_fStartTime = iter.fCreateTime;
		m_tCurrMemberProperty[iIndex2].m_fPos[0] = iter.vPivot_Pos.x;
		m_tCurrMemberProperty[iIndex2].m_fPos[1] = iter.vPivot_Pos.y;
		m_tCurrMemberProperty[iIndex2].m_fPos[2] = iter.vPivot_Pos.z;
		m_tCurrMemberProperty[iIndex2].m_fScale[0] = iter.vPivot_Scale.x;
		m_tCurrMemberProperty[iIndex2].m_fScale[1] = iter.vPivot_Scale.y;
		m_tCurrMemberProperty[iIndex2].m_fScale[2] = iter.vPivot_Scale.z;
		m_tCurrMemberProperty[iIndex2].m_fRotation[0] = iter.vPivot_Rotation.x;
		m_tCurrMemberProperty[iIndex2].m_fRotation[1] = iter.vPivot_Rotation.y;
		m_tCurrMemberProperty[iIndex2].m_fRotation[2] = iter.vPivot_Rotation.z;
		iIndex2++;
	}
}

void Widget_GroupEffectMaker::Set_MeshEffectList()
{
	// For. Clear prev List
	if (nullptr != m_pszMeshEffects)
		delete[] m_pszMeshEffects;
	m_vecMeshEffects.clear();
	m_iNumMeshEffects = 0;
	m_iMeshEffect = { 0 };

	// For. Fill List 
	wstring assetPath = L"..\\Resources\\EffectData\\MeshEffectData\\";
	for (auto& entry : fs::recursive_directory_iterator(assetPath))
	{
		if (entry.is_directory())
			continue;

		if (entry.path().extension().wstring() != L".dat")
			continue;

		string tag = entry.path().filename().string();
		Utils::DetachExt(tag);
		m_vecMeshEffects.push_back(tag);
	}

	m_iNumMeshEffects = (_uint)m_vecMeshEffects.size();
	m_pszMeshEffects = new const char* [m_iNumMeshEffects];

	int iIndex = 0;
	for (auto iter : m_vecMeshEffects)
	{
		m_pszMeshEffects[iIndex] = m_vecMeshEffects[iIndex].c_str();
		iIndex++;
	}

	if (0 != m_vecMeshEffects.size())
		m_strMeshEffect = m_pszMeshEffects[m_iMeshEffect];
}

void Widget_GroupEffectMaker::Set_ParticleList()
{
	// For. Clear prev List
	if (nullptr != m_pszParticles)
	{
		for (_uint i = 0; i < m_iNumParticles; ++i)
			delete[] m_pszParticles[i];
	}
	m_vecParticles.clear();
	m_iNumParticles = 0;
	m_iParticle = { 0 };

	// For. Fill List 
	wstring assetPath = L"..\\Resources\\EffectData\\ParticleData\\";
	for (auto& entry : fs::recursive_directory_iterator(assetPath))
	{
		if (entry.is_directory())
			continue;

		if (entry.path().extension().wstring() != L".dat")
			continue;

		string tag = entry.path().string();
		tag = entry.path().filename().string();
		m_vecParticles.push_back(tag);
	}

	m_iNumParticles = (_uint)m_vecParticles.size();
	m_pszParticles = new const char* [m_iNumParticles];

	int iIndex = 0;
	for (auto iter : m_vecParticles)
	{
		m_pszParticles[iIndex] = m_vecParticles[iIndex].c_str();
		iIndex++;
	}

	if(0 != m_vecParticles.size())
		m_strParticle = m_pszParticles[m_iParticle];
}

void Widget_GroupEffectMaker::Widget_GroupMaker()
{
	// Child1 : Member Effect list
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysVerticalScrollbar;
		ImGui::BeginChild("##Child1_GroupEffect", ImVec2(ImGui::GetContentRegionAvail().x - 150, ImGui::GetContentRegionAvail().y), true, window_flags);

		Option_MemberEffectList();

		ImGui::EndChild();
	}

	ImGui::SameLine();

	// Child2 : Group combo box, add member button / is loop / play button / save button
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar;
		ImGui::BeginChild("##Child2_GroupEffect", ImVec2(150, ImGui::GetContentRegionAvail().y), false, window_flags);

		Option_GroupList();
		ImGui::Spacing();

		if (ImGui::Button("Add MeshEffect##GroupEffect"))
		{
			Set_MeshEffectList();
			m_bWidgetOn_AddMeshEffect = true;
		}
		ImGui::Spacing();

		//if (ImGui::Button("Add Particle##GroupEffect"))
		//{
		//	Set_ParticleList();
		//	m_bWidgetOn_AddParticle = true;
		//}
		//ImGui::Spacing();
		
		if (ImGui::Button("Save##GroupEffect"))
			Save();
		ImGui::SameLine();
		if (ImGui::Button("Play##GroupEffect"))
		{
			Delete();
			Create();
		}
		ImGui::Spacing();
		
		if (ImGui::Button("Create New Group##GroupEffect"))
			m_bWidgetOn_GetTag = true;
		ImGui::Spacing();

		if (ImGui::Button("Reset Scene"))
			Delete();

		ImGui::EndChild();
	}
}

void Widget_GroupEffectMaker::Widget_GetTag()
{
	ImGui::SeparatorText("Group Name");

	ImGui::InputText("##GetTag", m_szNewGroupEffectTag, MAX_PATH);

	if (ImGui::Button("Create##GetTag"))
	{
		Save(m_szNewGroupEffectTag);
		Set_GroupList();
		Create();
		m_bWidgetOn_GetTag = false;
	}
	if(ImGui::Button("Exit"))
		m_bWidgetOn_GetTag = false;
}

void Widget_GroupEffectMaker::Widget_AddMeshEffect()
{
	// Mesh Effect ComboBox
	if (ImGui::BeginCombo("Mesh Effect##GroupEffect", m_pszMeshEffects[m_iMeshEffect], ImGuiComboFlags_HeightLargest))
	{
		for (_uint n = 0; n < m_iNumMeshEffects; n++)
		{
			const bool is_selected = (m_iMeshEffect == n);
			if (ImGui::Selectable(m_pszMeshEffects[n], is_selected))
			{
				m_iMeshEffect = n;
				m_strMeshEffect = m_pszMeshEffects[m_iMeshEffect];
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// Add, Exit Button 
	if (ImGui::Button("Add"))
	{
		if ("None" != m_strMeshEffect)
		{
			AddMemberEffect(Utils::ToWString(m_strMeshEffect), GroupEffectData::TYPE_MESHEFFECT);
			m_bWidgetOn_AddMeshEffect = false;
		}
	}
	if (ImGui::Button("Exit"))
		m_bWidgetOn_AddMeshEffect = false;
}

void Widget_GroupEffectMaker::Widget_AddParticle()
{	
	// Particle ComboBox
	if (ImGui::BeginCombo("Particle##GroupEffect", m_pszParticles[m_iParticle], 0))
	{
		for (_uint n = 0; n < m_iNumMeshEffects; n++)
		{
			const bool is_selected = (m_iParticle == n);
			if (ImGui::Selectable(m_pszParticles[n], is_selected))
			{
				m_iParticle = n;
				m_strParticle = m_pszParticles[m_iParticle];
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	// Add, Exit Button 
	if (ImGui::Button("Add"))
	{
		if ("None" != m_strParticle)
		{
			AddMemberEffect(Utils::ToWString(m_strParticle), GroupEffectData::TYPE_PARTICLE);
			m_bWidgetOn_AddParticle = false;
		}

	}
	if (ImGui::Button("Exit"))
		m_bWidgetOn_AddParticle = false;
}

void Widget_GroupEffectMaker::Option_GroupList()
{
	/* If select Group 
		1. Erase prev GroupEffect GameObject
		2. Create GroupEffect GameObject with selected group tag */

	ImGui::Text("Group List");
	if(ImGui::BeginCombo("##GroupEffectList", m_pszGroups[m_iGroup], ImGuiComboFlags_HeightLargest))
	{
		for (_uint n = 0; n < m_iNumGroups; n++)
		{
			const bool is_selected = (m_iGroup == n);
			if (ImGui::Selectable(m_pszGroups[n], is_selected))
			{
				// 1. Erase all Effect in current group 
				Delete();

				// 2. Add GroupEffect to Current Scene  
				m_iGroup = n;
				m_strGroup = m_pszGroups[m_iGroup];
				Create();

				// 3. Update MemberEffect list
				Set_MemberEffectList();
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void Widget_GroupEffectMaker::Option_MemberEffectList()
{
	// ImGui::ListBox("##MemberEffect_GroupMaker", &m_iMemberEffect, VectorOfStringGetter2, &m_vecMemberEffects, m_iNumMemberEffects, 10);
	
	_int iIndex = 0;
	for (auto& iter : m_vecMemberEffects)
	{
		string strKey = iter;
		Utils::DetachExt(strKey);
		string strIndex = to_string(iIndex + 1);
		string strTreeNodeKey =  "Membesr" + strIndex + ": " + strKey;
		string strFloatKey = "Start time##" + strIndex + ": " + iter;
		string strFloat3Key1 = "Translation##" + strIndex + ": " + iter;
		string strFloat3Key2 = "Scale##" + strIndex + ": " + iter;
		string strFloat3Key3_x = "Rotation X(radian)##" + strIndex + ": " + iter;
		string strFloat3Key3_y = "Rotation Y(radian)##" + strIndex + ": " + iter;
		string strFloat3Key3_z = "Rotation Z(radian)##" + strIndex + ": " + iter;
		string strDeleteButton = "Delete##" + strIndex + ": " + iter;
		string strUpButton = "Up##" + strIndex + ": " + iter;
		string strDownButton = "Down##" + strIndex + ": " + iter;

		if(ImGui::TreeNode(strTreeNodeKey.c_str()))
		{ 
			if (ImGui::InputFloat(strFloatKey.c_str(), &m_tCurrMemberProperty[iIndex].m_fStartTime))
			{
				m_pCurrentGroup->Get_GroupEffect()->Set_Member_StartTime(iIndex, m_tCurrMemberProperty[iIndex].m_fStartTime);
			}
			if (ImGui::InputFloat3(strFloat3Key1.c_str(), m_tCurrMemberProperty[iIndex].m_fPos))
			{
				_float3 vPos = { m_tCurrMemberProperty[iIndex].m_fPos[0], m_tCurrMemberProperty[iIndex].m_fPos[1], m_tCurrMemberProperty[iIndex].m_fPos[2] };
				m_pCurrentGroup->Get_GroupEffect()->Set_Member_PivotPos(iIndex, vPos);
			}
			if(ImGui::InputFloat3(strFloat3Key2.c_str(), m_tCurrMemberProperty[iIndex].m_fScale))
			{
				_float3 vScale = { m_tCurrMemberProperty[iIndex].m_fScale[0], m_tCurrMemberProperty[iIndex].m_fScale[1], m_tCurrMemberProperty[iIndex].m_fScale[2] };
				m_pCurrentGroup->Get_GroupEffect()->Set_Member_PivotScale(iIndex, vScale);
			}
			if (ImGui::InputFloat(strFloat3Key3_x.c_str(), &m_tCurrMemberProperty[iIndex].m_fRotation[0]))
			{
				m_tCurrMemberProperty[iIndex].m_fRotation[0] = m_tCurrMemberProperty[iIndex].m_fRotation[0] * (XM_PI / 180.f);

				_float3 vRotation = { m_tCurrMemberProperty[iIndex].m_fRotation[0], m_tCurrMemberProperty[iIndex].m_fRotation[1], m_tCurrMemberProperty[iIndex].m_fRotation[2] };
				m_pCurrentGroup->Get_GroupEffect()->Set_Member_PivotRotation(iIndex, vRotation);
			}
			if (ImGui::InputFloat(strFloat3Key3_y.c_str(), &m_tCurrMemberProperty[iIndex].m_fRotation[1]))
			{
				m_tCurrMemberProperty[iIndex].m_fRotation[1] = m_tCurrMemberProperty[iIndex].m_fRotation[1] * (XM_PI / 180.f);

				_float3 vRotation = { m_tCurrMemberProperty[iIndex].m_fRotation[0], m_tCurrMemberProperty[iIndex].m_fRotation[1], m_tCurrMemberProperty[iIndex].m_fRotation[2] };
				m_pCurrentGroup->Get_GroupEffect()->Set_Member_PivotRotation(iIndex, vRotation);
			}
			if (ImGui::InputFloat(strFloat3Key3_z.c_str(), &m_tCurrMemberProperty[iIndex].m_fRotation[2]))
			{
				m_tCurrMemberProperty[iIndex].m_fRotation[2] = m_tCurrMemberProperty[iIndex].m_fRotation[2] * (XM_PI / 180.f);

				_float3 vRotation = { m_tCurrMemberProperty[iIndex].m_fRotation[0], m_tCurrMemberProperty[iIndex].m_fRotation[1], m_tCurrMemberProperty[iIndex].m_fRotation[2] };
				m_pCurrentGroup->Get_GroupEffect()->Set_Member_PivotRotation(iIndex, vRotation);
			}

			if (iIndex > 0)
			{
				if (ImGui::Button(strUpButton.c_str()))
				{
					_int iPrevIndex = iIndex - 1;

					// GroupEffect의 m_vMemberEffectData에서 순서 바꾸기 
					GroupEffectData::MemberEffect_Desc m_tTemp_i = m_pCurrentGroup->Get_GroupEffect()->Get_MemberEffectData_index(iIndex);
					GroupEffectData::MemberEffect_Desc m_tTemp_iPrev = m_pCurrentGroup->Get_GroupEffect()->Get_MemberEffectData_index(iPrevIndex);
					m_pCurrentGroup->Get_GroupEffect()->Set_MemberEffectData_index(iIndex, m_tTemp_iPrev);
					m_pCurrentGroup->Get_GroupEffect()->Set_MemberEffectData_index(iPrevIndex, m_tTemp_i);

					// m_tCurrMemberProperty에서 순서 바꾸기 또는 memberlist 리로드하기 
					Set_MemberEffectList();
				}
				ImGui::SameLine();
			}
			if (iIndex < _int(m_iNumMeshEffects - 1))
			{
				if (ImGui::Button(strDownButton.c_str()))
				{
					_int iNextIndex = iIndex + 1;

					// GroupEffect의 m_vMemberEffectData에서 순서 바꾸기 
					GroupEffectData::MemberEffect_Desc m_tTemp_i = m_pCurrentGroup->Get_GroupEffect()->Get_MemberEffectData_index(iIndex);
					GroupEffectData::MemberEffect_Desc m_tTemp_iPrev = m_pCurrentGroup->Get_GroupEffect()->Get_MemberEffectData_index(iNextIndex);
					m_pCurrentGroup->Get_GroupEffect()->Set_MemberEffectData_index(iIndex, m_tTemp_iPrev);
					m_pCurrentGroup->Get_GroupEffect()->Set_MemberEffectData_index(iNextIndex, m_tTemp_i);

					// m_tCurrMemberProperty에서 순서 바꾸기 또는 memberlist 리로드하기 
					Set_MemberEffectList();
				}
				ImGui::SameLine();
			}
			if (ImGui::Button(strDeleteButton.c_str()))
			{
				if(nullptr != m_pCurrentGroup)
					m_pCurrentGroup->Get_GroupEffect()->DeleteMember(Utils::ToWString(iter));
				string strFilePath = "..\\Resources\\EffectData\\GroupEffectData\\";
				strFilePath += (m_strGroup + ".dat");

				Save();

				RESOURCES.ReloadOrAddGroupEffectData(Utils::ToWString(m_strGroup), Utils::ToWString(strFilePath));
				Set_MemberEffectList();
			}
			ImGui::TreePop();
		}
		iIndex++;
	}
}

void Widget_GroupEffectMaker::Option_Effect(string strEffectTag, _int iIndex)
{
	string strKey_Tree = strEffectTag + to_string(iIndex) + "##Effect";
	string strKey_RadioButton1 = "Play Once##" + strEffectTag + to_string(iIndex);
	string strKey_RadioButton2 = "Play Loop##" + strEffectTag + to_string(iIndex);
	string strKey_RadioButton3 = "Show First Frame##" + strEffectTag + to_string(iIndex);
	string strKey_InputFloat = "Time##" + strEffectTag + to_string(iIndex);

	if (ImGui::TreeNode(strKey_Tree.c_str()))
	{
		ImGui::TreePop();
	}
}

void Widget_GroupEffectMaker::AddMemberEffect(const wstring& wstrTag, GroupEffectData::EFFECT_TYPE eType)
{
	if ("None" == m_strGroup)
		return;

	wstring wstrKey;
	if (GroupEffectData::TYPE_MESHEFFECT == eType)
		wstrKey = Utils::ToWString(m_strMeshEffect);
	else if (GroupEffectData::TYPE_PARTICLE == eType)
		wstrKey = Utils::ToWString(m_strParticle);

	GroupEffectData::MemberEffect_Desc tDesc
	{
		wstrKey,
		eType,
		0.f,
		false,
		_float3(0.f, 0.f, 0.f),
		_float3(1.f, 1.f, 1.f),
		_float3(0.f, 0.f, 0.f),
	};

	shared_ptr<GroupEffectData> pGroupEffectData = RESOURCES.Get<GroupEffectData>(Utils::ToWString(m_strGroup));
	pGroupEffectData->Add_MemberEffect(tDesc);

	// For. Delete current GroupEffect GameObject and recreate updated GroupEffect GameObject
	Delete();
	Create();
	Set_MemberEffectList(); 
}

void Widget_GroupEffectMaker::Create()
{
	/* Get GroupEffectData gameObject if resource manager already has GroupEffectData
	  if it does not, add new GroupEffectData to resource manager and get */
	// Erase prev created loop mesh effect 
	if (nullptr != m_pCurrentGroup)
		m_pCurrentGroup->Get_GroupEffect()->FreeLoopMember();

	if ("None" == m_strGroup)
		return;	

	CUR_SCENE->Get_Camera(L"Default")->Get_Camera()->Set_EffectToolOn(false);

	shared_ptr<GameObject> pGroupEffectObj = make_shared<GameObject>();
	
	// For. Transform 
	pGroupEffectObj->GetOrAddTransform();
	if (nullptr != CUR_SCENE->Get_GameObject(L"TestAnimModel"))
	{
		_float4x4 mModelWorldMatrix = CUR_SCENE->Get_GameObject(L"TestAnimModel")->Get_Transform()->Get_WorldMatrix();
		pGroupEffectObj->Get_Transform()->Set_WorldMat(mModelWorldMatrix);
	}
	else
		pGroupEffectObj->Get_Transform()->Set_WorldMat(XMMatrixIdentity());

	// For. GroupEffectData 
	wstring wstrFileName = Utils::ToWString(m_strGroup) + L".dat";
	wstring wtsrFilePath = TEXT("..\\Resources\\EffectData\\GroupEffectData\\") + wstrFileName;
	shared_ptr<GroupEffectData> pGroupEffectData = RESOURCES.GetOrAddGroupEffectData(Utils::ToWString(m_strGroup), wtsrFilePath);

	// For. GroupEffect component 
	shared_ptr<GroupEffect> pGroupEffect = make_shared<GroupEffect>();
	pGroupEffectObj->Add_Component(pGroupEffect);
	pGroupEffectObj->Get_GroupEffect()->Set_Tag(pGroupEffectData->Get_GroupEffectDataTag());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectData(pGroupEffectData->Get_MemberEffectData());
	pGroupEffectObj->Get_GroupEffect()->Set_InitWorldMatrix(pGroupEffectObj->Get_Transform()->Get_WorldMatrix());
	pGroupEffectObj->Get_GroupEffect()->Set_MemberEffectMaterials();
	pGroupEffectObj->Init();
	// For. bind to member 
	m_pCurrentGroup = pGroupEffectObj;

	// For. Add Effect GameObject to current scene
	EVENTMGR.Create_Object(pGroupEffectObj);
}

void Widget_GroupEffectMaker::Delete()
{
	if (nullptr != m_pCurrentGroup)
	{
		m_pCurrentGroup->Get_GroupEffect()->FreeLoopMember();
		EVENTMGR.Delete_Object(m_pCurrentGroup);
		m_pCurrentGroup = nullptr;
	}
}

void Widget_GroupEffectMaker::Save(const string& wstrNewGroupTag)
{
	// For. Save New Group Effect 
	if ("." != wstrNewGroupTag)
	{
		string strNewFilePath = "..\\Resources\\EffectData\\GroupEffectData\\";
		strNewFilePath += wstrNewGroupTag + ".dat";
		
		shared_ptr<GroupEffectData> tGroupEffectData = make_shared<GroupEffectData>();
		tGroupEffectData->Set_Tag(Utils::ToWString(wstrNewGroupTag));
		
		shared_ptr<GroupEffect> tGroupEffect = make_shared<GroupEffect>();
		tGroupEffect->Set_Tag(tGroupEffectData->Get_GroupEffectDataTag());

		tGroupEffect->Save(Utils::ToWString(strNewFilePath));
	}

	// For. Update Exist Group Effect 
	else if(nullptr != m_pCurrentGroup)
	{
		string strFilePath = "..\\Resources\\EffectData\\GroupEffectData\\";
		strFilePath += (m_strGroup + ".dat");
		m_pCurrentGroup->Get_GroupEffect()->Save(Utils::ToWString(strFilePath));
		vector <GroupEffectData::MemberEffect_Desc> vNewGroupEffectData = RESOURCES.ReloadOrAddGroupEffectData(Utils::ToWString(m_strGroup), Utils::ToWString(strFilePath))->Get_MemberEffectData();

		Set_MemberEffectList();
		m_pCurrentGroup->Get_GroupEffect()->Set_MemberEffectData(vNewGroupEffectData);
		m_pCurrentGroup->Get_GroupEffect()->Init();
	}

	// Delete prev group effect and create new group effect which has updated info 
	Delete();
	Create();
}
