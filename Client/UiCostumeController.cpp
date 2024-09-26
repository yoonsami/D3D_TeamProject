#include "pch.h"
#include "UiCostumeController.h" 

#include "Model.h"
#include "BaseUI.h"
#include "MeshRenderer.h"
#include "FontRenderer.h"
#include "MainUiController.h"
#include "ModelRenderer.h"
#include "ModelAnimator.h"
#include "InventoryModelRotation.h"
#include "WeaponScript.h"

UiCostumeController::UiCostumeController()
{
}

HRESULT UiCostumeController::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

    if (true == m_bIsInit)
        return S_OK;

    m_bIsInit = true;

    m_bIsHave.resize(6);
    m_bIsHave[0] = true;
    m_bIsHave[1] = true;
    m_bIsHave[2] = true;
    m_bIsHave[3] = false;
    m_bIsHave[4] = false;
    m_bIsHave[5] = false;

    m_vecHair.resize(6);
    m_vecUniform.resize(6);
    
    m_TempSet = m_OriginSet = pair(0, 0);

    // first : texture key, second : text
	{
		m_vecHair[0].first = L"Am_Ct_Hat_022";
		m_vecHair[0].second = L"보름달 대축제 헤어";

		m_vecHair[1].first = L"Am_Ct_Hat_020";
		m_vecHair[1].second = L"트로피컬 바캉스 헤어";

		m_vecHair[2].first = L"Am_ct_hat_009";
		m_vecHair[2].second = L"프라임 컬렉션 왕관";

		m_vecHair[3].first = L"Am_Ct_Hat_039";
		m_vecHair[3].second = L"월하미인 헤어";

		m_vecHair[4].first = L"Am_Ct_Hat_036";
		m_vecHair[4].second = L"쓰리테일 바캉스 모자";

		m_vecHair[5].first = L"Am_Ct_Hat_013";
		m_vecHair[5].second = L"해군 장교 모자";
	}


	{
		m_vecUniform[0].first = L"Am_Ct_Uniform_022";
		m_vecUniform[0].second = L"보름달 대축제 의상";

		m_vecUniform[1].first = L"Am_Ct_Uniform_020";
		m_vecUniform[1].second = L"트로피컬 바캉스 룩";

		m_vecUniform[2].first = L"Am_Light_Uniform_006";
		m_vecUniform[2].second = L"프라임 컬렉션 룩";

		m_vecUniform[3].first = L"Am_Ct_Uniform_039";
		m_vecUniform[3].second = L"월하미인 예복";

		m_vecUniform[4].first = L"Am_Ct_Uniform_037";
		m_vecUniform[4].second = L"쓰리테일 바캉스 룩";

		m_vecUniform[5].first = L"Am_Ct_Uniform_013";
		m_vecUniform[5].second = L"해군 장교 제복";
	}

    m_pController = CUR_SCENE->Get_UI(L"Main_UI_Controller");

    return S_OK;
}

void UiCostumeController::Tick()
{
	if (m_pOwner.expired())
		return;


}

