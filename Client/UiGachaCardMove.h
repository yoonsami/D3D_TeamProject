#pragma once
#include "MonoBehaviour.h"

class UiGachaCardMove : public MonoBehaviour
{
public:
    UiGachaCardMove(_uint iIndex, _bool bValue);

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<UiGachaCardMove>(m_iIndex,m_bIsUnique); }

    void Card_Open();
    void Add_Data();

private:
    void Move();
    void Idle();
    void Open();
    void End();

private:
    static _uint    iHeroIndex;

   enum class STATE { START, IDLE, OPEN, END, NONE };
   STATE            m_eState            = { STATE::START };

   _bool            m_bIsUnique         = { false };
   _uint            m_iIndex            = { 0 };

   _float           m_fMaxTime          = { 1.f };
   _float           m_fCheckTime        = { 0.f };
   
   enum class CHANGE_TYPE { DOWN, UP };
   CHANGE_TYPE      m_eChangeType    = { CHANGE_TYPE::DOWN };
   _float3          m_fOriginScale      = {};
   _float3          m_fCurScale         = {};
   _float           m_fScaleChangeValue = {};
   _float           m_fTrunPerTime      = {};

   _float           m_fTurnSpeed        = { 0.f };
   _float2          m_vecDir            = {};

   _float4          m_vecFirstPos       = {};
   _float4          m_vecArrivalPos     = {};

   // open 관련 변수
   CHANGE_TYPE      m_eOpenType         = { CHANGE_TYPE::DOWN };
   wstring          m_strTextureTag;
   HERO             m_eHero             = { HERO::MAX };

   DUMMY_HERO       m_eDummyHero        = { DUMMY_HERO::MAX };
};

