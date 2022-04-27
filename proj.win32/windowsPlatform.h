#ifndef _WINDOWSPLATFORM_H_
#define _WINDOWSPLATFORM_H_

#include "PlatformAbstraction.h"

namespace wreckingmadness {
    class WindowsPlatform : PlatformAbstraction {
    protected:
        virtual const char* getName() const override { return "WindowsPlatform"; };
    public:
        WindowsPlatform() {};
        virtual ~WindowsPlatform() {};
        virtual void shareImageFromFile(const std::string& fileName) override {
            CCLOG("[WindowsPlatform] Shared image from file %s", fileName.c_str());
        };
    };
}

#endif // _WINDOWSPLATFORM_H_