#include "Common.h"
#include "Scenes/MainMenuScene.h"
#include "Scenes/SettingsScene.h"
#include "Utilities.h"
#include "Integrations\ServiceClient.h"

using namespace cocos2d;
using namespace wreckingmadness;

Color4B Common::BackgroundColor = Color4B(170, 210, 230, 255);
Color4B Common::TextDarkColor = Color4B(161, 177, 177, 255);
Color4B Common::MetalColor = Color4B(172, 192, 193, 255);
Color4B Common::MetalColorLight = Color4B(180, 198, 199, 255);
Color4B Common::MetalColorDark = Color4B(116, 139, 139, 255);
Color4B Common::BoltColor = Color4B(161, 177, 177, 255);
Color4B Common::BoltColorDark = Color4B(150, 166, 166, 255);
Color4B Common::ToastBackgroundColor = Color4B(77, 77, 77, 255);
Color4B Common::ScorePercentageTextColor = Color4B(255, 221, 88, 255);

unsigned int Common::getTopLocalScore() {
    return UserDefault::getInstance()->getIntegerForKey(CONFIG_KEY_TOP_SCORE, 0);
}

void Common::processScore(unsigned int currentScore, ScoreUpdateSuccessCallback const& scoreUpdateSuccessCallback, ScoreUpdateFailureCallback const& scoreUpdateFailureCallback) {
    if (currentScore > getTopLocalScore()) {
        UserDefault::getInstance()->setIntegerForKey(CONFIG_KEY_TOP_SCORE, currentScore);
    }

    auto playerId = UserDefault::getInstance()->getStringForKey(CONFIG_KEY_PLAYER_ID, "");
    if (playerId != "")
        ServiceClient::getInstance().sendScore(playerId, currentScore, scoreUpdateSuccessCallback, scoreUpdateFailureCallback);
}

bool Common::getPlayerID() {
    if (UserDefault::getInstance()->getStringForKey(CONFIG_KEY_PLAYER_ID, "") != "")
        return true;

    auto successCallback = [](const std::string& playerId) {
        UserDefault::getInstance()->setStringForKey(CONFIG_KEY_PLAYER_ID, playerId);
    };

    ServiceClient::getInstance().getNewPlayerId(successCallback, []() {});
    return false;
}

Sprite* Common::spanCloud() {
    int num = Utilities::getRandom(1, 9); // sprite names
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