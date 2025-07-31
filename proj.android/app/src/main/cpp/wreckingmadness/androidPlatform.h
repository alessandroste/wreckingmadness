#ifndef _ANDROIDPLATFORM_H_
#define _ANDROIDPLATFORM_H_

#include "PlatformAbstraction.h"

namespace wreckingmadness {
    class AndroidPlatform : PlatformAbstraction {
    protected:
        virtual const char* getName() const override { return "AndroidPlatform"; };
    public:
        AndroidPlatform() {};
        virtual ~AndroidPlatform() {};
        virtual void shareImageFromFile(const std::string &fileName) override;
    };
}

#endif //_ANDROIDPLATFORM_H_
