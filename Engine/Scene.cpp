#include "pch.h"
#include "Scene.h"

#include "Utils.h"
#include "Model.h"
#include "Light.h"
#include "Camera.h"
#include "BaseUI.h"
#include "Material.h"
#include "RigidBody.h"
#include "MathUtils.h"
#include "FileUtils.h"
#include "FontRenderer.h"
#include "MeshCollider.h"
#include "MeshRenderer.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "OBBBoxCollider.h"
#include "SphereCollider.h"
#include "AABBBoxCollider.h"
#include "PointLightScript.h"

#include <filesystem>
namespace fs = std::filesystem;

Scene::Scene()
{

}

Scene::~Scene()
{
}

void Scene::Init()
{
	SSAO_MakeOffsetVector();

	SSAO_MakeFrustumFarCorners();

	auto& objects = m_GameObjects;
	for (auto& object : objects)
	{
		object->Init();
	}
}

void Scene::Tick()
{
	auto& objects = m_GameObjects;
	for (auto& object : objects)
	{
		if(true == object->Is_Tick())
			object->Tick();
	}
	PickUI();
}

void Scene::Late_Tick()
{
	auto& objects = m_GameObjects;
	for (auto& object : objects)
	{
		if (true == object->Is_Tick())
			object->Late_Tick();
	}
}

void Scene::Final_Tick()
{
	auto& objects = m_GameObjects;
	for (auto& object : objects)
	{
		if (true == object->Is_Tick())
			object->Final_Tick();
	}
}

void Scene::Render()
{

	Gather_LightData();

	Sort_GameObjects();


	Render_Shadow();

	Render_MotionBlur();

	Render_Deferred();

	if (g_SSAOData.g_bSSAO_On)
	{
		Render_SSAO();
		Render_SSAOBlur(2);
	}


	Render_Lights();

	Render_AfterUI();
	//Render_BlurEffect();
	Render_LightFinal();
	Render_MotionBlurFinal();

	Render_SkyBox();



	Render_DOFMap();
	Render_DOFMapScaling(2);

	Render_DOFFinal();



	Render_Fog();
	Render_Forward();

	Render_BloomMap();
	Render_BloomMapScaling();

	Render_BloomFinal();



	Render_OutLine();
	Render_Distortion();
	Render_Distortion_Final();
	Render_LensFlare();

	Render_Aberration();
	Render_RadialBlur();

	Render_Vignette();
	Render_Debug();
	//Render_ToneMapping();

	Render_UI();

}

HRESULT Scene::Load_Scene()
{
	return S_OK;
}

void Scene::Add_GameObject(shared_ptr<GameObject> object, _bool staticFlag)
{
	m_GameObjects.push_back(object);

	if (object->Get_Camera())
		m_Cameras.push_back(object); 

	if (object->Get_Light())
		m_Lights.push_back(object);

	if (object->Get_LayerIndex() == Layer_UI)
	{
		m_UI.push_back(object);

		if (nullptr != object->Get_Button())
			m_ButtonUI.push_back(object);
	}

	if (staticFlag)
		m_StaticObject.push_back(object);

	if (object->Get_Name() == L"Player")
		m_pPlayer = object;
}

void Scene::Add_GameObject_Front(shared_ptr<GameObject> object, _bool staticFlag)
{
	m_GameObjects.push_front(object);

	if (object->Get_Camera())
		m_Cameras.push_back(object);

	if (object->Get_Light())
		m_Lights.push_back(object);

	if (object->Get_LayerIndex() == Layer_UI)
	{
		m_UI.push_back(object);

		if (nullptr != object->Get_Button())
			m_ButtonUI.push_back(object);
	}

	if (staticFlag)
		m_StaticObject.push_front(object);

	if (object->Get_Name() == L"Player")
		m_pPlayer = object;
}

void Scene::Remove_GameObject(shared_ptr<GameObject> object)
{
	{
		auto findit = find(m_GameObjects.begin(), m_GameObjects.end(), object);
		if (findit != m_GameObjects.end())
			m_GameObjects.erase(findit);
	}
	{
		auto findit = find(m_Cameras.begin(), m_Cameras.end(), object);
		if (findit != m_Cameras.end())
			m_Cameras.erase(findit);
	}
	{
		auto findit = find(m_Lights.begin(), m_Lights.end(), object);
		if (findit != m_Lights.end())
			m_Lights.erase(findit);
	}
	{
		auto findit = find(m_UI.begin(), m_UI.end(), object);
		if (findit != m_UI.end())
			m_UI.erase(findit);
	}
	{
		auto findit = find(m_StaticObject.begin(), m_StaticObject.end(), object);
		if (findit != m_StaticObject.end())
			m_StaticObject.erase(findit);
	}
	{
		auto findit = find(m_ButtonUI.begin(), m_ButtonUI.end(), object);
		if (findit != m_ButtonUI.end())
			m_ButtonUI.erase(findit);
	}
}

shared_ptr<GameObject> Scene::Get_Camera(const wstring& cameraName)
{
	for (auto& camera : m_Cameras)
	{
		if (camera->Get_Camera()->Get_CameraName() == cameraName)
			return camera;
	}
	return nullptr;
}

shared_ptr<GameObject> Scene::Get_MainCamera()
{
	for (auto& camera : m_Cameras)
	{
		if (camera->Get_Camera()->Get_ProjType() == ProjectionType::Perspective)
			return camera;
	}
	return nullptr;
}

shared_ptr<GameObject> Scene::Get_UICamera()
{
	for (auto& camera : m_Cameras)
	{
		if (camera->Get_Camera()->Get_ProjType() == ProjectionType::Orthographic)
			return camera;
	}
	return nullptr;
}

shared_ptr<GameObject> Scene::Get_GameObject(const wstring& name)
{
	for (auto& gameObject : m_GameObjects)
	{
		if (gameObject->Get_Name() == name)
			return gameObject;
	}
	return nullptr;
}

_bool Scene::Is_Static(shared_ptr<GameObject> obj)
{
	for (auto& staticObj : m_StaticObject)
	{
		if (staticObj == obj)
			return true;
	}
	return false;
}

shared_ptr<GameObject> Scene::Get_UI(const wstring& strName)
{
	for (auto& gameObject : m_UI)
	{
		if (gameObject->Get_Name() == strName)
			return gameObject;
	}
	return nullptr;
}

void Scene::Swap_Object(const wstring& leftObjName, const wstring& rightObjName)
{
	auto leftIter = find_if(m_GameObjects.begin(), m_GameObjects.end(),
		[&](shared_ptr<GameObject>& obj)
		{
			if (obj->Get_Name() == leftObjName)
				return true;

			return false;
		});
	auto rightIter = find_if(m_GameObjects.begin(), m_GameObjects.end(),
		[&](shared_ptr<GameObject>& obj)
		{
			if (obj->Get_Name() == rightObjName)
				return true;

			return false;
		});

	if (leftIter != m_GameObjects.end() && rightIter != m_GameObjects.end())
	{
		auto temp = *leftIter;
		*leftIter = *rightIter;
		*rightIter = temp;

	}
}

