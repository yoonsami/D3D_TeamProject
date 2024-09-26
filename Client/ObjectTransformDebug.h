#pragma once
#include "MonoBehaviour.h"
class ObjectTransformDebug :
    public MonoBehaviour
{
public:
    virtual void Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<ObjectTransformDebug>(); }


public:
    void Set_Target(shared_ptr<GameObject> obj) { m_pTarget = obj; }
private:
    weak_ptr<GameObject> m_pTarget;
};

