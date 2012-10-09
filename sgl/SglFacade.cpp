#include "SglFacade.h"

#include "Context.h"
#include "types.h"
#include "sgl.h"

SglFacade::SglFacade() {}

int SglFacade::createContext(int width, int height) {
	Context context(width, height);
	int id = contexts.size();
	contexts.push_back(context);
	return id;
}

void SglFacade::destroyContext(int id) {
	contexts.erase(contexts.begin() + id);
}

void SglFacade::setContext(int id) {
	currentContextId = id;
	currentContext = &contexts.at(id);
}

int SglFacade::getContext() {
	return currentContextId;
}

float *SglFacade::getColorBufferPointer() {
	return (*currentContext).getColorBufferPointer();
}

void SglFacade::clearColor(float r, float g, float b, float alpha) {
	Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	(*currentContext).setClearColor(color);
}

void SglFacade::clear(unsigned what) {
	if (what && SGL_COLOR_BUFFER_BIT) {
		(*currentContext).clearColorBuffer();
	}
	if (what && SGL_DEPTH_BUFFER_BIT) {
		(*currentContext).clearDepthBuffer();
	}
}

void SglFacade::begin(sglEElementType mode) {
	(*currentContext).setElementType(mode);
}

void SglFacade::end(void) {
	(*currentContext).drawElements();
}

void SglFacade::vertex2f(float x, float y) {
	Point3 vertex;
	vertex.x = x;
	vertex.y = y;
	vertex.z = 0;
	(*currentContext).addVertex(vertex);
}

void SglFacade::vertex3f(float x, float y, float z) {
	Point3 vertex;
	vertex.x = x;
	vertex.y = y;
	vertex.z = z;
	(*currentContext).addVertex(vertex);
}

void SglFacade::color3f(float r, float g, float b) {
	Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	(*currentContext).setColor(color);
}
