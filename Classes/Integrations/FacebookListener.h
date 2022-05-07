#ifndef __FACEBOOKLISTENER_H__
#define __FACEBOOKLISTENER_H__

#if (SDKBOX_ENABLED && SDKBOX_FACEBOOK)
#include "sdkbox/Sdkbox.h"
#include "pluginfacebook/PluginFacebook.h"

namespace wreckingmadness {
    class FacebookListener : public sdkbox::FacebookListener
    {
    public:
        void onLogin(bool isLogin, const std::string& msg);
        void onSharedSuccess(const std::string& message);
        void onSharedFailed(const std::string& message);
        void onSharedCancel();
        void onAPI(const std::string& key, const std::string& jsonData);
        void onPermission(bool isLogin, const std::string& msg);
        void onFetchFriends(bool ok, const std::string& msg);
        void onRequestInvitableFriends(const sdkbox::FBInvitableFriendsInfo& friends);
        void onInviteFriendsWithInviteIdsResult(bool result, const std::string& msg);
        void onInviteFriendsResult(bool result, const std::string& msg);
        void onGetUserInfo(const sdkbox::FBGraphUser& userInfo);
    };
}
#endif
#endif