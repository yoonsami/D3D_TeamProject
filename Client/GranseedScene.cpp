#include "pch.h"
#include "GranseedScene.h"

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
#include "FieldScene.h"
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
#include "GroupEffectData.h"
#include "UiHpBarController.h"
#include "MapObjectScript.h"
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

#include "MeshEffectData.h"
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
#include "UIInteraction.h"
#include "UiQuestController.h"
#include "UiSettingController.h"
#include "UiBossDialog.h"
#include "UiCostumeController.h"
#include "UiUseItemSlot.h"
#include "UIShop.h"
#include "UiMessageCreater.h"
#include "UiDialogController.h"

#include <filesystem>
#include "GachaScene.h"
#include "KrisScene.h"
#include "MirScene.h"
#include "AttackColliderInfoScript.h"
#include "Shane_FSM.h"
#include "NPC_FSM.h"
#include "GranseedGuard01_FSM.h"
#include "GranseedGuard02_FSM.h"
#include "GranseedPotion_FSM.h"
#include "GranseedTraveler_FSM.h"
#include "GranseedChildren_FSM.h"
#include "UiMarkNpc.h"
#include "Hide_OrctongScript.h"
#include "WitcherSense.h"
#include "MathUtils.h"
#include "GroupEffect.h"
#include "Smoke_WitcherSense.h"
#include "Smoke_Detect.h"
namespace fs = std::filesystem;

GranseedScene::GranseedScene()
{
	m_strSceneName = L"GranseedScene";
}

GranseedScene::~GranseedScene()
{
}

void GranseedScene::Init()
{
	__super::Init();

	auto pPlayer = Get_Player();


	for(_int i =0; i< _int(HERO::MAX); ++i)
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

void GranseedScene::Tick()
{
	__super::Tick();

	if (KEYTAP(KEY_TYPE::V))
	{
		auto pObj = Get_UI(L"UI_Message_Controller");
		if (pObj)
			pObj->Get_Script<UiMessageCreater>()->Create_Message(Get_GameObject(L"GS_GachaMan"));
		//auto pObj = Get_UI(L"UI_Costume_Controller");
		//if (pObj)
		//	pObj->Get_Script<UiCostumeController>()->Create_Costume();
		//auto pObj = Get_UI(L"UI_Interaction");
		//if(pObj)
		//	pObj->Get_Script<UIInteraction>()->Create_Interaction(NPCTYPE::QUEST);
		//auto pObj = Get_UI(L"UI_Boss_Dialog");
		//if (pObj)
		//	pObj->Get_Script<UiBossDialog>()->Create_Dialog(BOSS::DELLONS);
	}
	if (KEYTAP(KEY_TYPE::V))
	{
		//auto pObj = Get_UI(L"UI_Shop_Controller");
		//if (pObj)
		//	pObj->Get_Script<UIShop>()->Remove_Shop();
		//auto pObj = Get_UI(L"UI_Costume_Controller");
		//if (pObj)
		//	pObj->Get_Script<UiCostumeController>()->Remove_Costume();
		//auto pObj = Get_UI(L"UI_Interaction");
		//if (pObj)
		//	pObj->Get_Script<UIInteraction>()->Create_Interaction(NPCTYPE::GACHA);
		//auto pObj = Get_UI(L"UI_Boss_Dialog");
		//if (pObj)
		//	pObj->Get_Script<UiBossDialog>()->Is_Finish();
	}

}

void GranseedScene::Late_Tick()
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

void GranseedScene::Final_Tick()
{
	__super::Final_Tick();
	PHYSX.Tick();
	
	if (KEYPUSH(KEY_TYPE::TAB) && KEYPUSH(KEY_TYPE::F4))
	{


		//shared_ptr<LoadingScene> scene = make_shared<LoadingScene>(make_shared<MirScene>());
		shared_ptr<LoadingScene> scene = make_shared<LoadingScene>(make_shared<FieldScene>());
		scene->Set_StaticObjects(m_StaticObject);
		PHYSX.Set_CharacterControllerNull();
		SCENE.Change_Scene(scene);
		g_bCutScene = false;
	}
	if (KEYPUSH(KEY_TYPE::TAB) && KEYPUSH(KEY_TYPE::F7))
	{


		shared_ptr<LoadingScene> scene = make_shared<LoadingScene>(make_shared<GranseedScene>());
		scene->Set_StaticObjects(m_StaticObject);
		PHYSX.Set_CharacterControllerNull();
		SCENE.Change_Scene(scene);
		g_bCutScene = false;
	}
}

HRESULT GranseedScene::Load_Scene()
{
	RESOURCES.Delete_NonStaticResources();
	PHYSX.Init();

	//Static
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Hero\\", true);

	RESOURCES.CreateModel(L"..\\Resources\\Models\\VfxMesh\\", true);
	RESOURCES.CreateModel(L"..\\Resources\\Models\\MapObject\\SkyBox\\", true);
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Weapon\\", true);

	//Map
	RESOURCES.CreateModel(L"..\\Resources\\Models\\MapObject\\Granseed\\", false);
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Npc\\Granseed\\", false);
	// Gacha
	RESOURCES.CreateModel(L"..\\Resources\\Models\\MapObject\\Kyle\\", false);
	RESOURCES.CreateModel(L"..\\Resources\\Models\\MapObject\\Yeopo\\", false);

	auto pPlayer = Load_Player();
	Load_Camera(pPlayer);
	Load_MapFile(L"GranseedMap", pPlayer);

	Load_Ui(pPlayer);
	Load_NPC(L"GranseedMap");
	Load_HideAndSeek(pPlayer);
	Load_Debug();
	return S_OK;
}

shared_ptr<GameObject> GranseedScene::Load_Player()
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
		ObjPlayer->Add_Component(make_shared<WitcherSense>());
		ObjPlayer->Get_Transform()->Set_State(Transform_State::POS, _float4(-0.f, 0.f, 0.f, 1.f));
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

			{
				shared_ptr<GameObject> attackCollider = make_shared<GameObject>();
				attackCollider->GetOrAddTransform();
				attackCollider->Add_Component(make_shared<SphereCollider>(1.f));
				attackCollider->Get_Collider()->Set_CollisionGroup(Player_Attack);

				ObjPlayer->Get_FSM()->Set_AttackCollider(attackCollider) ;

				Add_GameObject(attackCollider,true);
				attackCollider->Get_Collider()->Set_Activate(false);

				attackCollider->Add_Component(make_shared<AttackColliderInfoScript>());
				attackCollider->Set_Name(L"Player_AttackCollider");
				attackCollider->Get_Script<AttackColliderInfoScript>()->Set_ColliderOwner(ObjPlayer);

			}
		}
		ObjPlayer->Set_Name(L"Player");
		ObjPlayer->Set_VelocityMap(true);
		ObjPlayer->Set_Instancing(false);
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
		ObjWeapon->Set_Instancing(false);
		Add_GameObject(ObjWeapon,true);
		ObjPlayer->Get_FSM()->Set_Weapon(ObjWeapon);
		shared_ptr<MonoBehaviour> script = make_shared<HeroChangeScript>();
		ObjPlayer->Add_Component(script);
		return ObjPlayer;
	}
	return nullptr;
}

