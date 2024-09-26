#pragma once

class TimeMgr
{
	DECLARE_SINGLETON(TimeMgr)

public:
	void Initialize();
	void Tick();

	_uint GetFPS()					{ return m_iFps; }
	_double GetdoubleDeltaTime()	{ return m_dDeltaTime * _double(m_fTimeSpeedRatio); }
	_float GetfloatDeltaTime()		{ return _float(m_dDeltaTime * m_fTimeSpeedRatio); }
	_float GetAbsoluteTime()		{ return _float(m_dDeltaTime); }
	void Set_TimeSlow(_float fDuration, _float fRatio) { m_fSlowDuration = fDuration; m_fTimeSpeedRatio = fRatio; }

	_float Get_recipTimeRatio() { return 1.f / m_fTimeSpeedRatio; }

private:
	LARGE_INTEGER	m_liFrequency{};
	LARGE_INTEGER	m_liPrevCount{};
	LARGE_INTEGER	m_liCurrCount{};
	_double			m_dDeltaTime		= 0.;
	_double			m_dTimeScale		= 0.;

private:
	_float			m_fSlowDuration		= 0.f;
	_float			m_fAcc				= 0.f;
	_uint			m_iFrameCount		= 0;
	_double			m_dFrameTime		= 0.f;
	_uint			m_iFps				= 0;
	_float			m_fTimeSpeedRatio	= 1.f;
};

