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
		SQ_SpecialHero2,
		NONE,
	};
public:
	Gacha_FSM(const Gacha_FSM_Desc& desc);

public:
	virtual HRESULT Init() override;
	virtual void Tick() override;


private:
	virtual void State_Tick() override; // ���¸� �׻� ������Ʈ����
	virtual void State_Init() override; // ���°� �ٲ� �� �ѹ� �ʱ�ȭ ����

	virtual void Set_State(_uint iIndex) {};
	virtual void Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget, _uint iElementType) override{};

private:
	void SQ_SpecialHero();
	void SQ_SpecialHero_Init();
	void SQ_SpecialHero2();
	void SQ_SpecialHero2_Init();

private:
	STATE m_eCurState = STATE::SQ_SpecialHero;
	STATE m_ePreState = STATE::NONE;

private:
	Gacha_FSM_Desc m_Desc{};
	_float m_fAnimSpeed = 1.f;
	_float m_fAcc = 0.f;
	_float m_fDist = 5.f;
};

