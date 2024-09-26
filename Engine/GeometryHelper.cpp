#include "pch.h"
#include "GeometryHelper.h"

void GeometryHelper::Create3DRect(shared_ptr<Geometry<VTXTEXNORTANDATA>> _geometry, vector<pair<_float3, _float3>> _LDURPairVector)
{
	vector<VTXTEXNORTANDATA> vtx;
	vtx.resize(_LDURPairVector.size() * 2 + 2 /*페어 두점추가 + 최초4점*/);
	
	for (_uint i = 0; i < _LDURPairVector.size(); ++i)
	{
		_float3 LDPos = _LDURPairVector[i].first;
		_float3 RUPos = _LDURPairVector[i].second;

		if(i==0)
		{
			vtx[0].vPosition = _float3{ LDPos.x, RUPos.y, LDPos.z };
			vtx[1].vPosition = LDPos;
			vtx[2].vPosition = RUPos;
			vtx[3].vPosition = _float3{ RUPos.x, LDPos.y, RUPos.z };
		}
		else
		{
			// 사각형의 우상, 우하 정점
			vtx[i * 2 + 2].vPosition = RUPos;
			vtx[i * 2 + 3].vPosition = _float3{ RUPos.x, LDPos.y, RUPos.z };
		}
	}

	_geometry->Set_Vertices(vtx);

	vector<_uint> idx;
	idx.resize(_LDURPairVector.size() * 6);

	for (_uint i = 0; i < _LDURPairVector.size(); ++i)
	{
		if(i == 0)
		{
			// 첫사각형
			idx[0] = 0;
			idx[1] = 2;
			idx[2] = 3;
			idx[3] = 0;
			idx[4] = 3;
			idx[5] = 1;
		}
		else
		{
			idx[i * 6] = i * 2;
			idx[i * 6 + 1] = i * 2 + 2;
			idx[i * 6 + 2] = i * 2 + 3;
			idx[i * 6 + 3] = i * 2;
			idx[i * 6 + 4] = i * 2 + 3;
			idx[i * 6 + 5] = i * 2 + 1;
		}
	}
	_geometry->Set_Indices(idx);
}

void GeometryHelper::CreateGround(shared_ptr<Geometry<VTXTEXNORTANDATA>> _geometry, vector<pair<_float3, _float3>> _LURDPointsVector)
{
	vector<VTXTEXNORTANDATA> vtx;
	vtx.resize(_LURDPointsVector.size() * 4);

	for (size_t i = 0; i < _LURDPointsVector.size(); ++i)
	{
		_float3 LUPos = _LURDPointsVector[i].first;
		_float3 RDPos = _LURDPointsVector[i].second;

		// 좌상단 우하단 점 두개를 갖고 사각형을 만듦
		vtx[i * 4].vPosition = LUPos;
		vtx[i * 4 + 1].vPosition = _float3{ RDPos.x, LUPos.y, LUPos.z };
		vtx[i * 4 + 2].vPosition = RDPos;
		vtx[i * 4 + 3].vPosition = _float3{ LUPos.x, RDPos.y, RDPos.z };
	}

	_geometry->Set_Vertices(vtx);

	vector<_uint> idx;
	idx.resize(_LURDPointsVector.size() * 6);

	for (_uint i = 0; i < (_uint)_LURDPointsVector.size(); ++i)
	{
			idx[i * 6] = i * 4;
			idx[i * 6 + 1] = i * 4 + 1;
			idx[i * 6 + 2] = i * 4 + 2;
			idx[i * 6 + 3] = i * 4;
			idx[i * 6 + 4] = i * 4 + 2;
			idx[i * 6 + 5] = i * 4 + 3;
	}
	_geometry->Set_Indices(idx);
}

void GeometryHelper::CreateQuad(shared_ptr<Geometry<VTXCOLORDATA>> geometry, _float4 color)
{
	vector<VTXCOLORDATA> vtx;
	vtx.resize(4);

	vtx[0].vPosition = _float3{ -0.5f, 0.5f, 0.f };
	vtx[0].vColor = color;
	vtx[1].vPosition = _float3{ 0.5f, 0.5f, 0.f };
	vtx[1].vColor = color;
	vtx[2].vPosition = _float3{ 0.5f, -0.5f, 0.f };
	vtx[2].vColor = color;
	vtx[3].vPosition = _float3{ -0.5f, -0.5f, 0.f };
	vtx[3].vColor = color;
	geometry->Set_Vertices(vtx);

	vector<_uint> idx = { 0,1,2,0,2,3 };
	geometry->Set_Indices(idx);
}

