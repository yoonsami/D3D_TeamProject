#pragma once
#include "Client_FSM.h"

struct Gacha_FSM_Desc
{
	wstring strWeaponName = L"";
	wstring strAnimTag =L"";
	_uint iAnimStopFrame = 0;
};

class Gacha_FSM :
    public Client_FSM
{
	
	enum class STATE
	{
		SQ_SpecialHero,
		NONE,
	};
public:
	Gacha_FSM(const Gacha_FSM_Desc& desc);

public:
	virtual HRESULT Init() override;
	virtual void Tick() override;


private:
	virtual void State_Tick() override; // 상태를 항상 업데이트해줌
	virtual void State_Init() override; // 상태가 바뀔 때 한번 초기화 해줌

	virtual void Set_State(_uint iIndex) {};
	virtual void Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget) {};

private:
	void SQ_SpecialHero();
	void SQ_SpecialHero_Init();

private:
	STATE m_eCurState = STATE::SQ_SpecialHero;
	STATE m_ePreState = STATE::NONE;

private:
	Gacha_FSM_Desc m_Desc{};
	_float m_fAnimSpeed = 1.f;
	_float m_fAcc = 0.f;
};

