#include "SglFacade.h"

#include "Context.h"

SglFacade::SglFacade() {}

int SglFacade::createContext(int width, int height) {
	Context context(width, height);
	contexts.push_back(context);
	return 0;
}
