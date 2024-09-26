#include "pch.h"
#include "MainScene.h"
#include "ModelAnimator.h"
#include "ModelRenderer.h"
#include "Camera.h"
#include "Light.h"
#include "Model.h"
#include "CameraScript1.h"
#include "CameraScript2.h"

#include "FileUtils.h"
#include "Utils.h"
#include "MeshRenderer.h"
#include "BaseUI.h"
#include <filesystem>
namespace fs = std::filesystem;

MainScene::MainScene()
{
}

MainScene::~MainScene()
{
}

void MainScene::Init()
{
	__super::Init();
	COLLISION.Check_Group(_int(CollisionGroup::Player), _int(CollisionGroup::Monster));
}

void MainScene::Tick()
{
	__super::Tick();
}

void MainScene::Late_Tick()
{
	__super::Late_Tick();
}

void MainScene::Final_Tick()
{
	__super::Final_Tick();
}

HRESULT MainScene::Load_Scene()
{
	//RESOURCES.CreateModel(L"..\\Resources\\Models\\");
	//Load_DemoModel();
	//Load_Light();
	Load_Camera();
   Load_UI_Texture();

	return S_OK;
}

void MainScene::Load_DemoModel()
{
	
}

void MainScene::Load_Camera()
{
   {
      shared_ptr<GameObject> camera = make_shared<GameObject>();
      camera->GetOrAddTransform();
      camera->GetOrAddTransform()->Set_Speed(5.f);
      CameraDesc desc;
      desc.fFOV = XM_PI / 3.f;
      desc.strName = L"UI";
      desc.fSizeX = _float(g_iWinSizeX);
      desc.fSizeY = _float(g_iWinSizeY);
      desc.fNear = 0.1f;
      desc.fFar = 1000.f;
      camera->Add_Component(make_shared<Camera>(desc));
      camera->Get_Camera()->Set_ProjType(ProjectionType::Orthographic);
      camera->Get_Camera()->Set_CullingMaskAll();
      camera->Get_Camera()->Set_CullingMaskLayerOnOff(Layer_UI, false);
      Add_GameObject(camera);
   }
}

void MainScene::Load_Light()
{
	shared_ptr<GameObject> lightObj = make_shared<GameObject>();
	lightObj->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(0.f, 25.f, 0.f, 1.f));
	lightObj->GetOrAddTransform()->Set_LookDir(_float3(-1.f,-1.f,-1.f));
	{
		shared_ptr<Light> lightCom = make_shared<Light>();
		lightCom->Set_Diffuse(Color(1.f));
		lightCom->Set_Ambient(Color(0.8f));
		lightCom->Set_Specular(Color(0.f));
		lightCom->Set_Emissive(Color(0.f));
		lightCom->Set_LightType(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		lightObj->Add_Component(lightCom);
		
	}

	Add_GameObject(lightObj);
}

void MainScene::Load_UI_Texture()
{
   wstring assetPath = L"..\\Resources\\Textures\\UITexture";

   for (auto& entry : fs::recursive_directory_iterator(assetPath))
   {
      if (entry.is_directory())
         continue;

      wstring filePath = entry.path().wstring();
      wstring fileName = entry.path().filename().wstring();
      Utils::DetachExt(fileName);
      RESOURCES.Load<Texture>(fileName, filePath);
   }
}
