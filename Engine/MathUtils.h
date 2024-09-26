#pragma once

#include "Primitive3D.h"
struct MathUtils
{

	static _bool PointInSphere(const Point3D& point, const Sphere3D& sphere);
	static Point3D ClosestPoint(const Point3D& point, const Sphere3D& sphere);

	static _bool PointInAABB(const Point3D& point, const AABB3D& aabb);
	static Point3D ClosestPoint(const Point3D& point, const AABB3D& aabb);

	static _bool PointInOBB(const Point3D& point, const OBB3D& obb);
	static Point3D ClosestPoint(const Point3D& point, const OBB3D& obb);

	static _bool PointOnPlane(const Point3D& point, const Plane3D& plane);
	static Point3D ClosestPoint(const Point3D& point, const Plane3D& plane);

	static _bool PointInLine(const Point3D& point, const Line3D& line);
	static Point3D ClosestPoint(const Point3D& point, const Line3D& line);

	static _bool PointOnRay(const Point3D& point, const Ray3D& ray);
	static Point3D ClosestPoint(const Point3D& point, const Ray3D& ray);

	static _bool SphereSphere(const Sphere3D& s1, const Sphere3D& s2);
	static _bool SphereAABB(const Sphere3D& s, const AABB3D& aabb);
	static _bool SphereOBB(const Sphere3D& s, const OBB3D& obb);

	static _bool AABBAABB(const AABB3D& aabb1, const AABB3D& aabb2);
	static _bool AABBOBB(const AABB3D& aabb, const OBB3D& obb);
	static _bool AABBPlane(const AABB3D& aabb, const Plane3D& plane);

	static _bool OBBOBB(const OBB3D& obb1, const OBB3D& obb2);
	
	static _bool PlanePlane(const Plane3D& plane1, const Plane3D& plane2);

	static Interval3D GetInterval(const AABB3D& aabb, const _float3& axis);
	static Interval3D GetInterval(const OBB3D& obb, const _float3& axis);
	static _bool OverlapOnAxis(const AABB3D& aabb, const OBB3D& obb, const _float3& axis);
	static _bool OverlapOnAxis(const OBB3D& obb1, const OBB3D& obb2, const _float3& axis);


	static _bool RayCast(const Sphere3D& sphere, const Ray3D& ray, OUT _float& distance);
	static _bool RayCast(const AABB3D& aabb, const Ray3D& ray, OUT _float& distance);
	static _bool RayCast(const Plane3D& plane, const Ray3D& ray, OUT _float& distance);
	
	static _bool PointInTriangle(const Point3D& p, const Triangle3D& t);

	static Plane3D FromTriangle(const Triangle3D& t);
	static _float3 Barycentric(const Point3D& p, const Triangle3D& t);
	static _bool RayCast(const Triangle3D& t, const Ray3D& ray, OUT _float& distance);
	static _float3 ProjectVecOnVec(_float3 a, _float3 b);

	static float RandF()
	{
		return (float)(rand()) / (float)RAND_MAX;
	}

	static float RandF(float a, float b)
	{
		return a + RandF() * (b - a);
	}

	static float Get_RandomFloat(float min, float max)
	{
		if (min > max)
			std::swap(min, max);

		return min + RandF() * (max - min);
	}

	static _float3 Get_RandomVector(const _float3& min, const _float3& max)
	{
		_float3 tmp;
		tmp.x = Get_RandomFloat(min.x, max.x);
		tmp.y = Get_RandomFloat(min.y, max.y);
		tmp.z = Get_RandomFloat(min.z, max.z);
		return tmp;
	}

	static _float3 ToEulerAngles(Quaternion q)
	{
		_float3 angles;

		float sqw = q.w * q.w;
		float sqx = q.x * q.x;
		float sqy = q.y * q.y;
		float sqz = q.z * q.z;
		angles.x = asinf(2.0f * (q.w * q.x - q.y * q.z)); // rotation about x-axis
		angles.y = atan2f(2.0f * (q.x * q.z + q.w * q.y), (-sqx - sqy + sqz + sqw)); // rotation about y-axis
		angles.z = atan2f(2.0f * (q.x * q.y + q.w * q.z), (-sqx + sqy - sqz + sqw)); // rotation about z-axis

		if (isnan(angles.x) || isnan(angles.y) || isnan(angles.z))
		{
			int a = 0;
		}

		return angles;
	}

	static _float evaluateQuadraticEquation(_float x, const _float3& coefficients) {
		return coefficients.x * x * x + coefficients.y * x + coefficients.z;
	}

	static _float3 calculateQuadraticCoefficients(_float x1, _float y1, _float z1, _float x2, _float y2, _float z2) {
		_float a = (z2 - z1) / ((x2 - x1) * (x2 - x1));
		_float b = -2 * a * x1;
		_float c = z1 - a * x1 * x1 - b * x1;

		return { a, b, c };
	}

};

