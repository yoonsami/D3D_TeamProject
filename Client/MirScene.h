#pragma once
#include "Scene.h"

class MirScene :
	public Scene
{
public:
	MirScene();
	~MirScene();

public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Late_Tick() override;
	virtual void Final_Tick() override;

	virtual HRESULT Load_Scene() override;

private:
	shared_ptr<GameObject> Load_Player();

	void Load_Camera(shared_ptr<GameObject> pPlayer);
	void Load_Boss_Mir(shared_ptr<GameObject> pPlayer);
	void Load_Boss_Giant_Mir(shared_ptr<GameObject> pPlayer);
	void Load_Ui(shared_ptr<GameObject> pPlayer);
	
	void Load_Water();


};