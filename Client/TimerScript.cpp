#include "pch.h"
#include "TimerScript.h"

TimerScript::TimerScript(_float fTime)
	: m_fTime(fTime)
{
}

void TimerScript::Tick()
{
	m_fAcc += fDT;
	if (m_fAcc >= m_fTime)
	{
		if (m_Func)
			m_Func();

		EVENTMGR.Delete_Object(Get_Owner());
	}
}
