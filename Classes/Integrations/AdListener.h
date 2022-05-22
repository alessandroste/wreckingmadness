#ifndef _ADLISTENER_H
#define _ADLISTENER_H
#ifdef SDKBOX_ENABLED
#include "PluginAdMob\PluginAdMob.h"
#endif

namespace wreckingmadness {
#ifdef SDKBOX_ENABLED
    class AdListener : public sdkbox::AdMobListener {
    private:
        virtual void adViewDidReceiveAd(const std::string& name);
        virtual void adViewWillDismissScreen(const std::string& name);
        virtual void adViewDidFailToReceiveAdWithError(const std::string& name, const std::string& msg);
        virtual void adViewWillPresentScreen(const std::string& name) {}
        virtual void adViewDidDismissScreen(const std::string& name) {}
        virtual void adViewWillLeaveApplication(const std::string& name) {}
};
#endif
}
#endif