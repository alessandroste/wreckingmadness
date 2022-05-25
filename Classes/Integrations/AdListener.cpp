#include "AdListener.h"
#include "cocos2d.h"

#ifdef SDKBOX_ENABLED
using namespace cocos2d;
using namespace wreckingmadness;

void AdListener::adViewDidReceiveAd(const std::string& name) {
    CCLOG("[AdListener] Ad %s received", name.c_str());
}

void AdListener::adViewWillDismissScreen(const std::string& name) {
    sdkbox::PluginAdMob::cache("gameover");
    CCLOG("[AdListener] Ad %s dismissed", name.c_str());
}

void AdListener::adViewDidFailToReceiveAdWithError(
    const std::string &name,
    const std::string &msg) {
    CCLOG("[AdListener] For ad %s. Received error %s", name.c_str(), msg.c_str());
}

#endif