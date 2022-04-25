#include "AdListener.h"
#include "cocos2d.h"

using namespace cocos2d;
using namespace wreckingmadness;

void AdListener::adViewDidReceiveAd(const std::string &name) {
    if (name == "gameover")
        sdkbox::PluginAdMob::show("gameover");
    CCLOG("AD RECEIVED");
}

void AdListener::adViewWillDismissScreen(const std::string &name) {
    if (name == "gameover")
        sdkbox::PluginAdMob::cache("gameover");
    CCLOG("AD DISMISSED");
}