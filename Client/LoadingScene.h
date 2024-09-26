#pragma once
#include "Scene.h"

class Loader;

class LoadingScene :
    public Scene
{
public:
    LoadingScene(shared_ptr<Scene> nextScene);
    ~LoadingScene();

public:
    virtual void Init() override;
	virtual void Tick() override;
	virtual void Late_Tick() override;
    virtual void Final_Tick() override;
    void Set_StaticObjects(const list<shared_ptr<GameObject>>& objs);
    shared_ptr<GameObject> Get_StaticObjectFromLoader(const wstring& strTag);
    list<shared_ptr<GameObject>>& Get_StaticObjectsFromLoader() { return m_staticObjects; }

private:
    void Load_Ui();

private:
    shared_ptr<Scene> m_pNextScene = nullptr;
    shared_ptr<Loader> m_pLoader = nullptr;

	list<shared_ptr<GameObject>> m_staticObjects;
};

