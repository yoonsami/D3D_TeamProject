#include "pch.h"
#include "EventMgr.h"

void EventMgr::Tick()
{
	for (auto& event : m_vecEvent)
		Excute(event);
	m_vecEvent.clear();

	for (auto& obj : m_ObjToAdd)
		CUR_SCENE->Add_GameObject(obj.first,obj.second);
	m_ObjToAdd.clear();

	for (auto& obj : m_ObjToDelete)
		CUR_SCENE->Remove_GameObject(obj);
	m_ObjToDelete.clear();
}

void EventMgr::Create_Object(shared_ptr<GameObject> obj, _bool isStatic)
{
	tEvent evn{ EVENT_TYPE::CREATE_OBJECT,obj,(DWORD_PTR)isStatic };
	Add_Event(evn);
}

void EventMgr::Delete_Object(shared_ptr<GameObject> obj)
{
	tEvent evn{ EVENT_TYPE::DELETE_OBJECT,obj,(DWORD_PTR)0 };
	Add_Event(evn);
}

void EventMgr::Excute(const tEvent& event)
{
	switch (event.eEvent)
	{
	case EVENT_TYPE::CREATE_OBJECT:
		m_ObjToAdd.push_back(make_pair(event.lParam,(_bool)event.wParam));
		break;

	case EVENT_TYPE::DELETE_OBJECT:
		m_ObjToDelete.push_back(event.lParam);
	
	break;
	default:
		break;
	}


}
