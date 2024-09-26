#include "pch.h"
#include "UiGachaCardMove.h" 

#include "Utils.h"
#include "MeshRenderer.h"
#include "UiGachaController.h"
#include "UiCardDeckController.h"

_uint UiGachaCardMove::iHeroIndex = 0;

UiGachaCardMove::UiGachaCardMove(_uint iIndex, _bool bValue)
    : m_iIndex(iIndex)
	, m_bIsUnique(bValue)
{
}

HRESULT UiGachaCardMove::Init()
{
    if (m_pOwner.expired())
        return E_FAIL;

	m_fMaxTime = 0.5f;
	m_fCheckTime = 0.f;

	m_fOriginScale = { 272.f, 444.f, 0.f };
	m_fCurScale = m_fOriginScale;
	m_fScaleChangeValue = m_fOriginScale.x;
	m_fTrunPerTime = 2.1f;

	switch (m_iIndex)
	{
	case 0:
		m_vecFirstPos = _float4(80.f, -120.f, 5.f, 1.f);
		m_vecArrivalPos = _float4(-600.f, 230.f, 5.f, 1.f);
		break;
	case 1:
		m_vecFirstPos = _float4(60.f, -90.f, 5.1f, 1.f);
		m_vecArrivalPos = _float4(-300.f, 230.f, 5.1f, 1.f);
		break;

	case 2:
		m_vecFirstPos = _float4(40.f, -60.f, 5.2f, 1.f);
		m_vecArrivalPos = _float4(0.f, 230.f, 5.2f, 1.f);
		break;

	case 3:
		m_vecFirstPos = _float4(20.f, -30.f, 5.3f, 1.f);
		m_vecArrivalPos = _float4(300.f, 230.f, 5.3f, 1.f);
		break;

	case 4:
		m_vecFirstPos = _float4(0.f, 0.f, 5.4f, 1.f);
		m_vecArrivalPos = _float4(600.f, 230.f, 5.4f, 1.f);
		break;

	case 5:
		m_vecFirstPos = _float4(-20.f, 30.f, 5.5f, 1.f);
		m_vecArrivalPos = _float4(-600.f, -250.f, 5.5f, 1.f);
		break;

	case 6:
		m_vecFirstPos = _float4(-40.f, 60.f, 5.6f, 1.f);
		m_vecArrivalPos = _float4(-300.f, -250.f, 5.6f, 1.f);
		break;

	case 7:
		m_vecFirstPos = _float4(-60.f, 90.f, 5.7f, 1.f);
		m_vecArrivalPos = _float4(0.f, -250.f, 5.7f, 1.f);
		break;

	case 8:
		m_vecFirstPos = _float4(-80.f, 120.f, 5.8f, 1.f);
		m_vecArrivalPos = _float4(300.f, -250.f, 5.8f, 1.f);
		break;

	case 9:
		m_vecFirstPos = _float4(-100.f, 150.f, 5.9f, 1.f);
		m_vecArrivalPos = _float4(600.f, -250.f, 5.9f, 1.f);
		break;
	}
	m_vecDir = { m_vecArrivalPos.x - m_vecFirstPos.x,  m_vecArrivalPos.y - m_vecFirstPos.y };

	//_float fRand = Utils::Random_In_Range(0.f, 1.f);
	//fRand = 0.023f;
	//if (0.01f > fRand)
	//{
	//	m_eHero = HERO::ACE3;
	//	m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(L"UI_Gacha_Card_Back0"), TextureMapType::DIFFUSE);
	//}
	//else if (0.02f > fRand)
	//{
	//	m_eHero = HERO::DELLONS;
	//	m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(L"UI_Gacha_Card_Back1"), TextureMapType::DIFFUSE);
	//}
	//else if (0.03f > fRand)
	//{
	//	m_eHero = HERO::KYLE;
	//	m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(L"UI_Gacha_Card_Back3"), TextureMapType::DIFFUSE);
	//}
	//else if (0.04f > fRand)
	//{
	//	m_eHero = HERO::YEOPO;
	//	m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(L"UI_Gacha_Card_Back4"), TextureMapType::DIFFUSE);
	//}
	//else if (0.4f > fRand)
	//{
	//	m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(L"UI_Gacha_Card_Back2"), TextureMapType::DIFFUSE);
	//}
	//else
	//{
	//	m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(L"UI_Gacha_Card_Back5"), TextureMapType::DIFFUSE);
	//}

	if (true == m_bIsUnique)
	{
		switch (iHeroIndex++)
		{
		case 0:
			m_eHero = HERO::KYLE;
			break;
		case 1:
			m_eHero = HERO::YEOPO;
			break;
		case 2:
			m_eHero = HERO::YEONHEE;
			break;
		case 3:
			m_eHero = HERO::SHANE;
			break;
		default:
			m_eHero = HERO::KYLE;
			break;
		}
		
		m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(L"UI_Gacha_Card_Back0"), TextureMapType::DIFFUSE);

		if(HERO::MAX != m_eHero)
			m_strTextureTag = GET_DATA(m_eHero).KeyDeckSelect;
	}
	else
	{
		_uint iIndex = IDX(DUMMY_HERO::MAX);
		m_eDummyHero = static_cast<DUMMY_HERO>(rand() % iIndex);
		m_strTextureTag = GET_DATA(m_eDummyHero).KeyDeckSelect;
		m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(L"UI_Gacha_Card_Back5"), TextureMapType::DIFFUSE);
	}
	
	// HERO::PLAYER는 뽑는 목록에서 제외

    return S_OK;
}