void UiCostumeController::Create_Costume()
{
    if (true == m_bIsCreated)
        return;

    g_bIsCanRotation = false;

    m_TempSet = m_OriginSet;
    m_eType = COSTUME_TYPE::HAIR;

    m_bIsCreated = true;
    auto pScene = CUR_SCENE;
    pScene->Load_UIFile(L"..\\Resources\\UIData\\UI_Costume.dat", m_vecAddedObj);

    weak_ptr<GameObject> pE;
    _float4 vecPos = {};

    _uint iSize = IDX(m_vecAddedObj.size());
    for (_uint i = 0; i < iSize; ++i)
    {
        auto& pObj = m_vecAddedObj[i];
        if (true == pObj.expired())
            continue;

        wstring strName = pObj.lock()->Get_Name();
        if (L"UI_Costume_Exit_Button" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Remove_Costume();
                });
        }
        else if (L"UI_Costume_Hair_Button" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Change_Costume_Type(COSTUME_TYPE::HAIR);
                });
        }
        else if (L"UI_Costume_Uniform_Button" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Change_Costume_Type(COSTUME_TYPE::UNIFORM);
                });
        }
        else if (L"UI_Costume_Select" == strName)
        {
            pObj.lock()->Set_Render(false);
        }
        else if (L"UI_Costume_Inven_0" == strName)
        {
            if (0 == m_OriginSet.first)
                vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);

            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Select_Inven(0);
                });
        }
        else if (L"UI_Costume_Inven_1" == strName)
        {
            if (1 == m_OriginSet.first)
                vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);

            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Select_Inven(1);
                });
        }
        else if (L"UI_Costume_Inven_2" == strName)
        {
            if (2 == m_OriginSet.first)
                vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);

            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Select_Inven(2);
                });
        }
        else if (L"UI_Costume_Inven_3" == strName)
        {
            if (3 == m_OriginSet.first)
                vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);

            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Select_Inven(3);
                });
        }
        else if (L"UI_Costume_Inven_4" == strName)
        {
            if (4 == m_OriginSet.first)
                vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);

            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Select_Inven(4);
                });
        }
        else if (L"UI_Costume_Inven_5" == strName)
        {
            if (5 == m_OriginSet.first)
                vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);

            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Select_Inven(5);
                });
        }
        else if (L"UI_Costume_Set_Button" == strName)
        {
            pObj.lock()->Get_Button()->AddOnClickedEvent([this]()
                {
                    this->Change_Costume();
                });
        }
        else if (L"UI_Costume_E" == strName)
        {
            pE = pObj;
        }
        else if (L"UI_Costume_Inven_3_Have" == strName)
        {
            if (true == m_bIsHave[3])
                pObj.lock()->Get_FontRenderer()->Get_Text() = L"보유중";
        }
        else if (L"UI_Costume_Inven_4_Have" == strName)
        {
            if (true == m_bIsHave[4])
                pObj.lock()->Get_FontRenderer()->Get_Text() = L"보유중";
        }
        else if (L"UI_Costume_Inven_5_Have" == strName)
        {
            if (true == m_bIsHave[5])
                pObj.lock()->Get_FontRenderer()->Get_Text() = L"보유중";
        }
    }

    if (false == m_pController.expired())
        m_pController.lock()->Get_Script<MainUiController>()->Set_MainUI_Render(false);
    
    _float4 vecTemp = pE.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecTemp.x = vecPos.x + -95.f;
    vecTemp.y = vecPos.y + 50.f;
    pE.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecTemp);

    {
        shared_ptr<GameObject> garaModel = make_shared<GameObject>();
        garaModel->GetOrAddTransform()->Set_LookDir(_float3(0.f,0.f,-1.f));
		{
			shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
			shared_ptr<ModelAnimator> renderer = make_shared<ModelAnimator>(shader);
			{
				auto model = RESOURCES.Get<Model>(L"Player");
				renderer->Set_Model(model);
			}
			garaModel->Add_Component(renderer);
			garaModel->Add_Component(make_shared<InventoryModelRotation>());
			garaModel->Get_Animator()->Set_CurrentAnim(L"b_idle", true, 1.f);
			garaModel->Set_LayerIndex(Layer_AfterUI);
			garaModel->Set_Name(L"TestUIModel");
		}

        EVENTMGR.Create_Object(garaModel);
        {
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
				desc.pWeaponOwner = garaModel;

				ObjWeapon->Add_Component(make_shared<WeaponScript>(desc));
			}
			ObjWeapon->Set_Name(L"Weapon_Test_Model");
            ObjWeapon->Set_LayerIndex(Layer_AfterUI);
            EVENTMGR.Create_Object(ObjWeapon);
        }
    }
    {
        shared_ptr<GameObject> ui = make_shared<GameObject>();
        ui->GetOrAddTransform()->Set_State(Transform_State::POS,_float4(0.f,0.f,20.f,1.f));
        ui->GetOrAddTransform()->Scaled(_float3(1920.f,1080.f,1.f));
        shared_ptr<MeshRenderer> renderer = make_shared<MeshRenderer>(RESOURCES.Get<Shader>(L"Shader_Mesh.fx"));
        auto mesh = RESOURCES.Get<Mesh>(L"Point");
        renderer->Set_Mesh(mesh);

        auto material = make_shared<Material>();
        material->Set_TextureMap(RESOURCES.Get<Texture>(L"AFTER_UI_ToneMappingTarget"),TextureMapType::DIFFUSE);
        renderer->Set_Material(material);
        renderer->Set_Pass(19);
        ui->Add_Component(renderer);
        ui->Set_LayerIndex(Layer_UI);
        ui->Set_Name(L"Character_UI");
        EVENTMGR.Create_Object(ui);
    }

}