void GeometryHelper::CreateQuad(shared_ptr<Geometry<VTXTEXDATA>> geometry)
{
	vector<VTXTEXDATA> vtx;
	vtx.resize(4);

	vtx[0].vPosition = _float3{ -0.5f, 0.5f, 0.f };
	vtx[0].vTexCoord = _float2(0.f,0.f);
	vtx[1].vPosition = _float3{ 0.5f, 0.5f, 0.f };
	vtx[1].vTexCoord = _float2(1.f,0.f);
	vtx[2].vPosition = _float3{ 0.5f, -0.5f, 0.f };
	vtx[2].vTexCoord = _float2(1.f, 1.f);
	vtx[3].vPosition = _float3{ -0.5f, -0.5f, 0.f };
	vtx[3].vTexCoord = _float2(0.f, 1.f);
	geometry->Set_Vertices(vtx);

	vector<_uint> idx = { 0,1,2,0,2,3 };
	geometry->Set_Indices(idx);
}

void GeometryHelper::CreateCube(shared_ptr<Geometry<VTXTEXDATA>> geometry)
{
	vector<VTXTEXDATA> vtx(24);
	_float w2 = 1.f;
	_float h2 = 1.f;
	_float d2 = 1.f;
	_uint index = 0;
	//front
	vtx[index++] = VTXTEXDATA{ _float3(-w2,+h2,-d2),_float2(0.f,0.f) };
	vtx[index++] = VTXTEXDATA{ _float3(+w2,+h2,-d2),_float2(1.f,0.f) };
	vtx[index++] = VTXTEXDATA{ _float3(+w2,-h2,-d2),_float2(1.f,1.f) };
	vtx[index++] = VTXTEXDATA{ _float3(-w2,-h2,-d2),_float2(0.f,1.f) };

	//back
	vtx[index++] = VTXTEXDATA{ _float3(+w2,+h2,+d2),_float2(0.f,0.f) };
	vtx[index++] = VTXTEXDATA{ _float3(-w2,+h2,+d2),_float2(1.f,0.f) };
	vtx[index++] = VTXTEXDATA{ _float3(-w2,-h2,+d2),_float2(1.f,1.f) };
	vtx[index++] = VTXTEXDATA{ _float3(+w2,-h2,+d2),_float2(0.f,1.f) };

	// up
	vtx[index++] = VTXTEXDATA{ _float3(-w2,+h2,+d2),_float2(0.f,0.f) };
	vtx[index++] = VTXTEXDATA{ _float3(+w2,+h2,+d2),_float2(1.f,0.f) };
	vtx[index++] = VTXTEXDATA{ _float3(+w2,+h2,-d2),_float2(1.f,1.f) };
	vtx[index++] = VTXTEXDATA{ _float3(-w2,+h2,-d2),_float2(0.f,1.f) };

	//down
	vtx[index++] = VTXTEXDATA{ _float3(+w2,-h2,+d2),_float2(0.f,0.f) };
	vtx[index++] = VTXTEXDATA{ _float3(-w2,-h2,+d2),_float2(1.f,0.f) };
	vtx[index++] = VTXTEXDATA{ _float3(-w2,-h2,-d2),_float2(1.f,1.f) };
	vtx[index++] = VTXTEXDATA{ _float3(+w2,-h2,-d2),_float2(0.f,1.f) };

	//Left
	vtx[index++] = VTXTEXDATA{ _float3(-w2,+h2,+d2),_float2(0.f,0.f) };
	vtx[index++] = VTXTEXDATA{ _float3(-w2,+h2,-d2),_float2(1.f,0.f) };
	vtx[index++] = VTXTEXDATA{ _float3(-w2,-h2,-d2),_float2(1.f,1.f) };
	vtx[index++] = VTXTEXDATA{ _float3(-w2,-h2,+d2),_float2(0.f,1.f) };

	//Right
	vtx[index++] = VTXTEXDATA{ _float3(+w2,+h2,-d2),_float2(0.f,0.f) };
	vtx[index++] = VTXTEXDATA{ _float3(+w2,+h2,+d2),_float2(1.f,0.f) };
	vtx[index++] = VTXTEXDATA{ _float3(+w2,-h2,+d2),_float2(1.f,1.f) };
	vtx[index++] = VTXTEXDATA{ _float3(+w2,-h2,-d2),_float2(0.f,1.f) };
	geometry->Set_Vertices(vtx);

	index = 0;
	vector<_uint> idx(36);

	for (_uint i = 0; i < 6; ++i)
	{
		_uint startIndex = i * 4;
		idx[index++] = startIndex; 	idx[index++] = startIndex + 1; 	idx[index++] = startIndex + 2;
		idx[index++] = startIndex;	idx[index++] = startIndex + 2;	idx[index++] = startIndex + 3;
	}
	geometry->Set_Indices(idx);
}

