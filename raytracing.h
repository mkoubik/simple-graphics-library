#ifndef __RAYTRACING_H__
#define __RAYTRACING_H__

#include <vector>

#include "common.h"

using namespace std;

class Ray
{
	public:
		Vector4f origin, direction;

		Ray(Vector4f &_origin, Vector4f &_direction)
		{
			origin = _origin;
			direction = _direction;
		}
};

class Primitive
{
	public:
		virtual bool findIntersection(Ray *ray, float *distance) = 0;
		virtual bool getNormal(Vector4f &point) = 0;
};

class Triangle : public Primitive
{
	private:
		Vertex4f v1, v2, v3;

	public:
		Triangle(Vertex4f &_v1, Vertex4f &_v2, Vertex4f &_v3)
		{
			v1 = _v1;
			v2 = _v2;
			v3 = _v3;
		}

		bool findIntersection(Ray *ray, float *distance)
		{
			Vector4f e1 = v2 - v1;
			Vector4f e2 = v3 - v1;
			Vector4f s1 = ray->direction.crossProduct(e2);
			float divisor = s1.dotProduct(e1);
			if (divisor == 0.)
				return false;
			float invDivisor = 1.f / divisor;
			// Compute first barycentric coordinate
			Vector4f d = ray->origin - v1;
			float b1 = ray->direction.dotProduct(s1) * invDivisor;
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
			*distance = t;
			return true;
		}

		bool getNormal(Vector4f &point)
		{
			return false;
		}
};

class Sphere : public Primitive
{
	private:
		Vertex4f center;
		float radius;
	public:
		Sphere(Vertex4f _center, const float _radius)
		{
			center = _center;
			radius = _radius;
		}

	bool findIntersection(Ray *ray, float *distance)
	{
		Vector4f dst = ray->origin - center;
		const float b = dst.dotProduct(ray->direction);
		const float c = dst.dotProduct(dst) - radius * radius;
		const float d = b * b - c;

		if (d > 0) {
			*distance = -b - sqrtf(d);
			if (*distance < 0.0f)
				*distance = -b + sqrtf(d);
			return true;
		}
		return false;
	}

	bool getNormal(Vector4f &point)
	{
		return false;
	}
};

class PhongMaterial
{
	public:
		float r, g, b, kd, ks, shine, t, ior;

		PhongMaterial(const float &_r, const float &_g, const float &_b, const float &_kd, const float &_ks)
		{
			r = _r;
			g = _g;
			b = _b;
			kd = _kd;
			ks = _ks;
		}

		PhongMaterial(const float &_r, const float &_g, const float &_b, const float &_kd, const float &_ks,
			const float &_shine, const float &_t, const float &_ior)
		{
			r = _r;
			g = _g;
			b = _b;
			kd = _kd;
			ks = _ks;
			shine = _shine;
			t = _t;
			ior = _ior;
		}
};

class PointLight
{
	public:
		float x, y, z;
		float r, g, b;

		PointLight(const float &_x, const float &_y, const float &_z, const float &_r, const float &_g, const float &_b)
		{
			x = _x;
			y = _y;
			z = _z;
			r = _r;
			g = _g;
			b = _b;
		}
};

class Raytracer
{
	private:
		vector<Primitive*> primitives;
		Matrix4f invertedMatrix;

		Color castRay(Ray ray)
		{
			float distance = 0.0f;
			for (int i = 0; i < (int) primitives.size(); i++) {
				if (primitives[i]->findIntersection(&ray, &distance))
					return Color(0.8, 0.8, 0.8);
			}
			return Color(0.2, 0.2, 0.2);
		}

	public:
		void addPrimitive(Primitive *primitive)
		{
			primitives.push_back(primitive);
		}

		void setMVPMatrix(Matrix4f matrix)
		{
			invertedMatrix = matrix.getInversion();
		}

		void raytrace(Color *colorBuffer, int width, int height)
		{
			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					colorBuffer[y * width + x] = castRay(createRay(x, y, width, height));
				}
			}
		}

		inline Ray createRay(int x, int y, int width, int height)
		{
			float w = 1.0f;
			Vector4f origin = invertedMatrix * Vector4f(2.0f * x / width - 1.0f, 2.0f * y / height - 1.0f, -1.0f, w);
			w = 1.0f/origin.w;
			origin = origin * w;

			Vector4f direction = invertedMatrix * Vector4f(2.0f * x / width - 1.0f, 2.0f * y / height - 1.0f, 1.0f, w);
			w = 1.0f/direction.w;
			direction = direction * w;
			direction = direction - origin;

			// TODO: vymyslet efektivnejsi normovani vektoru
			float invSize = 1 / sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
			direction = direction * invSize;

			Ray ray = Ray(origin, direction);
			return ray;
		}
};

#endif /* __RAYTRACING_H__ */
