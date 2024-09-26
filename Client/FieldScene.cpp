#include "pch.h"
#include "FieldScene.h"


#include "WaterUVSliding.h"

#include "Utils.h"
#include "Model.h"
#include "Light.h"
#include "Loader.h"
#include "Camera.h"
#include "BaseUI.h"
#include "Geometry.h"
#include "RigidBody.h"
#include "FileUtils.h"
#include "ModelMesh.h"
#include "CustomFont.h"
#include "LoadingScene.h"
#include "BaseCollider.h"
#include "WeaponScript.h"
#include "Player_FSM.h"
#include "SpearAce_FSM.h"
#include "MeshRenderer.h"
#include "FontRenderer.h"
#include "MeshCollider.h"
#include "UiCharChange.h"
#include "ModelAnimator.h"
#include "ModelRenderer.h"
#include "OBBBoxCollider.h"
#include "SphereCollider.h"
#include "AABBBoxCollider.h"
#include "UiGachaCardMove.h"
#include "Debug_CreateMotionTrail.h"
#include "CounterMotionTrailScript.h"
#include "TerrainRenderer.h"
#include "UiHpBarController.h"
#include "MapObjectScript.h"
#include "Terrain.h"
#include "UiCoolEndEffect.h"
#include "MainCameraScript.h"
#include "HeroChangeScript.h"
#include "DemoCameraScript1.h"
#include "DemoCameraScript2.h"
#include "CoolTimeCheckScript.h"
#include "UiSkillButtonEffect.h"
#include "CharacterController.h"
#include "ObjectTransformDebug.h"
#include "Silversword_Soldier_FSM.h"
#include "Succubus_Scythe_FSM.h"
#include "Undead_Priest_FSM.h"
#include "Alpaca_FSM.h"
#include "Wolf_FSM.h"
#include "EntSoldier_FSM.h"

#include "Companion_Spike_FSM.h"
#include "Companion_Dellons_FSM.h"
#include "Companion_Shane_FSM.h"

#include "UiGachaController.h"
#include "Boss_Mir_FSM.h"
#include "Boss_Dellons_FSM.h"
#include "Boss_Spike_FSM.h"
#include "DemoAnimationController1.h"
#include "UiCardDeckController.h"
#include "MainUiController.h"
#include "UiTargetLockOn.h"
#include "UiMonsterHp.h"
#include "UiDamageCreate.h"
#include "UIBossHpBar.h"
#include "UiComboEffect.h"
#include "UiSkillGauge.h"


#include <filesystem>
#include "GachaScene.h"
#include "MirScene.h"
#include "UiSettingController.h"
#include "UiQuestController.h"
#include "UiBossDialog.h"
#include "UIInteraction.h"
#include "NeutralAlpaca_FSM.h"
#include "MathUtils.h"

namespace fs = std::filesystem;

FieldScene::FieldScene()
{
	m_strSceneName = L"FieldScene";
}

FieldScene::~FieldScene()
{
}

void FieldScene::Init()
{
	__super::Init();
	for (_int i = 0; i < _int(HERO::MAX); ++i)
	{
		auto& tagData = GET_DATA(HERO(i));

		shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

		{
			shared_ptr<GameObject> obj = make_shared<GameObject>();
			obj->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(0.f, -200.f, 0.f, 1.f));
			shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
			animator->Set_Model(RESOURCES.Get<Model>(tagData.ModelTag));
			obj->Add_Component(animator);
			obj->Set_Instancing(false);
			Add_GameObject(obj);
		}

	}


}

void FieldScene::Tick()
{
	__super::Tick();
}

void FieldScene::Late_Tick()
{

	__super::Late_Tick();

	// Test
	{
		if (KEYTAP(KEY_TYPE::X))
		{
			if (m_bRenderDebug)
				m_bRenderDebug = false;
			else
				m_bRenderDebug = true;
		}
	}
}

void FieldScene::Final_Tick()
{
	__super::Final_Tick();
	PHYSX.Tick();

	if (KEYPUSH(KEY_TYPE::TAB) && KEYPUSH(KEY_TYPE::F4))
	{

		/*GachaSceneDesc sceneDesc{ L"YeopoMap",HERO::YEOPO};
			SCENE.Add_SubScene(make_shared<GachaScene>(sceneDesc));
		SCENE.Exchange_Scene();*/


		shared_ptr<LoadingScene> scene = make_shared<LoadingScene>(make_shared<MirScene>());
		PHYSX.Set_CharacterControllerNull();

		scene->Set_StaticObjects(m_StaticObject);
		SCENE.Change_Scene(scene);
		g_bCutScene = false;
	}
}

