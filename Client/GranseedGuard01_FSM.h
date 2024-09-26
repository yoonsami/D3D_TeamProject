#pragma once
#include "NPC_FSM.h"
class GranseedGuard01_FSM :
    public NPC_FSM
{
public:
	enum class STATE
	{
		b_idle_02,
		b_idle_03,
		b_idle_04,
		b_idle_05,
		b_idle_06,
		b_idle_07,
		b_idle_08,
		b_idle_09,
		n_idle,
		run,
		talk,
		walk,
		
		NONE
	};
public:
	GranseedGuard01_FSM();
	~GranseedGuard01_FSM();


public:
	virtual HRESULT Init() override;
	virtual void Tick() override;
	virtual void Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget) override {}
	void InteractWithPlayer() override;
private:
	virtual void State_Tick() override;
	virtual void State_Init() override;
	void	n_idle();
	void	n_idle_Init();
	void	run();
	void	run_Init();
	void	talk();
	void	talk_Init();
	void	walk();
	void	walk_Init();

private:

	virtual void Set_State(_uint iIndex) override;



private:
	STATE m_eCurState = STATE::n_idle;
	STATE m_ePreState = STATE::NONE;


};

