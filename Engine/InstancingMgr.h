#pragma once
#include "InstancingBuffer.h"
class GameOjbect;

class InstancingMgr
{
	DECLARE_SINGLETON(InstancingMgr)

public:
	void Render(vector<shared_ptr<GameObject>>& gameObjects);

	void Render_Shadow(vector<shared_ptr<GameObject>>& gameObjects);
	void Render_MeshEffect(list <weak_ptr<GameObject>>& gameObjects);
	void Clear_Data();
	void Add_Data(InstanceID instanceId, InstancingData& data);

	shared_ptr<InstancingBuffer>& Get_Buffer(InstanceID id) 
	{ 
		return m_Buffers[id];
	}
private:
	//Render
	void Render_MeshRenderer(vector<shared_ptr<GameObject>>& gameObjects);
	void Render_ModelRenderer(vector<shared_ptr<GameObject>>& gameObjects);
	void Render_Animator(vector<shared_ptr<GameObject>>& gameObjects);
	void Render_Collider(vector<shared_ptr<GameObject>>& gameObjects);


	//RenderShadow
	void Render_ModelRenderer_Shadow(vector<shared_ptr<GameObject>>& gameObjects);
	void Render_Animator_Shadow(vector<shared_ptr<GameObject>>& gameObjects);
	
	//



private:
	map<InstanceID, shared_ptr<InstancingBuffer>> m_Buffers;
};

