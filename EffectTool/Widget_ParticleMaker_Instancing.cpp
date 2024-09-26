#include "pch.h"
#include "Widget_ParticleMaker_Instancing.h"

/* Components */
#include "Particle.h"

Widget_ParticleMaker_Instancing::Widget_ParticleMaker_Instancing()
{
}

Widget_ParticleMaker_Instancing::~Widget_ParticleMaker_Instancing()
{
}

void Widget_ParticleMaker_Instancing::Initialize()
{
	Set_Texture_List();
}

void Widget_ParticleMaker_Instancing::Tick()
{
	ImGui::SetNextWindowPos(ImVec2(g_iWinSizeX - 450, 0));
	ImGui::Begin("Particle Maker");
	ImGui_ParticleMaker();
	ImGui::End();
}

void Widget_ParticleMaker_Instancing::Set_Texture_List()
{
	/* ../Resources/Universal/ 폴더 내의 택스처들의 이름을 리스트로 만들어 저장하기 */ \
	
	m_vecUniversalTextures.push_back("None");

	wstring assetPath = L"..\\Resources\\Textures\\Universal\\";
	for (auto& entry : fs::recursive_directory_iterator(assetPath))
	{
		if (entry.is_directory())
			continue;

		if (entry.path().extension().wstring() != L".tga" && entry.path().extension().wstring() != L".png")
			continue;

		string tag = entry.path().string();
		tag = entry.path().filename().string();
		m_vecUniversalTextures.push_back(tag);
	}

	m_iNumUniversalTextures = (_uint)m_vecUniversalTextures.size();
	m_pszUniversalTextures = new const char* [m_iNumUniversalTextures];

	int iIndex = 0;
	for (auto iter : m_vecUniversalTextures)
	{
		m_pszUniversalTextures[iIndex] = m_vecUniversalTextures[iIndex].c_str();
		iIndex++;
	}
}

void Widget_ParticleMaker_Instancing::ImGui_ParticleMaker()
{
	Option_ParticleObjectProperty();
	Option_ParticleProperty();
	Option_Textures();
	Option_Color();
	Option_Transform();
	Option_Movement();

	/* For. Create, Save, Load Effect */
	ImGui::Spacing();
	ImGui::SeparatorText(" Create/Delete ");
	if (ImGui::Button("Create"))
		Create();
	ImGui::SameLine();
	if (ImGui::Button("Save"))
		Save();
	ImGui::SameLine();
	if (ImGui::Button("Load"))
		Load();
	ImGui::SameLine();
}

void Widget_ParticleMaker_Instancing::Option_ParticleObjectProperty()
{
	ImGui::SeparatorText("Particle Object's Property");

	ImGui::InputText("Tag", m_szTag, MAX_PATH);
	ImGui::Spacing();

	ImGui::InputFloat("Duration", &m_fDuration);
	ImGui::Spacing();

	ImGui::InputInt2("Particle Count(min, max)", m_iParticleCnt);
	ImGui::InputInt("Max Accumulated Count", &m_iMaxInstanceCnt);
	ImGui::Spacing();

	ImGui::Checkbox("Creaet Once", &m_bIsbCreateOnce);
	if (!m_bIsbCreateOnce)
		ImGui::InputFloat("Create Interval", &m_fCreateInterval);
	ImGui::Spacing();
}

