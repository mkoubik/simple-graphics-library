#include "Context.h"
#include <stdlib.h>
#include <vector>
#include "sgl.h"

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
	if (elementType == SGL_POINTS) {
		drawPoints();
	}
	vertexBuffer.clear();
}

void Context::addVertex(Point3 vertex) {
	vertexBuffer.push_back(vertex);
}

void Context::setColor(Color color) {
	currentColor = color;
}

// private:

void Context::drawPoints() {
	std::vector<Point3>::iterator it;
	int x, y;
	for (it=vertexBuffer.begin(); it<vertexBuffer.end(); it++) {
		x = (int) ((*it).x+1)/2;
		y = (int) ((*it).y+1)/2;
		pixels[width * y + x] = currentColor;
	}
}
