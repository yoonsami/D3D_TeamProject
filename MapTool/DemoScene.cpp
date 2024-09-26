#include "pch.h"
#include "DemoScene.h"
#include "ModelAnimator.h"
#include "ModelRenderer.h"
#include "Camera.h"
#include "Light.h"
#include "Model.h"
#include "DemoCameraScript1.h"
#include "DemoCameraScript2.h"
#include "DemoAnimationController1.h"
#include "DemoFSM.h"
#include "Imgui_Manager.h"
#include "WaterUVSliding.h"

GranseedScene::GranseedScene()
{
}

GranseedScene::~GranseedScene()
{
}

void GranseedScene::Init()
{
	__super::Init();
	COLLISION.Check_Group(_int(CollisionGroup::Player), _int(CollisionGroup::Monster));

	GET_SINGLE(ImGui_Manager).ImGui_SetUp();
	GET_SINGLE(ImGui_Manager).ImGui_Tick();
}

void GranseedScene::Tick()
{
	__super::Tick();

	GET_SINGLE(ImGui_Manager).ImGui_Tick();
	if (KEYHOLD(KEY_TYPE::LALT))
		INPUT.Set_Mouse_Move(false);
	else
		INPUT.Set_Mouse_Move(true);

}

void GranseedScene::Late_Tick()
{
	if (KEYTAP(KEY_TYPE::X))
	{
		if (m_bRenderDebug)
			m_bRenderDebug = false;
		else
			m_bRenderDebug = true;
	}

	__super::Late_Tick();
}

void GranseedScene::Final_Tick()
{
	__super::Final_Tick();
}

void GranseedScene::Render()
{
	__super::Render();

	GET_SINGLE(ImGui_Manager).ImGui_Render();
}

void GranseedScene::Render_SampleMapObject()
{
	if (Get_Camera(L"SampleCamera"))
	{
		shared_ptr<Camera> sampleCamera = Get_Camera(L"SampleCamera")->Get_Camera();
		Camera::S_View = sampleCamera->Get_ViewMat();
		Camera::S_Proj = sampleCamera->Get_ProjMat();

		sampleCamera->Render_Forward();
	}
}

HRESULT GranseedScene::Load_Scene()
{
	RESOURCES.CreateModel(L"..\\Resources\\Models\\MapObject\\Skybox\\");
	RESOURCES.CreateModel(L"..\\Resources\\Models\\MapObject\\Field\\");
	RESOURCES.CreateModel(L"..\\Resources\\Models\\Character\\Hero\\Spear_Ace\\");
	Load_DemoModel();
	shared_ptr<GameObject> sky = make_shared<GameObject>();
	sky->GetOrAddTransform();
	sky->Add_Component(make_shared<ModelRenderer>(RESOURCES.Get<Shader>(L"SkyBox.fx")));
	sky->Get_ModelRenderer()->Set_Model(RESOURCES.Get<Model>(L"SkyBox"));
	sky->Set_Name(L"SkyBase");
	sky->Add_Component(make_shared<WaterUVSliding>());
	Add_GameObject(sky);

	Load_Light();
	Load_Camera();

	return S_OK;
}

void GranseedScene::Load_DemoModel()
{
	{
		shared_ptr<GameObject> testObj = make_shared<GameObject>();
		// Transform Component

		testObj->Add_Component(make_shared<Transform>());
		//testObj->GetOrAddTransform();

		testObj->Get_Transform()->Set_State(Transform_State::POS, _float4(0.f, 0.f, 0.f, 1.f));
		{
			shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");

			shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(shader);
			{
				shared_ptr<Model> model = RESOURCES.Get<Model>(L"Spear_Ace");
				animator->Set_Model(model);
			}

			testObj->Add_Component(animator);
		}

		{
			//shared_ptr<SpearAce_FSM> fsm = make_shared<SpearAce_FSM>();
			//testObj->Add_Component(fsm);
		}
		testObj->Set_Name(L"Player");

		Add_GameObject(testObj);
	}
}

void GranseedScene::Load_Camera()
{
	{
		//카메라로 사용할 GameObject 생성
		shared_ptr<GameObject> camera = make_shared<GameObject>();

		// Transform Component 추가
		camera->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(0.f, 3.f, -3.f, 1.f));

		camera->GetOrAddTransform()->Set_Speed(5.f);

		// 카메라 Component 생성 
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
		//Layer_UI에 있는 오브젝트를 컬링하겠다.
		camera->Get_Camera()->Set_CullingMaskLayerOnOff(Layer_UI, true);

		// MonoBehaviour(Component 중 고정이 아닌것들) 추가
		// 일부러 기능 나눠놨음
		camera->Add_Component(make_shared<DemoCameraScript1>());
		camera->Add_Component(make_shared<DemoCameraScript2>());

		Add_GameObject(camera);
	}
	// 샘플오브젝트카메라
	{
		//카메라로 사용할 GameObject 생성
		shared_ptr<GameObject> camera = make_shared<GameObject>();

		// Transform Component 추가
		camera->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(0.f, -100.f + 3.f, -3.f, 1.f));

		// 카메라 Component 생성 
		CameraDesc desc;
		desc.fFOV = XM_PI / 3.f;
		desc.strName = L"SampleCamera";
		desc.fSizeX = _float(g_iWinSizeX * 0.2f);
		desc.fSizeY = _float(g_iWinSizeY * 0.2f);
		desc.fNear = 0.1f;
		desc.fFar = 1000.f;
		shared_ptr<Camera> cameraComponent = make_shared<Camera>(desc);
		camera->Add_Component(cameraComponent);

		camera->Get_Camera()->Set_ProjType(ProjectionType::Perspective);
		//스카이박스 컬링.
		camera->Get_Camera()->Set_CullingMaskLayerOnOff(Layer_Skybox, true);

		Add_GameObject(camera);
	}
}

void GranseedScene::Load_Light()
{
	shared_ptr<GameObject> lightObj = make_shared<GameObject>();
	lightObj->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(-10.f, 25.f, -10.f, 1.f));
	lightObj->GetOrAddTransform()->Set_LookDir(_float3(2.f,-4.f,3.f));
	{
		// LightComponent 생성 후 세팅
		shared_ptr<Light> lightCom = make_shared<Light>();
		//lightCom->Set_Diffuse(Color(1.f));
		//lightCom->Set_Ambient(Color(0.8f));
		//lightCom->Set_Specular(Color(0.f));
		//lightCom->Set_Emissive(Color(0.f));
		lightCom->Set_Ambient(Color(0.33f));
		lightCom->Set_Diffuse(Color(0.22f, 0.28f, 0.82f, 1.f));
		lightCom->Set_Specular(Color(1.f));
		lightCom->Set_Emissive(Color(1.f));
		lightCom->Set_LightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		lightObj->Add_Component(lightCom);
		
	}
	Add_GameObject(lightObj);
}