void GeometryHelper::CreateSphere(shared_ptr<Geometry<VTXTEXDATA>> geometry)
{
	_float fRadius = 0.5f;
	_uint iStackCount = 20;
	_uint iSliceCount = 20;

	vector<VTXTEXDATA> vtx;

	VTXTEXDATA v;
	v.vPosition = _float3(0.f, fRadius, 0.f);
	v.vTexCoord = _float2(0.5f, 0.f);
	vtx.push_back(v);

	_float fStackAngle = XM_PI / iStackCount;
	_float fSliceAngle = XM_2PI / iSliceCount;

	_float fDeltaU = 1.f / static_cast<_float>(iSliceCount);
	_float fDeltaV = 1.f / static_cast<_float>(iStackCount);

	for (_uint y = 1; y <= iStackCount - 1; ++y)
	{
		_float pi = y * fStackAngle;

		for (_uint x = 0; x <= iSliceCount; ++x)
		{
			_float theta = x * fSliceAngle;

			v.vPosition.x = fRadius * sinf(pi) * cosf(theta);
			v.vPosition.y = fRadius * cosf(pi);
			v.vPosition.z = fRadius * sinf(pi) * sinf(theta);

			v.vTexCoord = _float2(fDeltaU * x, fDeltaV * y);
			vtx.push_back(v);
		}
	}

	v.vPosition = _float3(0.f, -fRadius, 0.f);
	v.vTexCoord = _float2(0.5f, 1.f);
	vtx.push_back(v);

	geometry->Set_Vertices(vtx);

	vector<_uint> idx;

	for (_uint i = 0; i <= iSliceCount; ++i)
	{
		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	_uint iRingVtxCount = iSliceCount + 1;

	for (_uint y = 0; y < iStackCount - 2; ++y)
	{
		for (_uint x = 0; x < iSliceCount; ++x)
		{
			// y,x - y, x+1
			// l      /
			// y+1, x
			idx.push_back(1 + (y) * iRingVtxCount + (x));
			idx.push_back(1 + (y) * iRingVtxCount + (x+1));
			idx.push_back(1 + (y+1) * iRingVtxCount + (x));

			//    y, x+1
			// /	  l  
			// y+1,x - y+1,x+1

			idx.push_back(1 + (y+1)*iRingVtxCount + (x));
			idx.push_back(1 + (y)*iRingVtxCount + (x + 1));
			idx.push_back(1 + (y + 1) * iRingVtxCount + (x+1));
		}
	}

	_uint iBottomIndex = static_cast<_uint>(vtx.size()) - 1;
	_uint iLastRingStartIndex = iBottomIndex - iRingVtxCount;
	for (_uint i = 0; i < iSliceCount; ++i)
	{
		//last+i - last+i +1
		// bottom
		idx.push_back(iBottomIndex);
		idx.push_back(iLastRingStartIndex + i);
		idx.push_back(iLastRingStartIndex + i + 1);
	
	}

	geometry->Set_Indices(idx);
}

void GeometryHelper::CreateGrid(shared_ptr<Geometry<VTXTEXDATA>> geometry, _uint iSizeX, _uint iSizeZ)
{
	vector<VTXTEXDATA> vtx;

	for (_uint z = 0; z < iSizeZ + 1; ++z)
	{
		for (_uint x = 0; x < iSizeX + 1; ++x)
		{
			_float fTileInterval = 0.1f;
			VTXTEXDATA v;
			v.vPosition = _float3(static_cast<_float>(x * fTileInterval), 0, static_cast<_float>(z * fTileInterval));
			v.vTexCoord = _float2(static_cast<_float>(x * fTileInterval), static_cast<_float>(iSizeZ * fTileInterval - z * fTileInterval));

			vtx.push_back(v);
		}
	}

	geometry->Set_Vertices(vtx);

	vector<_uint> idx;

	for (_uint z = 0; z < iSizeZ; ++z)
	{
		for (_uint x = 0; x < iSizeX; ++x)
		{
			_uint index[] = { (iSizeX + 1) * (z + 1) + x,
				 (iSizeX + 1) * (z + 1) + (x + 1),
				 (iSizeX + 1) * (z)+x + 1,
				 (iSizeX + 1) * (z)+x
			};
			idx.push_back(index[0]); idx.push_back(index[1]); idx.push_back(index[2]);
			idx.push_back(index[0]); idx.push_back(index[2]); idx.push_back(index[3]);
		}
	}
	geometry->Set_Indices(idx);
}

void GeometryHelper::CreateQuad(shared_ptr<Geometry<VTXTEXNORDATA>> geometry)
{
	vector<VTXTEXNORDATA> vtx;
	vtx.resize(4);

	vtx[0].vPosition = _float3{ -0.5f, 0.5f, 0.f };
	vtx[0].vTexCoord = _float2(0.f, 0.f);
	vtx[0].vNormal = _float3{ 0.f,0.f,-1.f };
	vtx[1].vPosition = _float3{ 0.5f, 0.5f, 0.f };
	vtx[1].vTexCoord = _float2(1.f, 0.f);
	vtx[1].vNormal = _float3{ 0.f,0.f,-1.f };
	vtx[2].vPosition = _float3{ 0.5f, -0.5f, 0.f };
	vtx[2].vTexCoord = _float2(1.f, 1.f);
	vtx[2].vNormal = _float3{ 0.f,0.f,-1.f };
	vtx[3].vPosition = _float3{ -0.5f, -0.5f, 0.f };
	vtx[3].vTexCoord = _float2(0.f, 1.f);
	vtx[3].vNormal = _float3{ 0.f,0.f,-1.f };
	geometry->Set_Vertices(vtx);

	vector<_uint> idx = { 0,1,2,0,2,3 };
	geometry->Set_Indices(idx);
}

void GeometryHelper::CreateCube(shared_ptr<Geometry<VTXTEXNORDATA>> geometry)
{
	vector<VTXTEXNORDATA> vtx(24);
	_float w2 = 1.f;
	_float h2 = 1.f;
	_float d2 = 1.f;
	_uint index = 0;
	//front
	vtx[index++] = VTXTEXNORDATA{ _float3(-w2,+h2,-d2),_float2(0.f,0.f) ,_float3(0.f,0.f,-1.f) };
	vtx[index++] = VTXTEXNORDATA{ _float3(+w2,+h2,-d2),_float2(1.f,0.f) ,_float3(0.f,0.f,-1.f) };
	vtx[index++] = VTXTEXNORDATA{ _float3(+w2,-h2,-d2),_float2(1.f,1.f) ,_float3(0.f,0.f,-1.f) };
	vtx[index++] = VTXTEXNORDATA{ _float3(-w2,-h2,-d2),_float2(0.f,1.f) ,_float3(0.f,0.f,-1.f) };

	//back
	vtx[index++] = VTXTEXNORDATA{ _float3(+w2,+h2,+d2),_float2(0.f,0.f) ,_float3(0.f,0.f,1.f) };
	vtx[index++] = VTXTEXNORDATA{ _float3(-w2,+h2,+d2),_float2(1.f,0.f) ,_float3(0.f,0.f,1.f) };
	vtx[index++] = VTXTEXNORDATA{ _float3(-w2,-h2,+d2),_float2(1.f,1.f) ,_float3(0.f,0.f,1.f) };
	vtx[index++] = VTXTEXNORDATA{ _float3(+w2,-h2,+d2),_float2(0.f,1.f) ,_float3(0.f,0.f,1.f) };

	// up
	vtx[index++] = VTXTEXNORDATA{ _float3(-w2,+h2,+d2),_float2(0.f,0.f) ,_float3(0.f,1.f,0.f)};
	vtx[index++] = VTXTEXNORDATA{ _float3(+w2,+h2,+d2),_float2(1.f,0.f),_float3(0.f,1.f,0.f) };
	vtx[index++] = VTXTEXNORDATA{ _float3(+w2,+h2,-d2),_float2(1.f,1.f),_float3(0.f,1.f,0.f) };
	vtx[index++] = VTXTEXNORDATA{ _float3(-w2,+h2,-d2),_float2(0.f,1.f),_float3(0.f,1.f,0.f) };

	//down
	vtx[index++] = VTXTEXNORDATA{ _float3(+w2,-h2,+d2),_float2(0.f,0.f),_float3(0.f,-1.f,0.f) };
	vtx[index++] = VTXTEXNORDATA{ _float3(-w2,-h2,+d2),_float2(1.f,0.f),_float3(0.f,-1.f,0.f)};
	vtx[index++] = VTXTEXNORDATA{ _float3(-w2,-h2,-d2),_float2(1.f,1.f),_float3(0.f,-1.f,0.f)};
	vtx[index++] = VTXTEXNORDATA{ _float3(+w2,-h2,-d2),_float2(0.f,1.f),_float3(0.f,-1.f,0.f)};

	//Left
	vtx[index++] = VTXTEXNORDATA{ _float3(-w2,+h2,+d2),_float2(0.f,0.f),_float3(-1.f,0.f,0.f) };
	vtx[index++] = VTXTEXNORDATA{ _float3(-w2,+h2,-d2),_float2(1.f,0.f),_float3(-1.f,0.f,0.f)};
	vtx[index++] = VTXTEXNORDATA{ _float3(-w2,-h2,-d2),_float2(1.f,1.f),_float3(-1.f,0.f,0.f)};
	vtx[index++] = VTXTEXNORDATA{ _float3(-w2,-h2,+d2),_float2(0.f,1.f),_float3(-1.f,0.f,0.f)};

	//Right
	vtx[index++] = VTXTEXNORDATA{ _float3(+w2,+h2,-d2),_float2(0.f,0.f),_float3(1.f,0.f,0.f) };
	vtx[index++] = VTXTEXNORDATA{ _float3(+w2,+h2,+d2),_float2(1.f,0.f),_float3(1.f,0.f,0.f) };
	vtx[index++] = VTXTEXNORDATA{ _float3(+w2,-h2,+d2),_float2(1.f,1.f),_float3(1.f,0.f,0.f) };
	vtx[index++] = VTXTEXNORDATA{ _float3(+w2,-h2,-d2),_float2(0.f,1.f),_float3(1.f,0.f,0.f) };
	geometry->Set_Vertices(vtx);

	index = 0;
	vector<_uint> idx(36);

	for (_uint i = 0; i < 6; ++i)
	{
		_uint startIndex = i * 4;
		idx[index++] = startIndex; 	idx[index++] = startIndex + 1; 	idx[index++] = startIndex + 2;
		idx[index++] = startIndex;	idx[index++] = startIndex + 2;	idx[index++] = startIndex + 3;
	}
	geometry->Set_Indices(idx);
}

void GeometryHelper::CreateSphere(shared_ptr<Geometry<VTXTEXNORDATA>> geometry)
{
	_float fRadius = 0.5f;
	_uint iStackCount = 20;
	_uint iSliceCount = 20;

	vector<VTXTEXNORDATA> vtx;

	VTXTEXNORDATA v;
	v.vPosition = _float3(0.f, fRadius, 0.f);
	v.vTexCoord = _float2(0.5f, 0.f);
	v.vNormal = v.vPosition;
	v.vNormal.Normalize();
	vtx.push_back(v);
	_float fStackAngle = XM_PI / iStackCount;
	_float fSliceAngle = XM_2PI / iSliceCount;

	_float fDeltaU = 1.f / static_cast<_float>(iSliceCount);
	_float fDeltaV = 1.f / static_cast<_float>(iStackCount);

	for (_uint y = 1; y <= iStackCount - 1; ++y)
	{
		_float pi = y * fStackAngle;

		for (_uint x = 0; x <= iSliceCount; ++x)
		{
			_float theta = x * fSliceAngle;

			v.vPosition.x = fRadius * sinf(pi) * cosf(theta);
			v.vPosition.y = fRadius * cosf(pi);
			v.vPosition.z = fRadius * sinf(pi) * sinf(theta);

			v.vTexCoord = _float2(fDeltaU * x, fDeltaV * y);
			v.vNormal = v.vPosition;
			v.vNormal.Normalize();
			vtx.push_back(v);
		}
	}

	v.vPosition = _float3(0.f, -fRadius, 0.f);
	v.vTexCoord = _float2(0.5f, 1.f);
	v.vNormal = v.vPosition;
	v.vNormal.Normalize();
	vtx.push_back(v);

	geometry->Set_Vertices(vtx);

	vector<_uint> idx;

	for (_uint i = 0; i <= iSliceCount; ++i)
	{
		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	_uint iRingVtxCount = iSliceCount + 1;

	for (_uint y = 0; y < iStackCount - 2; ++y)
	{
		for (_uint x = 0; x < iSliceCount; ++x)
		{
			// y,x - y, x+1
			// l      /
			// y+1, x
			idx.push_back(1 + (y)*iRingVtxCount + (x));
			idx.push_back(1 + (y)*iRingVtxCount + (x + 1));
			idx.push_back(1 + (y + 1) * iRingVtxCount + (x));

			//    y, x+1
			// /	  l  
			// y+1,x - y+1,x+1

			idx.push_back(1 + (y + 1) * iRingVtxCount + (x));
			idx.push_back(1 + (y)*iRingVtxCount + (x + 1));
			idx.push_back(1 + (y + 1) * iRingVtxCount + (x + 1));
		}
	}

	_uint iBottomIndex = static_cast<_uint>(vtx.size()) - 1;
	_uint iLastRingStartIndex = iBottomIndex - iRingVtxCount;
	for (_uint i = 0; i < iSliceCount; ++i)
	{
		//last+i - last+i +1
		// bottom
		idx.push_back(iBottomIndex);
		idx.push_back(iLastRingStartIndex + i);
		idx.push_back(iLastRingStartIndex + i + 1);

	}

	geometry->Set_Indices(idx);
}

void GeometryHelper::CreateGrid(shared_ptr<Geometry<VTXTEXNORDATA>> geometry, _uint iSizeX, _uint iSizeZ)
{
	vector<VTXTEXNORDATA> vtx;

	for (_uint z = 0; z < iSizeZ + 1; ++z)
	{
		for (_uint x = 0; x < iSizeX + 1; ++x)
		{
			VTXTEXNORDATA v;
			v.vPosition = _float3(static_cast<_float>(x), 0, static_cast<_float>(z));
			v.vTexCoord = _float2(static_cast<_float>(x), static_cast<_float>(iSizeZ - z));
			v.vNormal = _float3(0.f, 1.f, 0.f);
			vtx.push_back(v);
		}
	}

	geometry->Set_Vertices(vtx);

	vector<_uint> idx;

	for (_uint z = 0; z < iSizeZ; ++z)
	{
		for (_uint x = 0; x < iSizeX; ++x)
		{
			_uint index[] = { (iSizeX + 1) * (z + 1) + x,
				 (iSizeX + 1) * (z + 1) + (x + 1),
				 (iSizeX + 1) * (z)+x + 1,
				 (iSizeX + 1) * (z)+x
			};
			idx.push_back(index[0]); idx.push_back(index[1]); idx.push_back(index[2]);
			idx.push_back(index[0]); idx.push_back(index[2]); idx.push_back(index[3]);
		}
	}
	geometry->Set_Indices(idx);
}

void GeometryHelper::CreateQuad(shared_ptr<Geometry<VTXTEXNORTANDATA>> geometry)
{
	vector<VTXTEXNORTANDATA> vtx;
	vtx.resize(4);

	vtx[0].vPosition = _float3{ -0.5f, 0.5f, 0.f };
	vtx[0].vTexCoord = _float2(0.f, 0.f);
	vtx[0].vNormal = _float3{ 0.f,0.f,-1.f };
	vtx[0].vTangent = _float3(1.0f, 0.f, 0.f);

	vtx[1].vPosition = _float3{ 0.5f, 0.5f, 0.f };
	vtx[1].vTexCoord = _float2(1.f, 0.f);
	vtx[1].vNormal = _float3{ 0.f,0.f,-1.f };
	vtx[1].vTangent = _float3(1.0f, 0.f, 0.f);

	vtx[2].vPosition = _float3{ 0.5f, -0.5f, 0.f };
	vtx[2].vTexCoord = _float2(1.f, 1.f);
	vtx[2].vNormal = _float3{ 0.f,0.f,-1.f };
	vtx[2].vTangent = _float3(1.0f, 0.f, 0.f);

	vtx[3].vPosition = _float3{ -0.5f, -0.5f, 0.f };
	vtx[3].vTexCoord = _float2(0.f, 1.f);
	vtx[3].vNormal = _float3{ 0.f,0.f,-1.f };
	vtx[3].vTangent = _float3(1.0f, 0.f, 0.f);

	geometry->Set_Vertices(vtx);

	vector<_uint> idx = { 0,1,2,0,2,3 };
	geometry->Set_Indices(idx);
}

void GeometryHelper::CreateCube(shared_ptr<Geometry<VTXTEXNORTANDATA>> geometry)
{
	vector<VTXTEXNORTANDATA> vtx(24);
	_float w2 = 1.f;
	_float h2 = 1.f;
	_float d2 = 1.f;
	_uint index = 0;
	//front
	vtx[index++] = VTXTEXNORTANDATA{ _float3(-w2,+h2,-d2),_float2(0.f,0.f) ,_float3(0.f,0.f,-1.f),_float3(1.f,0.f,0.f)};
	vtx[index++] = VTXTEXNORTANDATA{ _float3(+w2,+h2,-d2),_float2(1.f,0.f) ,_float3(0.f,0.f,-1.f),_float3(1.f,0.f,0.f) };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(+w2,-h2,-d2),_float2(1.f,1.f) ,_float3(0.f,0.f,-1.f),_float3(1.f,0.f,0.f) };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(-w2,-h2,-d2),_float2(0.f,1.f) ,_float3(0.f,0.f,-1.f),_float3(1.f,0.f,0.f) };

	//back
	vtx[index++] = VTXTEXNORTANDATA{ _float3(+w2,+h2,+d2),_float2(0.f,0.f) ,_float3(0.f,0.f,1.f),_float3(-1.f,0.f,0.f) };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(-w2,+h2,+d2),_float2(1.f,0.f) ,_float3(0.f,0.f,1.f),_float3(-1.f,0.f,0.f)  };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(-w2,-h2,+d2),_float2(1.f,1.f) ,_float3(0.f,0.f,1.f),_float3(-1.f,0.f,0.f)  };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(+w2,-h2,+d2),_float2(0.f,1.f) ,_float3(0.f,0.f,1.f),_float3(-1.f,0.f,0.f)  };

	// up
	vtx[index++] = VTXTEXNORTANDATA{ _float3(-w2,+h2,+d2),_float2(0.f,0.f) ,_float3(0.f,1.f,0.f),_float3(1.f,0.f,0.f) };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(+w2,+h2,+d2),_float2(1.f,0.f),_float3(0.f,1.f,0.f) ,_float3(1.f,0.f,0.f)};
	vtx[index++] = VTXTEXNORTANDATA{ _float3(+w2,+h2,-d2),_float2(1.f,1.f),_float3(0.f,1.f,0.f) ,_float3(1.f,0.f,0.f)};
	vtx[index++] = VTXTEXNORTANDATA{ _float3(-w2,+h2,-d2),_float2(0.f,1.f),_float3(0.f,1.f,0.f) ,_float3(1.f,0.f,0.f)};

	//down
	vtx[index++] = VTXTEXNORTANDATA{ _float3(+w2,-h2,+d2),_float2(0.f,0.f),_float3(0.f,-1.f,0.f),_float3(-1.f,0.f,0.f)  };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(-w2,-h2,+d2),_float2(1.f,0.f),_float3(0.f,-1.f,0.f),_float3(-1.f,0.f,0.f)  };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(-w2,-h2,-d2),_float2(1.f,1.f),_float3(0.f,-1.f,0.f),_float3(-1.f,0.f,0.f)  };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(+w2,-h2,-d2),_float2(0.f,1.f),_float3(0.f,-1.f,0.f),_float3(-1.f,0.f,0.f)  };

	//Left
	vtx[index++] = VTXTEXNORTANDATA{ _float3(-w2,+h2,+d2),_float2(0.f,0.f),_float3(-1.f,0.f,0.f),_float3(0.f,0.f, -1.f) };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(-w2,+h2,-d2),_float2(1.f,0.f),_float3(-1.f,0.f,0.f),_float3(0.f,0.f, -1.f) };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(-w2,-h2,-d2),_float2(1.f,1.f),_float3(-1.f,0.f,0.f),_float3(0.f,0.f, -1.f) };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(-w2,-h2,+d2),_float2(0.f,1.f),_float3(-1.f,0.f,0.f),_float3(0.f,0.f, -1.f) };

	//Right
	vtx[index++] = VTXTEXNORTANDATA{ _float3(+w2,+h2,-d2),_float2(0.f,0.f),_float3(1.f,0.f,0.f),_float3(0.f,0.f, 1.f) };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(+w2,+h2,+d2),_float2(1.f,0.f),_float3(1.f,0.f,0.f),_float3(0.f,0.f, 1.f)  };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(+w2,-h2,+d2),_float2(1.f,1.f),_float3(1.f,0.f,0.f),_float3(0.f,0.f, 1.f)  };
	vtx[index++] = VTXTEXNORTANDATA{ _float3(+w2,-h2,-d2),_float2(0.f,1.f),_float3(1.f,0.f,0.f),_float3(0.f,0.f, 1.f)  };
	geometry->Set_Vertices(vtx);

	index = 0;
	vector<_uint> idx(36);

	for (_uint i = 0; i < 6; ++i)
	{
		_uint startIndex = i * 4;
		idx[index++] = startIndex; 	idx[index++] = startIndex + 1; 	idx[index++] = startIndex + 2;
		idx[index++] = startIndex;	idx[index++] = startIndex + 2;	idx[index++] = startIndex + 3;
	}
	geometry->Set_Indices(idx);
}

