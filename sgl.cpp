//---------------------------------------------------------------------------
// sgl.cpp
// Empty implementation of the SGL (Simple Graphics Library)
// Date:  2011/11/1
// Author: Jaroslav Krivanek, Jiri Bittner CTU Prague
//---------------------------------------------------------------------------

#include "sgl.h"
#include "context.h"

using namespace std;

bool sglBeginCalled = false;

/// Current error code.
static sglEErrorCode _libStatus = SGL_NO_ERROR;

static inline void setErrCode(sglEErrorCode c) 
{
  if(_libStatus==SGL_NO_ERROR)
    _libStatus = c;
}

//---------------------------------------------------------------------------
// sglGetError()
//---------------------------------------------------------------------------
sglEErrorCode sglGetError(void) 
{
  sglEErrorCode ret = _libStatus;
  _libStatus = SGL_NO_ERROR;
  return ret;
}

//---------------------------------------------------------------------------
// sglGetErrorString()
//---------------------------------------------------------------------------
const char* sglGetErrorString(sglEErrorCode error)
{
  static const char *errStrigTable[] = 
  {
      "Operation succeeded",
      "Invalid argument(s) to a call",
      "Invalid enumeration argument(s) to a call",
      "Invalid call",
      "Quota of internal resources exceeded",
      "Internal library error",
      "Matrix stack overflow",
      "Matrix stack underflow",
      "Insufficient memory to finish the requested operation"
  };

  if((int)error<(int)SGL_NO_ERROR || (int)error>(int)SGL_OUT_OF_MEMORY ) {
    return "Invalid value passed to sglGetErrorString()"; 
  }

  return errStrigTable[(int)error];
}

/*------------------------------ MY VARIABLES ------------------------------*/
sglContextManager *sglCM;
int x_w, y_w;
float z_w;

// Hodnota 2 * PI
const float M_2PI = (float)(2 * M_PI);

// Pocet segmentu elipsy
const int ELLIPSE_SEGMENTS = 40; 

bool isFillingPossible = false;				// Rozhoduje, zda bude volana metoda fillElement() -> isFillingPossible = true (napr. SGL_POLYGON), 

// Predpocitane hodnoty sin(9.0f * M_PI / 180 * i) pro vykreslovani elipsy -> 9.0f protoze 360 / 40
const static float ellipseSinTable[ELLIPSE_SEGMENTS] = {
 0.000000e+0f,	1.564345e-1f,	3.090170e-1f,	4.539905e-1f,	5.877852e-1f,
 7.071068e-1f,	8.090170e-1f,	8.910065e-1f,	9.510565e-1f,	9.876884e-1f,
 1.000000e+0f,	9.876884e-1f,	9.510565e-1f,	8.910065e-1f,	8.090170e-1f,
 7.071068e-1f,	5.877852e-1f,	4.539906e-1f,	3.090170e-1f,	1.564344e-1f,
-8.742278e-8f, -1.564344e-1f,  -3.090170e-1f,  -4.539905e-1f,  -5.877854e-1f,
-7.071067e-1f, -8.090171e-1f,  -8.910065e-1f,  -9.510565e-1f,  -9.876883e-1f,
-1.000000e+0f, -9.876883e-1f,  -9.510565e-1f,  -8.910064e-1f,  -8.090171e-1f,
-7.071069e-1f, -5.877853e-1f,  -4.539905e-1f,  -3.090169e-1f,  -1.564344e-1f
};

// Predpocitane hodnoty cos(9.0f * M_PI / 180 * i) pro vykreslovani elipsy -> 9.0f protoze 360 / 40
const static float ellipseCosTable[ELLIPSE_SEGMENTS] = {
 1.000000e+0f,	9.876884e-1f,	9.510565e-1f,	8.910065e-1f,	8.090170e-1f,
 7.071068e-1f,	5.877852e-1f,	4.539905e-1f,	3.090170e-1f,	1.564345e-1f,
-4.371139e-8f, -1.564344e-1f,  -3.090170e-1f,  -4.539906e-1f,  -5.877852e-1f,
-7.071068e-1f, -8.090171e-1f,  -8.910065e-1f,  -9.510565e-1f,  -9.876884e-1f,
-1.000000e+0f, -9.876884e-1f,  -9.510565e-1f,  -8.910065e-1f,  -8.090169e-1f,
-7.071068e-1f, -5.877851e-1f,  -4.539907e-1f,  -3.090171e-1f,  -1.564345e-1f,
 1.192488e-8f,	1.564345e-1f,	3.090171e-1f,	4.539907e-1f,	5.877851e-1f,
 7.071066e-1f,	8.090169e-1f,	8.910065e-1f,	9.510565e-1f,	9.876884e-1f
};

