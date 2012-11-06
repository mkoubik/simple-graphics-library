#ifndef __COMMON_H__
#define __COMMON_H__

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include <stack>
#include <vector>

using namespace std;

inline int round(const float &x)
{
	return (int)(x + 0.5f);
}

class Color
{
	public:
		float r, g, b;

		Color(void)
		{
		}

		Color(const float & _r, const float & _g, const float & _b)
		{
			r = _r;
			g = _g;
			b = _b;
		}
};

class Edge
{
	public:
		float x;				// Aktualni souradnice X pruseciku s rozkladovym radkem
		float d_x;				// Prirustek v ose X pri prechodu na dalsi radek
		int d_y;				// Pocet radku, do nichz hrana zasahuje
		float z;				// Souradnice osy Z pro pocatek hrany
		float d_z;				// Prirustek v ose Z pri prechodu na dalsi radek

		/*
		 * Konstruktor
		 *
		 * @param x - aktualni souradnice x pruseciku s rozkladovym radkem
		 * @param d_x - prirustek x pri prechodu na dalsi radek
		 * @param d_y - pocet radku, do nichz hrana zasahuje
		 *
		 */
		Edge(float x, float d_x, int d_y)
		{
			this->x = x;
			this->d_x = d_x;
			this->d_y = d_y;
		}

		/*
		 * Konstruktor
		 *
		 * @param x - aktualni souradnice x pruseciku s rozkladovym radkem
		 * @param d_x - prirustek x pri prechodu na dalsi radek
		 * @param d_y - pocet radku, do nichz hrana zasahuje
		 * @param z - 
		 * @param d_z - 
		 *
		 */
		Edge(float x, float d_x, int d_y, float z, float d_z)
		{
			this->x = x;
			this->d_x = d_x;
			this->d_y = d_y;
			this->z = z;
			this->d_z= d_z;
		}
};

class Vector4f
{
	public:
		float x, y, z, w;

		Vector4f(void)
		{
		}

		Vector4f(const float &_x, const float &_y, const float &_z = 0.0f, const float &_w = 1.0f)
		{
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}
};

class Matrix4f
{
	public:
		float values[4][4];

		Matrix4f(void)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{					
					this->values[i][j] = 0.0f;
				}
			}
		}

		void loadMatrix4f(const float *matrix)
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					values[i][j] = matrix[i + 4 * j];
				}
			}
		}

		void loadMatrix4f(const float matrix[4][4])
		{
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					values[i][j] = matrix[i][j];
				}
			}
		}

		Matrix4f operator * (const float* matrixArray)
		{
			Matrix4f result;

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{					
					result.values[i][j] = this->values[i][0] * matrixArray[0 + 4 * j]
									    + this->values[i][1] * matrixArray[1 + 4 * j]
									    + this->values[i][2] * matrixArray[2 + 4 * j]
									    + this->values[i][3] * matrixArray[3 + 4 * j];
				}
			}

			return result;
		}

		Matrix4f operator * (const Matrix4f & matrix)
		{
			Matrix4f result;

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{					
					result.values[i][j] = this->values[i][0] * matrix.values[0][j] + 
										  this->values[i][1] * matrix.values[1][j] + 
										  this->values[i][2] * matrix.values[2][j] + 
										  this->values[i][3] * matrix.values[3][j]; 
				}
			}

			return result;
		}
};

class Vertex4f
{
	public:
		int x, y;
		float z, w;

		Vertex4f(void)
		{
		}

		Vertex4f(const int &_x, const int &_y, const float &_z = 0.0f, const float &_w = 1.0f)
		{
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}
};

Vector4f operator * (const Matrix4f &matrix, const Vector4f &v)
{
	Vector4f result;

	result.x = matrix.values[0][0] * v.x +
			   matrix.values[0][1] * v.y +
			   matrix.values[0][2] * v.z +
			   matrix.values[0][3];
  
	result.y = matrix.values[1][0] * v.x +
			   matrix.values[1][1] * v.y +
			   matrix.values[1][2] * v.z +
			   matrix.values[1][3];

	result.z = matrix.values[2][0] * v.x +
			   matrix.values[2][1] * v.y +
			   matrix.values[2][2] * v.z +
			   matrix.values[2][3];

	result.w = matrix.values[3][0] * v.x +
			   matrix.values[3][1] * v.y +
			   matrix.values[3][2] * v.z +
			   matrix.values[3][3];

	return result;
}

#endif /* __COMMON_H__*/
