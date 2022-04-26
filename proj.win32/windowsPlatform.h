#ifndef _WINDOWSPLATFORM_H_
#define _WINDOWSPLATFORM_H_

#include "PlatformAbstraction.h"

namespace wreckingmadness {
    class WindowsPlatform : PlatformAbstraction {
    public:
        virtual void shareImageFromFile(const std::string& fileName) {
            CCLOG("[WindowsPlatform] Shared image from file %s", fileName.c_str());
        };
    };
}

#endif // _WINDOWSPLATFORM_H_