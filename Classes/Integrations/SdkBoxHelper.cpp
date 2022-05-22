#include "cocos2d.h"
#include "SdkBoxHelper.h"
#ifdef SDKBOX_ENABLED
#include "sdkbox/Sdkbox.h"
#include "PluginAdMob/PluginAdMob.h"
#include "AdListener.h"
#endif

using namespace cocos2d;
using namespace wreckingmadness;

std::string SdkBoxHelper::AdTypeString(AdType adType) {
    switch (adType) {
    case AdType::GAMEOVER:
        return "gameover";
    default:
        throw std::exception();
    }
}

void SdkBoxHelper::CloseAd(AdType adType) {
    auto adTypeName = AdTypeString(adType);
#ifdef SDKBOX_ENABLED
    sdkbox::PluginAdMob::hide(adTypeName);
#endif
}

void SdkBoxHelper::CacheAd(AdType adType) {
    auto adTypeName = AdTypeString(adType);
#ifdef SDKBOX_ENABLED
    sdkbox::PluginAdMob::cache(adTypeName);
#endif
}

void SdkBoxHelper::ShowAd(AdType adType) {
    auto adTypeName = AdTypeString(adType);
#ifdef SDKBOX_ENABLED
    if (sdkbox::PluginAdMob::isAvailable(adTypeName)) {
        CCLOG("[SdkBoxHelper] Showing ad %s", adTypeName.c_str());
        sdkbox::PluginAdMob::show(adTypeName);
    }
#endif
}

void wreckingmadness::SdkBoxHelper::Init() {
#ifdef SDKBOX_ENABLED
    CCLOG("[SdkBoxHelper] SdkBox enabled");
#endif
}

void wreckingmadness::SdkBoxHelper::PluginInit() {
#ifdef SDKBOX_ENABLED
    sdkbox::PluginAdMob::init();
    sdkbox::PluginAdMob::setGDPR(true);
    sdkbox::PluginAdMob::setListener(new AdListener());
    CCLOG("[SdkBoxHelper] Initialized AdMob");
#ifdef SDKBOX_FACEBOOK
    sdkbox::PluginFacebook::init();
#endif
#endif
}

bool wreckingmadness::SdkBoxHelper::FacebookLogout() {
#if (SDKBOX_ENABLED && SDKBOX_FACEBOOK)
    if (sdkbox::PluginFacebook::isLoggedIn()) {
        sdkbox::PluginFacebook::logout();
        return true;
    }
#endif
    return false;
}