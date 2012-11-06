//---------------------------------------------------------------------------
// context.h
// Additional header file for SGL (Simple Graphics Library)
// Date:  2012/09/28
// Author: Jan Fikar
//---------------------------------------------------------------------------

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#define _USE_MATH_DEFINES

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <map>
#include <stack>
#include <vector>

#include "common.h"
#include "raytracing.h"

using namespace std;

class sglContext
{
	private:
		map <int, vector <Edge*> > tableEdges;
		vector <Edge*> listActualEdges;


		inline void drawPoint(const Vertex4f &u)
		{
			// Vykresleni bodu dle zadane hodnoty pointSize, kde hodnoty x, y jsou brany
			// jako "stred" bodu a dle vypocteneho offsetu, je tento stred "obalen" dalsimi
			// body
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
			int x, d_x, y, d_y, k1, k2, p, step;
			float m;

			d_x = v.x - u.x;
			d_y = v.y - u.y;

			m = (float)d_y / d_x;
		
			// Pokud se nejedna o vodorovnou nebo svislou usecku
			if (d_x != 0 && d_y != 0)
			{
				// Ridici osou je osa X
				if(fabs(m) <= 1.0f)
				{
					// Pokud nutno, prohozeni pocatecniho a koncoveho bodu z duvodu
					// vzorkovani zlevo doprava
					if(d_x < 0)
					{
						Vertex4f temp = u;
						u = v;
						v = temp;
								
						d_x = -d_x;
						d_y = -d_y;
					}

					// Podle hodnot bodu na ose Y menime krok - zda budeme pricitat,
					// nebo odcitat
					if(d_y < 0)
					{
						step = -1;
						d_y = -d_y;
					}
					else
					{
						step = 1;
					}

					// Pocatecni nastaveni konstant
					k1 = 2 * d_y;
					k2 = 2 * (d_y - d_x);

					p = 2 * d_y - d_x;
					x = u.x;
					y = u.y;

					setPixel(x, y);

					while(x < v.x)
					{
						x++;
						if(p >= 0)
						{
							y += step;
							p += k2;
						}
						else
						{
							p += k1;
						}

						setPixel(x, y);
					}
				}
				// Ridici osou je osa Y
				else
				{
					// Pokud nutno, prohozeni pocatecniho a koncoveho bodu z duvodu
					// vzorkovani zdola nahoru
					if(d_y < 0)
					{
						Vertex4f temp = u;
						u = v;
						v = temp;
								
						d_x *= -1;
						d_y *= -1;
					}

					// Podle hodnot bodu na ose X menime krok - zda budeme pricitat,
					// nebo odcitat
					if(d_x < 0)
					{
						step = -1;
						d_x = -d_x;
					}
					else
					{
						step = 1;
					}

					// Pocatecni nastaveni konstant
					k1 = 2 * d_x;
					k2 = 2 * (d_x - d_y);

					p = 2 * d_x - d_y;
					x = u.x;
					y = u.y;

					setPixel(x, y);

					while(y < v.y)
					{
						y++;
						if(p >= 0)
						{
							x += step;
							p += k2;
						}
						else
						{
							p += k1;
						}

						setPixel(x, y);
					}
				}
			}
			else
			{
				// Jedna se o svislou usecku
				if(d_x == 0)
				{
					if(u.y > v.y)
					{
						Vertex4f temp = u;
						u = v;
						v = temp;
					}

					while(u.y < v.y)
					{
						setPixel(u.x, u.y);
						u.y++;
					}
				}
				// Jedna se o vodorovnou usecku
				else
				{
					if(u.x > v.x)
					{
						Vertex4f temp = u;
						u = v;
						v = temp;
					}

					while(u.x < v.x)
					{
						setPixel(u.x, u.y);
						u.x++;
					}
				}
			}
		}

		inline void drawFillingLine(Vertex4f u, Vertex4f v)
		{
			// Vodorovna usecka //
			int index = u.x + u.y * width;
			if(u.x < v.x)
			{
				while(u.x < v.x)
				{
					colorBuffer[index] = color;

					u.x++;
					index++;
				}
			}
			else
			{
				while(v.x < u.x)
				{
					colorBuffer[index] = color;

					u.x--;
					index--;
				}
			}
		}