void GeometryHelper::CreateSphere(shared_ptr<Geometry<VTXTEXNORTANDATA>> geometry)
{
	_float fRadius = 1.f;
	_uint iStackCount = 20;
	_uint iSliceCount = 20;

	vector<VTXTEXNORTANDATA> vtx;

	VTXTEXNORTANDATA v;
	v.vPosition = _float3(0.f, fRadius, 0.f);
	v.vTexCoord = _float2(0.5f, 0.f);
	v.vNormal = v.vPosition;
	v.vNormal.Normalize();
	v.vTangent = _float3(1.0f, 0.0f, 0.0f);
	v.vTangent.Normalize();
	vtx.push_back(v);
	_float fStackAngle = XM_PI / iStackCount;
	_float fSliceAngle = XM_2PI / iSliceCount;

	_float fDeltaU = 1.f / static_cast<_float>(iSliceCount);
	_float fDeltaV = 1.f / static_cast<_float>(iStackCount);

	for (_uint y = 1; y <= iStackCount - 1; ++y)
	{
		_float pi = y * fStackAngle;

		for (_uint x = 0; x <= iSliceCount; ++x)
		{
			_float theta = x * fSliceAngle;

			v.vPosition.x = fRadius * sinf(pi) * cosf(theta);
			v.vPosition.y = fRadius * cosf(pi);
			v.vPosition.z = fRadius * sinf(pi) * sinf(theta);

			v.vTexCoord = _float2(fDeltaU * x, fDeltaV * y);
			v.vNormal = v.vPosition;
			v.vNormal.Normalize();

			v.vTangent.x = -fRadius * sinf(pi) * sinf(theta);
			v.vTangent.y = 0.0f;
			v.vTangent.z = fRadius * sinf(pi) * cosf(theta);
			v.vTangent.Normalize();

			vtx.push_back(v);
		}
	}

	v.vPosition = _float3(0.f, -fRadius, 0.f);
	v.vTexCoord = _float2(0.5f, 1.f);
	v.vNormal = v.vPosition;
	v.vNormal.Normalize();

	v.vTangent = _float3(1.0f, 0.0f, 0.0f);
	v.vTangent.Normalize();

	vtx.push_back(v);

	geometry->Set_Vertices(vtx);

	vector<_uint> idx(36);

	for (_uint i = 0; i <= iSliceCount; ++i)
	{
		idx.push_back(0);
		idx.push_back(i + 2);
		idx.push_back(i + 1);
	}

	_uint iRingVtxCount = iSliceCount + 1;

	for (_uint y = 0; y < iStackCount - 2; ++y)
	{
		for (_uint x = 0; x < iSliceCount; ++x)
		{
			// y,x - y, x+1
			// l      /
			// y+1, x
			idx.push_back(1 + (y)*iRingVtxCount + (x));
			idx.push_back(1 + (y)*iRingVtxCount + (x + 1));
			idx.push_back(1 + (y + 1) * iRingVtxCount + (x));

			//    y, x+1
			// /	  l  
			// y+1,x - y+1,x+1

			idx.push_back(1 + (y + 1) * iRingVtxCount + (x));
			idx.push_back(1 + (y)*iRingVtxCount + (x + 1));
			idx.push_back(1 + (y + 1) * iRingVtxCount + (x + 1));
		}
	}

	_uint iBottomIndex = static_cast<_uint>(vtx.size()) - 1;
	_uint iLastRingStartIndex = iBottomIndex - iRingVtxCount;
	for (_uint i = 0; i < iSliceCount; ++i)
	{
		//last+i - last+i +1
		// bottom
		idx.push_back(iBottomIndex);
		idx.push_back(iLastRingStartIndex + i);
		idx.push_back(iLastRingStartIndex + i + 1);

	}

	geometry->Set_Indices(idx);
}

