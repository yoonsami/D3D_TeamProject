#pragma once
#include "ImGuiToolMgr.h"

class Widget_EffectMaker_Mesh
{
public:
	enum MeshEffectType {MT_PARTICLE, MT_EFFECT, MT_END};
	enum ParticleMovement {PM_END};

	typedef struct tagOption
	{
		_bool					bIsOption_On = { false };

		_int					iColoringOption = { 0 };
		pair<_int, string>		Texture = { 0, "None" };
		_int					iFlipOption = { 0 };

		_bool					bUseSolidColor = { false };
		ImVec4					vColorBase1 = ImVec4(1.f, 1.f, 1.f, 1.f);
		ImVec4					vColorBase2 = ImVec4(1.f, 1.f, 1.f, 1.f);
		ImVec4					vColorDest1 = ImVec4(1.f, 1.f, 1.f, 1.f);
		ImVec4					vColorDest2 = ImVec4(1.f, 1.f, 1.f, 1.f);

		_float					fTiling_Op[2] = { 0.f, 0.f };
		_float					fUVSpeed_Op[2] = { 0.f, 0.f };
		_float					fContrast = { 1.f };
		_float					fAlphaOffset = { 1.f };
	}Texture_Option;

public:
	Widget_EffectMaker_Mesh();
	~Widget_EffectMaker_Mesh();

public:
	void	Initialize();
	void	Tick();

private:
	/* Initialize List */
	void					Set_Mesh_List();
	void					Set_Texture_List();
	void					Set_FinishedEffect_List();

	/* ImGui Windows */
	void					ImGui_EffectMaker();
	void					ImGui_FinishedEffect();
	void					ImGui_SaveMsgBox();
	void					ImGui_TextureList();

	/* Options */
	void					Option_Guizmo();
	void					Option_Property();
	void					Option_Mesh();
	void					Option_SpriteAnimation();

	void					Option_TextureOp(_int iIndex);

	void					Option_RimLight();
	void					Option_Normal();
	void					Option_Blend();
	void					Option_Overlay();
	void					Option_Dissolve();
	void					Option_Distortion();

	void					Option_InitTransform();
	void					Option_Movement();

	/* Sub Widgets */
	void					SubWidget_TextureCombo(_int* iSelected, string* strSelected, string strFilePath, const char* pszWidgetKey);
	void					SubWidget_TextureList();
	void					SubWidget_ImageViewer(string strFileName, string strFilePath, const char* pszWidgetKey);
	void					SubWidget_SettingTexUV(_float* arrTiling, _float* arrTexUVSpeed, const char* pszWidgetKey, const char* pszWidgetKey2);
	void					SubWidget_Curve1(_float2* pPoints, const string& strKey); // 0 ~ 1
	void					SubWidget_Curve2(_float2* pPoints, const string& strKey); // 0 ~ 1, -1 ~ 1
	
	/* Function */
	void					Create();
	void					Save();
	void					Load();
	void					Show_Guizmo();

	/* Utils */
	ImVec2					CatMull_Rom(_float2* pPoints, _float t);
	Color					ImVec4toColor(ImVec4 imvec);
	ImVec2					XmVec2toImVec2(_float2 iSrc);
	ImVec4					ColorToImVec4(Color color);
	_int					GetIndex_FromTexList(string strValue);
	_int					GetIndex_FromMeshList(string strValue);
	_bool					Equal(_float2 vSrc, _float2 vDest);
	_bool					Equal(_float3 vSrc, _float3 vDest);
	_bool					Equal(ImVec4 vSrc, ImVec4 vDest);
	_bool					Equal(_float* arrSrc, _float* arrDest, _int iSize);

private:
	/* Widget On/Off*/
	_bool					m_bSaveMsgBox_On = { false };
	_bool					m_bTextureList_On = { false };

	/* For. guizmo */
	weak_ptr<GameObject>    m_pCurrMeshEffect;
	_bool					m_bIsMeshEffectLocked = { false };
	_bool					m_bGuizmoTranslation_On = { false };
	_bool					m_bGuizmoRotation_On = { false };

	/* Use in SubWidget_TextureList */
	_int*					m_iTexture_TextureList;
	string*					m_pTextureTag_TextureList;
	const char*				m_pszWidgetKey_TextureList;

	/* Effect List */
	_uint					m_iNumFinishedEffects = { MT_PARTICLE };
	vector<string>			m_vecFinishedEffects;
	const char**			m_pszFinishedEffects = { nullptr };
	_int					m_iFinishedObject = { 0 };
	string					m_strFinishedObject = {"None"};

	/* Property */
	char					m_szTag[MAX_PATH] = "-";
	_float					m_fDuration = { 3.f };
	_bool					m_bLightOn = { false };
	_float					m_fLightIntensity = { 1.f };
	_bool					m_bUseFadeOut = { true };
	_int					m_iSamplerType = { 1 };
	_bool					m_bIsLoop = { false };
	_bool					m_bIsFollowGroup_OnlyTranslate = { false };
	_bool					m_bIsFollowGroup_LookSameDir = { false };
	_bool					m_bIsFDistortion = { false };
	_bool					m_bIsSSD = { false };

	/* Mesh Count */
	_int					m_iMeshCnt = { 1 };
	_float					m_fCreateInterval = { 0.5f };
	_int					m_iParticleDurationOption = { 0 };
	_float					m_fParticleDuration[2] = { 0.1f, 1.f };

	/* Mesh list */
	_uint					m_iNumMeshes = { 0 };
	vector<string>			m_vecMeshes;
	const char**			m_pszMeshes = { nullptr };
	_uint					m_iMesh = { 0 };
	string					m_strMesh = { "None" };