/*--------------------------------------------------------------------------*/

//---------------------------------------------------------------------------
// Initialization functions
//---------------------------------------------------------------------------

void sglInit(void)
{
	try
	{
		sglCM = new sglContextManager();
	}
	catch(bad_alloc)
	{
		setErrCode(SGL_OUT_OF_MEMORY);
	}
}

void sglFinish(void)
{
	delete sglCM;
	sglCM = NULL;
}

int sglCreateContext(int width, int height)
{
	/*
	if(sglCM->actualContextIndex == sglCM->MAX_NUM_CONTEXTS)
		setErrCode(SGL_OUT_OF_RESOUCES);
	*/

	try
	{
		sglCM->contexts[sglCM->actualContextIndex] = new sglContext(height, width);
	}
	catch(bad_alloc)
	{
		setErrCode(SGL_OUT_OF_MEMORY);
	}

	sglCM->actualContextIndex++;

	return (sglCM->actualContextIndex - 1);
}

void sglDestroyContext(int id)
{
	if(sglCM->contexts[id] == NULL)
		setErrCode(SGL_INVALID_VALUE);

	delete sglCM->contexts[id];
	sglCM->contexts[id] = NULL;
}

void sglSetContext(int id)
{
	if(sglCM->contexts[id] == NULL)
		setErrCode(SGL_INVALID_VALUE);

	sglCM->ccID = id;
	sglCM->cc = sglCM->contexts[id];
}

int sglGetContext(void)
{
	if(sglCM->cc == NULL)
		setErrCode(SGL_INVALID_OPERATION);

	return sglCM->ccID;
}

float *sglGetColorBufferPointer(void)
{
	if(sglCM->cc == NULL)
		return 0;

	return (float *) sglCM->cc->colorBuffer;
}

//---------------------------------------------------------------------------
// Drawing functions
//---------------------------------------------------------------------------

void sglClearColor (float r, float g, float b, float alpha)
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	sglCM->cc->clearColor = Color(r, g, b);
}

void sglClear(unsigned what)
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	switch(what)
	{
		case SGL_COLOR_BUFFER_BIT:
			sglCM->cc->clearColorBuffer();
			break;
		
		case SGL_DEPTH_BUFFER_BIT:
			sglCM->cc->clearDepthBuffer();
			break;

		case SGL_COLOR_BUFFER_BIT + SGL_DEPTH_BUFFER_BIT:
			sglCM->cc->clearColorBuffer();
			sglCM->cc->clearDepthBuffer();
			break;
		
		default:
			setErrCode(SGL_INVALID_VALUE);
			break;
	}
}

void sglBegin(sglEElementType mode)
{
	if(sglBeginCalled)
		setErrCode(SGL_INVALID_OPERATION);

	if(mode < SGL_POINTS || mode >= SGL_LAST_ELEMENT_TYPE)
		setErrCode(SGL_INVALID_ENUM);

	sglBeginCalled = true;
	sglCM->cc->elementType = mode;
}

void sglEnd(void)
{
	if(!sglBeginCalled)
		setErrCode(SGL_INVALID_OPERATION);

	sglCM->cc->drawVertexBuffer();
	sglBeginCalled = false;
}

void sglVertex4f(float x, float y, float z, float w)
{
	
}

void sglVertex3f(float x, float y, float z)
{
	if (sglCM->cc->sceneDefinition) {
		sglCM->cc->vertexBuffer.push_back(Vertex4f(x, y, z));
	} else {
		sglCM->cc->transformationPipeline3D(x, y, z, x_w, y_w, z_w);
		sglCM->cc->vertexBuffer.push_back(Vertex4f(x_w, y_w, z_w));
	}
}

