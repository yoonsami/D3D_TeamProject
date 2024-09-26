#include "pch.h"
#include "Widget_Model_Controller.h"
#include "ModelAnimation.h"
#include "ModelAnimator.h"
#include "Model.h"
#include "ModelRenderer.h"

using namespace ImGui;

bool VectorOfStringGetter(void* data, int n, const char** out_text)
{
	const vector<string>* v = (vector<string>*)data;
	*out_text = (v[0][n]).c_str();
	return true;
}
/* Components */
#include "MeshEffect.h"
#include "Texture.h"
#include "ForcePosition.h"
#include "WeaponScript.h"

Widget_Model_Controller::Widget_Model_Controller()
{
}

Widget_Model_Controller::~Widget_Model_Controller()
{
}

void Widget_Model_Controller::Initialize()
{
	Load_ModelNames();
}

void Widget_Model_Controller::Tick()
{
	m_pControlObject = CUR_SCENE->Get_GameObject(L"TestAnimModel");


	Begin("Model Controller");
	static _bool checkBox = true;
	Checkbox("SkyBox On", &checkBox);
	if(CUR_SCENE->Get_GameObject(L"SkyBase"))
	CUR_SCENE->Get_GameObject(L"SkyBase")->Get_ModelRenderer()->Set_RenderState(checkBox);
	Model_Controller();
	End();
}

void Widget_Model_Controller::Load_ModelNames()
{
	wstring assetPath = L"..\\Resources\\Models";
	for (auto& entry : fs::recursive_directory_iterator(assetPath))
	{
		if (entry.is_directory())
			continue;

		if (entry.path().extension().wstring() != L".Model" && entry.path().extension().wstring() != L".model")
			continue;

		wstring tag = entry.path().filename().wstring();
		Utils::DetachExt(tag);

		m_ModelNames.push_back(Utils::ToString(tag));
	}
}

void Widget_Model_Controller::Model_Controller()
{
	if (BeginTabBar("##Model Controller"))
	{
		if (BeginTabItem("Model Select"))
		{
			Show_ModelList();
			Apply_Model();

			EndTabItem();
		}

		if(!m_pControlObject.expired())
		{
			if (BeginTabItem("Animation Select"))
			{
				m_pControlObject.lock()->Get_Animator()->Set_AnimState(false);
				Show_AnimationList();
				Apply_Anim();
				EndTabItem();
			}
			if (BeginTabItem("Animation Control"))
			{
				m_pControlObject.lock()->Get_Animator()->Set_AnimState(true);
				Control_Anim();

				EndTabItem();
			}
		}
		EndTabBar();
	}
}

void Widget_Model_Controller::Show_ModelList()
{

	SeparatorText("Model List");
	Combo("##Model List", &m_iCurrentModelIndex, VectorOfStringGetter, &m_ModelNames, int(m_ModelNames.size()));
}

