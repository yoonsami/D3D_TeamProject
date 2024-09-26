#pragma once
#define DECLARE_SINGLETON(classname)		\
private:									\
	classname() { }							\
public:										\
	static classname& GetInstance()			\
	{										\
		static classname s_instance;		\
		return s_instance;					\
	}

#define GET_SINGLE(classname)	classname::GetInstance()
#define MSG_BOX(MESSAGE)		MessageBox(0, TEXT(MESSAGE), TEXT("System Error"), MB_OK)
#define VK_MAX 0xff

#define DEVICE					GRAPHICS.Get_Device()
#define CONTEXT					GRAPHICS.Get_Context()

#define GAMEINSTANCE			GET_SINGLE(GameInstance)
#define GRAPHICS				GET_SINGLE(Graphics)

#define INPUT					GET_SINGLE(InputMgr)
#define SOUND					GET_SINGLE(SoundMgr)
#define TIME					GET_SINGLE(TimeMgr)
#define COLLISION				GET_SINGLE(CollisionMgr)
#define PHYSX					GET_SINGLE(PhysxMgr)
#define RESOURCES				GET_SINGLE(ResourceMgr)
#define INSTANCING				GET_SINGLE(InstancingMgr)
#define SCENE					GET_SINGLE(SceneMgr)
#define EVENTMGR				GET_SINGLE(EventMgr)
#define CUR_SCENE				SCENE.Get_CurScene()

#define fDT						TIME.GetfloatDeltaTime()
#define dDT						TIME.GetdoubleDeltaTime()
#define fABT					TIME.GetAbsoluteTime()

#define KEYHOLD(keyType)		INPUT.GetButtonHold(keyType)
#define KEYTAP(keyType)			INPUT.GetButtonTap(keyType)
#define KEYAWAY(keyType)		INPUT.GetButtonAway(keyType)
#define KEYPUSH(keyType)		(KEYHOLD(keyType) || KEYTAP(keyType) )

#define SRV_MAP_COUNT 16

#define EPSILON 0.0001f

#define  CMP(value1,value2) fabsf((value1)-(value2)) < EPSILON

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define MAX_LEVEL_COUNT 32
#define SHADOWMAP_SIZE 8192.f