HRESULT FieldScene::Load_Scene()
{
	RESOURCES.Delete_NonStaticResources();
	PHYSX.Init();

	//Static
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Hero\\", true);
	RESOURCES.CreateModel(L"..\\Resources\\Models\\VfxMesh\\", true);
	RESOURCES.CreateModel(L"..\\Resources\\Models\\MapObject\\SkyBox\\", true);
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Weapon\\", true);

	//Map
	RESOURCES.CreateModel(L"..\\Resources\\Models\\MapObject\\Field\\", false);

	//Monster
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Monster\\Boss_Spike\\", false);
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Monster\\Silversword_Soldier\\", false);
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Monster\\Alpaca_White\\", false);
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Monster\\Alpaca_Brown\\", false);
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Monster\\Alpaca_Black\\", false);

	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Monster\\Wolf\\", false);
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Monster\\Succubus_Scythe\\", false);
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Monster\\Undead_Priest\\", false);
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Monster\\EntSoldier\\", false);

	auto player = Load_Player();
	Load_Camera(player);
	Load_MapFile(L"FieldMap", player);

	Load_Companion(L"Spike", player, _float4{ 118.471f, -1.26f, 59.8f, 1.f});
	Load_Companion(L"Dellons", player, _float4{ 128.471f, -2.98f, 78.3f, 1.f });
	Load_Companion(L"Shane", player, _float4{ 98.f, -0.6f, 73.76f, 1.f});

	Load_Monster(3, L"Wolf", player);
	Load_Monster(3, L"Silversword_Soldier", player);
	Load_Monster(3, L"Succubus_Scythe", player);
	Load_Monster(3, L"Undead_Priest", player);
	Load_Monster(3, L"EntSoldier", player);

	Load_Ui(player);
	return S_OK;
}

void FieldScene::Load_MapFile(const wstring& _mapFileName, shared_ptr<GameObject> pPlayer)
{
	// 오브젝트로드
	__super::Load_MapFile(_mapFileName, pPlayer);
	
	// 
	Load_Water();
	// 터레인로드
	Load_Terrain();
	
}