void Scene::Load_UIFile(const wstring& strDataFilePath, const list<shared_ptr<GameObject>>& staticObjects, _bool bRender, _bool bTick)
{
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(strDataFilePath, FileMode::Read);

	_uint iSize = file->Read<_uint>();
	for (_uint i = 0; i < iSize; ++i)
	{
		auto UiObject = make_shared<GameObject>();

		wstring strObjectName = Utils::ToWString(file->Read<string>());
		UiObject->Set_Name(strObjectName);

		shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Mesh.fx"));
		auto mesh = RESOURCES.Get<Mesh>(L"Point");
		renderer->Set_Mesh(mesh);

		auto material = make_shared<Material>();
		for (_uint i = 0; i < MAX_TEXTURE_MAP_COUONT; ++i)
		{
			_bool bIsUseTexture = file->Read<_bool>();

			if (true == bIsUseTexture)
				material->Set_TextureMap(RESOURCES.Get<Texture>(Utils::ToWString(file->Read<string>())), static_cast<TextureMapType>(i));
		}

		for (_uint i = 0; i < MAX_SUB_SRV_COUNT; ++i)
		{
			_bool bIsUseSubmap = file->Read<_bool>();

			if (true == bIsUseSubmap)
				material->Set_SubMap(i, RESOURCES.Get<Texture>(Utils::ToWString(file->Read<string>())));
		}
		renderer->Set_Material(material);

		_float4x4 matWorld = file->Read<_float4x4>();
		UiObject->GetOrAddTransform()->Set_WorldMat(matWorld);

		_uint iPass = file->Read<_uint>();
		renderer->Set_Pass(iPass);

		RenderParams tagParam = file->Read<RenderParams>();
		renderer->Get_RenderParamDesc() = tagParam;
		UiObject->Add_Component(renderer);

		_bool bIsUseBaseUi = file->Read<_bool>();
		if (true == bIsUseBaseUi)
		{
			auto BaseUi = make_shared<BaseUI>();
			BaseUI::BASEUIDESC tagBaseDesc = file->Read<BaseUI::BASEUIDESC>();
			BaseUi->Get_Desc() = tagBaseDesc;
			UiObject->Add_Component(BaseUi);
		}

		_bool bIsUseFont = file->Read<_bool>();
		if (true == bIsUseFont)
		{
			wstring wstrText = Utils::ToWstringUtf8(file->Read<string>());
			auto pFontRenderer = make_shared<FontRenderer>(wstrText);

			wstring strFont = Utils::ToWString(file->Read<string>());
			Color vecColor = file->Read<Color>();
			_float fSize = file->Read<_float>();
			pFontRenderer->Set_Font(RESOURCES.Get<CustomFont>(strFont), vecColor, fSize);

			UiObject->Add_Component(pFontRenderer);
		}

		UiObject->Set_LayerIndex(Layer_UI);
		UiObject->Set_Instancing(false);
		UiObject->Set_Tick(bTick);
		UiObject->Set_Render(bRender);

		_bool bIsStatic = file->Read<_bool>();

		// Temp
		if(bIsStatic == false)
			Add_GameObject(UiObject, bIsStatic);
		else
		{
			_bool alreadyHas = false;
			for (auto& obj : staticObjects)
			{
				if (obj->Get_Name() == UiObject->Get_Name())
				{
					alreadyHas = true;
					break;
				}
			}

			if(!alreadyHas)
				Add_GameObject(UiObject, bIsStatic);
		}
	}
}

void Scene::Load_UIFile(const wstring& strDataFilePath, vector<weak_ptr<GameObject>>& addedObjects)
{
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(strDataFilePath, FileMode::Read);

	_uint iSize = file->Read<_uint>();
	addedObjects.resize(iSize);
	for (_uint i = 0; i < iSize; ++i)
	{
		auto UiObject = make_shared<GameObject>();

		wstring strObjectName = Utils::ToWString(file->Read<string>());
		UiObject->Set_Name(strObjectName);

		shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Mesh.fx"));
		auto mesh = RESOURCES.Get<Mesh>(L"Point");
		renderer->Set_Mesh(mesh);

		auto material = make_shared<Material>();
		for (_uint i = 0; i < MAX_TEXTURE_MAP_COUONT; ++i)
		{
			_bool bIsUseTexture = file->Read<_bool>();

			if (true == bIsUseTexture)
				material->Set_TextureMap(RESOURCES.Get<Texture>(Utils::ToWString(file->Read<string>())), static_cast<TextureMapType>(i));
		}

		for (_uint i = 0; i < MAX_SUB_SRV_COUNT; ++i)
		{
			_bool bIsUseSubmap = file->Read<_bool>();

			if (true == bIsUseSubmap)
				material->Set_SubMap(i, RESOURCES.Get<Texture>(Utils::ToWString(file->Read<string>())));
		}
		renderer->Set_Material(material);

		_float4x4 matWorld = file->Read<_float4x4>();
		UiObject->GetOrAddTransform()->Set_WorldMat(matWorld);

		_uint iPass = file->Read<_uint>();
		renderer->Set_Pass(iPass);

		RenderParams tagParam = file->Read<RenderParams>();
		renderer->Get_RenderParamDesc() = tagParam;
		UiObject->Add_Component(renderer);

		_bool bIsUseBaseUi = file->Read<_bool>();
		if (true == bIsUseBaseUi)
		{
			auto BaseUi = make_shared<BaseUI>();
			BaseUI::BASEUIDESC tagBaseDesc = file->Read<BaseUI::BASEUIDESC>();
			BaseUi->Get_Desc() = tagBaseDesc;
			UiObject->Add_Component(BaseUi);
		}

		_bool bIsUseFont = file->Read<_bool>();
		if (true == bIsUseFont)
		{
			wstring wstrText = Utils::ToWstringUtf8(file->Read<string>());
			auto pFontRenderer = make_shared<FontRenderer>(wstrText);

			wstring strFont = Utils::ToWString(file->Read<string>());
			Color vecColor = file->Read<Color>();
			_float fSize = file->Read<_float>();
			pFontRenderer->Set_Font(RESOURCES.Get<CustomFont>(strFont), vecColor, fSize);

			UiObject->Add_Component(pFontRenderer);
		}

		UiObject->Set_LayerIndex(Layer_UI);
		UiObject->Set_Instancing(false);
		UiObject->Set_Render(true);

		_bool bIsStatic = file->Read<_bool>();

		
		Add_GameObject(UiObject, bIsStatic);
		addedObjects[i] = UiObject;
	}
}

