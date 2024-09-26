#include "pch.h"
#include "DemoScene.h"

// For. Components
#include "Light.h"
#include "Camera.h"
#include "ModelAnimator.h"
#include "ModelRenderer.h"

// For. Script
#include "FreeCameraScript.h"

// For. Etc
#include "Model.h"
#include "ImGuiToolMgr.h"

GranseedScene::GranseedScene()
{
}

void GranseedScene::Init()
{
	__super::Init();

	COLLISION.Check_Group(_int(CollisionGroup::Player), _int(CollisionGroup::Monster));

	// For. Initialize ImGui Tool mgr
	IMGUITOOL_MGR.Init(DEVICE.Get(), CONTEXT.Get());
	IMGUITOOL_MGR.Tick();
}

void GranseedScene::Tick()
{
	__super::Tick();
	IMGUITOOL_MGR.Tick();

	if (KEYHOLD(KEY_TYPE::LALT) && KEYHOLD(KEY_TYPE::LBUTTON))
		INPUT.Set_Mouse_Move(false);
	else
		INPUT.Set_Mouse_Move(true);
}

void GranseedScene::Late_Tick()
{
	__super::Late_Tick();
}

void GranseedScene::Final_Tick()
{
	__super::Final_Tick();
	PHYSX.Tick();
}

void GranseedScene::Render()
{
	__super::Render();

	IMGUITOOL_MGR.Render();
}

HRESULT GranseedScene::Load_Scene()
{
	PHYSX.Init();
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Parts\\Player\\", true);
	RESOURCES.LoadParts();
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Hero\\");
	RESOURCES.CreateModel(L"..\\Resources\\Models\\VfxMesh\\");
	RESOURCES.CreateModel(L"..\\Resources\\Models\\MapObject\\SkyBox\\");
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Weapon\\");
	RESOURCES.CreateModel(L"..\\Resources\\Models\\MapObject\\Mir\\");
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Monster\\");

	RESOURCES.LoadParts();
	Load_DemoModel();
	Load_Camera();

	//Load_MapFile(L"GranseedMap", nullptr);
	Load_MapFile(L"MirMap", nullptr);
	return S_OK;
}

void GranseedScene::Load_DemoModel()
{
	// For. ���� ���� �� GameObject���� 
	shared_ptr<GameObject> testObj = make_shared<GameObject>();

	// For. GameObject�� Transform Component �߰�
		// ���� �ϳ� ���. AddComponent �Ǵ� GetOrAddTransform(������ ��ȯ ������ ������ ��ȯ)
	testObj->Add_Component(make_shared<Transform>());
		//testObj->GetOrAddTransform();

	// For. GameObject�� Transform �������ֱ� 
	testObj->Get_Transform()->Set_State(Transform_State::POS, _float4(0.f, 0.f, 0.f, 1.f));
	testObj->Get_Transform()->Scaled(_float3(3.f,1.f,3.f));
	
	// For. Animator�����Ҷ� �ʿ��� 
	shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

	// For. GameObject�� ���� animator component �����ϱ� 
	shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
	
	// For. Animator component�� ���� Model �����ϱ� 
	shared_ptr<Model> model = RESOURCES.Get<Model>(L"GridPlane");

	// For. Animator�� Model ���̱�
	animator->Set_Model(model);

	// For. ���� GameObject�߰� 
	Add_GameObject(testObj);
}

void GranseedScene::Load_Light()
{
	// For. ���� �߰��� �� ������ �� GameObject���� 
	shared_ptr<GameObject> lightObj = make_shared<GameObject>();

	//// For. GameObject�� Transform ���� 
	lightObj->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(0.f, 25.f, 0.f, 1.f));
	lightObj->GetOrAddTransform()->Set_LookDir(_float3(-1.f, -1.f, -1.f));

	//// For. GameObject�� ���� LightComponent ����
	shared_ptr<Light> lightCom = make_shared<Light>();
	lightCom->Set_Diffuse(Color(1.f));
	lightCom->Set_Ambient(Color(0.8f));
	lightCom->Set_Specular(Color(0.f));
	lightCom->Set_Emissive(Color(0.f));
	lightCom->Set_LightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);

	//// For. GameObject�� Light Component ���̱� 
	lightObj->Add_Component(lightCom);

	//// For. ���� �� ������ �ϴ� GameObject �߰� 
	Add_GameObject(lightObj);
}

void GranseedScene::Load_Camera()
{
	/* UI�� �ƴ� �Ϲ� �𵨵��� �׸��� ī�޶� �����ؼ� ���� �߰��ϱ� */

	// For. ���� �߰��� Camera�� �� GameObject ���� 
	shared_ptr<GameObject> camera = make_shared<GameObject>();

	// For. ī�޶� Transform ���� 
	camera->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(0.f, 0.f, 0.f, 1.f));
	camera->GetOrAddTransform()->Set_Speed(5.f);

	// For. GameObject�� ���� ī�޶� ������Ʈ ���� 
	CameraDesc desc;
	desc.fFOV = XM_PI / 3.f;
	desc.strName = L"Default";
	desc.fSizeX = _float(g_iWinSizeX);
	desc.fSizeY = _float(g_iWinSizeY);
	desc.fNear = 0.1f;
	desc.fFar = 1000.f;
	shared_ptr<Camera> cameraComponent = make_shared<Camera>(desc);

	// For. ������ ī�޶� ������Ʈ�� GameObject�� ���̱� 
	camera->Add_Component(cameraComponent);

	// For. GameObject�� ī�޶� ������Ʈ�� � ������ ���� �����ϱ� 
	camera->Get_Camera()->Set_ProjType(ProjectionType::Perspective);

	// For. GameObject�� ī�޶� ������Ʈ�� UI�� �׸��� �ʵ��� ���� 
	camera->Get_Camera()->Set_CullingMaskLayerOnOff(Layer_UI, true);

	// For. GameObject�� key input�� ���� �̵��� ���� ��ũ��Ʈ ���̱� 
	camera->Add_Component(make_shared<FreeCameraScript>());

	// For. ���� ������ ī�޶� �߰� 
	Add_GameObject(camera);
}