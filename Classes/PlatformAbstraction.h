#ifndef _PLATFORMABSTRACTION_H
#define _PLATFORMABSTRACTION_H

#include "cocos2d.h"

namespace wreckingmadness {
    class PlatformAbstraction {
    protected:
        static PlatformAbstraction* instance;
    public:
        PlatformAbstraction();
        virtual void shareImageFromFile(const std::string& fileName) {
            CCLOG("[PlatformAbstraction] Shared image from file %s", fileName.c_str());
        };
        static PlatformAbstraction* getInstance();
    };
}

#endif //_PLATFORMABSTRACTION_H
