#include "Context.h"
#include <stdlib.h>

Context::Context(int w, int h) : width(w), height(h) {
	pixels = (Color*) malloc(sizeof(Color) * width * height);
}

float* Context::getColorBufferPointer() {
	return &(pixels[0].r);
}

void Context::setClearColor(Color color) {
	clearColor = color;
}

void Context::clearColorBuffer() {
	for (int i=0;i<width*height;i++) {
		pixels[i] = clearColor;
	}
}

void Context::clearDepthBuffer() {
}

void Context::setElementType(sglEElementType mode) {
	elementType = mode;
}

void Context::drawElements() {
}

void Context::addVertex(Point3 vertex) {
	vertexBuffer.push_back(vertex);
}
