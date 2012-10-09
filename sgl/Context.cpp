#include "Context.h"

Context::Context(int w, int h) : width(w), height(h) {}

float* Context::getColorBufferPointer() {
	return &(pixels[0].r);
	// return (float*)pixels;
}
