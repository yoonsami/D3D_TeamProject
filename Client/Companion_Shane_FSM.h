#pragma once
#include "Companion_FSM.h"
#include "ForwardMovingSkillScript.h"

class Companion_Shane_FSM :
	public Companion_FSM
{
public:
	enum class STATE
	{
		talk_01,
		n_idle,
		b_idle,
		b_run_start,
		b_run,
		b_run_end_r,
		b_run_end_l,
		b_sprint,
		b_walk,
		die,
		stun,
		airborne_start, //airborne_start -> airborne_end -> airborne_up
		airborne_end,
		airborne_up, // airborne_end_up Animation = knock_end_up animation
		hit, //normal_hit
		knock_start, // knock_start -> knock_end -> knock_end_loop -> knock_up
		knock_end,
		knock_end_loop,
		knock_end_hit, // hit on knock_state
		knock_up,
		knockdown_start, //knockdown_start -> knockdown_end -> knock_up
		knockdown_end,
		skill_1100, //normal attack1
		skill_1200, //normal attack2
		skill_1300, //normal attack3
		skill_1400, //normal attack4

		skill_91100, //Evade
		skill_93100, //Backward_Evade

		skill_100100, // Skill 1-1 // Bubble 2
		skill_100200, // Skill 1-2 // 
		skill_200100, // Skill 2-1 // BUBBLE 2 
		skill_200200, // Skill 2-2
		skill_300100, // Skill 3  // Bubble X
		skill_500100, // Skill 4 // Bubble X
		skill_502100, // Skill 5 // Bubble X
		NONE
	};
public:
	Companion_Shane_FSM();
	~Companion_Shane_FSM();


public:
	virtual HRESULT Init() override;
	virtual void Tick() override;
	virtual void Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget) override;


private:
	virtual void State_Tick() override; // 상태를 항상 업데이트해줌
	virtual void State_Init() override; // 상태가 바뀔 때 한번 초기화 해줌
	virtual void Set_State(_uint iIndex) override;

	void talk_01();
	void talk_01_Init();
	void n_idle();
	void n_idle_Init();
	void b_idle();
	void b_idle_Init();
	void b_run_start();
	void b_run_start_Init();
	void b_run();
	void b_run_Init();
	void b_run_end_r();
	void b_run_end_r_Init();
	void b_run_end_l();
	void b_run_end_l_Init();
	void b_sprint();
	void b_sprint_Init();
	void b_walk();
	void b_walk_Init();
	void die();
	void die_Init();
	void stun();
	void stun_Init();

	void airborne_start();
	void airborne_start_Init();
	void airborne_end();
	void airborne_end_Init();
	void airborne_up();
	void airborne_up_Init();
	void hit();
	void hit_Init();
	void knock_start();
	void knock_start_Init();
	void knock_end();
	void knock_end_Init();
	void knock_end_loop();
	void knock_end_loop_Init();
	void knock_end_hit();
	void knock_end_hit_Init();
	void knock_up();
	void knock_up_Init();
	void knockdown_start();
	void knockdown_start_Init();
	void knockdown_end();
	void knockdown_end_Init();


	void skill_1100();
	void skill_1100_Init();
	void skill_1200();
	void skill_1200_Init();
	void skill_1300();
	void skill_1300_Init();
	void skill_1400();
	void skill_1400_Init();

	void skill_91100();
	void skill_91100_Init();
	void skill_93100();
	void skill_93100_Init();
	void skill_100100();
	void skill_100100_Init();
	void skill_100200();
	void skill_100200_Init();
	void skill_200100();
	void skill_200100_Init();
	void skill_200200();
	void skill_200200_Init();
	void skill_300100();
	void skill_300100_Init();

	void skill_500100();
	void skill_500100_Init();

	void skill_502100();
	void skill_502100_Init();

	void Set_AttackSkill();

	void Create_200100_Clone(_uint iCloneIndex);
	void StunSetting();
private:
	STATE m_eCurState = STATE::b_idle;
	STATE m_ePreState = STATE::NONE;
	STATE m_eCurSkillState = STATE::NONE;

	_uint m_iCloneIndex = 0;
};