void GeometryHelper::CreateGrid(shared_ptr<Geometry<VTXTEXNORTANDATA>> geometry, _uint iSizeX, _uint iSizeZ)
{
	vector<VTXTEXNORTANDATA> vtx;

	for (_uint z = 0; z < iSizeZ + 1; ++z)
	{
		for (_uint x = 0; x < iSizeX + 1; ++x)
		{
			VTXTEXNORTANDATA v;
			v.vPosition = _float3(static_cast<_float>(x), 0, static_cast<_float>(z));
			v.vTexCoord = _float2(static_cast<_float>(x), static_cast<_float>(iSizeZ - z));
			v.vNormal = _float3(0.f, 1.f, 0.f);
			v.vTangent = _float3(1.f, 0.f, 0.f);
			vtx.push_back(v);
		}
	}

	geometry->Set_Vertices(vtx);

	vector<_uint> idx;

	for (_uint z = 0; z < iSizeZ; ++z)
	{
		for (_uint x = 0; x < iSizeX; ++x)
		{
			_uint index[] = { (iSizeX + 1) * (z + 1) + x,
				 (iSizeX + 1) * (z + 1) + (x + 1),
				 (iSizeX + 1) * (z)+x + 1,
				 (iSizeX + 1) * (z)+x
			};
			idx.push_back(index[0]); idx.push_back(index[1]); idx.push_back(index[2]);
			idx.push_back(index[0]); idx.push_back(index[2]); idx.push_back(index[3]);
		}
	}
	geometry->Set_Indices(idx);
}

void GeometryHelper::CreatePoint(shared_ptr<Geometry<VTXTEXNORTANDATA>> geometry)
{
	vector<VTXTEXNORTANDATA> vtx(1);
	vtx[0] = VTXTEXNORTANDATA{ _float3(0.f,0.f,0.f),_float2(0.5f,0.5f) ,_float3(0.f,0.f,-1.f),_float3(1.f,0.f,0.f) };

	geometry->Set_Vertices(vtx);

	vector<_uint> idx(1);
	idx[0] = 0;

	geometry->Set_Indices(idx);
}