void Scene::Load_MapFile(const wstring& _mapFileName, shared_ptr<GameObject> pPlayer)
{
	// 세이브 파일 이름으로 로드하기
	wstring strFilePath = L"..\\Resources\\MapData\\";
	strFilePath += _mapFileName + L".dat";
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(strFilePath, FileMode::Read);

	// 스카이박스 생성 중복 체크 후 재생성
	shared_ptr<GameObject> pSkyBox = Get_GameObject(L"SkyBase");
	// 기존에 있다면 지움
	if (pSkyBox != nullptr)
	{
		Remove_GameObject(pSkyBox);
	}
	shared_ptr<GameObject> sky = make_shared<GameObject>();
	sky->GetOrAddTransform();
	wstring strSkyBoxTextureName = Utils::ToWString(file->Read<string>());
	shared_ptr<ModelRenderer> SkyRenderer = make_shared<ModelRenderer>(RESOURCES.Get<Shader>(L"SkyBox.fx"));
	SkyRenderer->Set_Model(RESOURCES.Get<Model>(strSkyBoxTextureName));
	sky->Add_Component(SkyRenderer);
	sky->Set_Name(L"SkyBase");
	Add_GameObject(sky);

	// 광원 정보 가져와서 방향성광원 적용 및 점광원 생성하기
	// 방향성광원
	 // 포지션
	shared_ptr<GameObject> DirectionalLightObject = make_shared<GameObject>();
	DirectionalLightObject->GetOrAddTransform()->Set_State(Transform_State::POS, file->Read<_float4>());
	DirectionalLightObject->Get_Transform()->LookAt(_float4(file->Read<_float3>(), 1.f));

	DirectionalLightObject->Add_Component(make_shared<Light>());
	DirectionalLightObject->Get_Light()->Set_LightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
	// 색깔
	LightColor DirLightColor;
	file->Read<LightColor>(DirLightColor);
	DirectionalLightObject->Get_Light()->Set_Ambient(DirLightColor.ambient);
	DirectionalLightObject->Get_Light()->Set_Diffuse(DirLightColor.diffuse);
	DirectionalLightObject->Get_Light()->Set_Specular(DirLightColor.specular);
	DirectionalLightObject->Get_Light()->Set_Emissive(_float4(1.f));
	Add_GameObject(DirectionalLightObject);
	// 점광원정보 가져오고 불러오기
	_int iNumPointLight = file->Read<_int>();
	for (_int i = 0; i < iNumPointLight; ++i)
	{
		LightInfo loadPointLightInfo;
		file->Read<_float4>(loadPointLightInfo.vPosition);
		file->Read<LightColor>(loadPointLightInfo.color);
		file->Read<_float>(loadPointLightInfo.range);
		shared_ptr<GameObject> PointLight = make_shared<GameObject>();
		PointLight->Set_Name(L"PointLight");
		//트랜스폼
		PointLight->GetOrAddTransform()->Set_State(Transform_State::POS, loadPointLightInfo.vPosition);
		{
			// LightComponent 생성 후 세팅
			shared_ptr<Light> lightCom = make_shared<Light>();
			lightCom->Set_Ambient(loadPointLightInfo.color.ambient);
			lightCom->Set_Diffuse(loadPointLightInfo.color.diffuse);
			lightCom->Set_Specular(loadPointLightInfo.color.specular);
			lightCom->Set_Emissive(loadPointLightInfo.color.emissive);
			lightCom->Set_LightType(LIGHT_TYPE::POINT_LIGHT);
			lightCom->Set_LightRange(loadPointLightInfo.range);

			PointLight->Add_Component(lightCom);
		}
		Add_GameObject(PointLight);

		// 점광원효과 기본초기화
		shared_ptr<PointLightScript> pPLE = make_shared<PointLightScript>();
		PointLight->Add_Component(pPLE);
		pPLE->Init();

		// 점광원 효과정보
		_bool bUseEffect = false;
		file->Read<_bool>(bUseEffect);
		pPLE->Set_bUseEffect(bUseEffect);
		if (bUseEffect) // 점광원효과가 있는 녀석이라면
		{
			_float4 AS = { 1.f, 1.f, 1.f, 1.f };
			file->Read<_float4>(AS);
			pPLE->Set_AmbientStart(AS);
			_float4 AT = { 1.f, 1.f, 1.f, 1.f };
			file->Read<_float4>(AT);
			pPLE->Set_AmbientTarget(AT);
			_float4 DS = { 1.f, 1.f, 1.f, 1.f };
			file->Read<_float4>(DS);
			pPLE->Set_DiffuseStart(DS);
			_float4 DE = { 1.f, 1.f, 1.f, 1.f };
			file->Read<_float4>(DE);
			pPLE->Set_DiffuseTarget(DE);
			_float SR = { 0.f };
			file->Read<_float>(SR);
			pPLE->Set_StartRange(SR);
			_float TR = { 0.f };
			file->Read<_float>(TR);
			pPLE->Set_TargetRange(TR);
			_float SP = { 0.f };
			file->Read<_float>(SP);
			pPLE->Set_Speed(SP);
			// 랜덤으로 델타타임과 PM세팅
			pPLE->Set_DeltaTime(Utils::Random_In_Range(0.f, 1.f));
			pPLE->Set_DeltaPM(rand() % 2 > 0 ? 1.f : -1.f);
		}
	}

	// 벽정보 불러오기 및 벽생성
	vector<pair<_float3, _float3>> WallRectPosLDRU;
	WallRectPosLDRU.clear();
	_int iNumWall = 0;
	file->Read<_int>(iNumWall);
	if(iNumWall >0)
	{
		WallRectPosLDRU.resize(iNumWall);
		// 벽정보 읽어오기
		for (_int i = 0; i < iNumWall; ++i)
			file->Read<pair<_float3, _float3>>(WallRectPosLDRU[i]);
		// 벽정보를 기반으로 벽메시 생성
		shared_ptr<Mesh> WallMesh = make_shared<Mesh>();
		WallMesh->Create3DRect(WallRectPosLDRU);
		// 메시를 기반으로 벽오브젝트 생성
		shared_ptr<GameObject> WallObject = make_shared<GameObject>();
		WallObject->Set_Name(L"MapWall");
		WallObject->GetOrAddTransform();
		// 메시렌더러
		shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Mesh.fx"));
		renderer->Set_Mesh(WallMesh);
		// 메시를 통해 메시콜라이더 생성
		shared_ptr<MeshCollider> pCollider = make_shared<MeshCollider>(*WallMesh.get());
		WallObject->Add_Component(pCollider);
		pCollider->Set_Activate(true);
		// 리지드바디 생성
		auto rigidBody = make_shared<RigidBody>();
		rigidBody->Create_RigidBody(pCollider, WallObject->GetOrAddTransform()->Get_WorldMatrix());
		WallObject->Add_Component(rigidBody);
		Add_GameObject(WallObject);
	}

	// 바닥정보 불러오기 및 바닥생성
	vector<pair<_float3, _float3>> GroundRectPosLURD;
	GroundRectPosLURD.clear();
	_int iNumGround = 0;
	file->Read<_int>(iNumGround);
	if (iNumGround > 0)
	{
		GroundRectPosLURD.resize(iNumGround);
		// 벽정보 읽어오기
		for (_int i = 0; i < iNumGround; ++i)
			file->Read<pair<_float3, _float3>>(GroundRectPosLURD[i]);
		// 벽정보를 기반으로 벽메시 생성
		shared_ptr<Mesh> GroundMesh = make_shared<Mesh>();
		GroundMesh->CreateGround(GroundRectPosLURD);
		// 메시를 기반으로 벽오브젝트 생성
		shared_ptr<GameObject> GroundObject = make_shared<GameObject>();
		GroundObject->Set_Name(L"MapGround");
		GroundObject->GetOrAddTransform();
		// 메시렌더러
		shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Mesh.fx"));
		renderer->Set_Mesh(GroundMesh);
		// 메시를 통해 메시콜라이더 생성
		shared_ptr<MeshCollider> pCollider = make_shared<MeshCollider>(*GroundMesh.get());
		GroundObject->Add_Component(pCollider);
		pCollider->Set_Activate(true);
		// 리지드바디 생성
		auto rigidBody = make_shared<RigidBody>();
		rigidBody->Create_RigidBody(pCollider, GroundObject->GetOrAddTransform()->Get_WorldMatrix());
		GroundObject->Add_Component(rigidBody);
		Add_GameObject(GroundObject);
	}

	// 오브젝트 개수 불러오기
	_int iNumObjects = file->Read<_int>();

	for (_int i = 0; i < iNumObjects; ++i)
	{
		// 맵오브젝트에 필요한 모든정보
		string strName = "";
		_float fUVWeight = { 1.f }; // UV비율 : 기본1배
		_bool bShadow = false;
		_bool bBlur = false;
		// Component
		   // Transform
		_bool bTransform = true;
		_float4x4 WorldMatrix = XMMatrixIdentity();
		// Collider
		_bool bCollider = false;
		// 0:Sphere 1:AABB 2:OBB 3:Mesh
		_int iColliderType = static_cast<_int>(ColliderType::OBB);
		// ColliderDesc
		_float3 ColliderOffset = _float3{ 0.f, 0.f, 0.f };
		_float ColRadius = { 0.f };
		_float3 ColBoundingSize = _float3{ 0.f, 0.f, 0.f };
		string ColModelName = "";
		// Culling
		_float3 CullPos = _float3{ 0.f, 0.f, 0.f };
		_float CullRadius = { 0.f };
		// CullMode
		_char bCullNone = 0;
		// Dummy
		_float4x4 matDummyData = _float4x4::Identity;

		wstring strObjectName = Utils::ToWString(file->Read<string>());
		strName = Utils::ToString(strObjectName);

		file->Read<_float>(fUVWeight);
		file->Read<_bool>(bShadow);
		file->Read<_bool>(bBlur);
		file->Read<_bool>(bTransform);
		if (bTransform)
			file->Read<_float4x4>(WorldMatrix);
		file->Read<_bool>(bCollider);
		if (bCollider)
		{
			file->Read<_int>(iColliderType);
			file->Read<_float3>(ColliderOffset);
			switch (static_cast<ColliderType>(iColliderType))
			{
			case ColliderType::Sphere:
				file->Read<_float>(ColRadius);
				break;
			case ColliderType::AABB:
				file->Read<_float3>(ColBoundingSize);
				break;
			case ColliderType::OBB:
				file->Read<_float3>(ColBoundingSize);
				break;
			case ColliderType::Mesh:
				//file->Read<string>(ColModelName);
				break;
			default:
				break;
			}
		}
		file->Read<_float3>(CullPos);
		file->Read<_float>(CullRadius);
		file->Read<_char>(bCullNone);
		file->Read<_float4x4>(matDummyData);

// 오브젝트 생성
		shared_ptr<GameObject> CreateObject = make_shared<GameObject>();
		CreateObject->Set_Name(strObjectName);
		CreateObject->Set_DrawShadow(bShadow);
		CreateObject->Set_Blur(bBlur);
		CreateObject->Set_CullPos(CullPos);
		CreateObject->Set_CullRadius(CullRadius);
		// 이름을 사용하여 모델생성
		// 고유번호를 제거하여 모델명을 얻어옴
		_int iPureNameSize = 0;
		while (strName[iPureNameSize] != '-' && iPureNameSize < strName.size())
		{
			++iPureNameSize;
		}
		string strModelName = strName.substr(0, iPureNameSize);
		// 콜라이더에서 사용하는 모델명도 저장
		ColModelName = strModelName;
		// 모델생성
		shared_ptr<Model> model = RESOURCES.Get<Model>(Utils::ToWString(strModelName));
		shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
		// Anim_으로 시작하는 녀석들은 애니메이션모델, 아니면 논애님
		if (strModelName.find("Anim_") != std::string::npos)
		{
			shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
			CreateObject->Add_Component(animator);
			animator->Set_Model(model);
			// 시작애니메이션의 인덱스, 반복여부, 재생속도
			animator->Set_CurrentAnim(0, true, 1.f);
		}
		else
		{
			shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
			CreateObject->Add_Component(renderer);
			renderer->Set_Model(model);
			// 컬방향

				renderer->Set_PassType((ModelRenderer::INSTANCE_PASSTYPE)bCullNone);

			renderer->SetVec4(0, _float4(fUVWeight,0,0,0));
			CreateObject->Set_FrustumCulled(true);
		}

		// 트랜스폼 생성
		if (bTransform)
		{
			CreateObject->Add_Component(make_shared<Transform>());
			CreateObject->Get_Transform()->Set_WorldMat(WorldMatrix);
		}

		// 콜라이더 생성
		if (bCollider)
		{
			switch (static_cast<ColliderType>(iColliderType))
			{
			case ColliderType::Sphere:
			{
				shared_ptr<SphereCollider> pCollider = make_shared<SphereCollider>(ColRadius);
				pCollider->Set_Offset(ColliderOffset);
				CreateObject->Add_Component(pCollider);
				//pCollider->Set_CollisionGroup(MAPObject);
				pCollider->Set_Activate(true);
				break;
			}
			case ColliderType::AABB:
			{
				shared_ptr<AABBBoxCollider> pCollider = make_shared<AABBBoxCollider>(ColBoundingSize);
				pCollider->Set_Offset(ColliderOffset);
				CreateObject->Add_Component(pCollider);
				//pCollider->Set_CollisionGroup(MAPObject);
				pCollider->Set_Activate(true);
				break;
			}
			case ColliderType::OBB:
			{
				shared_ptr<OBBBoxCollider> pCollider = make_shared<OBBBoxCollider>(ColBoundingSize);
				pCollider->Set_Offset(ColliderOffset);
				CreateObject->Add_Component(pCollider);
				//pCollider->Set_CollisionGroup(MAPObject);
				pCollider->Set_Activate(true);
				_float3 vObjPos = CreateObject->Get_Transform()->Get_State(Transform_State::POS).xyz();
				auto rigidBody = make_shared<RigidBody>();
				CreateObject->Add_Component(rigidBody);
				rigidBody->Create_CapsuleRigidBody(CreateObject->Get_CullPos(), ColBoundingSize.x, CreateObject->Get_CullRadius() * 5.f);
				break;
			}
			case ColliderType::Mesh:
			{
				shared_ptr<MeshCollider> pCollider = make_shared<MeshCollider>(Utils::ToWString(ColModelName));
				pCollider->Set_Offset(ColliderOffset);
				CreateObject->Add_Component(pCollider);
				//pCollider->Set_CollisionGroup(MAPObject);
				pCollider->Set_Activate(true);
				auto rigidBody = make_shared<RigidBody>();
				rigidBody->Create_RigidBody(pCollider, CreateObject->GetOrAddTransform()->Get_WorldMatrix());
				CreateObject->Add_Component(rigidBody);
				break;
			}
			default:
				break;
			}
		}
		

		Add_GameObject(CreateObject);
	}

	// 플레이어 생성위치, 룩앳
	_float4 PlayerCreatePos = { 0.f, 0.f, 0.f, 1.f };
	_float4 PlayerLookAtPos = { 0.f, 0.f, 0.f, 1.f };
	file->Read<_float4>(PlayerCreatePos);
	// 위험방지
	PlayerCreatePos.w = 1.f;
	file->Read<_float4>(PlayerLookAtPos);
	// 위험방지
	PlayerLookAtPos.w = 1.f;

	shared_ptr<GameObject> PlayerPtr = pPlayer;
	// 플레이어가 있고 스폰지점과룩앳지점이 다를때만 변경
	if (PlayerPtr != nullptr && PlayerCreatePos != PlayerLookAtPos)
	{
		PlayerPtr->Get_Transform()->Set_State(Transform_State::POS, PlayerCreatePos);
		PlayerPtr->Get_Transform()->LookAt(PlayerLookAtPos);
	}

	// 셰이더옵션 로드
		//RenderOption
	file->Read<_float>(g_fBrightness);
	file->Read<_float>(g_fContrast);
	file->Read<_float>(g_Saturation);
	//Bloom
	file->Read<_bool>(g_BloomData.g_BloomOn);
	file->Read<_float>(g_BloomData.g_BloomMin);
	//TonMapping
	file->Read<_int>(g_iTMIndex);
	file->Read<_float>(g_fMaxWhite);
	//SSAO
	file->Read<_bool>(g_SSAOData.g_bSSAO_On);
	file->Read<_float>(g_SSAOData.g_fOcclusionRadius);
	file->Read<_float>(g_SSAOData.g_OcclusionFadeStart);
	file->Read<_float>(g_SSAOData.g_OcclusionFadeEnd);
	//MotionBlur
	file->Read<_bool>(g_MotionBlurData.g_bMotionBlurOn);
	file->Read<_int>(g_MotionBlurData.g_iBlurCount);
	//FogOption
	file->Read<_bool>(g_FogData.g_FogOn);
	file->Read<_float>(g_FogData.g_fogStart);
	file->Read<_float>(g_FogData.g_fogEnd);
	file->Read<_float>(g_FogData.g_fogDensity);
	file->Read<_int>(g_FogData.g_fogMode);
	file->Read<Color>(g_FogData.g_fogColor);
	//LensFlare
	file->Read<_bool>(g_bLensFlare);
	//DOF
	file->Read<_bool>(g_DOFData.g_bDOF_On);
	file->Read<_float>(g_DOFData.g_FocusDepth);
	file->Read<_float>(g_DOFData.g_DOFRange);
	//LightOption
	file->Read<_float>(g_LightPowerData.g_specularPower);
	file->Read<_float>(g_LightPowerData.g_rimPower);
	file->Read<Color>();
	//OtherOption
	file->Read<_bool>(g_bDrawOutline);
	file->Read<_bool>(g_bFXAAOn);
	file->Read<_bool>(g_bAberrationOn);
	file->Read<_bool>(g_bPBR_On);
	file->Read<_float>(g_lightAttenuation);
	file->Read<_float>(g_ambientRatio);
}

