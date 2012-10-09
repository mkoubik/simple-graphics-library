#include "Context.h"
#include <stdlib.h>

Context::Context(int w, int h) : width(w), height(h) {
	pixels = (Color*) malloc(sizeof(Color) * width * height);
}

float* Context::getColorBufferPointer() {
	return &(pixels[0].r);
	// return (float*)pixels;
}