void FieldScene::Load_Water()
{


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
	Add_GameObject_Front(obj);
}
shared_ptr<GameObject> FieldScene::Load_Player()
{
	if (CUR_SCENE && typeid(*CUR_SCENE.get()) == typeid(LoadingScene))
	{
		auto pPlayer = static_pointer_cast<LoadingScene>(CUR_SCENE)->Get_StaticObjectFromLoader(L"Player");
		if (pPlayer && pPlayer->Get_CharacterController())
		{
			auto& desc = pPlayer->Get_CharacterController()->Get_CapsuleControllerDesc();
			desc.radius = 0.5f;
			desc.height = 5.f;
			_float3 vPos = pPlayer->Get_Transform()->Get_State(Transform_State::POS).xyz();
			desc.position = { vPos.x, vPos.y, vPos.z };
			pPlayer->Get_CharacterController()->Create_Controller();
			return pPlayer;
		}
	}
	
	{
		// Add. Player
		shared_ptr<GameObject> ObjPlayer = make_shared<GameObject>();

		ObjPlayer->Add_Component(make_shared<Transform>());

		ObjPlayer->Get_Transform()->Set_State(Transform_State::POS, _float4(0.f, 0.f, 0.f, 1.f));
		{
			shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

			shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
			{
				shared_ptr<Model> model = RESOURCES.Get<Model>(L"Player");
				model->AddParts(L"Am_Ct_Hat_022", PARTS_INFO::Hair);
				model->AddParts(L"Am_Head_001", PARTS_INFO::Head);
				model->AddParts(L"Am_Ct_Uniform_022", PARTS_INFO::Uniform);
				animator->Set_Model(model);

			}

			ObjPlayer->Add_Component(animator);
			ObjPlayer->Add_Component(make_shared<Player_FSM>());
		}
		ObjPlayer->Set_Name(L"Player");
		ObjPlayer->Set_VelocityMap(true);
		ObjPlayer->Add_Component(make_shared<OBBBoxCollider>(_float3{ 0.5f, 0.8f, 0.5f })); //obbcollider
		ObjPlayer->Get_Collider()->Set_CollisionGroup(Player_Body);
		ObjPlayer->Get_Collider()->Set_Activate(true);

		{
			auto controller = make_shared<CharacterController>();
			ObjPlayer->Add_Component(controller);
			auto& desc = controller->Get_CapsuleControllerDesc();
			desc.radius = 0.5f;
			desc.height = 5.f;
			_float3 vPos = ObjPlayer->Get_Transform()->Get_State(Transform_State::POS).xyz();
			desc.position = { vPos.x, vPos.y, vPos.z };
			controller->Create_Controller();
		}
		ObjPlayer->Set_DrawShadow(true);
		ObjPlayer->Set_ObjectGroup(OBJ_TEAM);

		Add_GameObject(ObjPlayer,true);

		//Add. Player's Weapon
		shared_ptr<GameObject> ObjWeapon = make_shared<GameObject>();

		ObjWeapon->Add_Component(make_shared<Transform>());
		{
			shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

			shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
			{
				shared_ptr<Model> model = RESOURCES.Get<Model>(L"Weapon_Player");
				renderer->Set_Model(model);
			}

			ObjWeapon->Add_Component(renderer);

			WeaponScript::WEAPONDESC desc;
			desc.strBoneName = L"Bip001-Prop1";
			desc.matPivot = _float4x4::CreateRotationX(-XM_PI / 2.f) * _float4x4::CreateRotationZ(XM_PI);
			desc.pWeaponOwner = ObjPlayer;

			ObjWeapon->Add_Component(make_shared<WeaponScript>(desc));
		}
		ObjWeapon->Set_DrawShadow(true);
		ObjWeapon->Set_Name(L"Weapon_Player");
		ObjWeapon->Set_VelocityMap(true);
		Add_GameObject(ObjWeapon,true);

		ObjPlayer->Add_Component(make_shared<HeroChangeScript>());


		return ObjPlayer;

	}
	return nullptr;
}

void FieldScene::Load_Terrain()
{
	// 타일텍스쳐로드
	wstring partsPath = L"..\\Resources\\Textures\\MapObject\\TerrainTile\\";
	for (auto& entry : fs::recursive_directory_iterator(partsPath))
	{

		if (entry.is_directory())
			continue;

		string fileName = entry.path().filename().string();
		Utils::DetachExt(fileName);

		// 타일의 텍스쳐이름을 리소스에 로드
		wstring TileTexture = L"..\\Resources\\Textures\\MapObject\\TerrainTile\\";
		TileTexture += Utils::ToWString(fileName) + L".dds";
		auto texture = RESOURCES.Load<Texture>(Utils::ToWString(fileName), TileTexture);
		if (!texture)
		{
			Utils::ChangeExt(TileTexture, L".tga");
			texture = RESOURCES.Load<Texture>(Utils::ToWString(fileName), TileTexture);
		}
	}

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

	_int loadedSizeX = 0;
	_int loadedSizeY = 0;
	file->Read<_int>(loadedSizeX);
	file->Read<_int>(loadedSizeY);

// 터레인버퍼생성
	loadedTerrain->Get_Geometry()->Set_Vertices(loadedVertices);
	loadedTerrain->Get_Geometry()->Set_Indices(loadedIndices);
	loadedTerrain->Create_Buffer();

	/// 지형오브젝트 생성
	shared_ptr<GameObject> TerrainObject = make_shared<GameObject>();
	TerrainObject->Set_Name(L"Terrain");
	TerrainObject->GetOrAddTransform();
// 터레인생성
	// 메시렌더러
	//shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Terrain.fx"));
	//renderer->Set_Mesh(loadedTerrain);
	//renderer->Set_Pass(18); // 터레인패스
	shared_ptr<TerrainRenderer> renderer = make_shared<TerrainRenderer>(RESOURCES.Get<Shader>(L"Shader_Terrain.fx"));
	renderer->CreateGrid(loadedSizeX, loadedSizeY);

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
		auto HeightMap = RESOURCES.GetOrAddTexture(L"HeightMap1",L"..\\Resources\\Textures\\MapObject\\TerrainTile\\HeightMap.png");
		if (HeightMap == nullptr)
		{
			MSG_BOX("NoSubTexture");
			return;
		}
		material->Set_TextureMap(HeightMap, TextureMapType::TEXTURE9);
	}

	renderer->Set_Material(material);

	// 메시를 통해 메시콜라이더 생성
	shared_ptr<MeshCollider> pCollider = make_shared<MeshCollider>(*loadedTerrain.get());
	TerrainObject->Add_Component(pCollider);
	pCollider->Set_Activate(true);
	// 리지드바디 생성
	auto rigidBody = make_shared<RigidBody>();
	rigidBody->Create_RigidBody(pCollider, TerrainObject->GetOrAddTransform()->Get_WorldMatrix());
	TerrainObject->Add_Component(rigidBody);
	Add_GameObject(TerrainObject);

	TerrainObject->Add_Component(renderer);

}