void GranseedScene::Load_Camera(shared_ptr<GameObject> pPlayer)
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
		camera->Get_Camera()->Set_CullingMaskLayerOnOff(Layer_AfterUI, true);

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
	{
		shared_ptr<GameObject> camera = make_shared<GameObject>();

		camera->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(0.7f, 1.f, -3.f, 1.f));

		CameraDesc desc;
		desc.fFOV = XM_PI / 3.f;
		desc.strName = L"After_UI";
		desc.fSizeX = _float(g_iWinSizeX);
		desc.fSizeY = _float(g_iWinSizeY);
		desc.fNear = 0.1f;
		desc.fFar = 1000.f;
		shared_ptr<Camera> cameraComponent = make_shared<Camera>(desc);

		camera->Add_Component(cameraComponent);

		camera->Get_Camera()->Set_ProjType(ProjectionType::Perspective);

		camera->Get_Camera()->Set_CullingMaskAll();
		camera->Get_Camera()->Set_CullingMaskLayerOnOff(Layer_AfterUI, false);

		Add_GameObject(camera);
	}

}

void GranseedScene::Load_Ui(shared_ptr<GameObject> pPlayer)
{
	list<shared_ptr<GameObject>>& tmp = static_pointer_cast<LoadingScene>(CUR_SCENE)->Get_StaticObjectsFromLoader();
	Load_UIFile(L"..\\Resources\\UIData\\UI_Controller.dat", tmp, false);

	Load_UIFile(L"..\\Resources\\UIData\\UI_Main.dat", tmp);
	Load_UIFile(L"..\\Resources\\UIData\\UI_Main_Button.dat", tmp);
	Load_UIFile(L"..\\Resources\\UIData\\UI_Char_Change.dat", tmp);
	Load_UIFile(L"..\\Resources\\UIData\\UI_Card_Deck.dat", tmp, false, false);
	Load_UIFile(L"..\\Resources\\UIData\\UI_Target_LockOn.dat", tmp, false, false);
	Load_UIFile(L"..\\Resources\\UIData\\UI_Cur_Quest.dat", tmp, false, false);
	Load_UIFile(L"..\\Resources\\UIData\\UI_Setting.dat", tmp, false, false);

	{
		weak_ptr<GameObject> pObj = Get_UI(L"Main_UI_Controller");
		if (false == pObj.expired())
		{
			auto pScript = make_shared<MainUiController>();
			pObj.lock()->Add_Component(pScript);
		}
	}

	{
		weak_ptr<GameObject> pObj = Get_UI(L"UI_NpcDialog_Controller");
		if (false == pObj.expired())
		{
			auto pScript = make_shared<UiDialogController>();
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
		weak_ptr<GameObject> pObj = Get_UI(L"UI_Shop_Controller");
		if (false == pObj.expired())
		{
			auto pScript = make_shared<UIShop>();
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
		weak_ptr<GameObject> pObj = Get_UI(L"UI_UseItem_Slot_Controller");
		if (false == pObj.expired())
		{
			auto pScript = make_shared<UiUseItemSlot>();
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
		auto pObj = Get_UI(L"UI_Costume_Controller");
		if (nullptr != pObj)
		{
			auto pScript = make_shared<UiCostumeController>();
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
		auto pObj = Get_UI(L"UI_Main_Button0");
		if (nullptr != pObj)
		{
			pObj->Get_Button()->AddOnClickedEvent([]()
				{
					CUR_SCENE->Get_UI(L"UI_Setting_Controller")->Get_Script<UiSettingController>()->Set_Render(true);
				});
		}
	}

	{
		auto pObj = Get_UI(L"UI_Main_Button1");
		if (nullptr != pObj)
		{
			pObj->Get_Button()->AddOnClickedEvent([]()
				{
					CUR_SCENE->Get_UI(L"UI_Card_Deck_Controller")->Get_Script<UiCardDeckController>()->Set_Render(true);
				});
		}
	}

	{
		auto pObj = Get_UI(L"UI_Main_Button2");
		if (nullptr != pObj)
		{
			pObj->Get_Button()->AddOnClickedEvent([]()
				{
					CUR_SCENE->Get_UI(L"UI_Costume_Controller")->Get_Script<UiCostumeController>()->Create_Costume();
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










	// ※※※※※※※※※※※※※※※
	// add only GranseedScene
	// ※※※※※※※※※※※※※※※

	{
		auto pObj = make_shared<GameObject>();
		pObj->Set_LayerIndex(Layer_UI);
		pObj->Set_Instancing(false);
		pObj->Set_Name(L"UI_Message_Controller");

		auto pScript = make_shared<UiMessageCreater>();
		pObj->Add_Component(pScript);

		Add_GameObject(pObj);
	}

	// ※※※※※※※※※※※※※※※
	// add only GranseedScene
	// ※※※※※※※※※※※※※※※







}

void GranseedScene::Load_Debug()
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

void GranseedScene::Load_NPC(const wstring& dataFileName)
{
	wstring strFilePath = L"..\\Resources\\MapData\\" + dataFileName + L".subdata";
	shared_ptr<FileUtils> file = make_shared<FileUtils>();
	file->Open(strFilePath, FileMode::Read);

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

		if (isMoving)
		{
			switch (iFSMIndex)
			{
			case NPC_FSM::POTION:
			{
				auto fsm = make_shared<GranseedPotion_FSM>();
				fsm->Set_MinMovePos(vMinPos);
				fsm->Set_MaxMovePos(vMaxPos);
				obj->Add_Component(fsm);
				obj->Add_Component(make_shared<UiMarkNpc>(NPCTYPE::POTION));

			}

			break;
			case NPC_FSM::GUARD1:
			{
				auto fsm = make_shared<GranseedGuard01_FSM>();
				fsm->Set_MinMovePos(vMinPos);
				fsm->Set_MaxMovePos(vMaxPos);
				obj->Add_Component(fsm);
			}

				break;
			case NPC_FSM::GUARD2:
			{
				auto fsm = make_shared<GranseedGuard02_FSM>();
				fsm->Set_MinMovePos(vMinPos);
				fsm->Set_MaxMovePos(vMaxPos);
				obj->Add_Component(fsm);
			}

				break;
			case NPC_FSM::TRAVELER:
			{
				auto fsm = make_shared<GranseedTraveler_FSM>();
				fsm->Set_MinMovePos(vMinPos);
				fsm->Set_MaxMovePos(vMaxPos);
				obj->Add_Component(fsm);

				obj->Add_Component(make_shared<UiMarkNpc>(NPCTYPE::GACHA));

			}

				break;
			case NPC_FSM::CHILDREN:
			{
				auto fsm = make_shared<GranseedChildren_FSM>();
				fsm->Set_MinMovePos(vMinPos);
				fsm->Set_MaxMovePos(vMaxPos);
				obj->Add_Component(fsm);
			}
			break;
			}
		}
		obj->Set_DrawShadow(true);
		Add_GameObject(obj);
	}
	
}

void GranseedScene::Load_HideAndSeek(shared_ptr<GameObject> pPlayer)
{
	auto obj = CUR_SCENE->Get_UI(L"UI_Dialog_Controller");
	if (obj && obj->Get_Script<UiQuestController>()->Get_QuestState(QUESTINDEX::HIDE_AND_SEEK))
		return;

	{
		vector<_float3> pos(5);
		pos[0] = { -14.834f, -0.02f,-1.5f };
		pos[1] = { 17.015f, -4.667f, 64.654f };
		pos[2] = { 9.966f, 0.205f, 14.260f };
		pos[3] = { 12.1f, -4.667f,21.194f };
		pos[4] = { -14.834f, -4.667f, 21.161f };

		shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
		shared_ptr<Model> model = RESOURCES.Get<Model>(L"Granseed_Orctong");
		for (_uint i = 0; i < 5; ++i)
		{
			shared_ptr<GameObject> orctong = make_shared<GameObject>();
			orctong->GetOrAddTransform()->Scaled(_float3(1.5f));
			orctong->GetOrAddTransform()->Rotation(_float3(1.f, 0.f, 0.f), XM_PI / 2.f);
			_float4 vPos = _float4(pos[i], 1.f) + _float4(0.f, 0.4f, 0.f, 0.f);
			orctong->GetOrAddTransform()->Set_State(Transform_State::POS, vPos);
			orctong->Add_Component(make_shared<ModelRenderer>(shader));
			orctong->Get_ModelRenderer()->Set_Model(model);
			orctong->Get_ModelRenderer()->Set_RenderState(false);
			orctong->Get_ModelRenderer()->Set_PassType(ModelRenderer::PASS_DEFAULT);
			orctong->Set_CullPos(vPos.xyz());
			orctong->Set_CullRadius(0.4f);
			orctong->Set_Instancing(false);
			orctong->Set_DrawShadow(true);
			orctong->Add_Component(make_shared<SphereCollider>(0.4f));
			orctong->Set_Name(L"Hide_Orctong" + to_wstring(i));


			Add_GameObject(orctong);
		}
	}

	{
		_float x2 = -0.437f, y2 = 0.3f, z2 = 11.372f;
		_float x1 = -14.628f, y1 = 0.3f, z1 = -1.933f;
	
		_float3 coefficients = MathUtils::calculateQuadraticCoefficients(x1, y1, z1, x2, y2, z2);

		vector<_float3>quadraticCurvePoints;

		for (_float x = x1; x <= x2; x += (x2 - x1) / 10.f) {
			_float z = MathUtils::evaluateQuadraticEquation(x, coefficients);
			_float3 vRandomOffset = MathUtils::Get_RandomVector(_float3(0.f, 0.3f, 0.f), _float3(0.f, -0.3f, 0.f));
			_float4 vPos = _float4({ x, y1, z }, 1.f);
			shared_ptr<GameObject> obj = make_shared<GameObject>();
			obj->GetOrAddTransform()->Set_State(Transform_State::POS, vPos + vRandomOffset);

			_int skillIndex = rand() % 3;
		

			wstring strSkilltag = L"Witcher_Sense" + to_wstring(skillIndex);
			Set_Name(strSkilltag);

			wstring wstrFileName = strSkilltag + L".dat";
			wstring wtsrFilePath = TEXT("..\\Resources\\EffectData\\GroupEffectData\\") + wstrFileName;
			shared_ptr<GroupEffectData> pGroupEffectData = RESOURCES.GetOrAddGroupEffectData(strSkilltag, wtsrFilePath);
			shared_ptr<GroupEffect> pGroupEffect = make_shared<GroupEffect>();
			obj->Add_Component(pGroupEffect);
			obj->Get_GroupEffect()->Set_Tag(pGroupEffectData->Get_GroupEffectDataTag());
			obj->Get_GroupEffect()->Set_MemberEffectData(pGroupEffectData->Get_MemberEffectData());
			obj->Get_GroupEffect()->Set_InitWorldMatrix(obj->Get_Transform()->Get_WorldMatrix());
			obj->Get_GroupEffect()->Set_MemberEffectMaterials();
			obj->Set_Name(strSkilltag);
			obj->Get_GroupEffect()->Set_TickRenderState(false, false);
			obj->Add_Component(make_shared<Smoke_WitcherSense>(pPlayer));

			Add_GameObject(obj);
		}

	}

	{
		shared_ptr<GameObject> smellScript = make_shared<GameObject>();
		smellScript->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(-0.437f, -0.02f, 11.372f, 1.f));
		smellScript->Add_Component(make_shared<Smoke_Detect>());
		Add_GameObject(smellScript);

	}

}
