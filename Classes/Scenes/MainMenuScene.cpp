#include "MainMenuScene.h"

#include "GameScene.h"
#include "SettingsScene.h"
#include "Utilities.h"
#include "Common.h"
#include "SoundService.h"
#include "../Integrations/SdkBoxHelper.h"

using namespace cocos2d;
using namespace wreckingmadness;

#define BALL_ANGLE 15
#define BALL_SPEED 10
#define BALL_GRAVITY 9.81f
#define CLOUD_NUM 20
#define CLOUD_SPEED 20
#define CLOUD_SPEED_OFFSET 10

Scene* MainMenuScene::createScene() {
    auto scene = Scene::create();
    auto layer = MainMenuScene::create();
    scene->addChild(layer);
    return scene;
}

bool MainMenuScene::init() {
    if (!LayerColor::initWithColor(Common::BackgroundColor)) {
        return false;
    }
    vOrigin = Director::getInstance()->getVisibleOrigin();
    viewSize = Director::getInstance()->getVisibleSize();

    // menu entries
    auto labelStart = Label::createWithTTF("START", TEXT_FONT, TEXT_SIZE_START);
    labelStart->enableOutline(Color4B(0, 0, 0, 255), 2);
    auto buttonStart = MenuItemLabel::create(labelStart, std::bind(&MainMenuScene::startGame));
    auto labelSettings = Label::createWithTTF("SETTINGS", TEXT_FONT, TEXT_SIZE_DEFAULT);
    labelSettings->enableOutline(Color4B(0, 0, 0, 255), 2);
    auto buttonSettings = MenuItemLabel::create(labelSettings, std::bind(&MainMenuScene::menuSettingsCallback));
    auto labelExit = Label::createWithTTF("EXIT", TEXT_FONT, TEXT_SIZE_DEFAULT);
    labelExit->enableOutline(Color4B(0, 0, 0, 255), 2);
    auto buttonExit = MenuItemLabel::create(labelExit, std::bind(&MainMenuScene::menuCloseCallback));
    auto menu = Menu::createWithArray({ buttonStart, buttonSettings, buttonExit });
    menu->alignItemsVertically();

    // add menu
    menu->setPosition(vOrigin + Vec2(viewSize.width / 2, viewSize.height / 3));
    addChild(menu, 2);

    // add logo
    auto logo = Sprite::create(SPRITE_NAME_LOGO);
    logo->setPosition(vOrigin + Vec2(viewSize.width / 2, viewSize.height * 2 / 3));
    logo->setScale(viewSize.width / logo->getContentSize().width);
    addChild(logo, 2);

    // add ball
    ball = new Ball(5);
    ballSprite = ball->getNode();
    ballSprite->setPosition(vOrigin + Vec2(viewSize.width / 2, viewSize.height));
    ballSprite->setRotation(-BALL_ANGLE / 2.0f);
    deltatime = 0;
    addChild(ballSprite, 1);

    // clouds
    for (int i = 0; i < CLOUD_NUM; i++)
        spanCloud(true);

    // sounds
    SoundService::preloadEffects();
    SoundService::playBackgroundMusic();
    
    return true;
}

void MainMenuScene::update(float dt) {
    deltatime += dt;
    if (ballSprite != nullptr) {
        float l = this->ball->getLength();
        float theta = BALL_ANGLE * sin(sqrt(BALL_GRAVITY / l) * deltatime * BALL_SPEED);
        ballSprite->setRotation(theta);
    }
}

void MainMenuScene::spanCloud(bool random) {
    auto cloud = Common::spanCloud();
    auto cloud_height = vOrigin.y + Utilities::getRandom() * viewSize.height;
    auto cloud_vel = CLOUD_SPEED_OFFSET + Utilities::getRandom() * CLOUD_SPEED;
    auto cloud_width = cloud->getBoundingBox().size.width;
    auto cloud_space = viewSize.width + cloud_width * 2;
    auto cloud_x = vOrigin.x + viewSize.width + cloud_width;
    if (random)
        cloud_x = vOrigin.x + Utilities::getRandom() * viewSize.width;
    cloud->setPosition(Vec2(cloud_x, cloud_height));
    addChild(cloud, 0);
    auto newCloud = CallFunc::create([this]() { spanCloud(false); });
    cloud->runAction(Sequence::create(MoveBy::create(cloud_space / cloud_vel, Vec2(-cloud_space, 0)),
                                      newCloud, RemoveSelf::create(), nullptr));
}

void MainMenuScene::onEnterTransitionDidFinish() {
    scheduleUpdate();
}

void MainMenuScene::onExitTransitionDidFinish() {
    unscheduleUpdate();
}

void MainMenuScene::startGame() {
    SdkBoxHelper::CloseAd(AdType::GAMEOVER);
    SoundService::playEffect(Effect::HIT);
    auto transition = TransitionFade::create(0.5, GameScene::createScene());
    Director::getInstance()->replaceScene(transition);
}

void MainMenuScene::menuCloseCallback() {
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void MainMenuScene::menuSettingsCallback() {
    SoundService::playEffect(Effect::HIT);
    Common::enterSettingsScene();
}