void sglVertex2f(float x, float y)
{
	sglCM->cc->transformationPipeline2D(x, y, x_w, y_w);

	sglCM->cc->vertexBuffer.push_back(Vertex4f(x_w, y_w));
}

void sglCircle(float x, float y, float z, float radius)
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	if (radius < 0.0f)
		setErrCode(SGL_INVALID_VALUE);

	// Transformace stredu kruznice
	sglCM->cc->transformationPipeline3D(x, y, z, x_w, y_w, z_w);

	// Vypocet polomeru - Radius is scaled by the overall scale [x,y] of the concatenated
    // (modelview o projection o viewport) tranformation. This scaling
    // factor is computed as the square root of the determinant of the 2x2 upper left
    // block of the contatenated transformation matrix
	const float det = (sglCM->cc->viewHalfWidth  * sglCM->cc->MVP_matrix.values[0][0]) *
					  (sglCM->cc->viewHalfHeight * sglCM->cc->MVP_matrix.values[1][1]) -
					  (sglCM->cc->viewHalfHeight * sglCM->cc->MVP_matrix.values[0][1]) *
					  (sglCM->cc->viewHalfWidth  * sglCM->cc->MVP_matrix.values[1][0]);
	const int radius_scaled = round(radius * sqrtf(det));

	int	x_c = 0;
	int	y_c = radius_scaled;
	int	dvex = 3;
	int	dvey = 2 * radius_scaled - 2;
	int	p = 1 - radius_scaled;

	switch (sglCM->cc->areaMode)
	{
		case SGL_POINT:
			break;

		case SGL_LINE:
			sglCM->cc->elementType = SGL_POINTS;
			sglCM->cc->pointSize = 1;

			while(x_c <= y_c)
			{
				sglCM->cc->vertexBuffer.push_back(Vertex4f( x_c + x_w,  y_c + y_w));
				sglCM->cc->vertexBuffer.push_back(Vertex4f(-x_c + x_w,  y_c + y_w));
				sglCM->cc->vertexBuffer.push_back(Vertex4f( y_c + x_w,  x_c + y_w));
				sglCM->cc->vertexBuffer.push_back(Vertex4f(-y_c + x_w,  x_c + y_w));
				sglCM->cc->vertexBuffer.push_back(Vertex4f( x_c + x_w, -y_c + y_w));
				sglCM->cc->vertexBuffer.push_back(Vertex4f(-x_c + x_w, -y_c + y_w));
				sglCM->cc->vertexBuffer.push_back(Vertex4f( y_c + x_w, -x_c + y_w));
				sglCM->cc->vertexBuffer.push_back(Vertex4f(-y_c + x_w, -x_c + y_w));
          
				if(p > 0)
				{
					p = p - dvey;
					dvey = dvey - 2;
					y_c--;
				}

				p += dvex;
				dvex += 2;
				x_c++;
			}

			sglCM->cc->drawVertexBuffer();
			break;

		case SGL_FILL:
			while(x_c <= y_c)
			{
				sglCM->cc->vertexBuffer.push_back(Vertex4f( x_c + x_w,  y_c + y_w));
				sglCM->cc->vertexBuffer.push_back(Vertex4f(-x_c + x_w,  y_c + y_w));
				sglCM->cc->vertexBuffer.push_back(Vertex4f( y_c + x_w,  x_c + y_w));
				sglCM->cc->vertexBuffer.push_back(Vertex4f(-y_c + x_w,  x_c + y_w));
				sglCM->cc->vertexBuffer.push_back(Vertex4f( x_c + x_w, -y_c + y_w));
				sglCM->cc->vertexBuffer.push_back(Vertex4f(-x_c + x_w, -y_c + y_w));
				sglCM->cc->vertexBuffer.push_back(Vertex4f( y_c + x_w, -x_c + y_w));
				sglCM->cc->vertexBuffer.push_back(Vertex4f(-y_c + x_w, -x_c + y_w));

				if(p > 0)
				{
					p = p - dvey;
					dvey = dvey - 2;
					y_c--;
				}

				p += dvex;
				dvex += 2;
				x_c++;
			}

			sglCM->cc->fillCircle();

			break;

		default:
			exit(1);
			break;
	}

}

