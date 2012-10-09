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
	sglEElementType elementType;
	std::vector<Point3> buffer;
	Color clearColor;
public:
	Context(int w, int h);
	float* getColorBufferPointer();
	void setClearColor(Color color);
	void clearColorBuffer();
	void clearDepthBuffer();
};

#endif /* __CONTEXT_H__ */
