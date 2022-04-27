#ifndef _PLATFORMABSTRACTION_H
#define _PLATFORMABSTRACTION_H

#include "cocos2d.h"

namespace wreckingmadness {
    class PlatformAbstraction {
    protected:
        static PlatformAbstraction* platformInstance;
        virtual const char* getName() const { return "PlatformAbstraction"; };
    public:
        PlatformAbstraction();
        virtual ~PlatformAbstraction();
        PlatformAbstraction(PlatformAbstraction const&) = delete;
        void operator=(PlatformAbstraction const&) = delete;
        virtual void shareImageFromFile(const std::string& fileName);
        static PlatformAbstraction* getInstance() { return platformInstance; };
        const char* getNamePublic() const { return getName(); }; 
    };
}

#endif //_PLATFORMABSTRACTION_H
