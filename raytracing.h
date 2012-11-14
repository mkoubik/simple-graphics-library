#ifndef __RAYTRACING_H__
#define __RAYTRACING_H__

#include <vector>

#include "common.h"
#include "primitives.h"

using namespace std;

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
