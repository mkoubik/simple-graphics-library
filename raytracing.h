#ifndef __RAYTRACING_H__
#define __RAYTRACING_H__

#include <vector>

#include "common.h"
#include "primitives.h"

// #define ADAPTIVE_ANTIALIASING 1
#define ANTIALIASING_TRESHOLD 0.1f
#define ANTIALIASING_LEVEL 8

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

				if (primitive->material->kd > 0) {
					float diffI = normal.dotProduct(l) * primitive->material->kd;
					diffuse += ((light->color * primitive->material->color) * diffI);
				}

				if (primitive->material->ks > 0) {
					Vector4f half = (l - ray->direction) * 0.5;
					half.normalize();
					float specI = powf(normal.dotProduct(half), primitive->material->shine * 4) * primitive->material->ks;
					specular += (light->color * specI);
				}
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

		#ifdef ADAPTIVE_ANTIALIASING

		vector<Color*> getNeigbours(Color *colorBuffer, int width, int height, int i)
		{
			vector<Color*> neighbours = vector<Color*>();
			bool leftEdge = (i % width == 0);
			bool rightEdge = (i % width == width - 1);
			bool topEdge = (i < width);
			bool bottomEdge = (i > width * height - width - 1);

			if (!leftEdge && !topEdge)
				neighbours.push_back(&colorBuffer[i - width - 1]);	// left-top
			if (!topEdge)
				neighbours.push_back(&colorBuffer[i - width]);		// top
			if (!rightEdge && !topEdge)
				neighbours.push_back(&colorBuffer[i - width + 1]);	// right-top
			if (!leftEdge)
				neighbours.push_back(&colorBuffer[i - 1]);			// left
			if (!rightEdge)
				neighbours.push_back(&colorBuffer[i + 1]);			// right
			if (!leftEdge && !bottomEdge)
				neighbours.push_back(&colorBuffer[i - width - 1]);	// left-bottom
			if (!bottomEdge)
				neighbours.push_back(&colorBuffer[i - width]);		// bottom
			if (!rightEdge && !bottomEdge)
				neighbours.push_back(&colorBuffer[i - width + 1]);	// right-bottom

			return neighbours;
		}

		#endif

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

			#ifdef ADAPTIVE_ANTIALIASING
			bool doAntialiasing[height * width];
			Color current;
			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					current = colorBuffer[y * width + x];

					// maximalni rozdil mezi barvou aktualniho pixelu a barvami (az) 8 sousednich
					float maxDifference = current.getMaxDifference(getNeigbours(colorBuffer, width, height, y * width + x));
					if (maxDifference > ANTIALIASING_TRESHOLD) {
						doAntialiasing[y * width + x] = true;
					}
				}
			}
			float subpixelSize = 1.f / ANTIALIASING_LEVEL;
			for (int x = 0; x < width; x++) {
				for (int y = 0; y < height; y++) {
					if (doAntialiasing[y * width + x]) {
						// jittering
						float xx = (float) x - 0.5f;
						Color average = Color();
						for (int a=0; a<ANTIALIASING_LEVEL; a++) {
							float yy = (float) y - 0.5f;
							for (int b=0; b<ANTIALIASING_LEVEL; b++) {
								float randomX = (float)rand()/(float)RAND_MAX;
								float randomY = (float)rand()/(float)RAND_MAX;
								average += castRay(createRay(xx + randomX * subpixelSize, yy + randomY * subpixelSize, width, height));
								yy += subpixelSize;
							}
							xx += subpixelSize;
						}
						average = average * (1 / pow(ANTIALIASING_LEVEL, 2));
						colorBuffer[y * width + x] = average;
					}
				}
			}
			#endif
		}

		inline Ray createRay(float x, float y, int width, int height)
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
