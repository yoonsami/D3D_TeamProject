#pragma once
#include "Client_FSM.h"
#include "ForwardMovingSkillScript.h"

class Yeopo_FSM :
	public Client_FSM
{
public:
	enum class STATE
	{
		b_idle,
		b_run_start,
		b_run,
		b_run_end_r,
		b_run_end_l,
		b_sprint,
		b_walk,
		die,
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
		skill_100200, // Skill 1-1  // Bubble 2
		skill_100300, // Skill 1-2
		skill_200100, // Skill 2    // Bubble X
		skill_300100, // Skill 3-1  // Bubble 4
		skill_300200, // Skill 3-2
		skill_300300, // Skill 3-3
		skill_300400, // Skill 3-4
		skill_400100, // Skill 4 (Ride RedHorse) // Bubble X
		skill_501100, // Skill 5  // Bubble X

		//Ride Horse
		SQ_RideHorse_Idle,
		SQ_RideHorse_Run,
		SQ_RideHorse_Stop,
		SQ_RideHorse_End,

		SQ_SpecialHero_Yeopo,

		NONE
	};
public:
	Yeopo_FSM();
	~Yeopo_FSM();


public:
	virtual HRESULT Init() override;
	virtual void Tick() override;
	virtual void Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget) override;


private:
	virtual void State_Tick() override; // 상태를 항상 업데이트해줌
	virtual void State_Init() override; // 상태가 바뀔 때 한번 초기화 해줌

	virtual void Set_State(_uint iIndex) override;

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

	void skill_100200();
	void skill_100200_Init();
	void skill_100300();
	void skill_100300_Init();

	void skill_200100();
	void skill_200100_Init();

	void skill_300100();
	void skill_300100_Init();
	void skill_300200();
	void skill_300200_Init();
	void skill_300300();
	void skill_300300_Init();
	void skill_300400();
	void skill_300400_Init();

	void skill_400100();
	void skill_400100_Init();

	void skill_501100();
	void skill_501100_Init();

	void SQ_RideHorse_Idle();
	void SQ_RideHorse_Idle_Init();
	void SQ_RideHorse_Run();
	void SQ_RideHorse_Run_Init();
	void SQ_RideHorse_Stop();
	void SQ_RideHorse_Stop_Init();
	void SQ_RideHorse_End();
	void SQ_RideHorse_End_Init();

	void RidingCoolCheck();
	void Create_Vehicle();
	void Create_Chain(const wstring& wstrModelTag, const wstring& wstrAnimTag, _float fWaitTime, _float fAnimSpeed, _float3 vScale, _float3 vPosOffset);
	void Set_VehicleState(_uint iAnimindex);

	void Use_Skill();
	void Use_Dash();
private:
	STATE m_eCurState = STATE::b_idle;
	STATE m_ePreState = STATE::NONE;

	COOLTIMEINFO m_tRidingEndDelay = { 0.4f,0.f };
	COOLTIMEINFO m_tRidingDelay = { 2.f,0.f };

	_bool m_bRidingCoolCheck = false;
	_float m_fRidingSpeed = 12.f;
	_float4 m_vRidingEndPos = _float4(0.f);
	_uint m_iTestCnt = 0;
};

