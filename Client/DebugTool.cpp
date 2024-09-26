#include "pch.h"
#include "DebugTool.h"
#include "Light.h"
#include <Utils.h>
#include "Model.h"
#include "Camera.h"
#include "MainApp.h"
#include "MainCameraScript.h"
#include "MeshRenderer.h"
#ifdef _DEBUGTOOL

bool VectorOfStringGetter(void* data, int n, const char** out_text)
{
	const vector<string>* v = (vector<string>*)data;
	*out_text = (v[0][n]).c_str();
	return true;
}
DebugTool::~DebugTool()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

HRESULT DebugTool::Init()
{

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::StyleColorsDark();
	srand((_uint)time(nullptr));
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(DEVICE.Get(), CONTEXT.Get());

    return S_OK;
}

void DebugTool::Tick()
{

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	if (m_bIsRender)
	{
		ImGui::Begin("Debug");
		if(CUR_SCENE)
		{
			if (BeginTabBar("##a"))
			{
				RenderOptionTab();
				ModelOptionTab();
				CameraOptionTab();
				EndTabBar();
			}
		}

		End();
	}

	if (KEYTAP(KEY_TYPE::ESC))
		m_bIsRender = !m_bIsRender;
}

void DebugTool::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DebugTool::RenderOptionTab()
{
	if(BeginTabItem("Render Option"))
	{
		Separator();
		string fps = "FPS : " + to_string(TIME.GetFPS());
		Text(fps.c_str());

		string preSecondTick = to_string(g_preSecondTick.count());
		Text(("pre Tick : " + preSecondTick).c_str());

		string preSecondRender = to_string(g_preSecondRender.count());
		Text(("pre Render : " + preSecondRender).c_str());

		string SecondTick = to_string(g_secondTick.count());
		Text(("cur Tick : " + SecondTick).c_str());

		string SecondRender = to_string(g_secondRender.count());
		Text(("cur Render : " + SecondRender).c_str());

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
			DragFloat("Motion Blur Power", &data.g_MotionBlurPower,0.01f,0.f,2.f);

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
				ColorEdit4("color", (_float*)&g_lightDiffuse);

			}
		}
		if (CollapsingHeader("RadialBlurData Option"))
		{
			_bool& g_bRadialBlurOn = CUR_SCENE->g_RadialBlurData.g_bRadialBlurOn;
			_float& g_fRadialBlurStrength = CUR_SCENE->g_RadialBlurData.g_fRadialBlurStrength;
			_int& g_iSamples = CUR_SCENE->g_RadialBlurData.g_iSamples;
			_float2& g_vCenterPos = CUR_SCENE->g_RadialBlurData.g_vCenterPos;
			_float& g_fNormalRadius = CUR_SCENE->g_RadialBlurData.g_fNormalRadius;
			Checkbox("g_bRadialBlurOn", &g_bRadialBlurOn);
			DragFloat("g_fRadialBlurStrength", &g_fRadialBlurStrength, 0.01f, 0.01f,1.f);
			InputInt("g_iSamples", &g_iSamples);
			DragFloat2("g_vCenterPos", (_float*)&g_vCenterPos, 0.01f, -0.5f,0.5f);
			DragFloat("g_fNormalRadius", &g_fNormalRadius, 0.01f, 0.01f,1.f);
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
			_float& g_shadowBias = CUR_SCENE->g_fShadowBias;
			DragFloat("g_shadowBias", &g_shadowBias, 0.00001f, 0.f, 1000.f,"%.6f");

			SeparatorText("Vignette");
			_bool& g_VignetteOn = CUR_SCENE->g_VignetteData.g_bVignetteOn;
			_float& g_fVignettePower = CUR_SCENE->g_VignetteData.g_fVignettePower;

			Checkbox("g_VignetteOn", &g_VignetteOn);
			DragFloat("g_fVignettePower", &g_fVignettePower, 0.1f, 0.0f, 20.f);

		}

		EndTabItem();
	}
	
	
}

