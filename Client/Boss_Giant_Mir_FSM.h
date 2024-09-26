#pragma once
#include "Client_FSM.h"
#include "ForwardMovingSkillScript.h"
#include "InstallationSkill_Script.h"
#include "FloorSkill_Script.h"

class Boss_Giant_Mir_FSM :
	public Client_FSM
{
public:
	enum class STATE
	{
		SQ_Spawn,
		groggy_start,
		groggy_loop,
		groggy_end,
		SQ_Leave_Groggy_Start,
		SQ_Leave_Groggy_End,
		SQ_Leave,
		b_idle,
		skill_1100, // Breath
		skill_1200, // TailAttack
		skill_2100, // Breath
		skill_7100, //Summon DragonBall
		skill_8100, //Summon Meteor
		skill_100000, // Breath 
		skill_100100, // Breath 

		skill_200000, // Breath 
		skill_200100, // Breath 

		NONE
	};


public:
	virtual HRESULT Init() override;
	virtual void Tick() override;
	virtual void Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget) override;


private:
	virtual void State_Tick() override; // 상태를 항상 업데이트해줌
	virtual void State_Init() override; // 상태가 바뀔 때 한번 초기화 해줌

	virtual void Set_State(_uint iIndex) override;

	void SQ_Spawn();
	void SQ_Spawn_Init();

	void groggy_start();
	void groggy_start_Init();
	void groggy_loop();
	void groggy_loop_Init();
	void groggy_end();
	void groggy_end_Init();
	void SQ_Leave_Groggy_Start();
	void SQ_Leave_Groggy_Start_Init();
	void SQ_Leave_Groggy_End();
	void SQ_Leave_Groggy_End_Init();
	void SQ_Leave();
	void SQ_Leave_Init();
	
	void b_idle();
	void b_idle_Init();
	void skill_1100();
	void skill_1100_Init();
	void skill_1200();
	void skill_1200_Init();
	void skill_2100();
	void skill_2100_Init();
	void skill_7100();
	void skill_7100_Init();
	void skill_8100();
	void skill_8100_Init();
	void skill_100000();
	void skill_100000_Init();
	void skill_100100();
	void skill_100100_Init();

	void skill_200000();
	void skill_200000_Init();
	void skill_200100();
	void skill_200100_Init();

	void Create_Meteor();
	void Create_Giant_Mir_Collider();
	void Create_DragonBall();
	void Set_AttackPattern();
	void Calculate_IntroHeadCam();
	void Calculate_LeaveHeadCam();
	void Setting_DragonBall();
	void Destroy_MapObject();
	void Update_Collider();
	void DeadSetting();

	void Set_Invincible(_bool bFlag);
	void Set_RigidBodyActivate(_bool flag);

	void TailAttackCollider_On(const wstring& skillname, _float fAttackDamage);
	void TailAttackCollider_Off();
	_float CamDistanceLerp(_float fStart, _float fEnd, _float fRatio);

private:
	STATE m_eCurState = STATE::b_idle;
	STATE m_ePreState = STATE::NONE;

	_float m_fIntroCamDistance = 0.f;
	_float m_fCamRatio = 0.f;

	COOLTIMEINFO m_tAttackCoolTime = { 2.f, 0.f };
	COOLTIMEINFO m_tBreathCoolTime = { 0.15f, 0.f };
	COOLTIMEINFO m_tMeteorCoolTime = { 2.f, 0.f };
	

	_bool m_bDragonBall = false;
	_bool m_bSummonMeteor = false;

	_uint m_iPreAttack = 100;
	_uint m_iCurMeteorCnt = 0;
	_uint m_iLimitMeteorCnt = 0;

	
	_uint m_iHeadBoneIndex = 0;
	_uint m_iMouseBoneIndex = 0;
	_uint m_iTailBoneIndex = 0;
	_uint m_iStomachBoneIndex = 0;
	_uint m_iLfootBoneIndex = 0;
	

	weak_ptr<GameObject> m_pTailCollider;
	weak_ptr<GameObject> m_pStomachCollider;
	weak_ptr<GameObject> m_pLfootCollider;
	weak_ptr<GameObject> m_pFootRigidBody;


	_float3 m_vHeadCamDir = _float3(0.f);
	_float4 m_vHeadBonePos = _float4(0.f);
	_float4 m_vHeadCamPos = _float4(0.f);
	_float4 m_vStomachBonePos = _float4(0.f);
	_float4 m_vLfootBonePos = _float4(0.f);
	_float4 m_vIntroCamPos = _float4(0.f);
	_float4 m_vLeaveCamPos = _float4(0.f);
	_float4 m_vLeaveCamLookPos = _float4(0.f);
	_float4 m_vFirstPos = _float4(0.f);
	_float4 m_vSetPlayerPos = _float4(0.f);


	_float4x4 HeadBoneMatrix = XMMatrixIdentity();
	_float4x4 MouseBoneMatrix = XMMatrixIdentity();
	_float4x4 TailBoneMatrix = XMMatrixIdentity();
	_float4x4 StomachBoneMatrix = XMMatrixIdentity();
	_float4x4 LfootBoneMatrix = XMMatrixIdentity();

	vector<weak_ptr<GameObject>>	m_vecDestroyObject;
};