void Widget_Model_Controller::Apply_Model()
{
	if (Button("Select Model"))
	{
		if (m_pControlObject.expired())
		{
			shared_ptr<GameObject> obj = make_shared<GameObject>();
			obj->Set_Name(L"TestAnimModel");
			obj->GetOrAddTransform();
			shared_ptr<ModelAnimator> animator = make_shared<ModelAnimator>(RESOURCES.Get<Shader>(L"Shader_Model.fx"));
			obj->Add_Component(animator);
			EVENTMGR.Create_Object(obj);
			m_pControlObject = obj;
		}

		shared_ptr<Model> model = RESOURCES.Get<Model>(Utils::ToWString(m_ModelNames[m_iCurrentModelIndex]));
		if (Utils::ToWString(m_ModelNames[m_iCurrentModelIndex]) == L"Player")
		{
			model->AddParts(L"Am_Ct_Hat_022", PARTS_INFO::Hair);
			model->AddParts(L"Am_Head_001", PARTS_INFO::Head);
			model->AddParts(L"Am_Ct_Uniform_022", PARTS_INFO::Uniform);
		}
		
		m_pControlObject.lock()->Get_Animator()->Set_Model(model);
		m_pControlObject.lock()->Get_Animator()->Set_CurrentAnim(0);
		m_pControlObject.lock()->Add_Component(make_shared<ForcePosition>());

		EVENTMGR.Delete_Object(CUR_SCENE->Get_GameObject(L"TestWeapon"));

		wstring weaponName;
		if (model->Get_ModelTag() == L"Spear_Ace")
		{
			weaponName = L"Weapon_Spear_Ace";
			{
				shared_ptr<GameObject> ObjWeapon = make_shared<GameObject>();
				ObjWeapon->Set_Name(L"TestWeapon");
				ObjWeapon->GetOrAddTransform();
				shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
				shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
				shared_ptr<Model> m = RESOURCES.Get<Model>(weaponName);
				renderer->Set_Model(m);
				ObjWeapon->Add_Component(renderer);
				WeaponScript::WEAPONDESC desc;
				desc.strBoneName = L"Bip001-Prop1";
				desc.matPivot = _float4x4::CreateRotationX(-XM_PI / 2.f) * _float4x4::CreateRotationZ(XM_PI);
				desc.pWeaponOwner = m_pControlObject.lock();
				ObjWeapon->Add_Component(make_shared<WeaponScript>(desc));
				EVENTMGR.Create_Object(ObjWeapon);
			}
		}
		else if (model->Get_ModelTag() == (L"Yeopo"))
		{
			weaponName = L"Weapon_Yeopo";
			{
				shared_ptr<GameObject> ObjWeapon = make_shared<GameObject>();
				ObjWeapon->Set_Name(L"TestWeapon");
				ObjWeapon->GetOrAddTransform();
				shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
				shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
				shared_ptr<Model> m = RESOURCES.Get<Model>(weaponName);
				renderer->Set_Model(m);
				ObjWeapon->Add_Component(renderer);
				WeaponScript::WEAPONDESC desc;
				desc.strBoneName = L"Bip001-Prop1";
				desc.matPivot = _float4x4::CreateRotationX(-XM_PI / 2.f) * _float4x4::CreateRotationZ(XM_PI);
				desc.pWeaponOwner = m_pControlObject.lock();
				ObjWeapon->Add_Component(make_shared<WeaponScript>(desc));
				EVENTMGR.Create_Object(ObjWeapon);
			}
		}
		else if (model->Get_ModelTag() == (L"Dellons"))
		{
			weaponName = L"Weapon_Dellons";
			{
				shared_ptr<GameObject> ObjWeapon = make_shared<GameObject>();
				ObjWeapon->Set_Name(L"TestWeapon");
				ObjWeapon->GetOrAddTransform();
				shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
				shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
				shared_ptr<Model> m = RESOURCES.Get<Model>(weaponName);
				renderer->Set_Model(m);
				ObjWeapon->Add_Component(renderer);
				WeaponScript::WEAPONDESC desc;
				desc.strBoneName = L"Bip001-Prop1";
				desc.matPivot = _float4x4::CreateRotationX(-XM_PI / 2.f) * _float4x4::CreateRotationZ(XM_PI);
				desc.pWeaponOwner = m_pControlObject.lock();
				ObjWeapon->Add_Component(make_shared<WeaponScript>(desc));
				EVENTMGR.Create_Object(ObjWeapon);
			}
		}
		else if (model->Get_ModelTag() == (L"Spike"))
		{
			weaponName = L"Weapon_Spike";
			{
				shared_ptr<GameObject> ObjWeapon = make_shared<GameObject>();
				ObjWeapon->Set_Name(L"TestWeapon");
				ObjWeapon->GetOrAddTransform();
				shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
				shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
				shared_ptr<Model> m = RESOURCES.Get<Model>(weaponName);
				renderer->Set_Model(m);
				ObjWeapon->Add_Component(renderer);
				WeaponScript::WEAPONDESC desc;
				desc.strBoneName = L"Bip001-Prop1";
				desc.matPivot = _float4x4::CreateRotationX(-XM_PI / 2.f) * _float4x4::CreateRotationZ(XM_PI);
				desc.pWeaponOwner = m_pControlObject.lock();
				ObjWeapon->Add_Component(make_shared<WeaponScript>(desc));
				EVENTMGR.Create_Object(ObjWeapon);
			}
		}
		else if (model->Get_ModelTag() == (L"Shane"))
		{
			weaponName = L"Weapon_Shane";
			{
				shared_ptr<GameObject> ObjWeapon = make_shared<GameObject>();
				ObjWeapon->Set_Name(L"TestWeapon");
				ObjWeapon->GetOrAddTransform();
				shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
				shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
				shared_ptr<Model> m = RESOURCES.Get<Model>(weaponName);
				renderer->Set_Model(m);
				ObjWeapon->Add_Component(renderer);
				WeaponScript::WEAPONDESC desc;
				desc.strBoneName = L"Bip001-Prop1";
				desc.matPivot = _float4x4::CreateRotationX(-XM_PI / 2.f) * _float4x4::CreateRotationZ(XM_PI);
				desc.pWeaponOwner = m_pControlObject.lock();
				ObjWeapon->Add_Component(make_shared<WeaponScript>(desc));
				EVENTMGR.Create_Object(ObjWeapon);
			}
		}
		else if (model->Get_ModelTag() == (L"Player"))
		{
			weaponName = L"Weapon_Player";
			{
				shared_ptr<GameObject> ObjWeapon = make_shared<GameObject>();
				ObjWeapon->Set_Name(L"TestWeapon");
				ObjWeapon->GetOrAddTransform();
				shared_ptr<Shader> shader = RESOURCES.Get<Shader>(L"Shader_Model.fx");
				shared_ptr<ModelRenderer> renderer = make_shared<ModelRenderer>(shader);
				shared_ptr<Model> m = RESOURCES.Get<Model>(weaponName);
				renderer->Set_Model(m);
				ObjWeapon->Add_Component(renderer);
				WeaponScript::WEAPONDESC desc;
				desc.strBoneName = L"Bip001-Prop1";
				desc.matPivot = _float4x4::CreateRotationX(-XM_PI / 2.f) * _float4x4::CreateRotationZ(XM_PI);
				desc.pWeaponOwner = m_pControlObject.lock();
				ObjWeapon->Add_Component(make_shared<WeaponScript>(desc));
				EVENTMGR.Create_Object(ObjWeapon);
			}
		}


		
	
	}
}

