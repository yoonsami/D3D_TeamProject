#pragma once
#include "mesh.h"

template<typename>
class Geometry;

class Terrain :
    public Mesh
{
public:
    Terrain();
    virtual ~Terrain();
};