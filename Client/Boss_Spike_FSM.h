#pragma once
#include "Client_FSM.h"
#include "ForwardMovingSkillScript.h"

class Boss_Spike_FSM :
	public Client_FSM
{
public:
	enum class STATE
	{
		SQ_Appear_01,
		SQ_Appear_02,
		SQ_Appear_03,
		Spawn,
		
		b_idle,
		b_run,
		
		gaze_b,
		gaze_f,
		gaze_l,
		gaze_r,

		SQ_Die,
		hit, //normal_hit
		hit_b,
		hit_f,
		hit_l,
		hit_r,
		groggy_start,
		groggy_end,
		groggy_loop,

		skill_1100, 
		skill_1200, 
		skill_1300, 
		skill_1400, 

		skill_9100, //Evade
		skill_9200, //Evade
		skill_9300, //Evade
		skill_9400, //Evade
		
		skill_2100, 
		skill_2200, 
		skill_3100, 
		skill_3200, 

		skill_6100,  // ULTIMATE
		skill_7100, 
		skill_8100, 

		skill_100000, 
		skill_100100, 
		skill_201100,  //CUTSCENE (DOWN)
		skill_201200,  //CUTSCENE (UP)


		NONE
	};
public:
	Boss_Spike_FSM();
	~Boss_Spike_FSM();


public:
	virtual HRESULT Init() override;
	virtual void Tick() override;
	virtual void Get_Hit(const wstring& skillname, _float fDamage,  shared_ptr<GameObject> pLookTarget) override;


private:
	virtual void State_Tick() override; // 상태를 항상 업데이트해줌
	virtual void State_Init() override; // 상태가 바뀔 때 한번 초기화 해줌
	virtual void Set_State(_uint iIndex) override;


	void SQ_Appear_01();
	void SQ_Appear_01_Init();
	void SQ_Appear_02();
	void SQ_Appear_02_Init();
	void SQ_Appear_03();
	void SQ_Appear_03_Init();
	void Spawn();
	void Spawn_Init();

	void b_idle();
	void b_idle_Init();
	void b_run();
	void b_run_Init();

	void gaze_b();
	void gaze_b_Init();
	void gaze_f();
	void gaze_f_Init();
	void gaze_l();
	void gaze_l_Init();
	void gaze_r();
	void gaze_r_Init();

	void SQ_Die();
	void SQ_Die_Init();

	
	void hit();
	void hit_Init();
	
	void groggy_start();
	void groggy_start_Init();
	void groggy_loop();
	void groggy_loop_Init();
	void groggy_end();
	void groggy_end_Init();


	void skill_1100();
	void skill_1100_Init();
	void skill_1200();
	void skill_1200_Init();
	void skill_1300();
	void skill_1300_Init();
	void skill_1400();
	void skill_1400_Init();

	//Evade Motion
	void skill_9100();
	void skill_9100_Init();
	void skill_9200();
	void skill_9200_Init();
	void skill_9300();
	void skill_9300_Init();
	void skill_9400();
	void skill_9400_Init();
	

	void skill_2100();
	void skill_2100_Init();
	void skill_2200();
	void skill_2200_Init();
	void skill_3100();
	void skill_3100_Init();
	void skill_3200();
	void skill_3200_Init();
	void skill_6100(); //Ulti
	void skill_6100_Init();
	void skill_7100();
	void skill_7100_Init();
	void skill_8100();
	void skill_8100_Init();

	//GroggyPattern
	void skill_100000();
	void skill_100000_Init();
	void skill_100100();
	void skill_100100_Init();

	//For. CutScene
	void skill_201100();
	void skill_201100_Init();
	void skill_201200();
	void skill_201200_Init();

	void Calculate_LipBoneMatrix();
	void Set_AttackSkill();
	void Create_BossSpikeChair();
	void Set_Gaze();
	void DeadSetting();
	void Create_CounterMotionTrail();

	_float3 Calculate_TargetTurnVector();
	_float CamDistanceLerp(_float fStart, _float fEnd, _float fRatio);

private:
	STATE m_eCurState = STATE::b_idle;
	STATE m_ePreState = STATE::NONE;
	STATE m_ePatternState = STATE::NONE;


	_float3 m_vTurnVector = _float3(0.f);
	_float4 m_vFirstPos = _float4(0.f);
	_float m_fTurnSpeed = XM_PI * 5.f;

	_float m_fDieCamDistance = 0.f;
	_float m_fCamRatio = 0.f;

	_float m_fGroggyStateAnimationSpeed = 1.f;
	_float m_fGroggyPatternAnimationSpeed = 0.15f;


	COOLTIMEINFO m_tAttackCoolTime = { 1.f, 0.f };
	COOLTIMEINFO m_tChaseCoolTime = { 2.f, 0.f };
	COOLTIMEINFO m_tSkillCoolTime = { 0.2f, 0.f };
	COOLTIMEINFO m_tGroggyPatternTimer = { 20.f, 0.f };


	_bool m_bChaseSkill = false;
	_bool m_bCounter = false;
	_bool m_bSetPattern = false;
	_bool m_bGroggyPattern = false;
	_bool m_bLastPattern = false;


	_uint m_iPreAttack = 100;
	
	
	_float4 m_vLastPatternCamStopPos = _float4(0.f);
	_float4 m_vPlayerBodyPos = _float4(0.f); //For. Intro 
	_float4 m_vChairPos = _float4(0.f); //For.Intro
	_float4 m_vDieCamStopPos = _float4(0.f); //For. DieCam
	_float4x4 m_DieCamWorld = XMMatrixIdentity(); //For. DieCam
	_float4x4 m_DieCamPlayerWorld = XMMatrixIdentity(); //For. DieCam
	
	_uint m_iLipBoneIndex = 0;
	_float4 m_vLipBonePos = _float4(0.f);
	_float4x4 m_LipBoneMatrix= XMMatrixIdentity();

	_uint m_iChairBoneIndex = 0;
	_float4 m_vChairBonePos = _float4(0.f);
	_float4x4 m_ChairBoneMatrix = XMMatrixIdentity();

};

