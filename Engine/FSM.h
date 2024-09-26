#pragma once
#include "Component.h"

class FSM abstract : public Component
{
public:
	FSM();
	virtual void OnCollision(shared_ptr<BaseCollider> pCollider, _float fGap) = 0;
	virtual void OnCollisionEnter(shared_ptr<BaseCollider> pCollider, _float fGap) = 0;
	virtual void OnCollisionExit(shared_ptr<BaseCollider> pCollider, _float fGap) = 0;
	virtual void Set_State(_uint iIndex) = 0;
	virtual void Get_Hit(const wstring& skillname, _float fDamage, shared_ptr<GameObject> pLookTarget) = 0;
	shared_ptr<GameObject> Get_AttackCollider() { return m_pAttackCollider.lock(); }
	void Set_AttackCollider(shared_ptr<GameObject> attackCollider) { m_pAttackCollider = attackCollider; }
	void FreeLoopMembers();

	void		Add_Effect(const wstring& strSkilltag, shared_ptr<MonoBehaviour> pScript = nullptr, const _float4x4& matPivot = _float4x4::Identity, _bool bUseAsItis = false);
	void		Add_And_Set_Effect(const wstring& strSkilltag, shared_ptr<MonoBehaviour> pScript = nullptr);
	void		Add_GroupEffectOwner(const wstring& strSkilltag, _float3 vPosOffset, _bool usePosAs, shared_ptr<MonoBehaviour> pScript = nullptr);


protected:
	virtual void State_Tick() = 0;
	virtual void State_Init() = 0;

	void Calculate_CamBoneMatrix();

	_uint		Get_CurFrame();
	_uint		Get_FinalFrame();
	void		Reset_Frame();
	_bool		Is_AnimFinished();
	_float3		Get_InputDirVector(); // Ű���� �Է¿� ���� ī�޶� ���� �չ��� : +x, ������ ���� : +y�� ũ�� 1¥�� ���� ��ȯ
	void		Soft_Turn_ToInputDir(const _float3& vInputDir, _float turnSpeed);
	_float3		Soft_Turn_ToTarget(const _float3& vTargetPos, _float turnSpeed);
	_float3		Soft_Turn_ToTarget(const _float4& vTargetPos, _float turnSpeed);

	_bool		Get_SuperArmor() { return m_bSuperArmor; }
	void		Set_SuperArmor(_bool bFlag) { m_bSuperArmor = bFlag; }

	_bool Target_In_AttackRange(_float* pGap = nullptr);
	_bool LookingTarget_In_AttackRange(_float* pGap = nullptr);
	_bool Target_In_DetectRange();
	_bool TargetGroup_In_DetectRange(_uint eType);
	_bool Target_In_GazeCheckRange();

	_bool CounterAttackCheck(_float fCheckDegree);
	void Set_DirToTarget();
	void Set_DirToTargetOrInput(_uint eType);
	void Set_DirToTarget_Monster(_uint eType);
	void Look_DirToTarget(_float fTurnSpeed = XM_PI * 5.f);
	shared_ptr<GameObject> Find_TargetInFrustum(_uint eType, _bool bFrustumCheck = true);
	_bool Init_CurFrame(const _uint curFrame);

	void		KillAllEffect();
	void		Update_GroupEffectWorldPos(const _float4x4& mWorldMatrix);

	void		Add_FDistortion_Effect(const wstring& strSkilltag);
	void		Add_And_Set_FDistortion_Effect(const wstring& strSkilltag);
	void		Add_FDistortion_GroupEffectOwner(const wstring& strSkilltag, _float3 vPosOffset);
	void		Update_FDistortion_GroupEffectWorldPos();

	void		Cal_SkillCamDirection(const _float dist);
	_bool		Check_Combo(_uint minFrame, KEY_TYPE eKeyType);

