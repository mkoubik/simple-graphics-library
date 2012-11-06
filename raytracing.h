#ifndef __RAYTRACING_H__
#define __RAYTRACING_H__

#include "common.h"

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
		bool findIntersection(Ray &ray, float &distance);
		bool getNormal(Vector4f &point);
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

		bool findIntersection(Ray &ray, float &distance)
		{
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
		vector <Primitive*> primitives;

	public:
		void addPrimitive(Primitive *primitive)
		{
			primitives.push_back(primitive);
		}

		void raytrace(Color *colorBuffer, int width, int height)
		{
			Color c = Color(0, 1, 0.5);
			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					colorBuffer[y * width + x] = c;
				}
			}
		}
};

#endif /* __RAYTRACING_H__ */
