#pragma once

enum class HERO 
{ 
	PLAYER,
	ACE3,
	KYLE,
	YEOPO,
	DELLONS,
	SPIKE,
	YEONHEE,
	SHANE,
	MAX 
};

enum class DUMMY_HERO
{

};

enum class BOSS
{
	MIR,
	AXE,
	SPIKE,
	DELLONS,
	MAX
};

enum LEVEL
{ 
	LEVEL_STATIC,
	LEVEL_LOADING,
	LEVEL_LOGO,
	LEVEL_GAMEPLAY,
	LEVEL_END
};

enum ObjectType
{
	OBJ_PLAYER,
	OBJ_MONSTER,
	OBJ_NPC,
	OBJ_END
};

enum CollisionGroup
{
	Player_Body,
	Player_Attack,
	Player_Skill,

	Monster_Body,
	Monster_Attack,
	Monster_Skill,

	MAPObject,

	Script,

	END = MAX_COLLIDER_GROUP
};

enum SkillType
{
	SKILL1,		// 1�� ��ų
	SKILL2,		// 2�� ��ų
	SKILL3,		// 3�� ��ų
	SKILL4,		// 4�� ��ų
	SKILL5,		// �ó��� ��ų
	EVADE,		// ȸ��
	DEFAULT,	// �⺻ ����
	SkillEnd
};

enum ElementType
{
	DARK,
	EARTH,
	FIRE,
	LIGHT,
	WATER,
	WIND,
	ElementEnd
};

enum SkillInfo
{
	CHARGING,
	HOLDING,
	MOVING,
	NONE
};

enum class NPCTYPE
{
	QUEST,
	GACHA,
	MAX
};