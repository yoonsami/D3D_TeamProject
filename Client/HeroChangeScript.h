#pragma once
#include "MonoBehaviour.h"

class FSM;
class ModelAnimator;

class HeroChangeScript : public MonoBehaviour
{
public:
    HeroChangeScript();
    ~HeroChangeScript();

public:
    virtual HRESULT Init() override;
    virtual void Tick() override;
    
    void Change_Hero(HERO eHero);
    void Change_Player();
	virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<HeroChangeScript>(); }

private:
    void Change_Character_Weapon(const wstring& weaponname,shared_ptr<GameObject> weapon);
    void Change_To_Input(HERO eHero);

};

