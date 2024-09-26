#pragma once
#include "BindShaderDesc.h"

class GameObject;

class Scene : public enable_shared_from_this<Scene>
{
public:
	enum SCENE_STATE
	{
		SCENE_READY,
		SCENE_PLAY,
		SCENE_DISAPPEAR,
		SCENE_NONE,
	};

public:
	Scene();
	~Scene();

public:
	virtual void Init();
	virtual void Tick();
	virtual void Late_Tick();
	virtual void Final_Tick();
	virtual void Render();
	virtual HRESULT Load_Scene();

	friend class EventMgr;
	virtual void Add_GameObject_Front(shared_ptr<GameObject> object, _bool staticFlag = false);

	void Set_AttackCall(_bool bFlag) { m_bAttackCall = bFlag; }
	_bool Get_AttackCall() { return m_bAttackCall; }
protected:
	virtual void Add_GameObject(shared_ptr<GameObject> object, _bool staticFlag = false);
	virtual void Remove_GameObject(shared_ptr<GameObject> object);

public:
	wstring							Get_Name()			{ return m_strSceneName; }
	shared_ptr<GameObject>			Get_Light()			{ return m_Lights.empty() ? nullptr : *m_Lights.begin(); }
	list<shared_ptr<GameObject>>&	Get_Objects()		{ return m_GameObjects; }
	SCENE_STATE						Get_SceneState()	{ return m_eSceneState; }
	LightParams&					Get_LightParams()	{ return m_LightParams; }
	list<shared_ptr<GameObject>>&	Get_StaticObjects() { return m_StaticObject; }
	_float							Get_LoaingPercent() { return m_fLoadPercent; }
	shared_ptr<GameObject>			Get_UI(const wstring& strName);
	shared_ptr<GameObject>			Get_Camera(const wstring& cameraName);
	shared_ptr<GameObject>			Get_UICamera();
	shared_ptr<GameObject>			Get_MainCamera();
	shared_ptr<GameObject>			Get_GameObject(const wstring& name);
	shared_ptr<GameObject>			Get_Player() { return m_pPlayer.lock(); }

	_bool							Is_Static(shared_ptr<GameObject> obj);
	void							Set_Name(const wstring& name)		{ m_strSceneName = name; }
	void							Set_SceneState(SCENE_STATE state)	{ m_eSceneState = state; }

	void Swap_Object(const wstring& leftObjName, const wstring& rightObjName);
	void Render_ToneMapping();
	void Render_BackBuffer();
	
	
	void Load_UIFile(const wstring& path, const list<shared_ptr<GameObject>>& staticObjects, _bool bRender = true, _bool bTick = true);
	void Load_UIFile(const wstring& path, vector<weak_ptr<GameObject>>& addedObjects);

	const wstring& Get_FinalRenderTarget() { return m_wstrFinalRenderTarget; }

protected:
	virtual void Load_MapFile(const wstring& _mapFileName,shared_ptr<GameObject> pPlayer);
	void PickUI();

protected:
	void Gather_LightData();
	void Sort_GameObjects();
	void Render_Shadow();
	void Render_MotionBlur();
	void Render_Deferred();
	void Render_SSAO();

	void Render_SSAOBlur(_uint blurCount);

	void Render_Lights();
	void Render_LightFinal();

	void Render_OutLine();

	void Render_MotionBlurFinal();
	void Render_SkyBox();
	void Render_Forward();

	void Render_BloomMap();
	void Render_BloomMapScaling();
	void Render_BloomFinal();
	void Render_DOFMap();
	void Render_DOFMapScaling(_uint blurCount);
	void Render_DOFFinal();

	void Render_Distortion();
	void Render_Distortion_Final();

	void Render_LensFlare();

	void Render_Fog();
	void Render_Aberration();
	void Render_RadialBlur();

	void Render_Vignette();

	void Render_Debug();

	void Render_UI();

	void Render_AfterUI();

