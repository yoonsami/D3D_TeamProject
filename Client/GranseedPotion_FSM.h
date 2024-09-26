#pragma once
#include "NPC_FSM.h"
class GranseedPotion_FSM :
    public NPC_FSM
{
public:
	enum class STATE
	{
		n_idle,
		talk,


		NONE
	};
public:
	GranseedPotion_FSM();
	~GranseedPotion_FSM();


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
	void	talk();
	void	talk_Init();


private:

	virtual void Set_State(_uint iIndex) override;



private:
	STATE m_eCurState = STATE::n_idle;
	STATE m_ePreState = STATE::NONE;
};

