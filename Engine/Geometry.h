#pragma once

template<typename T>
class Geometry
{
public:
	Geometry() {

	}
	~Geometry() {
	
	}

public:
	_uint Get_VerticesCount()			{ return static_cast<_uint>(m_Vertices.size()); }
	void* Get_VertexData()				{ return m_Vertices.data(); }
	vector<T>& Get_Vertices()		{ return m_Vertices; }

	_uint Get_IndicesCount()			{ return static_cast<_uint>(m_Indices.size()); }
	void* Get_IndexData()				{ return m_Indices.data(); }
	vector<_uint>& Get_Indices()	{ return  m_Indices; }

	void Add_Vertex(const T& vertex) { m_Vertices.push_back(vertex); }
	void Add_Vertices(const vector<T>& vertices) { m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end()); }
	void Set_Vertices(const vector<T>& vertices) { m_Vertices = vertices; }

	void Add_Index(_uint index) { m_Indices.push_back(index); }
	void Add_Indices(const vector<_uint>& indices) { m_Indices.insert(m_Indices.end(), indices.begin(), indices.end()); }
	void Set_Indices(const vector<_uint>& indices) { m_Indices = indices; }

private:
	vector<T> m_Vertices;
	vector<_uint> m_Indices;
};
