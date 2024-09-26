#pragma once
#include "MonoBehaviour.h"
class Hide_OrctongScript :
    public MonoBehaviour
{
public:
    Hide_OrctongScript(_uint index) : m_iOrctongIndex(index) {}

public:
    virtual void Tick() override;
    virtual shared_ptr<MonoBehaviour> Copy_Script() override { return make_shared<Hide_OrctongScript>(m_iOrctongIndex); }



private:
    _uint m_iOrctongIndex = 0;
};

