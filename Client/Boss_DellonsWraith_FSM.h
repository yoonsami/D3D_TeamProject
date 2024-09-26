#pragma once
#include "Client_FSM.h"
#include "ForwardMovingSkillScript.h"

class Boss_DellonsWraith_FSM :
	public Client_FSM
{
public:
	enum class STATE
	{
		FX_DellonsWraith_skill_30010, //Dellons Skill (300100)
		FX_DellonsWraith_skill_400100, //Dellons Skill 4 (400100)
		FX_DellonsWraith_skill_601100,
		FX_DellonsWraith_skill_903100,
		FX_DellonsWraith01_Special_01,
		FX_SQ_Dellons_QuestEnd_2,
		FX_Mn_Dellons_skill_5100, // Dellons Skill 4 (400100)
		FX_Mn_Dellons_skill_500200, // Dellons Skill 5 (501100)
		FX_Mn_Dellons_skill_5200,
		FX_Mn_Dellons_skill_6100,


		NONE
	};
public:
	Boss_DellonsWraith_FSM();
	~Boss_DellonsWraith_FSM();


public:
	virtual HRESULT Init() override;
	virtual void Tick() override;
	virtual void Get_Hit(const wstring& skillname,_float fDamage, shared_ptr<GameObject> pLookTarget) override;


private:
	virtual void State_Tick() override; // 상태를 항상 업데이트해줌
	virtual void State_Init() override; // 상태가 바뀔 때 한번 초기화 해줌
	virtual void Set_State(_uint iIndex) override;

	void FX_DellonsWraith_skill_30010();
	void FX_DellonsWraith_skill_30010_Init();
	void FX_DellonsWraith_skill_400100();
	void FX_DellonsWraith_skill_400100_Init();
	void FX_DellonsWraith_skill_601100();
	void FX_DellonsWraith_skill_601100_Init();
	void FX_DellonsWraith_skill_903100();
	void FX_DellonsWraith_skill_903100_Init();
	void FX_DellonsWraith01_Special_01();
	void FX_DellonsWraith01_Special_01_Init();
	void FX_SQ_Dellons_QuestEnd_2();
	void FX_SQ_Dellons_QuestEnd_2_Init();



	void FX_Mn_Dellons_skill_500200();
	void FX_Mn_Dellons_skill_500200_Init();
	void FX_Mn_Dellons_skill_5100();
	void FX_Mn_Dellons_skill_5100_Init();
	void FX_Mn_Dellons_skill_6100();
	void FX_Mn_Dellons_skill_6100_Init();
	void FX_Mn_Dellons_skill_5200();
	void FX_Mn_Dellons_skill_5200_Init();

public:
	void Remove_Wraith();

private:
	STATE m_eCurState = STATE::FX_DellonsWraith_skill_30010;
	STATE m_ePreState = STATE::NONE;
};