void Widget_ParticleMaker_Instancing::Option_ParticleProperty()
{
	ImGui::SeparatorText("Particle's Property");

	const char* pszItems_LifeTime[] = { "Static", "Random" };
	if (ImGui::BeginCombo("LifeTime Options##LifeTime", pszItems_LifeTime[m_iSelected_LifeTimeOption], 0))
	{
		for (_uint n = 0; n < IM_ARRAYSIZE(pszItems_LifeTime); n++)
		{
			const bool is_selected = (m_iSelected_LifeTimeOption == n);
			if (ImGui::Selectable(pszItems_LifeTime[n], is_selected))
				m_iSelected_LifeTimeOption = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	switch (m_iSelected_LifeTimeOption)
	{
	case 0: // Constant
		ImGui::InputFloat("LifeTime", &m_fLifeTime[0]);
		m_fLifeTime[1] = m_fLifeTime[0];
		break;
	case 1: // Random
		ImGui::InputFloat2("LifeTime(min, max)", m_fLifeTime);
		break;
	}
	ImGui::Spacing();

	const char* pszItems_Billbord[] = { "No Billbord", "Bilbord" };
	if (ImGui::BeginCombo("Options##Billbord", pszItems_Billbord[m_iSelected_BillbordOption], 0))
	{
		for (_uint n = 0; n < IM_ARRAYSIZE(pszItems_Billbord); n++)
		{
			const bool is_selected = (m_iSelected_BillbordOption == n);
			if (ImGui::Selectable(pszItems_Billbord[n], is_selected))
				m_iSelected_BillbordOption = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
}

void Widget_ParticleMaker_Instancing::Option_Textures()
{
	ImGui::SeparatorText("Textures");

	if (ImGui::BeginCombo("Shape", m_pszUniversalTextures[m_iSelected_Texture_Shape], 0))
	{
		for (_uint n = 0; n < m_iNumUniversalTextures; n++)
		{
			const bool is_selected = (m_iSelected_Texture_Shape == n);
			if (ImGui::Selectable(m_pszUniversalTextures[n], is_selected))
			{
				m_iSelected_Texture_Shape = n;
				m_strSelected_Texture_Shape = m_pszUniversalTextures[m_iSelected_Texture_Shape];
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	
	if (ImGui::BeginCombo("Dissolve##Texture", m_pszUniversalTextures[m_iSelected_Texture_Dissolve], 0))
	{
		for (_uint n = 0; n < m_iNumUniversalTextures; n++)
		{
			const bool is_selected = (m_iSelected_Texture_Dissolve == n);
			if (ImGui::Selectable(m_pszUniversalTextures[n], is_selected))
			{
				m_iSelected_Texture_Dissolve = n;
				m_strSelected_Texture_Dissolve = m_pszUniversalTextures[m_iSelected_Texture_Dissolve];
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("Option1##Texture", m_pszUniversalTextures[m_iSelected_Texture_Option1], 0))
	{
		for (_uint n = 0; n < m_iNumUniversalTextures; n++)
		{
			const bool is_selected = (m_iSelected_Texture_Option1 == n);
			if (ImGui::Selectable(m_pszUniversalTextures[n], is_selected))
			{
				m_iSelected_Texture_Option1 = n;
				m_strSelected_Texture_Option1 = m_pszUniversalTextures[m_iSelected_Texture_Option1];
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	if (ImGui::BeginCombo("Option2##Texture", m_pszUniversalTextures[m_iSelected_Texture_Option2], 0))
	{
		for (_uint n = 0; n < m_iNumUniversalTextures; n++)
		{
			const bool is_selected = (m_iSelected_Texture_Option2 == n);
			if (ImGui::Selectable(m_pszUniversalTextures[n], is_selected))
			{
				m_iSelected_Texture_Option2 = n;
				m_strSelected_Texture_Option2 = m_pszUniversalTextures[m_iSelected_Texture_Option2];
			}
			if (is_selected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void Widget_ParticleMaker_Instancing::Option_Color()
{
	ImGuiColorEditFlags ColorEdit_flags = 0 | ImGuiColorEditFlags_AlphaBar;	// RGB, Alpha Bar
	
	ImGui::SeparatorText("Color");

	ImGui::Text("Start Color");
	const char* pszItems[] = { "Use Shape Texture Color", "Static", "Gradation"};
	if (ImGui::BeginCombo("Options##Color", pszItems[m_iSelected_StartColorOption], 0))
	{
		for (_uint n = 0; n < IM_ARRAYSIZE(pszItems); n++)
		{
			const bool is_selected = (m_iSelected_StartColorOption == n);
			if (ImGui::Selectable(pszItems[n], is_selected))
				m_iSelected_StartColorOption = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	switch (m_iSelected_StartColorOption)
	{
	case 1: // Static
		ImGui::ColorEdit4("Selected Color", (float*)&m_vStartColor, ImGuiColorEditFlags_DisplayHSV | ColorEdit_flags);
		m_vEndColor = m_vStartColor;
		break;
	case 2: // Gradation
		ImGui::ColorEdit4("Start Color", (float*)&m_vStartColor, ImGuiColorEditFlags_DisplayHSV | ColorEdit_flags);
		ImGui::ColorEdit4("End Color", (float*)&m_vEndColor, ImGuiColorEditFlags_DisplayHSV | ColorEdit_flags);
		break;
	}
	ImGui::Spacing();

	if (0 != m_iSelected_StartColorOption)
	{
		ImGui::Text("Change Color while alive");
		const char* pszItems2[] = { "No Change", "Change" };
		if (ImGui::BeginCombo("Changing Options##Color", pszItems2[m_iSelected_ChangingColorOption], 0))
		{
			for (_uint n = 0; n < IM_ARRAYSIZE(pszItems2); n++)
			{
				const bool is_selected = (m_iSelected_ChangingColorOption == n);
				if (ImGui::Selectable(pszItems2[n], is_selected))
					m_iSelected_ChangingColorOption = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		switch (m_iSelected_ChangingColorOption)
		{
		case 0: // No Change
			m_vDestColor = m_vStartColor;
			break;
		case 1: // Change
			ImGui::ColorEdit4("Dest Color", (float*)&m_vDestColor, ImGuiColorEditFlags_DisplayHSV | ColorEdit_flags);
			break;
		}
	}
	ImGui::Spacing();

	ImGui::Text("Etc Coloring Option");
	//ImGui::Checkbox("On Fade Out", &m_bUseFadeOut); // ( does not works on Instance particle cause of blend off )
	ImGui::SliderFloat("Brigher Offset", &m_fGradationByAlpha_Brighter, 0.f, 1.f);
	ImGui::SliderFloat("Darker Offset", &m_fGradationByAlpha_Darker, 0.f, 1.f);
}

void Widget_ParticleMaker_Instancing::Option_Transform()
{
	ImGui::SeparatorText("Transform");

	if (ImGui::TreeNode("Create Position"))
	{
		ImGui::InputFloat3("Center Position", m_fCenterPosition);
		ImGui::InputFloat3("Range##CreatePosition", m_fCreateRange);
		ImGui::TreePop();
	}
	ImGui::Spacing();

	if(ImGui::TreeNode("Scale"))
	{
		ImGui::InputFloat2("Start Scale(min,max)", m_fStartScale);
		ImGui::Spacing();

		const char* pszItems[] = { "Static value", "Add to StartScale" };
		if (ImGui::BeginCombo("End Scale Options##Scale", pszItems[m_iSelected_EndScaleOption], 0))
		{
			for (_uint n = 0; n < IM_ARRAYSIZE(pszItems); n++)
			{
				const bool is_selected = (m_iSelected_EndScaleOption == n);
				if (ImGui::Selectable(pszItems[n], is_selected))
					m_iSelected_EndScaleOption = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		switch (m_iSelected_EndScaleOption)
		{
		case 0: // Constant
			ImGui::InputFloat2("End Scale(min,max)##EndScale", m_fEndScale);
			break;
		case 1: // Offset
			ImGui::InputFloat2("Offset to Add Start Scale(min,max)##EndScale", m_fEndScale);
			break;
		}
		ImGui::TreePop();
	}
	ImGui::Spacing();

	if(ImGui::TreeNode("Rotation"))
	{
		if (0 == m_iSelected_BillbordOption)
		{
			ImGui::Text("Rotation Speed");
			ImGui::InputFloat2("Speed (min, max)##RotationSpeed", m_fRotationSpeed_X);
			//ImGui::InputFloat2("Y (min, max)##RotationSpeed", m_fRotationSpeed_Y);
			//ImGui::InputFloat2("Z (min, max)##RotationSpeed", m_fRotationSpeed_Z);
			ImGui::Spacing();

			ImGui::Text("Random Look (Rotates about the axis.)");
			ImGui::InputFloat2("X (min, max)##RotationAngle", m_fRotationAngle_X);
			ImGui::InputFloat2("Y (min, max)##RotationAngle", m_fRotationAngle_Y);
			ImGui::InputFloat2("Z (min, max)##RotationAngle", m_fRotationAngle_Z);

		}
		else if (1 == m_iSelected_BillbordOption)
		{
			ImGui::Text(" ( Can't rotate rect while using Billbord) ");
			m_fRotationSpeed_X[0] = 0.f;
			m_fRotationSpeed_X[1] = 0.f;
			m_fRotationSpeed_Y[0] = 0.f;
			m_fRotationSpeed_Y[1] = 0.f;
			m_fRotationSpeed_Z[0] = 0.f;
			m_fRotationSpeed_Z[1] = 0.f;

			m_fRotationAngle_X[0] = 0.f;
			m_fRotationAngle_X[1] = 0.f;
			m_fRotationAngle_Y[0] = 0.f;
			m_fRotationAngle_Y[1] = 0.f;
			m_fRotationAngle_Z[0] = 0.f;
			m_fRotationAngle_Z[1] = 0.f;

		}
		ImGui::TreePop();
	}
	ImGui::Spacing();
}

void Widget_ParticleMaker_Instancing::Option_Movement()
{
	ImGui::SeparatorText("Particle Object's Property");

	const char* pszItems[] = { "No Move" };
	if (ImGui::BeginCombo("Movement", pszItems[m_iSelected_MovementOption], 0))
	{
		for (_uint n = 0; n < IM_ARRAYSIZE(pszItems); n++)
		{
			const bool is_selected = (m_iSelected_MovementOption == n);
			if (ImGui::Selectable(pszItems[n], is_selected))
				m_iSelected_MovementOption = n;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::Spacing();

	if (ImGui::TreeNode("Speed"))
	{
		ImGui::InputFloat2("Start Speed(min,max)", m_fStartSpeed);
		ImGui::Spacing();

		const char* pszItems[] = { "Static value", "Add to StartScale" };
		if (ImGui::BeginCombo("End Speed Options##Scale", pszItems[m_iSelected_EndSpeedOption], 0))
		{
			for (_uint n = 0; n < IM_ARRAYSIZE(pszItems); n++)
			{
				const bool is_selected = (m_iSelected_EndSpeedOption == n);
				if (ImGui::Selectable(pszItems[n], is_selected))
					m_iSelected_EndSpeedOption = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		switch (m_iSelected_EndSpeedOption)
		{
		case 0: // Static
			ImGui::InputFloat2("End Speed(min,max)##EndSpeed", m_fEndSpeed);
			break;
		case 1: // Offset
			ImGui::InputFloat2("Offset to Add Start Speed(min,max)##EndSpeed", m_fEndSpeed);
			break;
		}
		ImGui::TreePop();
	}
	ImGui::Spacing();
	
}

void Widget_ParticleMaker_Instancing::Create()
{
	// For. Particle이 될 게임오브젝트 생성
	shared_ptr<GameObject> ParticleObj = make_shared<GameObject>();

	// For. Setting GameObject 
	ParticleObj->Set_Name(Utils::ToWString(m_szTag));
	
	// For. GameObject에 transform component 붙이기 + 세팅하기 
	ParticleObj->GetOrAddTransform();
	ParticleObj->Get_Transform()->Set_State(Transform_State::POS, _float4(0.f, 0.f, 0.f, 1.f));

	// For. attach Particle component to GameObject
	shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Particle2.fx");
	shared_ptr<Particle> particle =  make_shared<Particle>(shader);
	ParticleObj->Add_Component(particle);

	// For. Setting Particle component
	Particle::DESC tParticleDesc{
		m_szTag,

		"../Resources/Textures/Universal/" + m_strSelected_Texture_Shape,
		"../Resources/Textures/Universal/" + m_strSelected_Texture_Dissolve,
		"../Resources/Textures/Universal/" + m_strSelected_Texture_Option1,
		"../Resources/Textures/Universal/" + m_strSelected_Texture_Option2,

		m_iSelected_StartColorOption,
		Color(m_vStartColor.x, m_vStartColor.y, m_vStartColor.z, m_vStartColor.w),
		Color(m_vEndColor.x, m_vEndColor.y, m_vEndColor.z, m_vEndColor.w),
		Color(m_vDestColor.x, m_vDestColor.y, m_vDestColor.z, m_vDestColor.w),
		
		m_fGradationByAlpha_Brighter,
		m_fGradationByAlpha_Darker,
		m_bUseFadeOut,

		m_fDuration,

		m_bIsbCreateOnce,
		m_fCreateInterval,

		m_iParticleCnt[0],
		m_iParticleCnt[1],
		m_iMaxInstanceCnt,

		_float2(m_fLifeTime),

		m_iSelected_BillbordOption,

		m_iSelected_MovementOption,
		_float4(m_vMovementOffsets),

		_float2(m_fStartSpeed),
		_float2(m_fEndSpeed),
		m_iSelected_EndSpeedOption,

		_float2(m_fStartScale),
		_float2(m_fEndScale),
		m_iSelected_EndScaleOption,

		_float3(m_fCenterPosition),
		_float3(m_fCreateRange),
		_float4(m_fCreateOffset),

		_float2(m_fRotationSpeed_X),
		_float2(m_fRotationSpeed_Y),
		_float2(m_fRotationSpeed_Z),
		_float2(m_fRotationAngle_X),
		_float2(m_fRotationAngle_Y),
		_float2(m_fRotationAngle_Z)
	};
	ParticleObj->Get_Particle()->Init(&tParticleDesc);

	// For. 위젯의 Target Object에 바인딩해두기 
	m_pTargetParticle = ParticleObj;

	// For. 씬에 GameObject 추가 
	EVENTMGR.Create_Object(ParticleObj);
}

void Widget_ParticleMaker_Instancing::Load()
{
}

void Widget_ParticleMaker_Instancing::Save()
{
}