void UiCostumeController::Remove_Costume()
{
    if (false == m_bIsCreated)
        return;

    m_bIsCreated = false;
    g_bIsCanRotation = true;

    auto& pEventMgr = EVENTMGR;

    for (_uint i = 0; i < IDX(m_vecAddedObj.size()); ++i)
    {
        auto& pObj = m_vecAddedObj[i];
        if (false == pObj.expired())
        {
            pEventMgr.Delete_Object(pObj.lock());
            pObj.reset();
        }
    }

    m_vecAddedObj.clear();

    if (false == m_pController.expired())
        m_pController.lock()->Get_Script<MainUiController>()->Set_MainUI_Render(true);

    auto model = RESOURCES.Get<Model>(L"Player");
    if (!model)
        return;

    model->AddParts(m_vecHair[m_OriginSet.first].first, PARTS_INFO::Hair);
    model->AddParts(m_vecUniform[m_OriginSet.second].first, PARTS_INFO::Uniform);

    EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"TestUIModel"));
    EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"Weapon_Test_Model"));
    EVENTMGR.Delete_Object(CUR_SCENE->Get_UI(L"Character_UI"));
}

void UiCostumeController::Change_Costume_Type(COSTUME_TYPE eType)
{
    if (m_eType == eType)
        return;

    m_eType = eType;

    auto& pResource = RESOURCES;

    _uint iSize = IDX(m_vecAddedObj.size());

    _float4 vecTemp = {};

    weak_ptr<GameObject> pE;
    switch (m_eType)
    {
    case COSTUME_TYPE::HAIR:
        for (_uint i = 0; i < iSize; ++i)
        {
            auto& pObj = m_vecAddedObj[i];
            if (true == pObj.expired())
                continue;

            wstring strName = pObj.lock()->Get_Name();
            if (L"UI_Costume_Hair_Button" == strName)
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(L"UI_Costume_Hat1"), TextureMapType::DIFFUSE);
            else if (L"UI_Costume_Uniform_Button" == strName)
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(L"UI_Costume_Uniform0"), TextureMapType::DIFFUSE);

            else if (L"UI_Costume_Inven_0" == strName)
            {
                if (0 == m_OriginSet.first)
                    vecTemp = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(m_vecHair[0].first), TextureMapType::DIFFUSE);
            }
            else if (L"UI_Costume_Inven_1" == strName)
            {
                if (1 == m_OriginSet.first)
                    vecTemp = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(m_vecHair[1].first), TextureMapType::DIFFUSE);
            }
            else if (L"UI_Costume_Inven_2" == strName)
            {
                if (2 == m_OriginSet.first)
                    vecTemp = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(m_vecHair[2].first), TextureMapType::DIFFUSE);
            }
            else if (L"UI_Costume_Inven_3" == strName)
            {
                if (3 == m_OriginSet.first)
                    vecTemp = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(m_vecHair[3].first), TextureMapType::DIFFUSE);
            }
            else if (L"UI_Costume_Inven_4" == strName)
            {
                if (4 == m_OriginSet.first)
                    vecTemp = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(m_vecHair[4].first), TextureMapType::DIFFUSE);
            }
            else if (L"UI_Costume_Inven_5" == strName)
            {
                if (5 == m_OriginSet.first)
                    vecTemp = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(m_vecHair[5].first), TextureMapType::DIFFUSE);
            }

            else if (L"UI_Costume_Inven_0_Font" == strName)
            {
                _float4 vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                vecPos.x = 440.f;
                pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
                pObj.lock()->Get_FontRenderer()->Get_Text() = m_vecHair[0].second;
            }
            else if (L"UI_Costume_Inven_1_Font" == strName)
            {
                _float4 vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                vecPos.x = 665.f;
                pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
                pObj.lock()->Get_FontRenderer()->Get_Text() = m_vecHair[1].second;
            }
            else if (L"UI_Costume_Inven_2_Font" == strName)
            {
                _float4 vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                vecPos.x = 440.f;
                pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
                pObj.lock()->Get_FontRenderer()->Get_Text() = m_vecHair[2].second;
            }
            else if (L"UI_Costume_Inven_3_Font" == strName)
            {
                _float4 vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                vecPos.x = 700.f;
                pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
                pObj.lock()->Get_FontRenderer()->Get_Text() = m_vecHair[3].second;
            }
            else if (L"UI_Costume_Inven_4_Font" == strName)
            {
                _float4 vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                vecPos.x = 430.f;
                pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
                pObj.lock()->Get_FontRenderer()->Get_Text() = m_vecHair[4].second;
            }
            else if (L"UI_Costume_Inven_5_Font" == strName)
            {
                _float4 vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                vecPos.x = 700.f;
                pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
                pObj.lock()->Get_FontRenderer()->Get_Text() = m_vecHair[5].second;
            }
            else if (L"UI_Costume_E" == strName)
            {
                
                pE = pObj;
                
            }
            else if (L"UI_Costume_Select" == strName)
            {
                pObj.lock()->Set_Render(false);
            }
        }
        break;

    case COSTUME_TYPE::UNIFORM:
        for (_uint i = 0; i < iSize; ++i)
        {
            auto& pObj = m_vecAddedObj[i];
            if (true == pObj.expired())
                continue;

            wstring strName = pObj.lock()->Get_Name();
            if (L"UI_Costume_Hair_Button" == strName)
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(L"UI_Costume_Hat0"), TextureMapType::DIFFUSE);
            else if (L"UI_Costume_Uniform_Button" == strName)
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(L"UI_Costume_Uniform1"), TextureMapType::DIFFUSE);

            else if (L"UI_Costume_Inven_0" == strName)
            {
                if (0 == m_OriginSet.second)
                    vecTemp = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(m_vecUniform[0].first), TextureMapType::DIFFUSE);
            }
            else if (L"UI_Costume_Inven_1" == strName)
            {
                if (1 == m_OriginSet.second)
                    vecTemp = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(m_vecUniform[1].first), TextureMapType::DIFFUSE);
            }
            else if (L"UI_Costume_Inven_2" == strName)
            {
                if (2 == m_OriginSet.second)
                    vecTemp = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(m_vecUniform[2].first), TextureMapType::DIFFUSE);
            }
            else if (L"UI_Costume_Inven_3" == strName)
            {
                if (3 == m_OriginSet.second)
                    vecTemp = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(m_vecUniform[3].first), TextureMapType::DIFFUSE);
            }
            else if (L"UI_Costume_Inven_4" == strName)
            {
                if (4 == m_OriginSet.second)
                    vecTemp = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(m_vecUniform[4].first), TextureMapType::DIFFUSE);
            }
            else if (L"UI_Costume_Inven_5" == strName)
            {
                if (5 == m_OriginSet.second)
                    vecTemp = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                pObj.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(pResource.Get<Texture>(m_vecUniform[5].first), TextureMapType::DIFFUSE);
            }

            else if (L"UI_Costume_Inven_0_Font" == strName)
            {
                _float4 vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                vecPos.x = 440.f;
                pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
                pObj.lock()->Get_FontRenderer()->Get_Text() = m_vecUniform[0].second;
            }
            else if (L"UI_Costume_Inven_1_Font" == strName)
            {
                _float4 vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                vecPos.x = 675.f;
                pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
                pObj.lock()->Get_FontRenderer()->Get_Text() = m_vecUniform[1].second;
            }
            else if (L"UI_Costume_Inven_2_Font" == strName)
            {
                _float4 vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                vecPos.x = 450.f;
                pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
                pObj.lock()->Get_FontRenderer()->Get_Text() = m_vecUniform[2].second;
            }
            else if (L"UI_Costume_Inven_3_Font" == strName)
            {
                _float4 vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                vecPos.x = 700.f;
                pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
                pObj.lock()->Get_FontRenderer()->Get_Text() = m_vecUniform[3].second;
            }
            else if (L"UI_Costume_Inven_4_Font" == strName)
            {
                _float4 vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                vecPos.x = 440;
                pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
                pObj.lock()->Get_FontRenderer()->Get_Text() = m_vecUniform[4].second;
            }
            else if (L"UI_Costume_Inven_5_Font" == strName)
            {
                _float4 vecPos = pObj.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
                vecPos.x = 700;
                pObj.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
                pObj.lock()->Get_FontRenderer()->Get_Text() = m_vecUniform[5].second;
            }
            else if (L"UI_Costume_E" == strName)
            {
                pE = pObj;
            }
            else if (L"UI_Costume_Select" == strName)
            {
                pObj.lock()->Set_Render(false);
            }
        }
        break;
    }

    _float4 vecPos = pE.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x = vecTemp.x + -95.f;
    vecPos.y = vecTemp.y + 50.f;
    pE.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
}