void UiGachaCardMove::Tick()
{
	if (m_pOwner.expired())
		return;

	switch (m_eState)
	{
	case STATE::START:
		Move();
		break;
	case STATE::IDLE:
		Idle();
		break;
	case STATE::OPEN:
		Open();
		break;
	case STATE::END:
		End();
		break;
	}
}

void UiGachaCardMove::Card_Open()
{
	if (STATE::END != m_eState)
	{
		m_eState = STATE::OPEN;
		m_fCheckTime = 0.f;
	}
}

void UiGachaCardMove::Add_Data()
{
	if (true == m_bIsUnique && HERO::MAX != m_eHero)
		DATAMGR.Set_Card_Inven(m_eHero);
	else if (DUMMY_HERO::MAX != m_eDummyHero)
		DATAMGR.Set_Card_Inven(m_eDummyHero);
}

void UiGachaCardMove::Move()
{
	_float fTime = fDT;
	m_fCheckTime += fTime;
	if (m_fMaxTime < m_fCheckTime)
	{
		m_fCheckTime = m_fMaxTime;
		m_eState = STATE::IDLE;
		
		//return;
	}
	
	auto pTransform = m_pOwner.lock()->GetOrAddTransform();
	_float4 vecPos = m_vecFirstPos;
	vecPos.x += m_vecDir.x * m_fCheckTime / m_fMaxTime;
	vecPos.y += m_vecDir.y * m_fCheckTime / m_fMaxTime;

	pTransform->Set_State(Transform_State::POS, vecPos);

	_float3 vecScale = m_fOriginScale;
	switch (m_eChangeType)
	{
	case CHANGE_TYPE::DOWN:
		vecScale.x -= m_fScaleChangeValue * m_fCheckTime / m_fMaxTime * 2.f;
		break;
	case CHANGE_TYPE::UP:
		vecScale.x = 0.1f;
		vecScale.x += m_fScaleChangeValue * (m_fCheckTime - m_fMaxTime / 2.f) / (m_fMaxTime - m_fMaxTime/ 2.f);
		break;
	}

	if (0.01f > vecScale.x)
	{
		vecScale.x = 0.01f;
		m_eChangeType = CHANGE_TYPE::UP;
	}
	else if (272.f < vecScale.x)
	{
		vecScale.x = 272.f;
		m_eChangeType = CHANGE_TYPE::DOWN;
	}

	m_pOwner.lock()->GetOrAddTransform()->Scaled(vecScale);
}

