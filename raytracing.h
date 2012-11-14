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

class Material
{
	public:
		float r, g, b, kd, ks, shine, t, ior;
		Color color;

		Material(const float &_r, const float &_g, const float &_b, const float &_kd, const float &_ks)
		{
			r = _r;
			g = _g;
			b = _b;
			kd = _kd;
			ks = _ks;
			color = Color(r, g, b);
		}

		Material(const float &_r, const float &_g, const float &_b, const float &_kd, const float &_ks,
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
			color = Color(r, g, b);
		}
};

class PointLight
{
	public:
		float x, y, z;
		float r, g, b;
		Vertex4f position;
		Color color;

		PointLight(const float &_x, const float &_y, const float &_z, const float &_r, const float &_g, const float &_b)
		{
			x = _x;
			y = _y;
			z = _z;
			r = _r;
			g = _g;
			b = _b;
			position = Vertex4f(x, y, z);
			color = Color(r, g, b);
		}
};

class Primitive
{
	public:
		Material *material;
		virtual bool findIntersection(Ray *ray, float *distance, Vector4f *intersection) = 0;
		virtual Vector4f getNormal() = 0;
};

class Triangle : public Primitive
{
	private:
		Vertex4f v1, v2, v3;
		Vector4f origin;
		Vector4f e1, e2;
		Vector4f normal;

	public:
		Triangle(Material *_material, Vertex4f &_v1, Vertex4f &_v2, Vertex4f &_v3)
		{
			material = _material;
			v1 = _v1;
			v2 = _v2;
			v3 = _v3;

			origin = Vector4f(v1.x, v1.y, v1.z);

			e1 = v2 - v1;
			e2 = v3 - v1;
			normal = e1.crossProduct(e2);

			normal.normalize();
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

		Vector4f getNormal()
		{
			return normal;
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

class ReflectionModel
{
	protected:
		vector<PointLight*> *pointLights;

	public:
		virtual Color getColor(Primitive *primitive, Ray *ray, Vector4f *intersection) = 0;

		void setPointLights(vector<PointLight*> *_pointLights)
		{
			pointLights = _pointLights;
		}
};

class DummyModel : public ReflectionModel
{
	public:
		Color getColor(Primitive *primitive, Ray *ray, Vector4f *intersection)
		{
			return primitive->material->color;
		}
};

class PhongModel : public ReflectionModel
{
	public:
		Color getColor(Primitive *primitive, Ray *ray, Vector4f *intersection)
		{
			Vector4f normal = primitive->getNormal();
			Color ambient = Color(0, 0, 0);
			Color diffuse = Color(0, 0, 0);
			Color specular = Color(0, 0, 0);
			for (int i = 0; i < (int) pointLights->size(); i++) {
				PointLight *light = (*pointLights)[i];
				Vector4f l = light->position - *intersection;
				l.normalize();

				float diffI = normal.dotProduct(l) * primitive->material->kd;
				diffuse = diffuse + ((light->color * primitive->material->color) * diffI);

				Vector4f half = (l - ray->direction) * 0.5;
				half.normalize();
				float specI = powf(normal.dotProduct(half), primitive->material->shine * 4) * primitive->material->ks;
				specular = specular + (light->color * specI);
			}
			return ambient + diffuse + specular;
		}
};

class Raytracer
{
	private:
		vector<Primitive*> primitives;
		vector<PointLight*> pointLights;
		Matrix4f invertedMatrix;
		Color backgroundColor;
		ReflectionModel *model;

		Color castRay(Ray ray)
		{
			float distance = 0.0f;
			float minDistance = FLT_MAX;
			Primitive *nearest = 0;
			Vector4f intersection, nearestIntersection;
			for (int i = 0; i < (int) primitives.size(); i++) {
				if (primitives[i]->findIntersection(&ray, &distance, &intersection)) {
					if (distance < minDistance) {
						minDistance = distance;
						nearest = primitives[i];
						nearestIntersection = intersection;
					}
				}
			}
			if (nearest)
				return model->getColor(nearest, &ray, &nearestIntersection);
				// return Color(nearest->material->r, nearest->material->g, nearest->material->b);
			return backgroundColor;
		}

	public:
		Raytracer(ReflectionModel *_model)
		{
			model = _model;
			model->setPointLights(&pointLights);
		}

		void addPrimitive(Primitive *primitive)
		{
			primitives.push_back(primitive);
		}

		void addPointLight(PointLight *light)
		{
			pointLights.push_back(light);
		}

		void setMVPMatrix(Matrix4f matrix)
		{
			invertedMatrix = matrix.getInversion();
		}

		void setBackgroundColor(Color color)
		{
			backgroundColor = color;
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