void Scene::PickUI()
{
	if (false == INPUT.Get_IsCanMove())
		return;

	if (!KEYPUSH(KEY_TYPE::LBUTTON))
		return;

	if (!Get_UICamera())
		return;

	POINT screenPt = INPUT.GetMousePosToPoint();
	_bool bIsTap = KEYTAP(KEY_TYPE::LBUTTON);

	const auto gameobjects = m_ButtonUI;
	for (auto& gameObject : gameobjects)
	{
		if (false == gameObject->Is_Render())
			continue;
		
		auto Button = gameObject->Get_Button();
		if (true == Button->Get_Type() && true == bIsTap)
		{
			if (Button->Picked(screenPt))
			{
				Button->InvokeOnClicked();

				break;
			}
		}

		else if (false == Button->Get_Type())
		{
			if (Button->Picked(screenPt))
			{
				Button->InvokeOnClicked();

				break;
			}
		}
		
	}

}

void Scene::Gather_LightData()
{
	ZeroMemory(&m_LightParams, sizeof(m_LightParams));

	auto& lights = m_Lights;
	for (auto& light : m_Lights)
	{
		const LightInfo& lightInfo = light->Get_Light()->Get_LightInfo();

		light->Get_Light()->Set_LightIndex(m_LightParams.lightCount);

		m_LightParams.lights[m_LightParams.lightCount] = lightInfo;
		m_LightParams.lightCount++;
	}
}

void Scene::Sort_GameObjects()
{
	for (auto& camera : m_Cameras)
	{
		camera->Get_Camera()->Sort_GameObject(shared_from_this());
	}
}

