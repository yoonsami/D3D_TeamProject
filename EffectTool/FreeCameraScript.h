#pragma once
#include "MonoBehaviour.h"

class FreeCameraScript final : public MonoBehaviour
{
public:
	virtual void	Tick() override;
	virtual shared_ptr<MonoBehaviour> Copy_Script()override { return make_shared<FreeCameraScript>(); }

private:
	_float			m_fRotationSpeed = XM_PI / 4.f;
};