void sglEllipse(float x, float y, float z, float a, float b)
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	if (a < 0.0f || b < 0.0f)
		setErrCode(SGL_INVALID_VALUE);

	float x_e = a + x;
	float y_e = y;

	switch (sglCM->cc->areaMode)
	{
		case SGL_POINT:
			break;

		case SGL_LINE:
			// Rozdeleni elipsy na pozadovany pocet segmentu a ulozeni jako SGL_LINE_LOOP
			sglBegin(SGL_LINE_LOOP);
				sglVertex2f(x_e, y_e);

				for (int i = 1; i < ELLIPSE_SEGMENTS; i++)
				{
					x_e = ellipseCosTable[i] * a + x;
					y_e = ellipseSinTable[i] * b + y;

					sglVertex2f(x_e, y_e);
				}
			sglEnd();
			break;

		case SGL_FILL:
			// Vykresleni elipsy jako SGL_POLYGON
			sglBegin(SGL_POLYGON);
				sglVertex2f(x_e, y_e);

				for (int i = 1; i < ELLIPSE_SEGMENTS; i++)
				{
					x_e = ellipseCosTable[i] * a + x;
					y_e = ellipseSinTable[i] * b + y;

					sglVertex2f(x_e, y_e);
				}
			sglEnd();

			break;

		default:
			exit(1);
			break;
	}
}

void sglArc(float x, float y, float z, float radius, float from, float to)
{
	if(radius < 0.0f)
		setErrCode(SGL_INVALID_VALUE);

	int numSegment = (int)(ceilf(40.0f * (to - from) / (M_2PI)));

	float angle = from;
	float d_angle = (1.0f / (float)(numSegment - 1)) * (to - from);

	float x_a = cosf(angle) * radius + x;
	float y_a = sinf(angle) * radius + y;

	switch (sglCM->cc->areaMode)
	{
		case SGL_POINT:
			break;

		case SGL_LINE:
			// Rozdeleni oblouku na odpovidajici pocet segmentu a ulozeni jako SGL_LINE_STRIP
			sglBegin(SGL_LINE_STRIP);
				sglVertex2f(x_a, y_a);

				for (int i = 1; i < numSegment; i++)
				{			
					angle += d_angle;
					x_a = cosf(angle) * radius + x;
					y_a = sinf(angle) * radius + y;

					sglVertex2f(x_a, y_a);
				}
			sglEnd();
			break;

		case SGL_FILL:
			// Vykresleni oblouku jako SGL_POLYGON
			sglBegin(SGL_POLYGON);
				sglVertex2f(x_a, y_a);

				for (int i = 1; i < numSegment; i++)
				{			
					angle += d_angle;
					x_a = cosf(angle) * radius + x;
					y_a = sinf(angle) * radius + y;

					sglVertex2f(x_a, y_a);
				}

				sglVertex2f(x, y);
			sglEnd();
			break;

		default:
			exit(1);
			break;
	}

}

//---------------------------------------------------------------------------
// Transform functions
//---------------------------------------------------------------------------

void sglMatrixMode( sglEMatrixMode mode )
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);
	
	// Podle sglEMatrixMode matrixMode rozhodujeme, jaka bude aktualni matice
	switch(mode)
	{
		// PROJECTION MATRIX
		case SGL_MODELVIEW:
			sglCM->cc->matrixMode = SGL_MODELVIEW;
			sglCM->cc->matrix = &sglCM->cc->MV_matrix;
			break;
		// PROJECTION MATRIX
		case SGL_PROJECTION:
			sglCM->cc->matrixMode = SGL_PROJECTION;
			sglCM->cc->matrix = &sglCM->cc->P_matrix;
			break;
		default:
			setErrCode(SGL_INVALID_ENUM);
			break;
	}
}

void sglPushMatrix(void)
{
	if(sglCM->cc->transformationBuffer.size() == sglContext::MAX_NUM_MATRICES)
		setErrCode(SGL_STACK_OVERFLOW);

	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	sglCM->cc->transformationBuffer.push(*sglCM->cc->matrix);
}

