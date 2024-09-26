#pragma once
#include "Client_FSM.h"
#include "ForwardMovingSkillScript.h"

class YeopoHorse_FSM :
	public Client_FSM
{
public:
	enum class STATE
	{
		n_idle,
		skill_400100_fx,
		skill_902100_fx,
		
		//Ride Horse
		SQ_RideHorse_Idle,
		SQ_RideHorse_Run,
		SQ_RideHorse_Stop,
		SQ_RideHorse_End,

		NONE
	};
public:
	YeopoHorse_FSM();
	~YeopoHorse_FSM();


public:
	virtual HRESULT Init() override;
	virtual void Tick() override;
	virtual void Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget) override;


private:
	virtual void State_Tick() override; // 상태를 항상 업데이트해줌
	virtual void State_Init() override; // 상태가 바뀔 때 한번 초기화 해줌
	virtual void Set_State(_uint iIndex) override;

	void n_idle();
	void n_idle_Init();
	void skill_400100_fx();
	void skill_400100_fx_Init();
	void skill_902100_fx();
	void skill_902100_fx_Init();

	void SQ_RideHorse_Idle();
	void SQ_RideHorse_Idle_Init();
	void SQ_RideHorse_Run();
	void SQ_RideHorse_Run_Init();
	void SQ_RideHorse_Stop();
	void SQ_RideHorse_Stop_Init();
	void SQ_RideHorse_End();
	void SQ_RideHorse_End_Init();
	

private:
	STATE m_eCurState = STATE::n_idle;
	STATE m_ePreState = STATE::NONE;
};

