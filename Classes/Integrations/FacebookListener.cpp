#if (SDKBOX_ENABLED && SDKBOX_FACEBOOK)
#include "FacebookListener.h"
#include "cocos2d.h"
#include "GameScene.h"
#include "Common.h"

using namespace sdkbox;
using namespace wreckingmadness;

void FacebookListener::onLogin(bool isLogin, const std::string& error) {
    CCLOG("##FB isLogin: %d, message: %s", isLogin, error.c_str());
    if (GameScene::getGame() != nullptr)
        if (isLogin)
            GameScene::getGame()->checkPostPerm();
        else
            GameScene::getGame()->com->makeToast("Could not login to Facebook", 2, GameScene::getGame());
}

void FacebookListener::onAPI(const std::string& tag, const std::string& jsonData) {
    CCLOG("##FB onAPI: tag -> %s, json -> %s", tag.c_str(), jsonData.c_str());
}

void FacebookListener::onSharedSuccess(const std::string& message) {
    CCLOG("##FB onSharedSuccess:%s", message.c_str());
    if (GameScene::getGame() != nullptr)
        if (GameScene::getGame()->com != nullptr) {
            GameScene::getGame()->com->makeToast("Shared", 2, GameScene::getGame());
            GameScene::getGame()->closeShare();
        }
}

void FacebookListener::onSharedFailed(const std::string& message) {
    CCLOG("##FB onSharedFailed:%s", message.c_str());
    if (GameScene::getGame() != nullptr)
        if (GameScene::getGame()->com != nullptr)
            GameScene::getGame()->com->makeToast("Share failed, check posting permissions", 2, GameScene::getGame());
}

void FacebookListener::onSharedCancel() {
    CCLOG("##FB onSharedCancel");
}

void FacebookListener::onPermission(bool isLogin, const std::string& error) {
    CCLOG("##FB onPermission: %d, error: %s", isLogin, error.c_str());
    if (GameScene::getGame() != nullptr) {
        bool found = false;
        for (auto& permission : sdkbox::PluginFacebook::getPermissionList()) {
            if (permission.data() == sdkbox::FB_PERM_PUBLISH_POST) {
                found = true;
                break;
            }
        }
        if (!found) {
            GameScene::getGame()->com->makeToast("You have to enable permission to share", 2, GameScene::getGame());
        }
        else {
            GameScene::getGame()->shareDialog();
        }
    }
}

void FacebookListener::onFetchFriends(bool ok, const std::string& msg) {
    CCLOG("##FB %s: %d = %s", __FUNCTION__, ok, msg.data());
}

void FacebookListener::onRequestInvitableFriends(const FBInvitableFriendsInfo& friends) {
    for (auto it = friends.begin(); it != friends.end(); ++it) {
        CCLOG("Invitable friend: %s", it->getFirstName().c_str());
    }
}

void FacebookListener::onInviteFriendsWithInviteIdsResult(bool result, const std::string& msg) {
    CCLOG("on invite friends with invite ids %s= '%s'", result ? "ok" : "error", msg.c_str());
}

void FacebookListener::onInviteFriendsResult(bool result, const std::string& msg) {
    CCLOG("on invite friends %s= '%s'", result ? "ok" : "error", msg.c_str());
}

void FacebookListener::onGetUserInfo(const sdkbox::FBGraphUser& userInfo) {
    CCLOG("Facebook id:'%s' name:'%s' last_name:'%s' first_name:'%s' email:'%s' installed:'%d'",
        userInfo.getUserId().c_str(),
        userInfo.getName().c_str(),
        userInfo.getFirstName().c_str(),
        userInfo.getLastName().c_str(),
        userInfo.getEmail().c_str(),
        userInfo.isInstalled ? 1 : 0
    );
}
#endif