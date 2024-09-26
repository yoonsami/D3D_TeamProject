#include "pch.h"
#include "SceneMgr.h"

void SceneMgr::Tick()
{
	if (m_pCurrScene == nullptr)
		return;


	

	m_pCurrScene->Tick();



}

void SceneMgr::Late_Tick()
{
	if (m_pCurrScene == nullptr)
		return;
	system_clock::time_point start_time = system_clock::now();
	m_pCurrScene->Late_Tick();
	system_clock::time_point end_time = system_clock::now();

	nanoseconds nano = end_time - start_time;

	if (KEYTAP(KEY_TYPE::F1))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F2))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F3))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F4))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F5))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F6))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F7))
		int a = 0;
}

void SceneMgr::Final_Tick()
{
	if (m_pCurrScene == nullptr)
		return;
	system_clock::time_point start_time = system_clock::now();
	m_pCurrScene->Final_Tick();
	system_clock::time_point end_time = system_clock::now();

	nanoseconds nano = end_time - start_time;

	if (KEYTAP(KEY_TYPE::F1))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F2))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F3))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F4))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F5))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F6))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F7))
		int a = 0;
}

void SceneMgr::Render()
{
	if (m_pCurrScene == nullptr)
		return;
	if (KEYTAP(KEY_TYPE::F1))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F2))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F3))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F4))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F5))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F6))
		int a = 0;
	if (KEYTAP(KEY_TYPE::F7))
		int a = 0;

	m_pCurrScene->Render();



}