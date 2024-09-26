#pragma once
#include "NPC_FSM.h"
class GranseedChildren_FSM :
    public NPC_FSM
{
public:
	enum class STATE
	{
		n_idle,
		n_run,
		n_walk,
		talk_01,
		Hide,
		Seek,
		Reset,
		NONE
	};
public:
	GranseedChildren_FSM();
	~GranseedChildren_FSM();


public:
	virtual HRESULT Init() override;
	virtual void Tick() override;
	virtual void Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget) override {}
	void InteractWithPlayer() override;

	virtual void Set_State(_uint iIndex) override;

private:
	virtual void State_Tick() override;
	virtual void State_Init() override;

	void	n_idle();
	void	n_idle_Init();
	void	talk_01();
	void	talk_Init();
	void	n_run();
	void	n_run_Init();
	void	n_walk();
	void	n_walk_Init();
	void	Hide();
	void	Hide_Init();
	void	Seek();
	void	Seek_Init();

	void	Reset();
	void	Reset_Init();


private:



private:
	STATE m_eCurState = STATE::n_idle;
	STATE m_ePreState = STATE::NONE;
};

