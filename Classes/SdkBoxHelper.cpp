#include "SdkBoxHelper.h"
#ifdef SDKBOX
#include "sdkbox/Sdkbox.h"
#endif

void wreckingmadness::SdkBoxHelper::Init() {
#ifdef SDKBOX
    CCLOG("Sandbox enabled");
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    // sdkbox::init("a2c6b56453d702eaaf90a7eb6060ff03", "4f6c1a0dd3580a65");
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // sdkbox::init("e4a5357d4990f05c776ac7c6007d59dc", "1dd0f011c6419710", "googleplay");
#endif
#endif
}

#ifdef SDKBOX
class ADListener : public sdkbox::AdMobListener
{
private:
    virtual void adViewDidReceiveAd(const std::string& name) {
        if (name == "gameover")
            sdkbox::PluginAdMob::show("gameover");
        CCLOG("AD RECEIVED");
    }
    virtual void adViewDidFailToReceiveAdWithError(const std::string& name, const std::string& msg) {}
    virtual void adViewWillPresentScreen(const std::string& name) {}
    virtual void adViewDidDismissScreen(const std::string& name) {}
    virtual void adViewWillDismissScreen(const std::string& name) {
        if (name == "gameover")
            sdkbox::PluginAdMob::cache("gameover");
        CCLOG("AD DISMISSED");
    }
    virtual void adViewWillLeaveApplication(const std::string& name) {}
};
#endif

void wreckingmadness::SdkBoxHelper::PluginInit() {
#ifdef SDKBOX
    sdkbox::PluginAdMob::init();
    sdkbox::PluginAdMob::cache("gameover");
    sdkbox::PluginAdMob::setListener(new ADListener());
    CCLOG("ADMOB INITIALIZED MENU");
    sdkbox::PluginFacebook::init();
#endif
}

bool wreckingmadness::SdkBoxHelper::FacebookLogout() {
#if (SDKBOX && SDKBOX_FACEBOOK)
    if (sdkbox::PluginFacebook::isLoggedIn()) {
        sdkbox::PluginFacebook::logout();
        return true;
    }
#endif

    return false;
}