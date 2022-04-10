#ifdef SDKBOX_ENABLED

#ifndef __FB_UTILS_H__
#define __FB_UTILS_H__

#include "pluginfacebook/PluginFacebook.h"
#include "sdkbox/Sdkbox.h"

class FBL : public sdkbox::FacebookListener
{
public:
    // Facebook callbacks
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

#endif
#endif