		inline void drawFillingLine(Vertex4f u, Vertex4f v, float depth, float depthStep)
		{
			// Vodorovna usecka //
			int index = u.x + u.y * width;
			if(u.x < v.x)
			{
				while(u.x < v.x)
				{
					if (depth > depthBuffer[index])
					{
						colorBuffer[index] = color;
						depthBuffer[index] = depth;
					}

					depth += depthStep;
					u.x++;
					index++;
				}
			}
			else
			{
				while(v.x < u.x)
				{
					if (depth > depthBuffer[index])
					{
						colorBuffer[index] = color;
						depthBuffer[index] = depth;
					}
					depth += depthStep;
					u.x--;
					index--;
				}
			}
		}

		inline void addEdge(Vertex4f u, Vertex4f v)
		{
			float d_x, d_y, d_z, smernice, d_xe, d_ze;

			if(depthTestEnabled)
			{				// Pokud treba -> prohozeni bodu tak, aby platila orientace smerem shora dolu //
				if(u.y < v.y)
				{
					Vertex4f temp = u;
					u = v;
					v = temp;
				}

				// Vypocet deltaX, deltaY a deltaZ //
				d_x = (float)(v.x - u.x);
				d_y = (float)(v.y - u.y);
				d_z = v.z - u.z;

				// Pokud deltaY = 0 -> vodorovna usecka -> nevykreslujeme //
				if(d_y != 0.0f)
				{
					// Pokud deltaX = 0 -> svisla usecka -> pristek X pri prechodu na dalsi radek = 0 //
					if(d_x == 0)
					{
						d_xe = 0.0f;
					}
					// Jinak vypocet smernice -> pristek X pri prechodu na dalsi radek = jeji prevracena hodnota //
					else
					{
						smernice = d_y / d_x;
						d_xe = -1 / smernice;
					}

					if(d_z == 0.0f)
						d_ze = 0.0f;
					else
						d_ze = d_z / fabs(d_y);

					// Zapis nove hrany do seznamu hran podle hodnoty y1 //
					tableEdges[u.y].push_back(new Edge((float)u.x, d_xe, u.y - v.y - 1, u.z, d_ze));
				}
			}
			else
			{
				// Pokud treba -> prohozeni bodu tak, aby platila orientace smerem shora dolu //
				if(u.y < v.y)
				{
					Vertex4f temp = u;
					u = v;
					v = temp;
				}

				// Vypocet deltaX a deltaY //
				d_x = (float)(v.x - u.x);
				d_y = (float)(v.y - u.y);

				// Pokud deltaY = 0 -> vodorovna usecka -> nevykreslujeme //
				if(d_y != 0.0f)
				{
					// Pokud deltaX = 0 -> svisla usecka -> pristek X pri prechodu na dalsi radek = 0 //
					if(d_x == 0)
					{
						d_xe = 0.0f;
					}
					// Jinak vypocet smernice -> pristek X pri prechodu na dalsi radek = jeji prevracena hodnota //
					else
					{
						smernice = d_y / d_x;
						d_xe = -1 / smernice;
					}
					// Zapis nove hrany do seznamu hran podle hodnoty y1 //
					tableEdges[u.y].push_back(new Edge((float)u.x, d_xe, u.y - v.y - 1));
				}
			}
		}

		inline void createEdges(void)
		{
			size_t i = 0, size = vertexBuffer.size() - 1;
			Vertex4f u, v;

			while(i < size)
			{
				u = vertexBuffer[i++];
				v = vertexBuffer[i];

				addEdge(u, v);
			}

			addEdge(v, vertexBuffer[0]);
		}