void UiCostumeController::Change_Costume()
{
    if (m_OriginSet == m_TempSet)
        return;

    m_OriginSet = m_TempSet;
    auto model = RESOURCES.Get<Model>(L"Player");
    if (!model)
        return;

    model->AddParts(m_vecHair[m_OriginSet.first].first, PARTS_INFO::Hair);
    model->AddParts(m_vecUniform[m_OriginSet.second].first, PARTS_INFO::Uniform);

    wstring strTemp = L"UI_Costume_Inven_";
    if (COSTUME_TYPE::HAIR == m_eType)
        strTemp += to_wstring(m_OriginSet.first);
    else if(COSTUME_TYPE::UNIFORM == m_eType)
        strTemp += to_wstring(m_OriginSet.second);

    weak_ptr<GameObject> pE;
    weak_ptr<GameObject> pInven;
    for (_uint i = 0; i < IDX(m_vecAddedObj.size()); ++i)
    {
        if (true == m_vecAddedObj[i].expired())
            continue;

        if (strTemp == m_vecAddedObj[i].lock()->Get_Name())
            pInven = m_vecAddedObj[i];
        else if (L"UI_Costume_E" == m_vecAddedObj[i].lock()->Get_Name())
            pE = m_vecAddedObj[i];
    }

    _float4 vecPos = pInven.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos.x += -95.f;
    vecPos.y += 50.f;
    vecPos.z = 4.5f;
    pE.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos);
}

