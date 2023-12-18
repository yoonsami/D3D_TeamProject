#pragma once
#include "MonoBehaviour.h"
#include "Utils.h"
class WeedScript :
    public MonoBehaviour
{
public:
    virtual shared_ptr<MonoBehaviour> Copy_Script() { return nullptr; }

    void Set_WeedTypeIndex(_int _iWeedTypeIndex) { m_iWeedTypeIndex = _iWeedTypeIndex; }
    void Set_WeedGroupIndex(_int _iWeedGroupIndex) { m_iWeedGroupIndex = _iWeedGroupIndex; }
    _int Get_WeedTypeIndex() { return m_iWeedTypeIndex; }
    _int Get_WeedGroupIndex() { return m_iWeedGroupIndex; }

private:
    // Ǯ�� �𵨹�ȣ
    _int m_iWeedTypeIndex = 0;
    // Ǯ�� �׷��ȣ
    _int m_iWeedGroupIndex = -1;
};

