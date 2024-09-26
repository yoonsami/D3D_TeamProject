#pragma once
#include "MonoBehaviour.h"

class UiMessageCreater : public MonoBehaviour
{
public:
    UiMessageCreater();

public:
    virtual HRESULT Init() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script() { return make_shared<UiMessageCreater>(); }

    void Create_Message(shared_ptr<GameObject> pTarget = nullptr);

private:
  

private:
  
};

