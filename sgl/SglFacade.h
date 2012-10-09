#ifndef __SGL_FACADE_H__
#define __SGL_FACADE_H__

#include <vector>
#include "Context.h"
#include "sgl.h"

class SglFacade {
private:
	std::vector<Context> contexts;
	int currentContextId;
	Context *currentContext;
public:
	SglFacade();
	int createContext(int width, int height);
	void destroyContext(int id);
	void setContext(int id);
	int getContext();
	float *getColorBufferPointer();
	void clearColor(float r, float g, float b, float alpha);
	void clear(unsigned what);
	void begin(sglEElementType mode);
	void end(void);
	void vertex2f(float x, float y);
	void vertex3f(float x, float y, float z);
	void color3f(float r, float g, float b);
};

#endif /* __SGL_FACADE_H__ */
