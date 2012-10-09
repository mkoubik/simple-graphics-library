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