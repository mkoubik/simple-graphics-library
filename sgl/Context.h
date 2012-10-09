#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <vector>
#include "sgl.h"
#include "types.h"

class Context {
private:
	int width;
	int height;
	Color *pixels;

	float pointSize;
	Color currentColor;
	Color clearColor;
	sglEElementType elementType;
	std::vector<Point3> vertexBuffer;

	void drawPoints();
public:
	Context(int w, int h);
	float* getColorBufferPointer();
	void setClearColor(Color color);
	void clearColorBuffer();
	void clearDepthBuffer();
	void setElementType(sglEElementType mode);
	void drawElements();
	void addVertex(Point3 vertex);
	void setColor(Color color);
};

#endif /* __CONTEXT_H__ */
