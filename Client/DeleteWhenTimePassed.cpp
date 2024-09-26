#include "pch.h"
#include "DeleteWhenTimePassed.h"

DeleteWhenTimePassed::DeleteWhenTimePassed(_float fTime)
	:m_fTime(fTime)
{
}

void DeleteWhenTimePassed::Final_Tick()
{
	m_fAcc += fDT;
	if (m_fAcc >= m_fTime)
		EVENTMGR.Delete_Object(Get_Owner());
}
