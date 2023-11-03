#include "pch.h"
#include "LoadingScene.h"

#include "Loader.h"

#include "Camera.h"
#include "GameObject.h"
#include "MeshRenderer.h"

LoadingScene::LoadingScene(shared_ptr<Scene> nextScene)
	:m_pNextScene(nextScene)
{
}

void LoadingScene::Init()
{
	{
		// For. Create Camera (For Watch UI)
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

		// For. ������ Camera�� ���� �߰���
		Add_GameObject(camera);
	}

	// For. Create Background
	// Create_LoadingBG();

	// For. Create Loader 
	m_pLoader = make_shared<Loader>(m_pNextScene);
	(m_pLoader->Init());

	__super::Init();
}

void LoadingScene::Tick()
{
	__super::Tick();
}

void LoadingScene::Late_Tick()
{
	__super::Late_Tick();
}

void LoadingScene::Final_Tick()
{
	__super::Final_Tick();

	if (!m_pLoader->m_bLoadFinished)
		return;

	SCENE.Change_Scene(m_pNextScene);
}

void LoadingScene::Create_LoadingBG()
{
	/* Logo �̹����� �� GameObject �����ϱ� */
	auto LoadingBackGround = make_shared<GameObject>();
	LoadingBackGround->GetOrAddTransform()->Set_State(Transform_State::POS, _float4(0, 0, 2, 1));
	LoadingBackGround->GetOrAddTransform()->Scaled(_float3(g_iWinSizeX, g_iWinSizeY, 1.f));

	// For. GameObject�� ���� Renderer �����ϱ� 
	shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Mesh.fx"));

	// For. GameObject�� ���� Renderer�� �߰��� Quad �޽�(rect ����) �����ϱ�(Ŭ���ϱ�)
	auto mesh = RESOURCES.Get<Mesh>(L"Quad");	// Resource manager�� �⺻���� ������Ÿ�Ե��� ����ִٰ� �����ϸ��. Quad : Rect���۰�����.  Get���� �ϳ� Ŭ���ؿ´ٴ� ��. 
	
	// For. Renderer�� �׸������ϴ� Rect �޽� �������ֱ�
	renderer->Set_Mesh(mesh);					// �׸� ���� �������� �߰��ϱ� 

	// For. 
	auto material = make_shared<Material>();
	material->Set_TextureMap(RESOURCES.Load<Texture>(L"Test", L"../Resources/Textures/Test.dds"), TextureMapType::DIFFUSE);
	renderer->Get_RenderParamDesc().SetVec4(0, _float4(1));
	renderer->Set_Material(material);
	renderer->Set_Pass(MeshRenderer::PASS_INFO::Default_UI);

	// For. ������ ���� ������Ʈ�� ������ ������Ʈ�� ���̱� 
	LoadingBackGround->Add_Component(renderer);

	// For. ������ ���� ������Ʈ�� ���� ä��� ( Ȥ�� �ش� ���ӿ�����Ʈ�� �����´ٰų� ������ ���� �� ������ )
	LoadingBackGround->Set_Name(L"LoadingBG");
	LoadingBackGround->Set_LayerIndex(Layer_UI);
	LoadingBackGround->Set_Instancing(false);

	// For. ������ ���ӿ�����Ʈ�� ���� �߰��ϱ� 
	Add_GameObject(LoadingBackGround);
}