void UiCostumeController::Select_Inven(_uint iIndex)
{
    if (false == m_bIsHave[iIndex])
        return;

    weak_ptr<GameObject> pSelect;
    weak_ptr<GameObject> pInven;
    wstring strTemp = L"UI_Costume_Inven_" + to_wstring(iIndex);
    for (_uint i = 0; i < IDX(m_vecAddedObj.size()); ++i)
    {
        if (true == m_vecAddedObj[i].expired())
            continue;

        if (L"UI_Costume_Select" == m_vecAddedObj[i].lock()->Get_Name())
        {
            m_vecAddedObj[i].lock()->Set_Render(true);
            pSelect = m_vecAddedObj[i];
        }

        else if (strTemp == m_vecAddedObj[i].lock()->Get_Name())
        {
            m_vecAddedObj[i].lock()->Set_Render(true);
            pInven = m_vecAddedObj[i];
        }
    }

    _float4 vecPos1 = pInven.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    _float4 vecPos2 = pSelect.lock()->GetOrAddTransform()->Get_State(Transform_State::POS);
    vecPos2.x = vecPos1.x;
    vecPos2.y = vecPos1.y;
    pSelect.lock()->GetOrAddTransform()->Set_State(Transform_State::POS, vecPos2);

    if (COSTUME_TYPE::HAIR == m_eType)
    {
        if (m_TempSet.first == iIndex)
            return;

        m_TempSet.first = iIndex;
    }
    else if (COSTUME_TYPE::UNIFORM == m_eType)
    {
        if (m_TempSet.second == iIndex)
            return;

        m_TempSet.second = iIndex;
    }

    auto model = RESOURCES.Get<Model>(L"Player");
    if (!model)
        return;

    model->AddParts(m_vecHair[m_TempSet.first].first, PARTS_INFO::Hair);
    model->AddParts(m_vecUniform[m_TempSet.second].first, PARTS_INFO::Uniform);
}

void UiCostumeController::Change_Costume_Have(_uint iIndex)
{
    if (IDX(m_bIsHave.size()) <= iIndex)
        return;

    m_bIsHave[iIndex] = true;
}
