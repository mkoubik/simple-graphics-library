#ifndef __PRIMITIVES_H__
#define __PRIMITIVES_H__

#include <vector>

#include "common.h"

using namespace std;


class Primitive
{
	public:
		Material *material;
		virtual bool findIntersection(Ray *ray, float *distance, Vector4f *intersection) = 0;
		virtual Vector4f getNormal() = 0;
};

class Triangle : public Primitive
{
	protected:
		Vertex4f v1, v2, v3;
		Vector4f e1, e2;
		Vector4f normal;

	public:
		Triangle(Material *_material, Vertex4f &_v1, Vertex4f &_v2, Vertex4f &_v3)
		{
			material = _material;
			v1 = _v1;
			v2 = _v2;
			v3 = _v3;

			e1 = v2 - v1;
			e2 = v3 - v1;
			normal = e1.crossProduct(e2);
			normal.normalize();
		}

		Vector4f getNormal()
		{
			return normal;
		}
};

class TriangleBarycentric : public Triangle
{
	private:
		Vector4f origin;

	public:
		TriangleBarycentric(Material *_material, Vertex4f &_v1, Vertex4f &_v2, Vertex4f &_v3) : Triangle(_material, _v1, _v2, _v3)
		{
			origin = Vector4f(v1.x, v1.y, v1.z);
		}

		bool findIntersection(Ray *ray, float *distance, Vector4f *intersection)
		{
			Vector4f s1 = ray->direction.crossProduct(e2);
			float divisor = s1.dotProduct(e1);
			if (divisor == 0.)
				return false;
			float invDivisor = 1.f / divisor;
			// Compute first barycentric coordinate
			Vector4f d = ray->origin - v1;
			float b1 = d.dotProduct(s1) * invDivisor;
			if (b1 < 0. || b1 > 1.)
				return false;
			// Compute second barycentric coordinate
			Vector4f s2 = d.crossProduct(e1);
			float b2 = ray->direction.dotProduct(s2) * invDivisor;
			if (b2 < 0. || b1 + b2 > 1.)
				return false;
			// Compute _t_ to intersection point
			float t = e2.dotProduct(s2) * invDivisor;
			// if (t < ray.mint || t > ray.maxt)
			// 	return false;

			Vector4f _intersection = e1 * b1 + e2 * b2 + origin;
			intersection->x = _intersection.x;
			intersection->y = _intersection.y;
			intersection->z = _intersection.z;
			*distance = t;
			return true;
		}
};

class Plane
{
	public:
		Vertex4f point;
		Vector4f normal;

		Plane() {}

		Plane(Vertex4f _point, Vector4f _normal)
		{
			point = _point;
			normal = _normal;
		}

		void findIntersection(Ray *ray, float *distance, Vector4f *intersection)
		{
			*distance = (normal.dotProduct(point - ray->origin)) / normal.dotProduct(ray->direction);
			Vector4f _intersection = ray->origin + (ray->direction * *distance);
			intersection->x = _intersection.x;
			intersection->y = _intersection.y;
			intersection->z = _intersection.z;
		}
};

class TrianglePlucker : public Triangle
{
	private:
		PluckerEdge e1, e2, e3;
	public:
		Plane plane;

		TrianglePlucker(Material *_material, Vertex4f &_v1, Vertex4f &_v2, Vertex4f &_v3) : Triangle(_material, _v1, _v2, _v3)
		{
			e1 = PluckerEdge(v1, v2);
			e2 = PluckerEdge(v2, v3);
			e3 = PluckerEdge(v3, v1);
			plane = Plane(v1, normal);
		}

		bool findIntersection(Ray *ray, float *distance, Vector4f *intersection)
		{
			// prusecik existuje prave tehdy, pokud paprsek miji vsechny hrany zleva, nebo vsechny zprava (nebo pokud nekterou protina)
			bool side1 = (ray->getPlucker()->side(e1) >= 0);
			bool side2 = (ray->getPlucker()->side(e2) >= 0);
			if (side1 != side2)
				return false;
			bool side3 = (ray->getPlucker()->side(e3) >= 0);
			if (side1 != side3)
				return false;
			plane.findIntersection(ray, distance, intersection);
			return true;
		}
};

class Sphere : public Primitive
{
	private:
		Vertex4f center;
		float radius;
		Vector4f lastRayIntersection;
	public:
		Sphere(Material *_material, Vertex4f _center, const float _radius)
		{
			material = _material;
			center = _center;
			radius = _radius;
		}

	bool findIntersection(Ray *ray, float *distance, Vector4f *intersection)
	{
		Vector4f dst = ray->origin - center;
		const float b = dst.dotProduct(ray->direction);
		const float c = dst.dotProduct(dst) - radius * radius;
		const float d = b * b - c;

		if (d > 0) {
			*distance = -b - sqrtf(d);
			if (*distance < 0.0f)
				*distance = -b + sqrtf(d);

			Vector4f _intersection = ray->origin + (ray->direction * *distance);
			intersection->x = _intersection.x;
			intersection->y = _intersection.y;
			intersection->z = _intersection.z;

			// ulozeni pruseciku pro budouci pouziti v getNormal()
			lastRayIntersection = *intersection;
			return true;
		}
		return false;
	}

	Vector4f getNormal()
	{
		Vector4f normal = (lastRayIntersection - center);
		normal.normalize();
		return normal;
	}
};

#endif /* __PRIMITIVES_H__ */
