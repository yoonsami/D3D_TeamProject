#pragma once
struct tEvent
{
	EVENT_TYPE	eEvent;
	shared_ptr<GameObject>	lParam;
	DWORD_PTR	wParam;
};
class EventMgr
{
	DECLARE_SINGLETON(EventMgr)


public:
	void Tick();
	void Add_Event(const tEvent& event) { m_vecEvent.push_back(event); }
	void Create_Object(shared_ptr<GameObject> obj, _bool isStatic = false);
	void Delete_Object(shared_ptr<GameObject>obj);

public:

private:
	void Excute(const tEvent& event);


private:
private:
	vector<tEvent> m_vecEvent;
	vector<pair<shared_ptr<GameObject>,_bool>> m_ObjToAdd;
	vector<shared_ptr<GameObject>> m_ObjToDelete;
};

