#pragma once

enum class COMPONENT_TYPE : _uchar
{
	CharacterController,
	Transform,
	MeshRenderer,
	ModelRenderer,
	EffectRenderer,
	Camera,
	Animator,
	Light,
	Collider,
	FSM,
	Physics,
	BaseUI,
	RigidBody,
	Billboard,
	ParticleSystem,
	Particle,
	MeshEffect,
	GroupEffect,
	GroupEffectOwner,
	TrailRenderer,
	FontRenderer,
	EffectController,
	MotionTrailRenderer,
	DistortionRenderer,
	TerrainRenderer,
	Script,
	End,
};

enum
{
	FIXED_COMPONENT_COUNT = static_cast<_uchar>(COMPONENT_TYPE::End) - 1,
};

class GameObject;
class Transform;
class CharacterController;
class Component
{
public:
	Component(COMPONENT_TYPE eType);
	virtual ~Component();

public:
	virtual HRESULT Init()		{ return S_OK; };
	virtual void Tick()			{};
	virtual void Late_Tick()	{};
	virtual void Final_Tick()	{};
	virtual void Fixed_Tick()	{};

public:
	COMPONENT_TYPE Get_Type() { return m_eType; }

	shared_ptr<GameObject> Get_Owner() { return m_pOwner.lock(); }
	void Set_Owner(shared_ptr<GameObject> pOwner) { m_pOwner = pOwner; }

	shared_ptr<Transform> Get_Transform();
	shared_ptr<CharacterController> Get_CharacterController();
protected:
	friend class GameObject;
	weak_ptr<GameObject> m_pOwner;
	COMPONENT_TYPE m_eType = COMPONENT_TYPE::End;
};
