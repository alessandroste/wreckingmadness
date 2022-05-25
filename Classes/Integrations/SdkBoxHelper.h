#ifndef _SDKBOXHELPER_H_
#define _SDKBOXHELPER_H_

namespace wreckingmadness {
    enum AdType {
        GAMEOVER
    };
    
    class SdkBoxHelper
    {
    private:
        static std::string AdTypeString(AdType adType);
        static const std::string Configuration;
    public:
        static void Init();
        static bool FacebookLogout();
        static void CloseAd(AdType adType);
        static void CacheAd(AdType adType);
        static void ShowAd(AdType adType);
    };
}

#endif