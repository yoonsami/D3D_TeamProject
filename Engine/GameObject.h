#pragma once
#include "Component.h"
#include "MonoBehaviour.h"

class FSM;
class Model;
class Light;
class BaseUI;
class Camera;
class Physics;
class RigidBody;
class Transform;
class Billboard;
class BaseCollider;
class MeshRenderer;
class FontRenderer;
class TrailRenderer;
//class MonoBehaviour;
class ModelAnimator;
class ModelRenderer;
class EffectRenderer;
class ParticleSystem;
class TerrainRenderer;
class GroupEffectOwner;
class EffectController;
class DistortionRenderer;
class CharacterController;
class MotionTrailRenderer;

/* Effect */
class GroupEffect;
class MeshEffect;
class Particle;

enum LayerMask
{
	Layer_Default = 0,
	Layer_UI = 1,
	Layer_Skybox = 2,
	Layer_Effect = 3,
	Layer_AfterUI = 4,
};

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	virtual ~GameObject();

public:
	virtual HRESULT Init();
	virtual void Tick() ;
	virtual void Late_Tick() ;
	virtual void Final_Tick();
	virtual void Fixed_Tick();

public:
	virtual HRESULT Add_Component(shared_ptr<Component> component)		final;
	virtual HRESULT Change_Component(shared_ptr<Component> component)	final;

public:
	_float Get_MaxHp()						{ return m_fMaxHP; }
	_float& Get_CurHp()						{ return m_fHP; }
	_float Get_HpRatio()					{ return m_fHP / m_fMaxHP; }
	_float3& Get_CullPos()					{ return m_vCullPos; }
	_bool Get_Instancing()					{ return m_bInstancing; }
	_uint Get_ObjectGroup()					{ return m_eObjectGroup; }
	_float Get_CullRadius()					{ return m_fCullRaidus; }
	_uchar Get_LayerIndex()					{ return m_iLayerIndex; }
	const wstring& Get_Name()				{ return m_strName; }
	_float Get_CurGroggyGauge()				{ return m_fGroggy_Gauge; }
	_float Get_Groggy_Gauge_Ratio()			{ return m_fGroggy_Gauge / m_fMaxGroggy_Gauge; }

	_bool Is_Tick()							{ return m_bIsTick; }
	_bool Is_Blured()						{ return m_bIsBlur; }
	_bool Is_Render()						{ return m_bIsRender; }
	_bool Is_DrawShadow()					{ return m_bDrawShadow; }
	_bool Is_TimeSlowed()					{ return m_bGetTimeSlow; }
	_bool Is_DrawAfterUI()					{ return m_bDrawAfterUI;}
	_bool Has_VelocityMap()					{ return m_bHasVelocityMap; }
	_bool Is_FrustumCulled()				{ return m_bFrustumCull; }
	void Set_Hp(_float fHP)					{ m_fHP = fHP; }
	void Set_Tick(_bool bValue)				{ m_bIsTick = bValue; }
	void Set_Blur(_bool flag)				{ m_bIsBlur = flag; }
	void Set_Name(const wstring& name)		{ m_strName = name; }
	void Set_MaxHp(_float fHP)				{ m_fMaxHP = fHP; m_fHP = fHP; }
	void Set_Render(_bool bValue)			{ m_bIsRender = bValue; }
	void Set_CullPos(const _float3& pos )	{ m_vCullPos = pos; }
	void Set_TimeSlowed(_bool flag)			{ m_bGetTimeSlow = flag; }
	void Set_CullRadius(_float radius)		{ m_fCullRaidus = radius; }
	void Set_LayerIndex(_uchar layerIndex)	{ m_iLayerIndex = layerIndex; }
	void Set_Instancing(_bool flag)			{ m_bInstancing = flag; }
	void Set_DrawShadow(_bool flag)			{ m_bDrawShadow = flag; }
	void Set_ObjectGroup(_uint index)		{ m_eObjectGroup = index; }
	void Set_VelocityMap(_bool flag)		{ m_bHasVelocityMap = flag; }
	void Set_FrustumCulled(_bool flag)		{ m_bFrustumCull = flag; }
	void Set_GroggyGauge(_float fGroggyGauge) { m_fGroggy_Gauge = fGroggyGauge; }
	void Set_DrawAfterUI(_bool flag)		{ m_bDrawAfterUI = flag; }

	void DeleteComponent(COMPONENT_TYPE type) { m_Components[static_cast<_uint>(type)] = nullptr; }

	shared_ptr<FSM> Get_FSM();
	SHADER_TYPE Get_ShaderType();
	shared_ptr<Light> Get_Light();
	shared_ptr<Model> Get_Model();
	shared_ptr<BaseUI> Get_Button();
	shared_ptr<Camera> Get_Camera();
	//shared_ptr<Physics> Get_Physics();
	shared_ptr<Particle> Get_Particle();
	shared_ptr<RigidBody> Get_RigidBody();
	shared_ptr<Transform> Get_Transform();
	shared_ptr<MeshEffect> Get_MeshEffect();
	shared_ptr<BaseCollider> Get_Collider();
	//shared_ptr<Billboard> Get_Billboard();
	shared_ptr<ModelAnimator> Get_Animator();
	shared_ptr<Transform> GetOrAddTransform();
	shared_ptr<GroupEffect> Get_GroupEffect();
	shared_ptr<MeshRenderer> Get_MeshRenderer();
	shared_ptr<FontRenderer> Get_FontRenderer();
	shared_ptr<ModelRenderer> Get_ModelRenderer();
	shared_ptr<TrailRenderer> Get_TrailRenderer();
	shared_ptr<ParticleSystem> Get_ParticleSystem();
	shared_ptr<TerrainRenderer> Get_TerrainRenderer();
	//shared_ptr<EffectRenderer> Get_EffectRenderer();
	shared_ptr<GroupEffectOwner> Get_GroupEffectOwner();
	shared_ptr<DistortionRenderer> Get_DistortionRenderer();
	shared_ptr<CharacterController> Get_CharacterController();
	shared_ptr<MotionTrailRenderer> Get_MotionTrailRenderer();
	shared_ptr<Component> Get_FixedComponent(COMPONENT_TYPE type);
	vector<shared_ptr<MonoBehaviour>>& Get_Scripts() { return m_Scripts; }
	
	_bool Get_Hurt(_float fDamage)
	{
		m_fHP -= fDamage;
		if (m_fHP <= 0)
		{
			m_fHP = 0;
			return true;
		}
		else
			return false;
	}

	_bool Get_GroggyGauge(_float fGroggyGauge)
	{
		m_fGroggy_Gauge -= fGroggyGauge;
		if (m_fGroggy_Gauge <= 0)
		{
			m_fGroggy_Gauge = 0;
			return true;
		}
		else
			return false;
	}

	template<typename T>
	shared_ptr<T> Get_Script();

