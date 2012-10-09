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
public:
	Context(int w, int h);
	float* getColorBufferPointer();
};

#endif /* __CONTEXT_H__ */
