#pragma once
#include "Scene.h"

class KrisScene :
	public Scene
{
public:
	KrisScene();
	~KrisScene();

public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Late_Tick() override;
	virtual void Final_Tick() override;

	virtual HRESULT Load_Scene() override;

private:
	shared_ptr<GameObject> Load_Player();

	void Load_Camera(shared_ptr<GameObject> pPlayer);
	void Load_Monster(_uint iCnt, const wstring& strMonsterTag, shared_ptr<GameObject> pPlayer, _bool bCharacterController = true);
	void Load_Boss_Dellons(shared_ptr<GameObject> pPlayer);

	void Load_Ui(shared_ptr<GameObject> pPlayer);
	void Load_Debug();

	void Load_EventScript();

};