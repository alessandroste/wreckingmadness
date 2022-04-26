#include "PlatformAbstraction.h"

using namespace wreckingmadness;

PlatformAbstraction* PlatformAbstraction::instance = new PlatformAbstraction();

PlatformAbstraction::PlatformAbstraction() {
    instance = this;
}

PlatformAbstraction* PlatformAbstraction::getInstance() {
    return instance;
}