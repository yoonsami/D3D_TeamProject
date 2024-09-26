#pragma once

class VertexBuffer;

struct InstancingData
{
	_float4x4 world =_float4x4::Identity;
};

#define MAX_MESH_INSTANCE 500

class InstancingBuffer
{
public:
	InstancingBuffer();
	~InstancingBuffer();

public:
	void Clear_Data();
	void Add_Data(InstancingData& data);
	void Push_Data();

public:
	_uint						Get_Count()		{ return static_cast<_uint> (m_Data.size()); }
	shared_ptr<VertexBuffer>	Get_Buffer()	{ return m_pInstanceBuffer; }

private:
	void Create_Buffer(_uint maxCount = MAX_MESH_INSTANCE);

private:
	shared_ptr<VertexBuffer>	m_pInstanceBuffer;
	vector<InstancingData>		m_Data;
	_ulonglong					m_iInstanceId		= { 0 };
	_uint						m_iMaxCount			= { 0 };
};