void FieldScene::Load_Camera(shared_ptr<GameObject> pPlayer)
{
	{
		//GameObj for Camera Create
		shared_ptr<GameObject> camera = make_shared<GameObject>();

		// Transform Component 
		camera->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(1.f, 1.f, 1.f, 1.f));

		camera->GetOrAddTransform()->Set_Speed(5.f);

		// Camera Component Add
		CameraDesc desc;
		desc.fFOV = XM_PI / 3.f;
		desc.strName = L"Default";
		desc.fSizeX = _float(g_iWinSizeX);
		desc.fSizeY = _float(g_iWinSizeY);
		desc.fNear = 0.1f;
		desc.fFar = 1000.f;
		shared_ptr<Camera> cameraComponent = make_shared<Camera>(desc);

		camera->Add_Component(cameraComponent);


		camera->Get_Camera()->Set_ProjType(ProjectionType::Perspective);
		//Layer_UI culling true
		camera->Get_Camera()->Set_CullingMaskLayerOnOff(Layer_UI, true);

		camera->Add_Component(make_shared<MainCameraScript>(pPlayer));

		Add_GameObject(camera);

		//Setting Camera
		pPlayer->Get_FSM()->Set_Camera(camera);
	}
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();

		camera->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(0.f, 0.f, 0.f, 1.f));

		CameraDesc desc;
		desc.fFOV = XM_PI / 3.f;
		desc.strName = L"UI_Cam";
		desc.fSizeX = _float(g_iWinSizeX);
		desc.fSizeY = _float(g_iWinSizeY);
		desc.fNear = 0.1f;
		desc.fFar = 1000.f;
		shared_ptr<Camera> cameraComponent = make_shared<Camera>(desc);

		camera->Add_Component(cameraComponent);

		camera->Get_Camera()->Set_ProjType(ProjectionType::Orthographic);

		camera->Get_Camera()->Set_CullingMaskAll();
		camera->Get_Camera()->Set_CullingMaskLayerOnOff(Layer_UI, false);

		Add_GameObject(camera);
	}


}

