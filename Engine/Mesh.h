#pragma once
#include "ResourceBase.h"

template<typename>
class Geometry;
class Mesh :
    public ResourceBase
{
public:
    Mesh();
    virtual ~Mesh();

public:
    void Create3DRect(vector<pair<_float3, _float3>> _LDURPairVector);
    void CreateGround(vector<pair<_float3, _float3>> _LURDPairVector);
    void CreateQuad();
    void CreateCube();
    void CreateGrid(_uint sizeX, _uint sizeZ);
    void CreateSphere();
    void CreatePoint();
    void Create_Buffer();

    shared_ptr<Geometry<VTXTEXNORTANDATA>> Get_Geometry() { return m_pGeometry; }
    shared_ptr<VertexBuffer> Get_VertexBuffer() { return m_pVB; }
    shared_ptr<IndexBuffer> Get_IndexBuffer() { return m_pIB; }

protected:
    shared_ptr<Geometry<VTXTEXNORTANDATA>> m_pGeometry;
    shared_ptr<VertexBuffer> m_pVB;
    shared_ptr<IndexBuffer> m_pIB;

};

