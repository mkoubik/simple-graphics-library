#ifndef __SGL_FACADE_H__
#define __SGL_FACADE_H__

#include <vector>
#include "Context.h"

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
};

#endif /* __SGL_FACADE_H__ */