void FieldScene::Load_Monster(_uint iCnt, const wstring& strMonsterTag, shared_ptr<GameObject> pPlayer, _bool bCharacterController)
{
	{
		for (_uint i = 0; i < iCnt; i++)
		{
			// Add. Monster
			shared_ptr<GameObject> ObjMonster = make_shared<GameObject>();

			ObjMonster->Add_Component(make_shared<Transform>());

			_float fRan = 0;
			if (rand() % 2 == 0)
				fRan = -1.f;
			else
				fRan = 1.f;

			_float3 vSpawnPos = MathUtils::Get_RandomVector(_float3{ 165.f, -1.3f, 50.f }, _float3{ 235.f, -1.3f, 60.f });
			ObjMonster->Get_Transform()->Set_State(Transform_State::POS, _float4{ vSpawnPos.x, vSpawnPos.y, vSpawnPos.z, 1.f });
			{
				shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

				shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
				{
					shared_ptr<Model> model = RESOURCES.Get<Model>(strMonsterTag);
					animator->Set_Model(model);
				}

				ObjMonster->Add_Component(animator);

				if (strMonsterTag == L"Silversword_Soldier")
					ObjMonster->Add_Component(make_shared<Silversword_Soldier_FSM>());
				else if (strMonsterTag == L"Succubus_Scythe")
					ObjMonster->Add_Component(make_shared<Succubus_Scythe_FSM>());
				else if (strMonsterTag == L"Undead_Priest")
					ObjMonster->Add_Component(make_shared<Undead_Priest_FSM>());
				else if (strMonsterTag.find(L"Bad_Alpaca") != wstring::npos)
					ObjMonster->Add_Component(make_shared<Alpaca_FSM>());
				else if (strMonsterTag.find(L"Alpaca") != wstring::npos)
					ObjMonster->Add_Component(make_shared<NeutralAlpaca_FSM>());
				else if (strMonsterTag == L"Wolf")
					ObjMonster->Add_Component(make_shared<Wolf_FSM>());
				else if (strMonsterTag == L"EntSoldier")
					ObjMonster->Add_Component(make_shared<EntSoldier_FSM>());

				ObjMonster->Get_FSM()->Set_Target(pPlayer);
			}
			ObjMonster->Add_Component(make_shared<OBBBoxCollider>(_float3{ 0.5f, 0.7f, 0.5f })); //obbcollider
			ObjMonster->Get_Collider()->Set_CollisionGroup(Monster_Body);
			ObjMonster->Get_Collider()->Set_Activate(true);

			wstring strMonsterName = strMonsterTag + to_wstring(i);
			ObjMonster->Set_Name(strMonsterName);
			{
				//Alpaca and Wolf don't use character controller
				if (bCharacterController)
				{
					auto controller = make_shared<CharacterController>();
					ObjMonster->Add_Component(controller);
					auto& desc = controller->Get_CapsuleControllerDesc();
					desc.radius = 0.5f;
					desc.height = 5.f;
					_float3 vPos = ObjMonster->Get_Transform()->Get_State(Transform_State::POS).xyz();
					desc.position = { vPos.x, vPos.y, vPos.z };
					controller->Create_Controller();
				}
			}
			ObjMonster->Set_ObjectGroup(OBJ_MONSTER);

			Add_GameObject(ObjMonster);
		}
	}
}

void FieldScene::Load_Companion(const wstring& strCompanionTag, shared_ptr<GameObject> pPlayer, _float4 vSpawnPos)
{
	{
		// Add. Companion
		shared_ptr<GameObject> ObjCompanion = make_shared<GameObject>();

		ObjCompanion->Add_Component(make_shared<Transform>());

		ObjCompanion->Get_Transform()->Set_State(Transform_State::POS, vSpawnPos);
		{
			shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

			shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
			{
				shared_ptr<Model> model = RESOURCES.Get<Model>(strCompanionTag);
				animator->Set_Model(model);
			}

			ObjCompanion->Add_Component(animator);

			if (strCompanionTag == L"Spike")
				ObjCompanion->Add_Component(make_shared<Companion_Spike_FSM>());
			else if (strCompanionTag == L"Dellons")
				ObjCompanion->Add_Component(make_shared<Companion_Dellons_FSM>());
			else if (strCompanionTag == L"Shane")
				ObjCompanion->Add_Component(make_shared<Companion_Shane_FSM>());


			ObjCompanion->Get_FSM()->Set_Target(pPlayer);
		}
		wstring CompanionName = L"Companion_" + strCompanionTag;
		ObjCompanion->Set_Name(CompanionName);
		ObjCompanion->Set_VelocityMap(true);
		ObjCompanion->Add_Component(make_shared<OBBBoxCollider>(_float3{ 0.5f, 0.8f, 0.5f })); //obbcollider
		ObjCompanion->Get_Collider()->Set_CollisionGroup(Player_Body);
		ObjCompanion->Get_Collider()->Set_Activate(true);

		{
			auto controller = make_shared<CharacterController>();
			ObjCompanion->Add_Component(controller);
			auto& desc = controller->Get_CapsuleControllerDesc();
			desc.radius = 0.5f;
			desc.height = 5.f;
			_float3 vPos = ObjCompanion->Get_Transform()->Get_State(Transform_State::POS).xyz();
			desc.position = { vPos.x, vPos.y, vPos.z };
			controller->Create_Controller();
		}
		ObjCompanion->Set_DrawShadow(true);
		ObjCompanion->Set_ObjectGroup(OBJ_TEAM);
		Add_GameObject(ObjCompanion);


		//Add. Companion's Weapon
		shared_ptr<GameObject> ObjWeapon = make_shared<GameObject>();

		ObjWeapon->Add_Component(make_shared<Transform>());
		{
			shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

			shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
			{
				wstring WeaponModelTag = L"Weapon_" + strCompanionTag;
				shared_ptr<Model> model = RESOURCES.Get<Model>(WeaponModelTag);
				renderer->Set_Model(model);
			}

			ObjWeapon->Add_Component(renderer);

			WeaponScript::WEAPONDESC desc;
			desc.strBoneName = L"Bip001-Prop1";
			desc.matPivot = _float4x4::CreateRotationX(-XM_PI / 2.f) * _float4x4::CreateRotationZ(XM_PI);
			desc.pWeaponOwner = ObjCompanion;

			ObjWeapon->Add_Component(make_shared<WeaponScript>(desc));
		}
		ObjWeapon->Set_DrawShadow(true);
		wstring CompanionWeaponName = L"Companion_Weapon_" + strCompanionTag;
		ObjWeapon->Set_Name(CompanionWeaponName);
		ObjWeapon->Set_VelocityMap(true);
		Add_GameObject(ObjWeapon);

	}
}

