#ifdef SDKBOX_ENABLED
#include "FBUtils.h"
#include "pluginfacebook/PluginFacebook.h"
#include "sdkbox/Sdkbox.h"
#include "cocos2d.h"
#include "GameScene.h"
#include "Common.h"

using namespace sdkbox;

void FBL::onLogin(bool isLogin, const std::string& error){
    CCLOG("##FB isLogin: %d, message: %s", isLogin, error.c_str());
}

void FBL::onAPI(const std::string& tag, const std::string& jsonData)
{
    CCLOG("##FB onAPI: tag -> %s, json -> %s", tag.c_str(), jsonData.c_str());
}

void FBL::onSharedSuccess(const std::string& message)
{
    CCLOG("##FB onSharedSuccess:%s", message.c_str());
	if (WreckingGame::getGame() != nullptr)
		if (WreckingGame::getGame()->gcomm != nullptr){
      WreckingGame::getGame()->gcomm->makeToast("Shared", 2, WreckingGame::getGame());
      WreckingGame::getGame()->closeShare();
    }
}

void FBL::onSharedFailed(const std::string& message)
{
    CCLOG("##FB onSharedFailed:%s", message.c_str());
	if (WreckingGame::getGame() != nullptr)
		if (WreckingGame::getGame()->gcomm != nullptr)
			WreckingGame::getGame()->gcomm->makeToast("Share failed, check posting permissions", 2, WreckingGame::getGame());
}

void FBL::onSharedCancel()
{
    CCLOG("##FB onSharedCancel");
}

void FBL::onPermission(bool isLogin, const std::string& error)
{
    CCLOG("##FB onPermission: %d, error: %s", isLogin, error.c_str());
}

void FBL::onFetchFriends(bool ok, const std::string& msg)
{
    CCLOG("##FB %s: %d = %s", __FUNCTION__, ok, msg.data());
}

void FBL::onRequestInvitableFriends( const FBInvitableFriendsInfo& friends )
{
    for (auto it = friends.begin(); it != friends.end(); ++it) {
        CCLOG("Invitable friend: %s", it->getFirstName().c_str());
    }
}

void FBL::onInviteFriendsWithInviteIdsResult( bool result, const std::string& msg )
{
    CCLOG("on invite friends with invite ids %s= '%s'", result?"ok":"error", msg.c_str());
}

void FBL::onInviteFriendsResult( bool result, const std::string& msg )
{
    CCLOG("on invite friends %s= '%s'", result?"ok":"error", msg.c_str());
}

void FBL::onGetUserInfo( const sdkbox::FBGraphUser& userInfo )
{
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