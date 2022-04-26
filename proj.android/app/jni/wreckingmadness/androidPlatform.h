#ifndef WRECKINGMADNESS_ANDROIDPLATFORM_H
#define WRECKINGMADNESS_ANDROIDPLATFORM_H

#include "PlatformAbstraction.h"

namespace wreckingmadness {
    class AndroidPlatform : public PlatformAbstraction {
    public:
        virtual void shareImageFromFile(const std::string &fileName) {
            CCLOG("[AndroidPlatform] Shared image from file %s", fileName.c_str());
        };
    };
}

#endif //WRECKINGMADNESS_ANDROIDPLATFORM_H