void Scene::Render_Shadow()
{
	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->OMSetRenderTargets();

	for (auto& light : m_Lights)
	{
		if(light->Get_Light()->Get_LightInfo().lightType != static_cast<_int>(LIGHT_TYPE::DIRECTIONAL_LIGHT))
			continue;

		light->Get_Light()->Render_Shadow(shared_from_this());

	}
	//GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::SHADOW)->UnBindSRV();
}

void Scene::Render_MotionBlur()
{
	if (!CUR_SCENE->g_MotionBlurData.g_bMotionBlurOn) return;
	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::MOTIONBLUR)->OMSetRenderTargets();

	if (Get_MainCamera())
	{
		shared_ptr<Camera> mainCamera = Get_MainCamera()->Get_Camera();
		mainCamera->Render_MotionBlur();
	}
}

void Scene::Render_Deferred()
{
	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->OMSetRenderTargets();

	if(Get_MainCamera())
	{
		shared_ptr<Camera> mainCamera = Get_MainCamera()->Get_Camera();
		mainCamera->Render_Deferred();
	}
	//GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::G_BUFFER)->UnBindSRV();
}



void Scene::Render_SSAO()
{
	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::SSAO)->OMSetRenderTargets();

	auto material = RESOURCES.Get<Material>(L"SSAO");
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");

	material->Push_SubMapData();
	static const _float4x4 T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	_float4x4 matViewToTexSpace =  Camera::Get_Proj() * T;
	material->Get_Shader()->GetMatrix("gViewToTexSpace")->SetMatrix((_float*)&matViewToTexSpace);
	material->Get_Shader()->GetVector("gOffsetVectors")->SetFloatVectorArray((_float*)m_vOffsets, 0, 14);
	material->Get_Shader()->GetVector("gFrustumCorners")->SetFloatVectorArray((_float*)m_vFrustumFarCorner, 0, 4);
	material->Get_Shader()->GetScalar("gOcclusionRadius")->SetFloat(g_SSAOData.g_fOcclusionRadius);
	material->Get_Shader()->GetScalar("gOcclusionFadeStart")->SetFloat(g_SSAOData.g_OcclusionFadeStart);
	material->Get_Shader()->GetScalar("gOcclusionFadeEnd")->SetFloat(g_SSAOData.g_OcclusionFadeEnd);


	mesh->Get_VertexBuffer()->Push_Data();
	mesh->Get_IndexBuffer()->Push_Data();

	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	material->Get_Shader()->DrawIndexed(0, 0, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

	m_wstrFinalRenderTarget = L"SSAOTarget";
}

void Scene::Render_SSAOBlur(_uint downSamplingCount)
{

	for (_uint i = 0; i < downSamplingCount; ++i)
	{
		RENDER_TARGET_GROUP_TYPE eType = static_cast<RENDER_TARGET_GROUP_TYPE>(static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOWNSAMPLER0) + i);
		GRAPHICS.Get_RTGroup(eType)->OMSetRenderTargets();

		auto material = RESOURCES.Get<Material>(L"Sampler");
		auto mesh = RESOURCES.Get<Mesh>(L"Quad");
		material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));

		material->Push_SubMapData();

		mesh->Get_VertexBuffer()->Push_Data();

		mesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		material->Get_Shader()->DrawIndexed(0, 2, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

		m_wstrFinalRenderTarget = L"DownSample" + to_wstring(i);
	}

	for (_int i = downSamplingCount - 1; i >=0 ; --i)
	{
		RENDER_TARGET_GROUP_TYPE eType = static_cast<RENDER_TARGET_GROUP_TYPE>(static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::UPSAMPLER0) + i);
		GRAPHICS.Get_RTGroup(eType)->OMSetRenderTargets();
		auto material = RESOURCES.Get<Material>(L"Sampler");
		auto mesh = RESOURCES.Get<Mesh>(L"Quad");
		material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));

		material->Push_SubMapData();

		mesh->Get_VertexBuffer()->Push_Data();
		mesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		material->Get_Shader()->DrawIndexed(0, 1, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

		m_wstrFinalRenderTarget = L"UpSample" + to_wstring(i);
	}
	RESOURCES.Get<Material>(L"LightMaterial")->Set_SubMap(3, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));
	
}

void Scene::Render_Lights()
{
	if (Get_MainCamera())
	{
		shared_ptr<Camera> mainCamera = Get_MainCamera()->Get_Camera();
		Camera::S_View = mainCamera->Get_ViewMat();
		Camera::S_Proj = mainCamera->Get_ProjMat();
	}

	m_LightParams.specularPower = g_LightPowerData.g_specularPower;
	m_LightParams.rimPower = g_LightPowerData.g_rimPower;

	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::LIGHTING)->OMSetRenderTargets();

	for (auto& light : m_Lights)
	{
		if (light->Get_Light()->Get_LightInfo().lightType == static_cast<_int>(LIGHT_TYPE::POINT_LIGHT))
		{
			auto& frustum = Get_MainCamera()->Get_Camera()->Get_Frustum();
			if (frustum.Contain_Sphere(light->Get_Light()->Get_LightInfo().vPosition.xyz(), light->Get_Light()->Get_LightInfo().range) == false)
				continue;
		}
		light->Get_Light()->Render();
	}
}

void Scene::Render_LightFinal()
{
	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::FINAL)->OMSetRenderTargets();

	auto material = RESOURCES.Get<Material>(L"LightFinal");
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");

	material->Get_Shader()->GetScalar("g_SSAO_On")->SetBool(g_SSAOData.g_bSSAO_On);

	material->Push_SubMapData();

	mesh->Get_VertexBuffer()->Push_Data();
	mesh->Get_IndexBuffer()->Push_Data();

	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	int techniqueIndex = g_bPBR_On ? 1 : 0;
	material->Get_Shader()->DrawIndexed(techniqueIndex,3, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

	m_wstrFinalRenderTarget = L"FinalTarget";
}

void Scene::Render_OutLine()
{
	if (!g_bDrawOutline)
		return;

	{
		GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::OUTLINE)->OMSetRenderTargets();

		auto material = RESOURCES.Get<Material>(L"Outline");
		auto mesh = RESOURCES.Get<Mesh>(L"Quad");

		material->Push_SubMapData();

		mesh->Get_VertexBuffer()->Push_Data();
		mesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		material->Get_Shader()->DrawIndexed(0, 0, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	}
	{
		GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::OUTLINEFINAL)->OMSetRenderTargets();

		auto material = RESOURCES.Get<Material>(L"OutlineFinal");
		auto mesh = RESOURCES.Get<Mesh>(L"Quad");

		material->Push_SubMapData();
		material->Set_SubMap(1, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));
		mesh->Get_VertexBuffer()->Push_Data();
		mesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		material->Get_Shader()->DrawIndexed(0, 1, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

		m_wstrFinalRenderTarget = L"OutLineFinalTarget";
	}
}

void Scene::Render_MotionBlurFinal()
{
	if (!g_MotionBlurData.g_bMotionBlurOn) return;
	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::MOTIONBLURFINAL)->OMSetRenderTargets();



	auto material = RESOURCES.Get<Material>(L"MotionBlurFinal");
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");
	material->Set_SubMap(1, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));
	material->Get_Shader()->GetScalar("g_BlurCount")->SetInt(g_MotionBlurData.g_iBlurCount);
	material->Get_Shader()->GetScalar("g_MotionBlurPower")->SetFloat(g_MotionBlurData.g_MotionBlurPower);

	material->Push_SubMapData();

	auto camera = Get_MainCamera()->Get_Camera();
	material->Get_Shader()->Push_GlobalData(camera->Get_ViewMat(), camera->Get_ProjMat());

	mesh->Get_VertexBuffer()->Push_Data();
	mesh->Get_IndexBuffer()->Push_Data();

	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	material->Get_Shader()->DrawIndexed(1, 0, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	m_wstrFinalRenderTarget = L"MotionBlurFinalTarget";
}

void Scene::Render_SkyBox()
{
	if (Get_MainCamera())
	{
		shared_ptr<Camera> mainCamera = Get_MainCamera()->Get_Camera();
		Camera::S_View = mainCamera->Get_ViewMat();
		Camera::S_Proj = mainCamera->Get_ProjMat();


		mainCamera->Render_SkyBox();
	}
}

