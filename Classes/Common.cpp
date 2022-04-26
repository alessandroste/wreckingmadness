#include "network\HttpClient.h"
#include "Common.h"
#include "Scenes/GameScene.h"
#include "Scenes/SettingsScene.h"
#include "Utilities.h"

using namespace cocos2d;
using namespace wreckingmadness;

bool requesting;
bool sending;

Color4B Common::BackgroundColor = Color4B(170, 210, 230, 255);
Color4B Common::TextDarkColor = Color4B(161, 177, 177, 255);
Color4B Common::MetalColor = Color4B(172, 192, 193, 255);
Color4B Common::MetalColorLight = Color4B(180, 198, 199, 255);
Color4B Common::MetalColorDark = Color4B(116, 139, 139, 255);
Color4B Common::BoltColor = Color4B(161, 177, 177, 255);
Color4B Common::BoltColorDark = Color4B(150, 166, 166, 255);
Color4B Common::ToastBackgroundColor = Color4B(77, 77, 77, 255);

Common::Common() {
    requesting = false;
    sending = false;
}

Common::~Common() = default;

unsigned int Common::getTopLocalScore() {
    return UserDefault::getInstance()->getIntegerForKey(CONFIG_KEY_TOP_SCORE);
}

void Common::setTopLocalScore(unsigned int score) {
    UserDefault::getInstance()->setIntegerForKey(CONFIG_KEY_TOP_SCORE, score);
}

void Common::sendScore(unsigned int score) {
    //    if (!sending && !requesting) {
    //        network::HttpRequest* httpc = new network::HttpRequest();
    //        std::string sscore = Utilities::to_string(currentScore);
    //        std::string sid = Utilities::to_string(UserDefault::getInstance()->getIntegerForKey("playerid"));
    //        std::string tobehashed = sid + sscore + SCORE_KEY;
    //        std::string url = API_LOC;
    //        url.append(UPD_LOC).append("?hash=").append(tobehashed);
    //        url.append("&id=").append(sid);
    //        url.append("&currentScore=").append(sscore);
    //        CCLOG("Updating new currentScore to %s", url.c_str());
    //        httpc->setUrl(url);
    //        httpc->setRequestType(network::HttpRequest::Type::GET);
    //        httpc->setResponseCallback(CC_CALLBACK_2(Common::onHttpRequestCompleted, this));
    //        cocos2d::network::HttpClient::getInstance()->send(httpc);
    //        sending = true;
    //        httpc->release();
    //    }
}

bool Common::getPlayerID() {
    //    if (UserDefault::getInstance()->getIntegerForKey("playerid", -1) != -1)
    //        return true;
    //    else if (!requesting) {
    //        network::HttpRequest* httpc = new network::HttpRequest();
    //        std::string url = API_LOC;
    //        url.append(GID_LOC);
    //        url.append("?hash=");
    //        url.append(md5(ID_KEY));
    //        CCLOG("Requesting a new ID from %s", url.c_str());
    //        httpc->setUrl(url);
    //        httpc->setRequestType(network::HttpRequest::Type::GET);
    //        httpc->setResponseCallback(CC_CALLBACK_2(Common::onHttpRequestCompleted, this));
    //        cocos2d::network::HttpClient::getInstance()->send(httpc);
    //        requesting = true;
    //        httpc->release();
    //        return false;
    //    }
    //    else return false;
    return true;
}

void Common::onHttpRequestCompleted(cocos2d::network::HttpClient * sender, cocos2d::network::HttpResponse * response) {
    if (requesting) {
        auto buffer = response->getResponseData();
        char* concatenated = (char*)malloc(buffer->size() + 1);
        std::string s2(buffer->begin(), buffer->end());
        std::strcpy(concatenated, s2.c_str());
        CCLOG("Response for ID request %s", concatenated);
        int id = atoi(concatenated);
        CCLOG("Casted ID is %d", id);
        if (id != 0) {
            UserDefault::getInstance()->setIntegerForKey(CONFIG_KEY_PLAYER_ID, id);
        }
        requesting = false;
    }
    if (sending) {
        auto buffer = response->getResponseData();
        char* concatenated = (char*)malloc(buffer->size() + 1);
        std::string s2(buffer->begin(), buffer->end());
        std::strcpy(concatenated, s2.c_str());
        CCLOG("Response for currentScore update %s", concatenated);
        float perc = atof(concatenated);
        //        
        //        if (g == nullptr) {
        //            CCLOG("Can't connect to game scene");
        //        }
        //        else {
        //            g->percReceived(perc);
        //        }
        sending = false;
    }
}

Sprite* Common::spanCloud() {
    int num = std::floor(Utilities::getRandom() * 9) + 1; // 9 is number of sprite versions
    auto spriteName = "clouds/cloud" + Utilities::to_string(num) + ".png";
    return Sprite::create(spriteName);
}

Sprite* Common::getSun() {
    auto ani = Animation::create();
    ani->addSpriteFrameWithFile("sun_1.png");
    ani->addSpriteFrameWithFile("sun_2.png");
    ani->setDelayPerUnit(1.0f / 2);
    auto sun = Sprite::create("sun_1.png");
    sun->runAction(RepeatForever::create(Animate::create(ani)));
    return sun;
}

void Common::enterSettingsScene() {
    auto transition = TransitionFade::create(0.5, SettingsScene::createScene());
    Director::getInstance()->replaceScene(transition);
}

void Common::enterMainMenuScene() {
    auto transition = TransitionFade::create(0.5, MainMenuScene::createScene());
    Director::getInstance()->replaceScene(transition);
}