void sglPopMatrix(void)
{
	if(sglCM->cc->transformationBuffer.size() == 0)
		setErrCode(SGL_STACK_UNDERFLOW);

	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	// Podle sglEMatrixMode matrixMode rozhodujeme, do jake matice bude ulozen vrchol zasobniku
	// MODELVIEW MATRIX
	if (sglCM->cc->matrixMode == SGL_MODELVIEW)
	{
		sglCM->cc->MV_matrix = sglCM->cc->transformationBuffer.top();
	}
	// PROJECTION MATRIX
	else
	{
		sglCM->cc->P_matrix = sglCM->cc->transformationBuffer.top();
	}

	sglCM->cc->transformationBuffer.pop();

	sglCM->cc->MVP_matrixUpdated = false;
}

void sglLoadIdentity(void)
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			sglCM->cc->matrix->values[i][j] = 0.0f;
		}
	}

	sglCM->cc->matrix->values[0][0] = 1.0f;
	sglCM->cc->matrix->values[1][1] = 1.0f;
	sglCM->cc->matrix->values[2][2] = 1.0f;
	sglCM->cc->matrix->values[3][3] = 1.0f;

	sglCM->cc->MVP_matrixUpdated = false;
}

void sglLoadMatrix(const float *matrix)
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	sglCM->cc->matrix->loadMatrix4f(matrix);

	sglCM->cc->MVP_matrixUpdated = false;
}

void sglMultMatrix(const float *matrix)
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	Matrix4f temp = *sglCM->cc->matrix;

	*sglCM->cc->matrix = temp * matrix;

	sglCM->cc->MVP_matrixUpdated = false;
}

void sglTranslate(float x, float y, float z)
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	for (int i = 0; i < 4; i++)
	{
		sglCM->cc->matrix->values[i][3]	= sglCM->cc->matrix->values[i][0] * x
										+ sglCM->cc->matrix->values[i][1] * y
										+ sglCM->cc->matrix->values[i][2] * z
										+ sglCM->cc->matrix->values[i][3];		
	}

	sglCM->cc->MVP_matrixUpdated = false;
}

void sglScale(float scalex, float scaley, float scalez)
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	for (int i = 0; i < 4; i++)
	{
		sglCM->cc->matrix->values[i][0] *= scalex;
		sglCM->cc->matrix->values[i][1] *= scaley;
		sglCM->cc->matrix->values[i][2] *= scalez;
	}

	sglCM->cc->MVP_matrixUpdated = false;
}

void sglRotate2D(float angle, float centerx, float centery)
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	float coeff1 = cos(angle);
	float coeff2 = -sin(angle);
	float coeff3 = -coeff2;

	Matrix4f temp = *sglCM->cc->matrix;

	// Vynasobeni slozenou matici: T * R * T_inverse
	for (int i = 0; i < 4; i++)
	{		
		sglCM->cc->matrix->values[i][0]	=  temp.values[i][0] * coeff1 + temp.values[i][1] * coeff3;
		sglCM->cc->matrix->values[i][1]	=  temp.values[i][0] * coeff2 + temp.values[i][1] * coeff1;
		sglCM->cc->matrix->values[i][3]	=  temp.values[i][0] * (centerx - centerx * coeff1 + centery * coeff3) 
										+  temp.values[i][1] * (centery - centerx * coeff3 - centery * coeff1)
										+  temp.values[i][3];		
	}

	sglCM->cc->MVP_matrixUpdated = false;
}

void sglRotateY(float angle)
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	float coeff1 = cosf(angle);
	float coeff2 = sinf(angle);
	float coeff3 = -sinf(angle);

	float values[4][4];

	// Soucin aktualni matice kontextu matrixCurrent a matice rotace podle osy Y
	for (int i = 0; i < 4; i++)
	{
		values[i][0] = sglCM->cc->matrix->values[i][0] * coeff1
					 + sglCM->cc->matrix->values[i][2] * coeff2;
		values[i][1] = sglCM->cc->matrix->values[i][1];
		values[i][2] = sglCM->cc->matrix->values[i][0] * coeff3 
					 + sglCM->cc->matrix->values[i][2] * coeff1;
		values[i][3] = sglCM->cc->matrix->values[i][3];	
	}
	
	memcpy(sglCM->cc->matrix->values, values, sizeof(values));

	sglCM->cc->MVP_matrixUpdated = false;
}