private:
	wstring m_strName = L"";
	array<shared_ptr<Component>, FIXED_COMPONENT_COUNT> m_Components;
	vector<shared_ptr<MonoBehaviour>> m_Scripts;

	_bool		m_bInstancing		= true;
	_bool		m_bIsBlur			= false;
	_bool		m_bHasVelocityMap	= false;
	_bool		m_bFrustumCull		= false;
	_bool		m_bDrawShadow		= false;
	_bool		m_bDrawAfterUI		= false;

	_uchar		m_iLayerIndex		= Layer_Default;
	_uint		m_eObjectGroup		= 0;

	_float		m_fCullRaidus		= 1.f;
	_float3		m_vCullPos			= { 0,0,0 };

	_float		m_fHP				= 100.f;
	_float		m_fMaxHP			= 100.f;

	_float		m_fGroggy_Gauge		= 100.f;
	_float		m_fMaxGroggy_Gauge	= 100.f;


	_bool		m_bIsRender			= { true };
	_bool		m_bIsTick			= { true };
	_bool		m_bGetTimeSlow		= true;
};

template<typename T>
inline shared_ptr<T> GameObject::Get_Script()
{
	for (auto& script : Get_Scripts())
	{
		if (typeid(*script.get()) == typeid(T))
			return static_pointer_cast<T>(script);
	}
	return nullptr;
}