void Scene::Render_Forward()
{
	if (Get_MainCamera())
	{
		shared_ptr<Camera> mainCamera = Get_MainCamera()->Get_Camera();
		Camera::S_View = mainCamera->Get_ViewMat();
		Camera::S_Proj = mainCamera->Get_ProjMat();


		mainCamera->Render_Forward();
	}
	//GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->UnBindSRV();
}

void Scene::Render_BloomMap()
{
	if (!g_BloomData.g_BloomOn)
		return;
	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::BLOOMMAP)->OMSetRenderTargets();

	auto material = RESOURCES.Get<Material>(L"BloomTarget");
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");
	material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));
	material->Push_SubMapData();
	material->Get_Shader()->GetScalar("g_BloomMin")->SetFloat(g_BloomData.g_BloomMin);
	mesh->Get_VertexBuffer()->Push_Data();
	mesh->Get_IndexBuffer()->Push_Data();

	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	material->Get_Shader()->DrawIndexed(0, 0, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
}

void Scene::Render_BloomMapScaling()
{
	if (!g_BloomData.g_BloomOn)
		return;

	for (_uchar i = 0; i < 3; ++i)
	{
		RENDER_TARGET_GROUP_TYPE eType = static_cast<RENDER_TARGET_GROUP_TYPE>(static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMDOWNSCALE0) + i);
		GRAPHICS.Get_RTGroup(eType)->OMSetRenderTargets();
		auto material = RESOURCES.Get<Material>(L"BloomDownScale" + to_wstring(i));
		auto mesh = RESOURCES.Get<Mesh>(L"Quad");
		//material->Get_Shader()->GetScalar("GaussianWeight")->SetFloatArray(a, 0, 25);
		//material->Get_Shader()->GetScalar("DownScalePower")->SetFloat(m_fDownScalePower);
		material->Push_SubMapData();

		mesh->Get_VertexBuffer()->Push_Data();
		mesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		material->Get_Shader()->DrawIndexed(0, 2, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	}

	for (_uchar i = 0; i < 3; ++i)
	{
		RENDER_TARGET_GROUP_TYPE eType = static_cast<RENDER_TARGET_GROUP_TYPE>(static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::BLOOMUPSCALE0) + i);
		GRAPHICS.Get_RTGroup(eType)->OMSetRenderTargets();
		auto material = RESOURCES.Get<Material>(L"BloomUpScale" + to_wstring(i));
		auto mesh = RESOURCES.Get<Mesh>(L"Quad");
		//	material->Get_Shader()->GetScalar("UpScalePower")->SetFloat(m_fUpScalePower);

		material->Push_SubMapData();

		mesh->Get_VertexBuffer()->Push_Data();
		mesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
		if(i != 2)
			material->Get_Shader()->DrawIndexed(0, 2, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
		else
			material->Get_Shader()->DrawIndexed(0, 1, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

	}

}

void Scene::Render_BloomFinal()
{
	if (!g_BloomData.g_BloomOn)
		return;

	{
		GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::BLOOMFINAL)->OMSetRenderTargets();

		auto material = RESOURCES.Get<Material>(L"BloomFinal");
		auto mesh = RESOURCES.Get<Mesh>(L"Quad");
		//	material->Get_Shader()->GetScalar("UpScalePower")->SetFloat(m_fUpScalePower);
		material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));
		material->Push_SubMapData();

		mesh->Get_VertexBuffer()->Push_Data();
		mesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		material->Get_Shader()->DrawIndexed(0, 1, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

		m_wstrFinalRenderTarget = L"BloomFinalTarget";
	}
}

void Scene::Render_DOFMap()
{
	if (!g_DOFData.g_bDOF_On)
		return;

	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::DOFMAP)->OMSetRenderTargets();

	auto material = RESOURCES.Get<Material>(L"DOFTarget");
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");
	material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));
	material->Push_SubMapData();
	material->Get_Shader()->GetScalar("g_FocusDepth")->SetFloat(g_DOFData.g_FocusDepth);
	material->Get_Shader()->GetScalar("g_DOFRange")->SetFloat(g_DOFData.g_DOFRange);
	mesh->Get_VertexBuffer()->Push_Data();
	mesh->Get_IndexBuffer()->Push_Data();

	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	material->Get_Shader()->DrawIndexed(1, 2, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
}

void Scene::Render_DOFMapScaling(_uint blurCount)
{
	if (!g_DOFData.g_bDOF_On)
		return;

	wstring rtTexture = m_wstrFinalRenderTarget;

	for (_uint i = 0; i < blurCount; ++i)
	{
		RENDER_TARGET_GROUP_TYPE eType = static_cast<RENDER_TARGET_GROUP_TYPE>(static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOWNSAMPLER0) + i);
		GRAPHICS.Get_RTGroup(eType)->OMSetRenderTargets();

		auto material = RESOURCES.Get<Material>(L"Sampler");
		auto mesh = RESOURCES.Get<Mesh>(L"Quad");
		material->Set_SubMap(0, RESOURCES.Get<Texture>(rtTexture));

		material->Push_SubMapData();

		mesh->Get_VertexBuffer()->Push_Data();

		mesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		material->Get_Shader()->DrawIndexed(0, 2, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

		rtTexture = L"DownSample" + to_wstring(i);
	}

	for (_int i = blurCount - 1; i >= 0; --i)
	{
		RENDER_TARGET_GROUP_TYPE eType = static_cast<RENDER_TARGET_GROUP_TYPE>(static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::UPSAMPLER0) + i);
		GRAPHICS.Get_RTGroup(eType)->OMSetRenderTargets();
		auto material = RESOURCES.Get<Material>(L"Sampler");
		auto mesh = RESOURCES.Get<Mesh>(L"Quad");
		material->Set_SubMap(0, RESOURCES.Get<Texture>(rtTexture));

		material->Push_SubMapData();

		mesh->Get_VertexBuffer()->Push_Data();
		mesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		material->Get_Shader()->DrawIndexed(0, 1, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

		rtTexture = L"UpSample" + to_wstring(i);
	}
	RESOURCES.Get<Material>(L"DOFFinal")->Set_SubMap(1, RESOURCES.Get<Texture>(rtTexture));

	/*for (_uchar i = 0; i < 3; ++i)
	{
		RENDER_TARGET_GROUP_TYPE eType = static_cast<RENDER_TARGET_GROUP_TYPE>(static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFDOWNSCALE0) + i);
		GRAPHICS.Get_RTGroup(eType)->OMSetRenderTargets();
		auto material = RESOURCES.Get<Material>(L"DOFDownScale" + to_wstring(i));
		auto mesh = RESOURCES.Get<Mesh>(L"Quad");

		material->Push_SubMapData();

		mesh->Get_VertexBuffer()->Push_Data();
		mesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		material->Get_Shader()->DrawIndexed(0, 2, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	}

	for (_uchar i = 0; i < 3; ++i)
	{
		RENDER_TARGET_GROUP_TYPE eType = static_cast<RENDER_TARGET_GROUP_TYPE>(static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::DOFUPSCALE0) + i);
		GRAPHICS.Get_RTGroup(eType)->OMSetRenderTargets();
		auto material = RESOURCES.Get<Material>(L"DOFUpScale" + to_wstring(i));
		auto mesh = RESOURCES.Get<Mesh>(L"Quad");

		material->Push_SubMapData();

		mesh->Get_VertexBuffer()->Push_Data();
		mesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (i != 2)
			material->Get_Shader()->DrawIndexed(0, 2, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
		else
			material->Get_Shader()->DrawIndexed(0, 1, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	}*/
}

void Scene::Render_DOFFinal()
{
	if (!g_DOFData.g_bDOF_On)
		return;

	{
		GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::DOFFINAL)->OMSetRenderTargets();

		auto material = RESOURCES.Get<Material>(L"DOFFinal");
		auto mesh = RESOURCES.Get<Mesh>(L"Quad");
		//	material->Get_Shader()->GetScalar("UpScalePower")->SetFloat(m_fUpScalePower);
		material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));
		material->Push_SubMapData();
		material->Get_Shader()->GetScalar("g_FocusDepth")->SetFloat(g_DOFData.g_FocusDepth);
		material->Get_Shader()->GetScalar("g_DOFRange")->SetFloat(g_DOFData.g_DOFRange);


		mesh->Get_VertexBuffer()->Push_Data();
		mesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		material->Get_Shader()->DrawIndexed(1, 1, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

		m_wstrFinalRenderTarget = L"DOFFinalTarget";
	}
}

