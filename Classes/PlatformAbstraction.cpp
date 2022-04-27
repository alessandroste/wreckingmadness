#include "PlatformAbstraction.h"

using namespace wreckingmadness;

PlatformAbstraction* PlatformAbstraction::platformInstance = nullptr;

PlatformAbstraction::PlatformAbstraction() {
    assert(!platformInstance);
    PlatformAbstraction::platformInstance = this;
    CCLOG("[PlatformAbstraction] Initialized platform with name %s", getName());
}

PlatformAbstraction::~PlatformAbstraction() {
    assert(this == PlatformAbstraction::platformInstance);
    PlatformAbstraction::platformInstance = nullptr;
}

void PlatformAbstraction::shareImageFromFile(const std::string &fileName) {
    CCLOG("[PlatformAbstraction] Shared image from file %s", fileName.c_str());
}