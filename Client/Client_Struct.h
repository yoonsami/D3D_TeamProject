#pragma once

typedef struct tagSkillTotalInfo
{
	_bool			bIsWork				= false;
	_bool			bIsEnd				= false;
	SkillInfo		eSKillInfo			= SkillInfo::NONE;
	_uint			iSkillCombo			= 0;
	_uint			iCurCombo			= 0;
	COOLTIMEINFO	CoolInfo			= {};
}TOTALSKILLINFO;

typedef struct tagHeroData
{
	HERO			Hero				= HERO::MAX;
	ElementType		Element				= ElementType::ElementEnd;

	_uint			Star				= 1;
	_uint			Level				= 1;

	_uint			Skill0Combo			= 0;
	_uint			Skill1Combo			= 0;
	_uint			Skill2Combo			= 0;
	_uint			Skill3Combo			= 0;
	_uint			Skill4Combo			= 0;
	_uint			Skill5Combo			= 0;

	SkillInfo		Skill1Info			= SkillInfo::NONE;
	SkillInfo		Skill2Info			= SkillInfo::NONE;
	SkillInfo		Skill3Info			= SkillInfo::NONE;
	SkillInfo		Skill4Info			= SkillInfo::NONE;
	SkillInfo		Skill5Info			= SkillInfo::NONE;

	_float			AttackDamage		= 1.f;
	_float			MaxHp				= 10.f;
	_float			CurHp				= 10.f;
	_float			PreHp				= 10.f;
	_float			Deffence			= 1.f;
	_float			HpRegen				= 1.f;
	_float			CriPercent			= 0.f;
	_float			CriDamageMul		= 1.f;

	_float			Skill1Cool			= 0.f;
	_float			Skill2Cool			= 0.f;
	_float			Skill3Cool			= 0.f;
	_float			Skill4Cool			= 0.f;
	_float			Skill5Cool			= 0.f;

	_float			Skill1DamageMul		= 1.f;
	_float			Skill2DamageMul		= 1.f;
	_float			Skill3DamageMul		= 1.f;
	_float			Skill4DamageMul		= 1.f;
	_float			Skill5DamageMul		= 1.f;

	wstring			ModelTag;
	wstring			WeaponTag;
	wstring			KeyAttack;
	wstring			KeyEvade;
	wstring			KeyWeapon;
	wstring			KeySkill1			= L"Skill_None";
	wstring			KeySkill2			= L"Skill_None";
	wstring			KeySkill3			= L"Skill_None";
	wstring			KeySkill4			= L"Skill_None";
	wstring			KeySkill5			= L"Skill_None";
	wstring			KeyChangeCard;
	wstring			KeyDeckMini;
	wstring			KeyDeckSelect;
	wstring			KeyHeroInfo;
	wstring			KeyHeroName;
	wstring			SkillHelp;

}HERODATA;

typedef struct tagBossData
{
	ElementType		Element				= ElementType::ElementEnd;
	_bool			IsUseGroggy			= false;
	wstring			Name;

}BOSSDATA;

typedef struct tagQuestData
{
	_bool		IsClear					= false;
	QUESTTYPE	Type					= QUESTTYPE::MAX;

	_uint		CurCount				= 0;
	_uint		MaxCount				= 0;

	wstring		Name;
	wstring		Info;
	wstring		Clear;

}QUESTDATA;