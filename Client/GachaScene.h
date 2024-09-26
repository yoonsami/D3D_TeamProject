#pragma once
#include "Scene.h"

struct GachaSceneDesc
{
	wstring strMapFileName;
	HERO eHeroType;
};

class GachaScene :
    public Scene
{
public:
	GachaScene(const GachaSceneDesc& desc);
	~GachaScene();

public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Late_Tick() override;
	virtual void Final_Tick() override;

	virtual HRESULT Load_Scene() override;

private:
	shared_ptr<GameObject> Load_Player();

	void Load_Camera();
	void Load_Ui();
	void Load_Light();

private:
	GachaSceneDesc m_Desc{};

};