void sglOrtho(float left, float right, float bottom, float top, float near, float far)
{
	// Koeficienty na diagonale matice
	float coeff1 = 2.0f / (right - left);
	float coeff2 = 2.0f / (top - bottom);
	float coeff3 = -2.0f / (far - near);

	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	float tz = -(far + near) / (far - near);

	Matrix4f temp = *sglCM->cc->matrix;

	for (int i = 0; i < 4; i++)
	{		
		sglCM->cc->matrix->values[i][0] = temp.values[i][0] * coeff1;
		sglCM->cc->matrix->values[i][1] = temp.values[i][1] * coeff2;
		sglCM->cc->matrix->values[i][2] = temp.values[i][2] * coeff3;
		sglCM->cc->matrix->values[i][3] = temp.values[i][0] * tx + 
										  temp.values[i][1] * ty + 
										  temp.values[i][2] * tz + 
										  temp.values[i][3];		
	}

	sglCM->cc->MVP_matrixUpdated = false;
}

void sglFrustum(float left, float right, float bottom, float top, float near, float far)
{
	float coeff1 = (2.0f * near) / (right - left);
	float coeff2 = (right + left) / (right - left);
	float coeff3 = (2.0f * near) / (top - bottom);
	float coeff4 = (top + bottom) / (top - bottom);
	float coeff5 = -(far + near) / (far - near);
	float coeff6 = -2.0f * (far * near) / (far - near);

	float values[4][4] = {
							{coeff1,     0.0f,   coeff2,     0.0f},
							{  0.0f,   coeff3,   coeff4,     0.0f},
							{  0.0f,     0.0f,   coeff5,   coeff6},
							{  0.0f,     0.0f,    -1.0f,     0.0f}
						 };

	memcpy(sglCM->cc->matrix->values, values, sizeof(values));

	sglCM->cc->MVP_matrixUpdated = false;
}

void sglViewport(int x, int y, int width, int height)
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	if (width < 0 || height < 0)
	{
		setErrCode(SGL_INVALID_VALUE);
	}
	else
	{
		sglCM->cc->viewX = x;
		sglCM->cc->viewY = y;

		sglCM->cc->viewHalfHeight = (float)(height / 2.0f);
		sglCM->cc->viewHalfWidth = (float)(width / 2.0f);
	}
}

//---------------------------------------------------------------------------
// Attribute functions
//---------------------------------------------------------------------------

void sglColor3f(float r, float g, float b)
{
	sglCM->cc->color = Color(r, g, b);
}

void sglAreaMode(sglEAreaMode mode)
{
	sglCM->cc->areaMode = mode;
}

void sglPointSize(float size)
{
	sglCM->cc->pointSize = (int)size;
}

void sglEnable(sglEEnableFlags cap) 
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	if(cap == SGL_DEPTH_TEST)
		sglCM->cc->depthTestEnabled = true;
	else
		setErrCode(SGL_INVALID_ENUM);
}

void sglDisable(sglEEnableFlags cap) 
{
	if(sglCM->cc == NULL || sglBeginCalled == true)
		setErrCode(SGL_INVALID_OPERATION);

	if(cap == SGL_DEPTH_TEST)
		sglCM->cc->depthTestEnabled = false;
	else
		setErrCode(SGL_INVALID_ENUM);
}

//---------------------------------------------------------------------------
// RayTracing oriented functions
//---------------------------------------------------------------------------

void sglBeginScene()
{
	sglCM->cc->beginScene();
}

void sglEndScene()
{
	sglCM->cc->endScene();
}

void sglSphere(const float x,
			   const float y,
			   const float z,
			   const float radius)
{
	sglCM->cc->addSphere(x, y, z, radius);
}

void sglMaterial(const float r,
				 const float g,
				 const float b,
				 const float kd,
				 const float ks,
				 const float shine,
				 const float T,
				 const float ior) {}

void sglPointLight(const float x,
				   const float y,
				   const float z,
				   const float r,
				   const float g,
				   const float b) {}

void sglRayTraceScene()
{
	sglCM->cc->raytraceScene();
}

void sglRasterizeScene() {}

void sglEnvironmentMap(const int width,
					   const int height,
					   float *texels)
{}

void sglEmissiveMaterial(
						 const float r,
						 const float g,
						 const float b,
						 const float c0,
						 const float c1,
						 const float c2
						 )
{}

