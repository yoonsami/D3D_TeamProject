#pragma once
#include "ImGuiToolMgr.h"
#include "GroupEffectData.h"

class Widget_GroupEffectMaker 
{
public:
	typedef struct tagMemberEffectProperty
	{
		_float		m_fStartTime;

		_float		m_fPos[3];
		_float		m_fScale[3];
		_float		m_fRotation[3];
	}MemberEffectProperty_DESC;

public:
	Widget_GroupEffectMaker();
	~Widget_GroupEffectMaker();

public:
	void	Initialize();
	void	Tick();

private:
	void	Set_GroupList();
	void	Set_MemberEffectList();
	void	Set_MeshEffectList();
	void	Set_ParticleList();

	void	Widget_GroupMaker();
	void	Widget_GetTag();
	void	Widget_AddMeshEffect();
	void	Widget_AddParticle();

	void	Option_GroupList();
	void	Option_MemberEffectList();
	void	Option_Effect(string strEffectTag, _int iIndex);
	
	void	AddMemberEffect(const wstring& wstrTag, GroupEffectData::EFFECT_TYPE eType);
	void	Create();
	void	Delete();	// Delete GroupEffect GameObject in m_pCurrentGroup
	void	Save(const string& wstrNewGroupTag = ".");

private:
	/* Group List */
	_uint					m_iNumGroups = { 0 };
	vector<string>			m_vecGroups;
	const char**			m_pszGroups = { nullptr };
	_int					m_iGroup = { 0 };
	string					m_strGroup = { "None" };
	
	/* Mesh Effect list */
	_uint					m_iNumMeshEffects = { 0 };
	vector<string>			m_vecMeshEffects;
	const char**			m_pszMeshEffects = { nullptr };
	_int					m_iMeshEffect = { 0 };
	string					m_strMeshEffect = { "None" };

	/* Particle list */
	_uint					m_iNumParticles = { 0 };
	vector<string>			m_vecParticles;
	const char**			m_pszParticles = { nullptr };
	_int					m_iParticle = { 0 };
	string					m_strParticle = { "None" };

	/* Member Effect List */
	_uint					m_iNumMemberEffects = { 0 };
	vector<string>			m_vecMemberEffects;
	const char**			m_pszMemberEffects = { nullptr };
	_int					m_iMemberEffect = { 0 };

	/* new */
	char					m_szNewGroupEffectTag[MAX_PATH] = { "-" };

	/* Widget On Options */
	_bool					m_bWidgetOn_GetTag = { false };
	_bool					m_bWidgetOn_AddMeshEffect = { false };
	_bool					m_bWidgetOn_AddParticle = { false };

	/* Save something in current */
	shared_ptr<GameObject>		m_pCurrentGroup = { nullptr };
	MemberEffectProperty_DESC*	m_tCurrMemberProperty = { nullptr };

	/* Owner */
	shared_ptr<GameObject>		m_pOwner;
};