void UiGachaCardMove::Idle()
{
	// 쉐이더 코드
	m_fCheckTime = 0.f;
}
	
void UiGachaCardMove::Open()
{
	/*switch (m_eChangeType)
	{
	case UiGachaCardMove::CHANGE_TYPE::DOWN:
		m_fCurScale.x -= m_fScaleChangeValue * fDT * m_fTrunPerTime / m_fMaxTime;
		break;
	case UiGachaCardMove::CHANGE_TYPE::UP:
		m_fCurScale.x += m_fScaleChangeValue * fDT * m_fTrunPerTime / m_fMaxTime;
		break;
	}

	if (0.01f > m_fCurScale.x)
	{
		m_fCurScale.x = 0.01f;
		m_eChangeType = CHANGE_TYPE::UP;
		if(0 != m_strTextureTag.length())
			m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(m_strTextureTag), TextureMapType::DIFFUSE);
	}
	else if (272.f < m_fCurScale.x)
	{
		m_fCurScale.x = 272.f;
		m_eState = STATE::IDLE;

		if(HERO::MAX != m_eHero)
			CUR_SCENE->Get_UI(L"UI_Card_Deck_Controller")->Get_Script<UiCardDeckController>()->Set_Hero(m_eHero);
	}

	m_pOwner.lock()->GetOrAddTransform()->Scaled(m_fCurScale);*/

	_float fTime = fDT;
	m_fCheckTime += fTime;
	if (m_fMaxTime * 1.1f < m_fCheckTime)
	{
		m_fCheckTime = m_fMaxTime;
		m_eState = STATE::END;
	
		m_pOwner.lock()->GetOrAddTransform()->Scaled(m_fOriginScale);
		return;
	}
	
	_float3 vecScale = m_fOriginScale;
	switch (m_eOpenType)
	{
	case CHANGE_TYPE::DOWN:
		vecScale.x -= m_fScaleChangeValue * m_fCheckTime / m_fMaxTime * 2.f;
		break;
	case CHANGE_TYPE::UP:
		vecScale.x = 0.1f;
		vecScale.x += m_fScaleChangeValue * (m_fCheckTime - m_fMaxTime / 2.f) / (m_fMaxTime - m_fMaxTime / 2.f);
		break;
	}

	if (0.01f > vecScale.x)
	{
		vecScale.x = 0.01f;
		m_eOpenType = CHANGE_TYPE::UP;
		if (0 != m_strTextureTag.length())
			m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(m_strTextureTag), TextureMapType::DIFFUSE);

		if (HERO::MAX != m_eHero)
			CUR_SCENE->Get_UI(L"UI_Gacha_Controller")->Get_Script<UiGachaController>()->Start_Effect(m_pOwner.lock()->GetOrAddTransform()->Get_State(Transform_State::POS), m_eHero);
	}
	else if (272.f < vecScale.x)
	{
		vecScale.x = 272.f;
		m_eState = STATE::END;

		//if (true == m_bIsUnique && HERO::MAX != m_eHero)
		//	DATAMGR.Set_Card_Inven(m_eHero);
		//else if(DUMMY_HERO::MAX != m_eDummyHero)
		//	DATAMGR.Set_Card_Inven(m_eDummyHero);
	}

	m_pOwner.lock()->GetOrAddTransform()->Scaled(vecScale);
}

void UiGachaCardMove::End()
{
	//m_eState = STATE::NONE;
	m_pOwner.lock()->GetOrAddTransform()->Scaled(m_fOriginScale);

	if (0 != m_strTextureTag.length())
		m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Set_TextureMap(RESOURCES.Get<Texture>(m_strTextureTag), TextureMapType::DIFFUSE);

	//wstring strName = m_pOwner.lock()->Get_MeshRenderer()->Get_Material()->Get_TextureMap(TextureMapType::DIFFUSE)->Get_Name();
	//if(m_strTextureTag == strName)
	//	m_eState = STATE::NONE;
}