void DebugTool::ModelOptionTab()
{
	auto model = RESOURCES.Get<Model>(L"Player");
	if (!model)
		return;

	if (BeginTabItem("Model Info"))
	{
		vector<string> partsNames[PARTS_MAX_COUNT];

		for (_uint i = 0; i < PARTS_MAX_COUNT; ++i)
		{
			const auto& parts = RESOURCES.Get_Parts(static_cast<PARTS_INFO>(i));
			for (auto& pair : parts)
			{
				partsNames[i].push_back(Utils::ToString(pair.first));
			}
		}
		static _int partsIndex[PARTS_MAX_COUNT] = {};

		for (_int i = 0; i < PARTS_MAX_COUNT; ++i)
		{
			if(i !=0 && i !=2)
				continue;
			string tag = "##PartsList" + to_string(i);
			Combo(tag.c_str(), &partsIndex[i], VectorOfStringGetter, &partsNames[i], int(partsNames[i].size()));
			SameLine();
			string buttonName = "Set Parts ##" + to_string(i);
			if (Button(buttonName.c_str()))
			{
				model->AddParts(Utils::ToWString(partsNames[i][partsIndex[i]]), static_cast<PARTS_INFO>(i));
			}
		}

		Separator();
		{
			shared_ptr<Model> model = RESOURCES.Get<Model>(L"Kyle");
			if(model)
			{
				static bool alpha_preview = true;
				static bool alpha_half_preview = false;
				static bool drag_and_drop = true;
				static bool options_menu = true;
				static bool hdr = true;

				static Color em = Color(0.f);
				ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
				ImGui::ColorEdit4("ModelEmissive", (float*)&em, ImGuiColorEditFlags_DisplayHSV | misc_flags);

				model->Get_MaterialByName(L"mi_Wp_Kyle_Chain")->Get_MaterialDesc().emissive = em;
				model->Get_MaterialByName(L"mi_Wp_Kyle")->Get_MaterialDesc().emissive = em;
			}
		}

		Separator();
		DragFloat("X", &CUR_SCENE->g_rotX, 0.01f, 0.f, 360);
		DragFloat("Y", &CUR_SCENE->g_rotY, 0.01f, 0.f, 360);
		DragFloat("Z", &CUR_SCENE->g_rotZ, 0.01f, 0.f, 360);
		
		Separator();
		DragFloat2("MinMaxTessellation Dist", (_float*)&GAMEINSTANCE.m_vMinMaxTessellationDistance, 1.f, 0.01f, 1000.f);
		DragFloat2("test", (_float*)&GAMEINSTANCE.m_vTest, 1.f, 0.01f, 1000.f);

		EndTabItem();
	}
}

void DebugTool::CameraOptionTab()
{
	auto water = CUR_SCENE->Get_GameObject(L"Water_Plane");
	if(water)
	{
		if (BeginTabItem("Camera"))
		{
			static bool alpha_preview = true;
			static bool alpha_half_preview = false;
			static bool drag_and_drop = true;
			static bool options_menu = true;
			static bool hdr = true;
			auto& desc = water->Get_MeshRenderer()->Get_RenderParamDesc();
			ImGuiColorEditFlags misc_flags = (hdr ? ImGuiColorEditFlags_HDR : 0) | (drag_and_drop ? 0 : ImGuiColorEditFlags_NoDragDrop) | (alpha_half_preview ? ImGuiColorEditFlags_AlphaPreviewHalf : (alpha_preview ? ImGuiColorEditFlags_AlphaPreview : 0)) | (options_menu ? 0 : ImGuiColorEditFlags_NoOptions);
			ImGui::ColorEdit4("vBaseColor1_Op1", (float*)&desc.vec4Params[1], ImGuiColorEditFlags_DisplayHSV | misc_flags);
			ImGui::ColorEdit4("vBaseColor2_Op1", (float*)&desc.vec4Params[2], ImGuiColorEditFlags_DisplayHSV | misc_flags);

			DragFloat("light intensity", &desc.floatParams[0], 0.01f, 0.f, 1.5f);
			DragFloat("wave speed", &desc.floatParams[1], 0.01f, 0.f, 1.5f);
			DragFloat("wave height", &desc.floatParams[2], 0.01f, 0.01f, 1.5f);
			InputInt("Rim Power", (_int*)(&desc.intParams[0]));
			InputInt("Specular Power", (_int*)(&desc.intParams[1]));


			EndTabItem();
		}
	}
	{
		auto bg = CUR_SCENE->Get_UI(L"UI_Costume_Bg0");
		if (bg)
		{
			if (BeginTabItem("BG UI1"))
			{
				_float3 vPos = bg->Get_Transform()->Get_State(Transform_State::POS).xyz();
				DragFloat3("Pos", (_float*)&vPos);
				bg->Get_Transform()->Set_State(Transform_State::POS, _float4(vPos, 1.f));



				EndTabItem();
			}
		}
	}
	{
		auto bg = CUR_SCENE->Get_UI(L"UI_Costume_Bg1");
		if (bg)
		{
			if (BeginTabItem("BG UI2"))
			{
				_float3 vPos = bg->Get_Transform()->Get_State(Transform_State::POS).xyz();
				DragFloat3("Pos", (_float*)&vPos);
				bg->Get_Transform()->Set_State(Transform_State::POS, _float4(vPos, 1.f));



				EndTabItem();
			}
		}
	}
	{
		auto bg = CUR_SCENE->Get_Camera(L"After_UI");
		if (bg)
		{
			if (BeginTabItem("After_UI Camera"))
			{
				_float3 vPos = bg->Get_Transform()->Get_State(Transform_State::POS).xyz();
				DragFloat3("Pos", (_float*)&vPos);
				bg->Get_Transform()->Set_State(Transform_State::POS, _float4(vPos, 1.f));


				EndTabItem();
			}
		}
	}
	{
		auto bg = CUR_SCENE->Get_GameObject(L"TestUIModel");
		if (bg)
		{
			if (BeginTabItem("Model"))
			{
				_float3 vPos = bg->Get_Transform()->Get_State(Transform_State::POS).xyz();
				DragFloat3("Pos", (_float*)&vPos);
				bg->Get_Transform()->Set_State(Transform_State::POS, _float4(vPos, 1.f));
				_float3 vRot = bg->Get_Transform()->Get_RollPitchYaw();
				DragFloat3("Rotation", (_float*)&vRot);
				bg->Get_Transform()->Set_Rotation(vRot);
				EndTabItem();
			}
		}
	}
}

#endif