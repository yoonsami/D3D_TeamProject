#pragma once
#include "Scene.h"

class FieldScene :
	public Scene
{
public:
	FieldScene();
	~FieldScene();

public:
	virtual void Init() override;
	virtual void Tick() override;
	virtual void Late_Tick() override;
	virtual void Final_Tick() override;

	virtual HRESULT Load_Scene() override;
	virtual void Load_MapFile(const wstring& _mapFileName, shared_ptr<GameObject> pPlayer) override;
	void Load_Water();

private:
	shared_ptr<GameObject> Load_Player();
	void Load_Terrain();

	void Load_Camera(shared_ptr<GameObject> pPlayer);
	void Load_Monster(_uint iCnt, const wstring& strMonsterTag, shared_ptr<GameObject> pPlayer, _bool bCharacterController = true);
	void Load_Companion(const wstring& strCompanionTag, shared_ptr<GameObject> pPlayer, _float4 vSpawnPos);

	void Load_Ui(shared_ptr<GameObject> pPlayer);
	void Load_Debug();

};