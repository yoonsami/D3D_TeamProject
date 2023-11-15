#pragma once
#include "FSM.h"

struct Gacha_FSM_Desc
{
	weak_ptr<GameObject> pWeapon;
	wstring strAnimTag =L"";
};

class Gacha_FSM :
    public FSM
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
	virtual void State_Tick() override; // ���¸� �׻� ������Ʈ����
	virtual void State_Init() override; // ���°� �ٲ� �� �ѹ� �ʱ�ȭ ����

private:
	void SQ_SpecialHero();
	void SQ_SpecialHero_Init();

private:
	STATE m_eCurState = STATE::SQ_SpecialHero;
	STATE m_ePreState = STATE::NONE;

private:
	Gacha_FSM_Desc m_Desc{};
};

