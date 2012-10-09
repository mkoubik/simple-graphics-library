#ifndef __SGL_FACADE_H__
#define __SGL_FACADE_H__

#include <vector>
#include "Context.h"

class SglFacade {
private:
	std::vector<Context> contexts;
public:
	SglFacade();
	int createContext(int width, int height);
};

#endif /* __SGL_FACADE_H__ */
