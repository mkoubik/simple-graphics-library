//---------------------------------------------------------------------------
// context.h
// Additional header file for SGL (Simple Graphics Library)
// Date:  2012/09/28
// Author: Jan Fikar
//---------------------------------------------------------------------------

#define _USE_MATH_DEFINES
#include <math.h>
#include <vector>
#include <stack>

using namespace std;

//---------------------------------------------------------------------------
// MY FUNCTIONS
//---------------------------------------------------------------------------

inline int round(const float &x)
{
	return (int)(x + 0.5f);
}

//---------------------------------------------------------------------------
// MY CLASSES
//---------------------------------------------------------------------------

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

class Vector4f
{
	public:
		float x, y, z, w;

		Vector4f()
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
		float values[4][4];		// Dvourozmerne pole reprezentujici hodnoty matice

		/*
		 * Konstruktor
		 *
		 */
		Matrix4f()
		{
			for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{					
						this->values[i][j] = 0.0f;
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

	/*
	public:
		static int const MATRIX_SIZE = 16;

		float values [16];

		Matrix4f()
		{
		}
	*/
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

class sglContext
{
	inline void drawPoint(const Vertex4f &u)
	{
		int offset = int(pointSize / 2);

		for(int i = u.y - offset; i <= u.y + offset; i++)
		{
			for(int j = u.x - offset; j <= u.x + offset; j++)
			{
				setPixel(j, i);
			}
		}

		setPixel(u.x, u.y);
	}

	inline void drawLine(Vertex4f u, Vertex4f v)
	{
		int x, d_x, y, d_y, k1, k2, p, increase;
		float m;

		d_x = v.x - u.x;
		d_y = v.y - u.y;

		m = (float)d_y / d_x;

		if(fabs(m) <= 1.0f)
		{
			if(d_x < 0)
			{
				Vertex4f temp = u;
				u = v;
				v = temp;
								
				d_x *= -1;
				d_y *= -1;
			}

			if(d_y < 0)
			{
				increase = -1;
				k1 = -2 * d_y;
				k2 = 2 * (-d_y - d_x);
			}
			else
			{
				increase = 1;
				k1 = 2 * d_y;
				k2 = 2 * (d_y - d_x);
			}

			p = 2 * d_y - d_x;
			x = u.x;
			y = u.y;

			while(x < v.x)
			{
				setPixel(x, y);

				x++;
				if(p >= 0)
				{
					y += increase;
					p += k2;
				}
				else
				{
					p += k1;
				}
			}
		}
		else
		{
			if(d_y < 0)
			{
				Vertex4f temp = u;
				u = v;
				v = temp;
								
				d_x *= -1;
				d_y *= -1;
			}

			if(d_x < 0)
			{
				increase = -1;
				k1 = -2 * d_x;
				k2 = 2 * (-d_x - d_y);
			}
			else
			{
				increase = 1;
				k1 = 2 * d_x;
				k2 = 2 * (d_x - d_y);
			}

			p = 2 * d_x - d_y;
			x = u.x;
			y = u.y;

			while(y < v.y)
			{
				setPixel(x, y);

				y++;
				if(p >= 0)
				{
					x += increase;
					p += k2;
				}
				else
				{
					p += k1;
				}
			}
		}
	}

	public:
		static unsigned int const MAX_NUM_MATRICES = 32;

		int height, width;
		Color *colorBuffer;
		vector<Vertex4f> vertexBuffer;
		sglEElementType elementType;
		int pointSize;
		Color clearColor, color;

		// TRANSFORMATION
		bool isMVP_matrixUpdated;
		sglEMatrixMode matrixMode;
		Matrix4f *matrix;
		Matrix4f MV_matrix, P_matrix, MVP_matrix;
		stack<Matrix4f> transformationBuffer;

		// VIEWPORT
		int viewX, viewY;
		float viewHalfHeight, viewHalfWidth;

		sglEAreaMode areaMode;

		sglContext(const int &_height, const int &_width)
		{
			height = _height;
			width = _width;
			colorBuffer = new Color[height * width];
			matrix = NULL;
			isMVP_matrixUpdated = false;
		}

		~sglContext(void)
		{
			delete [] colorBuffer;
		}

		inline void setPixel(const int &x, const int &y)
		{
			if(x < width && y < height)
				colorBuffer[x + y * width] = color;
		}

		void clearColorBuffer(void)
		{
			int dimension = height * width;
			for(int i = 0; i < dimension; i++)
			{
				colorBuffer[i] = clearColor;
			}
		}

		inline void transformationPipeline2D(const float &x, const float &y, int &x_w,  int &y_w)
		{
			// MVP TRANSFORMATION
			Vector4f NDC = MVP_matrix * Vector4f(x, y);

			// VIEWPORT TRANSFORMATION
			x_w = round((NDC.x + 1.0f) * (viewHalfWidth) + viewX);
			y_w = round((NDC.y + 1.0f) * (viewHalfHeight) + viewY);
		}

		inline void transformationPipeline3D(const float &x, const float &y, const float &z, int &x_w,  int &y_w, float &z_w)
		{
			float w_n = 1.0f;

			// MVP TRANSFORMATION
			Vector4f NDC = MVP_matrix * Vector4f(x, y, z, w_n);

			// PERSPECTIVE DIVISION
			w_n = 1.0f / w_n;

			NDC.x *= w_n;
			NDC.y *= w_n;
			NDC.z *= w_n;

			// VIEWPORT TRANSFORMATION
			x_w = round((NDC.x + 1.0f) * (viewHalfWidth) + viewX);
			y_w = round((NDC.y + 1.0f) * (viewHalfHeight) + viewY);

			z_w = NDC.z * (-1.0f);
		}

		inline void drawVertexBuffer(void)
		{
			Vertex4f u, v;
			size_t i = 0, size = vertexBuffer.size() - 1;

			switch(elementType)
			{
				case SGL_POINTS:
					while(i < vertexBuffer.size())
					{
						u = vertexBuffer[i++];
						drawPoint(u);
					}

					break;
				
				case SGL_LINES:
					while(i < vertexBuffer.size())
					{
						u = vertexBuffer[i++];
						v = vertexBuffer[i++];

						drawLine(u, v);
					}

					break;
				
				case SGL_LINE_STRIP:
					while(i < size)
					{
						u = vertexBuffer[i++];
						v = vertexBuffer[i];

						drawLine(u, v);
					}

					break;
				
				case SGL_LINE_LOOP:
					while(i < size)
					{
						u = vertexBuffer[i++];
						v = vertexBuffer[i];

						drawLine(u, v);
					}

					drawLine(v, vertexBuffer[0]);

					break;
				
				case SGL_TRIANGLES:
					break;
				
				case SGL_POLYGON:
					break;

				default:
					break;
			}

			vertexBuffer.clear();
		}

		inline void updateMVP_matrix()
		{
			MVP_matrix = P_matrix * MV_matrix;

			isMVP_matrixUpdated = true;
		}
};

class sglContextManager
{
	public:
		static int const MAX_NUM_CONTEXTS = 32;
		static int actualContextIndex;

		int ccID;
		sglContext *cc;
		sglContext *contexts [MAX_NUM_CONTEXTS];

		sglContextManager(void)
		{
			ccID = -1;
			cc = NULL;

			for(int i = 0; i < MAX_NUM_CONTEXTS; i++)
			{
				contexts[i] = NULL;
			}
		}

		~sglContextManager(void)
		{
			for(int i = 0; i < MAX_NUM_CONTEXTS; i++)
			{
				if(contexts[i] != NULL)
				{
					delete contexts[i];
				}
			}
		}
};

int sglContextManager::actualContextIndex = 0;