void Widget_Model_Controller::Show_AnimationList()
{
	auto& animations = m_pControlObject.lock()->Get_Animator()->Get_Model()->Get_Animations();

	m_AnimNames.clear();
	m_AnimNames.resize(animations.size());

	for (_uint i = 0; i < static_cast<_uint>(animations.size()); ++i)
		m_AnimNames[i] = Utils::ToString(animations[i]->name);

	SeparatorText("Anim List");
	ListBox("##Anim List", &m_iCurrentAnimIndex, VectorOfStringGetter, &m_AnimNames, int(m_AnimNames.size()));

}

void Widget_Model_Controller::Apply_Anim()
{
	static _float fAnimSpeed = 1.f;
	static _bool bAnimLoop = false;
	static _bool bSetPosition = false;

	DragFloat("Anim Speed", &fAnimSpeed, 0.05f, 0.1f, 5.f);
	Checkbox("Anim Loop", &bAnimLoop);
	if (Button("Select Animation"))
		m_pControlObject.lock()->Get_Animator()->Set_CurrentAnim(Utils::ToWString(m_AnimNames[m_iCurrentAnimIndex]), bAnimLoop, fAnimSpeed);

	Checkbox("Position Force To Zero", &bSetPosition);
	m_pControlObject.lock()->Get_Script<ForcePosition>()->bForcePosition = bSetPosition;

	static _int startFrame = 0;
	InputInt("Start Frame", &startFrame);
	static _int endFrame = 0;
	InputInt("End Frame", &endFrame);
	auto animator = m_pControlObject.lock()->Get_Animator();

	auto& tweenDesc = animator->Get_TweenDesc();

	const auto& animation = animator->Get_Model()->Get_AnimationByIndex(tweenDesc.curr.animIndex);

	_float timePerFrame = 1 / (animation->frameRate * animation->speed);

	Text(("Time from StartFrame to EndFrame : " + to_string(timePerFrame * (endFrame - startFrame))).c_str());


}

void Widget_Model_Controller::Control_Anim()
{
	auto animator = m_pControlObject.lock()->Get_Animator();

	auto& tweenDesc = animator->Get_TweenDesc();
	
	const auto& animation = animator->Get_Model()->Get_AnimationByIndex(tweenDesc.curr.animIndex);

	static _float fKeyFrame = 0.f;

	DragFloat("## anim", &fKeyFrame, 0.1f, 0.f, _float(animation->frameCount - 1));

	_uint curFrame = _uint(fKeyFrame);
	_uint nextFrame = curFrame + 1;
	if (nextFrame >= animation->frameCount - 1) nextFrame = animation->frameCount - 1;

	tweenDesc.curr.currentFrame = curFrame;
	tweenDesc.curr.nextFrame = nextFrame;
	tweenDesc.curr.ratio = fKeyFrame - curFrame;
}