void Scene::Render_Distortion()
{
	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::DISTORTION)->OMSetRenderTargets();
	if (Get_MainCamera())
	{
		shared_ptr<Camera> mainCamera = Get_MainCamera()->Get_Camera();
		Camera::S_View = mainCamera->Get_ViewMat();
		Camera::S_Proj = mainCamera->Get_ProjMat();


		mainCamera->Render_DistrotionEffects();
	}
}

void Scene::Render_Distortion_Final()
{
	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::DISTORTIONFINAL)->OMSetRenderTargets();
	
	auto material = RESOURCES.Get<Material>(L"Distorted_Final");
	material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");
	
	material->Push_SubMapData();
	mesh->Get_VertexBuffer()->Push_Data();
	mesh->Get_IndexBuffer()->Push_Data();
	
	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	material->Get_Shader()->DrawIndexed(0, 0, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	m_wstrFinalRenderTarget = L"DistortionFinalTarget";
}

void Scene::Render_LensFlare()
{
	if (!g_bLensFlare)
		return;

	if (!CUR_SCENE->Get_MainCamera())
		return;

	if (!CUR_SCENE->Get_Light())
		return;

	_float3 viewLightPos = _float3::Transform(m_LightParams.lights[0].vPosition.xyz(), Camera::Get_View());
	_float3 projLightPos = _float3::Transform(viewLightPos, Camera::Get_Proj());
	//[-1,1] -> [-.5, .5] //[1,-1] -> [-.5, .5]
	_float2 proj = { projLightPos.x,projLightPos.y };
	if (!(projLightPos.z <=1.f))
		return;
	proj.x = (proj.x) * 1.8f;
	proj.y = (proj.y) * -1.f;
	_float2 lightUV = { (projLightPos.x + 1.f) * 0.5f,(1.f - projLightPos.y) * 0.5f };

	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::LENSFLARE)->OMSetRenderTargets();

	auto material = RESOURCES.Get<Material>(L"LensFlare");
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");
	material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));

	auto camera = Get_MainCamera()->Get_Camera();
	material->Get_Shader()->Push_GlobalData(camera->Get_ViewMat(), camera->Get_ProjMat());


	material->Get_Shader()->GetVector("g_LightPos")->SetFloatVector((_float*)&proj);
	material->Get_Shader()->GetVector("g_LightUV")->SetFloatVector((_float*)&lightUV);
	material->Push_SubMapData();
	mesh->Get_VertexBuffer()->Push_Data();
	mesh->Get_IndexBuffer()->Push_Data();



	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	material->Get_Shader()->DrawIndexed(0, 0, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

	//for (_uchar i = 0; i < 3; ++i)
	//{
	//	RENDER_TARGET_GROUP_TYPE eType = static_cast<RENDER_TARGET_GROUP_TYPE>(static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::LENSFLAREDOWNSCALE0) + i);
	//	GRAPHICS.Get_RTGroup(eType)->OMSetRenderTargets();
	//	auto material = RESOURCES.Get<Material>(L"LensFlareDownScale" + to_wstring(i));
	//	auto mesh = RESOURCES.Get<Mesh>(L"Quad");
	//	//material->Get_Shader()->GetScalar("GaussianWeight")->SetFloatArray(a, 0, 25);
	//	//material->Get_Shader()->GetScalar("DownScalePower")->SetFloat(m_fDownScalePower);
	//	material->Push_SubMapData();

	//	mesh->Get_VertexBuffer()->Push_Data();
	//	mesh->Get_IndexBuffer()->Push_Data();

	//	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	material->Get_Shader()->DrawIndexed(0, 2, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	//}

	//for (_uchar i = 0; i < 3; ++i)
	//{
	//	RENDER_TARGET_GROUP_TYPE eType = static_cast<RENDER_TARGET_GROUP_TYPE>(static_cast<_uchar>(RENDER_TARGET_GROUP_TYPE::LENSFLAREUPSCALE0) + i);
	//	GRAPHICS.Get_RTGroup(eType)->OMSetRenderTargets();
	//	auto material = RESOURCES.Get<Material>(L"LensFlareUpScale" + to_wstring(i));
	//	auto mesh = RESOURCES.Get<Mesh>(L"Quad");
	//	//	material->Get_Shader()->GetScalar("UpScalePower")->SetFloat(m_fUpScalePower);

	//	material->Push_SubMapData();

	//	mesh->Get_VertexBuffer()->Push_Data();
	//	mesh->Get_IndexBuffer()->Push_Data();

	//	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	if (i != 2)
	//		material->Get_Shader()->DrawIndexed(0, 2, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	//	else
	//		material->Get_Shader()->DrawIndexed(0, 1, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	//}
	{
		GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::LENSFLAREFINAL)->OMSetRenderTargets();

		auto material = RESOURCES.Get<Material>(L"LensFlareFinal");
		auto mesh = RESOURCES.Get<Mesh>(L"Quad");
		//	material->Get_Shader()->GetScalar("UpScalePower")->SetFloat(m_fUpScalePower);
		material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));
		material->Push_SubMapData();
		auto camera = Get_MainCamera()->Get_Camera();
		material->Get_Shader()->Push_GlobalData(camera->Get_ViewMat(), camera->Get_ProjMat());

		mesh->Get_VertexBuffer()->Push_Data();
		mesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		material->Get_Shader()->DrawIndexed(0, 1, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

		m_wstrFinalRenderTarget = L"LensFlareFinalTarget";
	}

}

void Scene::Render_Fog()
{
	if (!g_FogData.g_FogOn)
		return;



	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::FOG)->OMSetRenderTargets();

	auto material = RESOURCES.Get<Material>(L"FogFinal");
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");
	material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));

	material->Get_Shader()->GetScalar("fogStart")->SetFloat(g_FogData.g_fogStart);
	material->Get_Shader()->GetScalar("fogEnd")->SetFloat(g_FogData.g_fogEnd);
	material->Get_Shader()->GetScalar("fogDensity")->SetFloat(g_FogData.g_fogDensity);
	material->Get_Shader()->GetScalar("fogMode")->SetInt(g_FogData.g_fogMode);
	material->Get_Shader()->GetVector("fogColor")->SetFloatVector((_float*)&g_FogData.g_fogColor);

	material->Push_SubMapData();
	mesh->Get_VertexBuffer()->Push_Data();
	mesh->Get_IndexBuffer()->Push_Data();

	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	material->Get_Shader()->DrawIndexed(3, 0, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	m_wstrFinalRenderTarget = L"FogTarget";
}

void Scene::Render_Aberration()
{
	if (!g_bAberrationOn)
		return;



	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::ABERRATION)->OMSetRenderTargets();

	auto material = RESOURCES.Get<Material>(L"AberrationFinal");
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");
	material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));

	material->Get_Shader()->GetScalar("g_AberrationPower")->SetFloat(g_fAberrationPower);

	material->Push_SubMapData();
	mesh->Get_VertexBuffer()->Push_Data();
	mesh->Get_IndexBuffer()->Push_Data();

	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	material->Get_Shader()->DrawIndexed(1, 0, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	m_wstrFinalRenderTarget = L"AberrationTarget";
}

void Scene::Render_RadialBlur()
{
	if (!g_RadialBlurData.g_bRadialBlurOn)
		return;

	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::RADIALBLUR)->OMSetRenderTargets();

	auto material = RESOURCES.Get<Material>(L"Sampler");
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");
	material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));

	material->Push_SubMapData();
	mesh->Get_VertexBuffer()->Push_Data();
	mesh->Get_IndexBuffer()->Push_Data();

	material->Get_Shader()->GetScalar("g_fRadialBlurStrength")->SetFloat(g_RadialBlurData.g_fRadialBlurStrength);
	material->Get_Shader()->GetScalar("g_fNormalRadius")->SetFloat(g_RadialBlurData.g_fNormalRadius);
	material->Get_Shader()->GetScalar("g_iSamples")->SetInt(g_RadialBlurData.g_iSamples);
	material->Get_Shader()->GetVector("g_vCenterPos")->SetFloatVector((_float*)&g_RadialBlurData.g_vCenterPos);

	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	material->Get_Shader()->DrawIndexed(1, 1, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	m_wstrFinalRenderTarget = L"RadialBlurTarget";
}