	void Render_FXAA();

private:
	void SSAO_MakeOffsetVector();
	void SSAO_MakeFrustumFarCorners();

protected:
	_float m_fLoadPercent = 0.f;
	wstring m_strSceneName = L"";
	list<shared_ptr<GameObject>> m_GameObjects;
	wstring m_strSceneDataPath = L"../Resources/Scene/";
	//Cache
	vector<shared_ptr<GameObject>> m_Cameras;
	//Cache
	vector<shared_ptr<GameObject>> m_Lights;
	vector<shared_ptr<GameObject>> m_UI;
	vector<shared_ptr<GameObject>> m_ButtonUI;

	LightParams m_LightParams;
	SCENE_STATE m_eSceneState = SCENE_READY;

	_bool m_bSceneFinished = false;
	_bool m_bAttackCall = false;

	_uint m_iLevelIndex = 0;
	_float4 m_vFrustumFarCorner[4];
	_float4 m_vOffsets[14];
	weak_ptr<GameObject> m_pPlayer;

protected:
	list<shared_ptr<GameObject>> m_StaticObject;

	wstring m_wstrFinalRenderTarget = L"";
	_bool m_bRenderDebug = false;

public:
	_float g_fBrightness = 0.f;
	_float g_fContrast = 1.f;
	_float g_Saturation = 1.f;
	_float g_fShadowBias = _float(0.00017f);
	_float g_fMaxWhite = 1.f;
	_int g_iTMIndex = 1;

	struct SSAOData
	{
		_bool g_bSSAO_On = true;
		_float g_fOcclusionRadius = 0.07f;
		_float g_OcclusionFadeStart = 0.3f;
		_float g_OcclusionFadeEnd = 1.0f;

	};
	SSAOData g_SSAOData{};

	struct FogData
	{
		_bool g_FogOn = false;
		_float g_fogStart = 15.f;
		_float g_fogEnd = 150.f;
		_int g_fogMode = 0;
		_float g_fogDensity = 1.f;
		Color g_fogColor = Color(1.f);
	};
	FogData g_FogData{};

	_bool g_bFXAAOn = false;
	_bool g_bAberrationOn = false;
	_float g_fAberrationPower = 0.f;
	_bool g_bLensFlare = false;

	_bool g_bDrawOutline = false;

	struct MotionBlurData
	{
		_bool g_bMotionBlurOn = false;
		int g_iBlurCount = 0;
		_float g_MotionBlurPower = 1.f;
	};
	MotionBlurData g_MotionBlurData{};

	struct BloomData
	{
		_bool g_BloomOn = false;
		_float g_BloomMin = 0.99f;
	};
	BloomData g_BloomData{};


	struct LIGHTPowerData
	{
		_float g_specularPower = 10.f;
		_float g_rimPower = 10.f;
	};
	LIGHTPowerData g_LightPowerData{};


	_bool g_bPBR_On = true;
	_float g_lightAttenuation = 100.f;
	_float g_ambientRatio = 0.5f;

	struct DOFData
	{
		_bool g_bDOF_On = false;
		_float g_FocusDepth = 7.f;
		_float g_DOFRange = 100.f;
	};
	DOFData g_DOFData{};

	nanoseconds time{};

	nanoseconds preSecond{};
	nanoseconds second{};

	struct RadialBlurData
	{
		_bool g_bRadialBlurOn = false;
		_float g_fRadialBlurStrength = 0.125f;
		_int g_iSamples = 64;
		_float2 g_vCenterPos = _float2(0.f);
		_float g_fNormalRadius = 0.1f;
	};
	RadialBlurData g_RadialBlurData{};

	struct VignetteData
	{
		_bool g_bVignetteOn = false;
		_float g_fVignettePower = 2.f;
	};
	VignetteData g_VignetteData{};

	_float g_DepthRange = 15.f;
	_float g_ClosestDepth = 1.f;

	_float g_rotX = 0.f;
	_float g_rotY = 0.f;
	_float g_rotZ = 0.f;
};

