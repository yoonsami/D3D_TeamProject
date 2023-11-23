#pragma once
#include "Geometry.h"
#include "VertexData.h"
class GeometryHelper
{
public:

	static void CreateQuad(shared_ptr<Geometry<VTXCOLORDATA>> geometry, _float4 color);

	static void CreateQuad(shared_ptr<Geometry<VTXTEXDATA>> geometry);
	static void CreateCube(shared_ptr<Geometry<VTXTEXDATA>> geometry);
	static void CreateSphere(shared_ptr<Geometry<VTXTEXDATA>> geometry);
	static void CreateGrid(shared_ptr<Geometry<VTXTEXDATA>> geometry, _uint iSizeX, _uint iSizeZ);

	static void CreateQuad(shared_ptr<Geometry<VTXTEXNORDATA>> geometry);
	static void CreateCube(shared_ptr<Geometry<VTXTEXNORDATA>> geometry);
	static void CreateSphere(shared_ptr<Geometry<VTXTEXNORDATA>> geometry);
	static void CreateGrid(shared_ptr<Geometry<VTXTEXNORDATA>> geometry, _uint iSizeX, _uint iSizeZ);

	// �����(���ϴ� ���ϴ����) ���͸� ���� ���޽ø� �������
	static void Create3DRect(shared_ptr<Geometry<VTXTEXNORTANDATA>> _geometry, vector<pair<_float3, _float3>> _LDURPairVector);
	// �����(�»�� ���ϴ�) ���͸� ���� �簢�������޽�(�ٴ�)�� ���� 
	static void CreateGround(shared_ptr<Geometry<VTXTEXNORTANDATA>> _geometry, vector<pair<_float3,_float3>> _LURDPointsVector);
	static void CreateQuad(shared_ptr<Geometry<VTXTEXNORTANDATA>> geometry);
	static void CreateCube(shared_ptr<Geometry<VTXTEXNORTANDATA>> geometry);
	static void CreateSphere(shared_ptr<Geometry<VTXTEXNORTANDATA>> geometry);
	static void CreateGrid(shared_ptr<Geometry<VTXTEXNORTANDATA>> geometry, _uint iSizeX, _uint iSizeZ);

	static void CreatePoint(shared_ptr<Geometry<VTXTEXNORTANDATA>> geometry);
};