void Scene::Render_Vignette()
{
	if (!g_VignetteData.g_bVignetteOn)
		return;

	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::VIGNETTE)->OMSetRenderTargets();

	auto material = RESOURCES.Get<Material>(L"AberrationFinal");
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");
	material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));

	material->Push_SubMapData();
	mesh->Get_VertexBuffer()->Push_Data();
	mesh->Get_IndexBuffer()->Push_Data();

	material->Get_Shader()->GetScalar("g_fVignettePower")->SetFloat(g_VignetteData.g_fVignettePower);

	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	material->Get_Shader()->DrawIndexed(1, 3, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	m_wstrFinalRenderTarget = L"VignetteTarget";
}

void Scene::Render_Debug()
{
	if (m_bRenderDebug)
	{
		for (auto& gameObject : m_GameObjects)
			if (gameObject->Get_Collider())
				gameObject->Get_Collider()->Render();
	}
}

void Scene::Render_UI()
{
	if (Get_UICamera())
	{
		shared_ptr<Camera> uiCamera = Get_UICamera()->Get_Camera();
		Camera::S_View = uiCamera->Get_ViewMat();
		Camera::S_Proj = uiCamera->Get_ProjMat();

		uiCamera->Render_Forward();
	}
}

void Scene::Render_AfterUI()
{
	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::AFTER_UI)->OMSetRenderTargets();
	if (Get_Camera(L"After_UI"))
	{
		shared_ptr<Camera> mainCamera = Get_Camera(L"After_UI")->Get_Camera();
		Camera::S_View = mainCamera->Get_ViewMat();
		Camera::S_Proj = mainCamera->Get_ProjMat();


		mainCamera->Render_AfterUI();
	}
	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::AFTER_UI_TONEMAPPING)->OMSetRenderTargets();

	{
		auto material = RESOURCES.Get<Material>(L"BackBufferRenderFinal");
		auto mesh = RESOURCES.Get<Mesh>(L"Quad");

		material->Set_SubMap(0, RESOURCES.Get<Texture>(L"AFTER_UITarget"));



		material->Get_Shader()->GetScalar("g_brightness")->SetFloat(g_fBrightness);
		material->Get_Shader()->GetScalar("g_contrast")->SetFloat(g_fContrast);
		material->Get_Shader()->GetScalar("g_saturation")->SetFloat(g_Saturation);
		material->Get_Shader()->GetScalar("g_max_white")->SetFloat(g_fMaxWhite);



		material->Push_SubMapData();
		mesh->Get_VertexBuffer()->Push_Data();
		mesh->Get_IndexBuffer()->Push_Data();

		CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (g_iTMIndex > 3) g_iTMIndex = 3;

		material->Get_Shader()->DrawIndexed(0, g_iTMIndex, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	}

}


void Scene::Render_ToneMapping()
{
	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::TONEMAPPING)->OMSetRenderTargets();

	auto material = RESOURCES.Get<Material>(L"BackBufferRenderFinal");
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");

	material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));



	material->Get_Shader()->GetScalar("g_brightness")->SetFloat(g_fBrightness);
	material->Get_Shader()->GetScalar("g_contrast")->SetFloat(g_fContrast);
	material->Get_Shader()->GetScalar("g_saturation")->SetFloat(g_Saturation);
	material->Get_Shader()->GetScalar("g_max_white")->SetFloat(g_fMaxWhite);



	material->Push_SubMapData();
	mesh->Get_VertexBuffer()->Push_Data();
	mesh->Get_IndexBuffer()->Push_Data();

	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if (g_iTMIndex > 3) g_iTMIndex = 3;

	material->Get_Shader()->DrawIndexed(0, g_iTMIndex, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

	m_wstrFinalRenderTarget = L"ToneMappingTarget";
	Render_FXAA();
}

void Scene::Render_FXAA()
{
	if (!g_bFXAAOn)
		return;
	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::FXAA)->OMSetRenderTargets();

	auto material = RESOURCES.Get<Material>(L"FXAARenderFinal");
	material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");

	material->Get_Shader()->GetScalar("Use")->SetBool(g_bFXAAOn);
	material->Push_SubMapData();
	mesh->Get_VertexBuffer()->Push_Data();
	mesh->Get_IndexBuffer()->Push_Data();

	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	material->Get_Shader()->DrawIndexed(0, 0, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);
	m_wstrFinalRenderTarget = L"FXAATarget";
}

void Scene::Render_BackBuffer()
{

	GRAPHICS.Get_RTGroup(RENDER_TARGET_GROUP_TYPE::SWAP_CHAIN)->OMSetRenderTargets();

	auto material = RESOURCES.Get<Material>(L"BackBufferRenderFinal");
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");
	material->Set_SubMap(0, RESOURCES.Get<Texture>(m_wstrFinalRenderTarget));

	material->Push_SubMapData();
	mesh->Get_VertexBuffer()->Push_Data();
	mesh->Get_IndexBuffer()->Push_Data();

	CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	material->Get_Shader()->DrawIndexed(2, 0, mesh->Get_IndexBuffer()->Get_IndicesNum(), 0, 0);

}

void Scene::SSAO_MakeOffsetVector()
{
	m_vOffsets[0] = _float4(+1.0f, +1.0f, +1.0f, 0.0f);
	m_vOffsets[1] = _float4(-1.0f, -1.0f, -1.0f, 0.0f);

	m_vOffsets[2] = _float4(-1.0f, +1.0f, +1.0f, 0.0f);
	m_vOffsets[3] = _float4(+1.0f, -1.0f, -1.0f, 0.0f);

	m_vOffsets[4] = _float4(+1.0f, +1.0f, -1.0f, 0.0f);
	m_vOffsets[5] = _float4(-1.0f, -1.0f, +1.0f, 0.0f);

	m_vOffsets[6] = _float4(-1.0f, +1.0f, -1.0f, 0.0f);
	m_vOffsets[7] = _float4(+1.0f, -1.0f, +1.0f, 0.0f);

	// 6 centers of cube faces
	m_vOffsets[8] = _float4(-1.0f, 0.0f, 0.0f, 0.0f);
	m_vOffsets[9] = _float4(+1.0f, 0.0f, 0.0f, 0.0f);

	m_vOffsets[10] = _float4(0.0f, -1.0f, 0.0f, 0.0f);
	m_vOffsets[11] = _float4(0.0f, +1.0f, 0.0f, 0.0f);

	m_vOffsets[12] = _float4(0.0f, 0.0f, -1.0f, 0.0f);
	m_vOffsets[13] = _float4(0.0f, 0.0f, +1.0f, 0.0f);

	for (_int i = 0; i < 14; ++i)
	{
		// Create random lengths in [0.25, 1.0].
		_float s = MathUtils::Get_RandomFloat(0.25f, 1.0f);

		XMVECTOR v = s * ::XMVector4Normalize(::XMLoadFloat4(&m_vOffsets[i]));

		::XMStoreFloat4(&m_vOffsets[i], v);
	}
}

void Scene::SSAO_MakeFrustumFarCorners()
{
	_float aspect = (_float)GRAPHICS.Get_ViewPort().Get_Width() / (_float)GRAPHICS.Get_ViewPort().Get_Height();

	_float farZ = 1000.f;

	_float halfHeight = farZ * tanf(0.5f * XM_PI / 3.f);
	_float halfWidth = aspect * halfHeight;

	if (Get_MainCamera())
	{
		farZ = Get_MainCamera()->Get_Camera()->Get_CameraDesc().fFar;
		halfHeight = farZ * tanf(0.5f * Get_MainCamera()->Get_Camera()->Get_CameraDesc().fFOV);
		halfWidth = aspect * halfHeight;
	}

	m_vFrustumFarCorner[0] = _float4(-halfWidth, +halfHeight, farZ, 0.0f);
	m_vFrustumFarCorner[1] = _float4(+halfWidth, +halfHeight, farZ, 0.0f);
	m_vFrustumFarCorner[2] = _float4(+halfWidth, -halfHeight, farZ, 0.0f);
	m_vFrustumFarCorner[3] = _float4(-halfWidth, -halfHeight, farZ, 0.0f);
}
