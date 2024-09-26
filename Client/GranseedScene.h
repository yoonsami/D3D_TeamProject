#pragma once
#include "Scene.h"

class GranseedScene :
	public Scene
{
public:
	GranseedScene();
	~GranseedScene();

public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Late_Tick() override;
	virtual void Final_Tick() override;

	virtual HRESULT Load_Scene() override;

private:
	shared_ptr<GameObject> Load_Player();

	void Load_Camera(shared_ptr<GameObject> pPlayer);

	void Load_Ui(shared_ptr<GameObject> pPlayer);
	void Load_Debug();

	void Load_NPC(const wstring& dataFileName);

	void Load_HideAndSeek(shared_ptr<GameObject> pPlayer);

};