	/* Texture list */
	_uint					m_iNumUniversalTextures = { 0 };
	vector<string>			m_vecUniversalTextures;
	const char**			m_pszUniversalTextures = { nullptr };

	/* Coloring Options */
	_bool					m_bColorChangingOn = { false };

	/* Sprite Animation */
	_bool					m_bUseSpriteAnimation = { false };
	_int					m_iSpriteAni_Count[2] = { 1, 1 }; 
	_float					m_fSpriteAni_Speed = { 1.f };

	/* Texture Options */
	_int					m_iTexOptionCnt = { 3 };
	Texture_Option			m_TexOption[3];

	/* Blend */
	_bool					m_bBlend_On = { false };
	pair<_int, string>		m_BlendTexture = { 0, "None" };
	_float					m_fTiling_Blend[2] = { 0.f, 0.f };
	_float					m_fUVSpeed_Blend[2] = { 0.f, 0.f };
	_float					m_fAlphaOffset_Blend = { 1.f };

	/* Overlay */
	_bool					m_bOverlay_On = { false };
	pair<_int, string>		m_OverlayTexture = { 0, "None" };
	ImVec4					m_vOverlayColor_Base = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec4					m_vOverlayColor_Dest = ImVec4(1.f, 1.f, 1.f, 1.f);
	_float					m_fTiling_Overlay[2] = { 0.f, 0.f };
	_float					m_fUVSpeed_Overlay[2] = { 0.f, 0.f };

	/* Rim Light */
	_bool					m_bRimLight_On = { false };
	ImVec4					m_vRimLightColor_Base = ImVec4(1.f, 1.f, 1.f, 1.f);
	ImVec4					m_vRimLightColor_Dest = ImVec4(1.f, 1.f, 1.f, 1.f);
	_float2					m_vCurvePoint_RimLight[4] = { _float2(0.f, 0.f),_float2(0.3f, 0.5f), _float2(0.7f, 0.5f), _float2(1.f, 1.f) };

	/* Normal */
	_bool					m_bNormal_On = { false };
	pair<_int, string>		m_NormalTexture = { 0, "None" };

	/* Dissolve */
	_bool					m_bDissolve_On = { false };
	pair<_int, string>		m_DissolveTexture = { 0, "None" };
	_float					m_fTiling_Dissolve[2] = { 0.f, 0.f };
	_float					m_fUVSpeed_Dissolve[2] = { 0.f, 0.f };
	_float2					m_vCurvePoint_Dissolve[4] = {_float2(0.f, 0.f),_float2(0.3f, 0.5f), _float2(0.7f, 0.5f), _float2(1.f, 1.f)};

	/* Distortion */
	_bool					m_bDistortion_On = { false };
	pair<_int, string>		m_DistortionTexture = { 0, "None" };
	_float					m_fTiling_Distortion[2] = { 0.f, 0.f };
	_float					m_fUVSpeed_Distortion[2] = { 0.f, 0.f };

	/* Initliaze Transform */
	_int					m_iInitPosOption = { 0 };					// static, random in range 
	_float					m_fPosRange[3] = { 0.f, 0.f, 0.f };		
	_float					m_fPosOffsetInTool[3] = { 0.f, 0.f, 0.f };	

	_int					m_iInitScaleOption = { 0 };					// static, random in range 
	_float					m_fInitScale_Min[3] = { 1.f, 1.f, 1.f };		
	_float					m_fInitScale_Max[3] = { 1.f, 1.f, 1.f };		

	_int					m_iInitRotationOption = { 0 };				// static, random in range 
	_float					m_fInitRotation_Min[3] = { -1.f, -1.f, -1.f };
	_float					m_fInitRotation_Max[3] = { 1.f, 1.f, 1.f };
	 
	/* Movement */
	_int					m_iTranslateOption = { 0 };			// no change, Go stright, Go back, Go left, Go right, Go to TargetPos, Go to Random TargetPos
	_float					m_fTranslateSpeed = { 0.f };                               
	_float					m_fEndPositionOffset_Min[3] = { 0.f, 0.f, 0.f };	
	_float					m_fEndPositionOffset_Max[3] = { 0.f, 0.f, 0.f };
	_int					m_iSpeedType = { 0 };
	_float2					m_vCurvePoint_Force[4] = { _float2(0.f, 0.f),_float2(0.3f, 0.5f), _float2(0.7f, 0.5f), _float2(1.f, 1.f) };	
	
	_int					m_iScalingOption = { 0 };				// no change, change to endscale
	_float					m_fEndScaleOffset[3] = { 0.f, 0.f, 0.f };
	_int					m_iScaleSpeedType = { 2 };
	_float2					m_vCurvePoint_Scale[4] = { _float2(0.f, 0.f),_float2(0.3f, 0.5f), _float2(0.7f, 0.5f), _float2(1.f, 1.f) };

	_int					m_iTurnOption = { 0 };			// no change, turn with static, turn with random 
	_float					m_fTurnSpeed = { 0.f };
	_float					m_fRandomAxis_Min[3] = { 0.f, 0.f, 0.f };		
	_float					m_fRandomAxis_Max[3] = { 0.f, 0.f, 0.f };
	_bool					m_bBillbordAxes[2] = { true, true};
	_int					m_iTurnAxesOption = { 0 };

	/* const */
	const string					m_strTexturePath = "../Resources/Textures/Universal/";
	const string					m_strNormalTexturePath = "../Resources/Textures/Universal/Normal/";
	const ImGuiColorEditFlags		ColorEdit_flags = 0 | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_HDR;

	/* for. decal */
	_int					m_iCubeMeshIndex = { 0 };
	const string			m_strSSDMesh = "Fxmesh_cube_01";
};