void FieldScene::Load_Ui(shared_ptr<GameObject> pPlayer)
{
	list<shared_ptr<GameObject>>& tmp = static_pointer_cast<LoadingScene>(CUR_SCENE)->Get_StaticObjectsFromLoader();
	Load_UIFile(L"..\\Resources\\UIData\\UI_Main.dat", tmp);
	Load_UIFile(L"..\\Resources\\UIData\\UI_Main_Button.dat", tmp);
	Load_UIFile(L"..\\Resources\\UIData\\UI_Char_Change.dat", tmp);
	Load_UIFile(L"..\\Resources\\UIData\\UI_Card_Deck.dat", tmp, false, false);
	Load_UIFile(L"..\\Resources\\UIData\\UI_Target_LockOn.dat", tmp, false, false);
	Load_UIFile(L"..\\Resources\\UIData\\UI_Cur_Quest.dat", tmp, false, false);
	Load_UIFile(L"..\\Resources\\UIData\\UI_Setting.dat", tmp, false, false);
	Load_UIFile(L"..\\Resources\\UIData\\UI_Controller.dat", tmp, false);

	{
		weak_ptr<GameObject> pObj = Get_UI(L"Main_UI_Controller");
		if (false == pObj.expired())
		{
			auto pScript = make_shared<MainUiController>();
			pObj.lock()->Add_Component(pScript);
		}
	}
	{
		weak_ptr<GameObject> pObj = Get_UI(L"UI_Gacha_Controller");
		if (false == pObj.expired())
		{
			auto pScript = make_shared<UiGachaController>();
			pObj.lock()->Add_Component(pScript);
		}
	}
	{
		weak_ptr<GameObject> pObj = Get_UI(L"UI_Card_Deck_Controller");
		if (false == pObj.expired())
		{
			auto pScript = make_shared<UiCardDeckController>();
			pObj.lock()->Add_Component(pScript);
		}
	}
	{
		weak_ptr<GameObject> pObj = Get_UI(L"UI_Damage_Controller");
		if (false == pObj.expired())
		{
			auto pScript = make_shared<UiDamageCreate>();
			pObj.lock()->Add_Component(pScript);
		}
	}
	{
		auto& pObj = pPlayer;
		if (nullptr != pObj)
		{
			auto pScript = make_shared<UiHpBarController>(0);
			pObj->Add_Component(pScript);
		}
	}
	{
		weak_ptr<GameObject> pObj = Get_UI(L"UI_Target_LockOn");
		if (false == pObj.expired())
		{
			auto pScript = make_shared<UiTargetLockOn>();
			pObj.lock()->Add_Component(pScript);
		}
	}
	{
		weak_ptr<GameObject> pObj = Get_UI(L"UI_Setting_Controller");
		if (false == pObj.expired())
		{
			auto pScript = make_shared<UiSettingController>();
			pObj.lock()->Add_Component(pScript);
		}
	}
	{
		weak_ptr<GameObject> pObj = Get_UI(L"UI_Dialog_Controller");
		if (false == pObj.expired())
		{
			auto pScript = make_shared<UiQuestController>();
			pObj.lock()->Add_Component(pScript);
		}
	}
	{
		weak_ptr<GameObject> pObj = Get_UI(L"UI_Boss_Dialog");
		if (false == pObj.expired())
		{
			auto pScript = make_shared<UiBossDialog>();
			pObj.lock()->Add_Component(pScript);
		}
	}
	{
		weak_ptr<GameObject> pObj = Get_UI(L"UI_Interaction");
		if (false == pObj.expired())
		{
			auto pScript = make_shared<UIInteraction>();
			pObj.lock()->Add_Component(pScript);
		}
	}
	{
		auto pObj = Get_UI(L"UI_Combo_Effect");
		if (nullptr != pObj)
		{
			auto pScript = make_shared<UiComboEffect>();
			pObj->Add_Component(pScript);
		}
	}
	{
		auto pObj = Get_UI(L"UI_Skill_Use_Gauge");
		if (nullptr != pObj)
		{
			auto pScript = make_shared<UiSkillGauge>();
			pObj->Add_Component(pScript);
		}
	}
	{
		weak_ptr<GameObject> pObj = Get_UI(L"UI_Card_Deck_Exit");
		if (false == pObj.expired())
		{
			pObj.lock()->Get_Button()->AddOnClickedEvent([]()
				{
					CUR_SCENE->Get_GameObject(L"UI_Card_Deck_Controller")->Get_Script<UiCardDeckController>()->Set_Render(false);
				});
		}
	}
	{
		weak_ptr<GameObject> pObj = Get_UI(L"UI_Char_Change");
		if (false == pObj.expired())
		{
			auto pScript = make_shared<UiCharChange>();
			pObj.lock()->Add_Component(pScript);
		}
	}
	{
		auto pObj = Get_UI(L"UI_Main_Button2");
		if (nullptr != pObj)
		{
			pObj->Get_Button()->AddOnClickedEvent([]()
				{
					CUR_SCENE->Get_UI(L"UI_Card_Deck_Controller")->Get_Script<UiCardDeckController>()->Set_Render(true);
				});
		}
	}
	{
		auto pObj = Get_UI(L"UI_Main_Button3");
		if (nullptr != pObj)
		{
			pObj->Get_Button()->AddOnClickedEvent([]()
				{
					CUR_SCENE->Get_UI(L"UI_Setting_Controller")->Get_Script<UiSettingController>()->Set_Render(true);
				});
		}
	}
	{
		for (_uint i = 0; i < 7; ++i)
		{
			if (1 == i)
				continue;

			wstring strName = L"UI_Skill" + to_wstring(i) + L"_Effect";
			auto pScript = make_shared<UiSkillButtonEffect>();
			auto pObj = Get_UI(strName);
			if (nullptr != pObj)
				pObj->Add_Component(pScript);
		}
	}
	{
		for (_uint i = 2; i < 7; ++i)
		{
			auto pScript = make_shared<UiCoolEndEffect>();
			wstring strTemp = L"UI_Cool_End";
			strTemp += to_wstring(i);
			auto pObj = Get_UI(strTemp);
			if (nullptr != pObj)
				pObj->Add_Component(pScript);
		}
	}
	{
		auto pScript = make_shared<CoolTimeCheckScript>();
		auto& pObj = pPlayer;
		if (nullptr != pObj) {
			pObj->Add_Component(pScript);
		}
	}
}

void FieldScene::Load_Debug()
{
	{
		shared_ptr<GameObject> debugText = make_shared<GameObject>();
		debugText->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(-300.f, 400.f, 5.f, 1.f));
		debugText->Add_Component(make_shared<FontRenderer>(L""));
		debugText->Get_FontRenderer()->Set_Font(RESOURCES.Get<CustomFont>(L"136ex"), Color(0.5f, 0.5f, 0.5f, 1.f), 1.f);
		debugText->Set_LayerIndex(Layer_UI);
		debugText->Add_Component(make_shared<ObjectTransformDebug>());
		debugText->Get_Script<ObjectTransformDebug>()->Set_Target(Get_GameObject(L"Boss_Spike"));
		Add_GameObject(debugText);
	}
}