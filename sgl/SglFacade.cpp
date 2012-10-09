#include "SglFacade.h"

#include "Context.h"

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
