#include "pch.h"
#include "MonoBehaviour.h"

MonoBehaviour::MonoBehaviour()
    :Component(COMPONENT_TYPE::Script)
{
}

MonoBehaviour::~MonoBehaviour()
{
}

HRESULT MonoBehaviour::Init()
{
    return S_OK;
}

void MonoBehaviour::Tick()
{
}

void MonoBehaviour::Late_Tick()
{
}

void MonoBehaviour::Final_Tick()
{
}