		inline void fillElement(void)
		{
			int x1, x2;
			float z1, z2, d_z, depthStep;

			int y;																	// Aktualni hodnota y radkoveho vyplnovani
			int y_next;																// Nasledujici nejblizsi hodnota y, kdy se pridaji dalsi hrany do seznamu aktualnich hran
			bool sorted;															// Rozhoduje, zda je seznam hran jiz serazen -> isSorted = true (urychleni razeni)
			map<int, vector <Edge*> >::reverse_iterator it = tableEdges.rbegin();	// Iterator ukazujici na konec tabulky tableEdges (prochazime od nejvyssi hodnoty y)

			y = it->first;															// y = nejvyssi hodnota y z tabulky tableEdges
			listActualEdges = tableEdges[y];										// Nahrani hran z nejvyssiho radku tabulky tableEdges do seznamu aktualnich hran listActualEdges
			tableEdges.erase(y);													// Odstraneni hran z tabulky listActualEdges
	
			// Pokud neni tabulka prazdna -> y_next = hodnota y, na ktere budeme pridavat dalsi hrany do listActualEdges //
			if(!tableEdges.empty())
			{
				y_next = it->first;
			}
			// Jinak nastavime na nejmensi moznou hodnotu -> 0 //
			else
			{
				y_next = 0;
			}

			// Prochazime tak dlouho, dokud neni prazdna jak tabulka tableEdges, tak seznam listActualEdges //
			while(!tableEdges.empty() || !listActualEdges.empty())
			{
				// Pokud jsme na radku s novymi hranami, musime je nahrat do seznamu listActualEdges//
				if(y == y_next)
				{
					listActualEdges.insert(listActualEdges.end(), 
					tableEdges[y].begin(), tableEdges[y].end());					// Nahrani hran z aktualne nejvyssiho radku tabulky tableEdges na konec seznamu aktualnich hran listActualEdges
					tableEdges.erase(y);											// Odstraneni hran z tabulky listActualEdges

					// Pokud neni tabulka prazdna -> y_next = hodnota y, na ktere budeme pridavat dalsi hrany do listActualEdges //
					if(!tableEdges.empty())
					{
						y_next = it->first;
					}
				}

				Edge* temp;
				// SHAKER SORT //
				/*
				int size = listActualEdges.size();
				for(int i = 0; i < size / 2; i++)
				{
					isSorted = true;
					for(int j = i; j < size - i - 1; j++)
					{
						if(listActualEdges[j]->x < listActualEdges[j + 1]->x)
						{
							temp = listActualEdges[j];
							listActualEdges[j] = listActualEdges[j + 1];
							listActualEdges[j + 1] = temp;
							isSorted = false;
						}
					}
			
					for(int j = size - 2 - i; j > i; j--)
					{
						if(listActualEdges[j]->x > listActualEdges[j - 1]->x)
						{
							temp = listActualEdges[j];
							listActualEdges[j] = listActualEdges[j - 1];
							listActualEdges[j - 1] = temp;
							isSorted = false;
						}
					}

					if(isSorted) break;
				}
				*/

				// BUBBLE SORT //
				for (int i = (listActualEdges.size() - 1); i > 0; i--)
				{
					sorted = true;
					for (int j = 1; j <= i; j++)
					{
						if (listActualEdges[j-1]->x > listActualEdges[j]->x)
						{
							sorted = false;
							temp = listActualEdges[j-1];
							listActualEdges[j-1] = listActualEdges[j];
							listActualEdges[j] = temp;
						}
					}
					// Pokud v pruchodu celym seznam nezmenime pozici ani jednoho prvku -> seznam serazen -> muzeme koncit
					if(sorted)
						break;
				}

				if (depthTestEnabled)
				{
					// Vyplneni elementu na aktualnim radku y (usecku vzdy vykreslime mezi sudym a lichym prvkem) se zohlednenim hloubky //
					for(unsigned int i = 0; i < listActualEdges.size(); i++)
					{				
						x1 = round(listActualEdges[i]->x);
						z1 = listActualEdges[i]->z;

						x2 = round(listActualEdges[++i]->x);
						z2 = listActualEdges[i]->z;

						d_z = z2 - z1;

						if (d_z == 0.0f)
							depthStep = 0.0f;
						else
							depthStep =  d_z / (x2 - x1);

						drawFillingLine(Vertex4f(x1, y), Vertex4f(x2, y), z1, depthStep);
						//drawLine(x1, y, x2, y, z1, depthStep);
					}

					unsigned int i = 0;
					// Prochazime cely seznam listActualEdges a aktualizujeme hodnoty jeho prvku, popr. odstranujeme //
					while(i < listActualEdges.size())
					{
						// Pokud se pocet radku, do nichz hrana zasahuje = 0 -> odstraneni hrany ze seznamu //
						if(listActualEdges[i]->d_y == 0)
						{
							delete listActualEdges[i];
							listActualEdges.erase(listActualEdges.begin() + i);
						}
						// Jinak aktualizujeme hodnotu zbyvajicich hran //
						else
						{
							listActualEdges[i]->d_y--;
							listActualEdges[i]->x += listActualEdges[i]->d_x;
							listActualEdges[i]->z += listActualEdges[i]->d_z;
							i++;
						}
					}
				}
				else
				{
					// Vyplneni elementu na aktualnim radku y (usecku vzdy vykreslime mezi sudym a lichym prvkem) //
					for(unsigned int i = 0; i < listActualEdges.size(); i++)
					{
						x1 = round(listActualEdges[i]->x);
						x2 = round(listActualEdges[++i]->x);
						drawFillingLine(Vertex4f(x1, y), Vertex4f(x2, y));
					}

					unsigned int i = 0;
					// Prochazime cely seznam listActualEdges a aktualizujeme hodnoty jeho prvku, popr. odstranujeme //
					while(i < listActualEdges.size())
					{
						// Pokud se pocet radku, do nichz hrana zasahuje = 0 -> odstraneni hrany ze seznamu //
						if(listActualEdges[i]->d_y == 0)
						{
							delete listActualEdges[i];
							listActualEdges.erase(listActualEdges.begin() + i);
						}
						// Jinak aktualizujeme hodnotu zbyvajicich hran //
						else
						{
							listActualEdges[i]->d_y--;
							listActualEdges[i]->x += listActualEdges[i]->d_x;
							i++;
						}
					}
				}

				y--;
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
		bool MVP_matrixUpdated;
		sglEMatrixMode matrixMode;
		Matrix4f *matrix;
		Matrix4f MV_matrix, P_matrix, MVP_matrix;
		stack<Matrix4f> transformationBuffer;

		// VIEWPORT
		int viewX, viewY;
		float viewHalfHeight, viewHalfWidth;

		sglEAreaMode areaMode;

		float *depthBuffer;
		bool depthTestEnabled;

		sglContext(const int &_height, const int &_width)
		{
			height = _height;
			width = _width;
			colorBuffer = new Color[height * width];
			matrix = NULL;
			MVP_matrixUpdated = false;

			depthBuffer = new float [height * width];

			depthTestEnabled = false;
		}

		~sglContext(void)
		{
			delete [] colorBuffer;

			delete [] depthBuffer;
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

		void clearDepthBuffer(void)
		{
			int dimension = height * width;
			for(int i = 0; i < dimension; i++)
			{
				depthBuffer[i] = -FLT_MAX;
			}
		}

		inline void transformationPipeline2D(const float &x, const float &y, int &x_w,  int &y_w)
		{
			if(!MVP_matrixUpdated)
				updateMVP_matrix();

			// MVP TRANSFORMATION
			Vector4f NDC = MVP_matrix * Vector4f(x, y);

			// VIEWPORT TRANSFORMATION
			x_w = round((NDC.x + 1.0f) * (viewHalfWidth) + viewX);
			y_w = round((NDC.y + 1.0f) * (viewHalfHeight) + viewY);
		}

		inline void transformationPipeline3D(const float &x, const float &y, const float &z, int &x_w,  int &y_w, float &z_w)
		{
			if(!MVP_matrixUpdated)
				updateMVP_matrix();

			float w_n = 1.0f;

			// MVP TRANSFORMATION
			Vector4f NDC = MVP_matrix * Vector4f(x, y, z, w_n);

			// PERSPECTIVE DIVISION
			w_n = 1.0f / NDC.w;

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
					switch (areaMode)
					{
						case SGL_POINT:
							break;

						case SGL_LINE:
							while(i < size)
							{
								u = vertexBuffer[i++];
								v = vertexBuffer[i];

								drawLine(u, v);
							}

							drawLine(v, vertexBuffer[0]);

							break;

						case SGL_FILL:
							createEdges();
							fillElement();

							break;

						default:
							exit(1);
							break;
					}

					break;

				default:
					break;
			}

			vertexBuffer.clear();
		}

		inline void fillCircle(void)
		{
			Vertex4f u, v;
			size_t i = 0;

			while(i < vertexBuffer.size())
			{
				u = vertexBuffer[i++];
				v = vertexBuffer[i++];

				drawLine(u, v);
			}

			vertexBuffer.clear();
		}

		inline void updateMVP_matrix()
		{
			MVP_matrix = P_matrix * MV_matrix;

			MVP_matrixUpdated = true;
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

#endif /* __CONTEXT_H__ */