	_bool		DeadCheck();
	void		Set_HitColor();
	void		Recovery_Color();



public:
	shared_ptr<GameObject> Get_Weapon() { return m_pWeapon.lock(); }
	void		Set_Target(shared_ptr<GameObject> pTarget);
	void		Set_Camera(shared_ptr<GameObject> pCamera);
	void		Set_Vehicle(shared_ptr<GameObject> pVehicle);
	void		Set_Weapon(shared_ptr<GameObject> pWeapon) { m_pWeapon = pWeapon; }
	void		Reset_Target();
	void		Reset_Weapon();
	void		Reset_Vehicle();
	void		Remove_Object();
	_float		Get_ChargingRatio() { return m_fChargingRatio; }

	_bool		Get_Invincible() { return m_bInvincible; }
	void		Set_Invincible(_bool bFlag) { m_bInvincible = bFlag; }

	_bool		Get_EntryTeam() { return m_bEntryTeam; }
	
 protected:
	weak_ptr<GameObject> m_pTarget;
	weak_ptr<GameObject> m_pAttackCollider;
	weak_ptr<GameObject> m_pSkillCollider;
	weak_ptr<GameObject> m_pWeapon;
	weak_ptr<GameObject> m_pCamera;
	weak_ptr<GameObject> m_pVehicle;

	weak_ptr<GameObject>			m_pLookingTarget;
	vector<weak_ptr<GameObject>>	m_vGroupEffect;

	//Frame Check
	_uint m_iPreFrame = 10000;
	_uint m_iCurFrame = 0;


	_bool m_bInitialize = false;
	_bool m_bInvincible = false;
	_bool m_bSuperArmor = false;
	_bool m_bCanCombo = false;
	_bool m_bIsDead = false;
	_bool m_bDetected = false;
	_bool m_bEntryTeam = false;

	_float3 m_vHitDir = _float3{ 0.f };
	_float3 m_vDirToTarget = _float3(0.f);

	_float m_fAttackRange = 2.f;
	_float m_fDetectRange = 2.f;
	_float m_fGazeCheckRange = 2.f;

	//For Camera CutScene
	_float4 m_vCamStopPos = _float4(0.f);
	_float4 m_vSkillCamRight = _float4(0.f);

	//BoneIndex ,BonePosition , BoneMatrix
	_uint m_iCenterBoneIndex = 0;
	_uint m_iDummy_CP_BoneIndex = 0;
	_uint m_iCamBoneIndex = 0;
	_uint m_iSkillCamBoneIndex = 0;
	_uint m_iSkillBoneIndex = 0;


	_float4 m_vCenterBonePos = _float4(0.f);
	_float4 m_vDummy_CP_BonePos = _float4(0.f);
	_float4 m_vCamBonePos = _float4(0.f);
	_float4 m_vSkillCamBonePos = _float4(0.f);


	_float4x4 m_CenterBoneMatrix = XMMatrixIdentity();
	_float4x4 m_Dummy_CP_BoneMatrix = XMMatrixIdentity();
	_float4x4 m_CamBoneMatrix = XMMatrixIdentity();
	_float4x4 m_SkillCamBoneMatrix = XMMatrixIdentity();

	_float4x4 matBoneMatrix = XMMatrixIdentity();

	//Speed For. Movement
	_float m_fRunSpeed = 6.f;
	_float m_fSprintSpeed = 8.f;
	_float m_fKnockBackSpeed = 6.f;
	_float m_fKnockDownSpeed = 6.f;

	//Speed For. Animation
	_float m_fNormalAttack_AnimationSpeed = 2.f;
	_float m_fSkillAttack_AnimationSpeed = 1.5f;
	_float m_fEvade_AnimationSpeed = 1.5f;
	_float m_fChargingRatio = 0.f;
	_float m_fCurrentHoldingSkillTime = 0.f;
	_float m_fMaxHoldingSkillTime = 0.f;
	_float m_fTimePerFrame = 0.f;

	//For Effect.Create
	_float m_fEffectCreateTimer[4] = { 0.f, 0.f, 0.f, 0.f };

	//MotionCoolTime
	COOLTIMEINFO m_tRunEndDelay = { 0.2f,0.f };
	COOLTIMEINFO m_tKnockDownEndCoolTime = { 2.f, 0.f };

	_float m_fColorRecoverySpeed = 3.f;
};

