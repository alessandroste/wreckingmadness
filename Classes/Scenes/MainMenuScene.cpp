#include <Utilities.h>
#include "MainMenuScene.h"
#include "GameScene.h"
#include "Common.h"
#include "SdkBoxHelper.h"
#include "SettingsScene.h"
#include "editor-support/cocostudio/SimpleAudioEngine.h"

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
    vorigin = Director::getInstance()->getVisibleOrigin();
    vsize = Director::getInstance()->getVisibleSize();

    // menu entries
    auto lbl_start = Label::createWithTTF("START", TEXT_FONT, TEXT_SIZE_START);
    lbl_start->enableOutline(Color4B(0, 0, 0, 255), 2);
    auto btn_start = MenuItemLabel::create(lbl_start, CC_CALLBACK_0(MainMenuScene::startGame, this));
    auto lbl_settings = Label::createWithTTF("SETTINGS", TEXT_FONT, TEXT_SIZE_DEFAULT);
    lbl_settings->enableOutline(Color4B(0, 0, 0, 255), 2);
    auto btn_settings = MenuItemLabel::create(lbl_settings, CC_CALLBACK_0(MainMenuScene::menuSettingsCallback, this));
    auto lbl_exit = Label::createWithTTF("EXIT", TEXT_FONT, TEXT_SIZE_DEFAULT);
    lbl_exit->enableOutline(Color4B(0, 0, 0, 255), 2);
    auto btn_exit = MenuItemLabel::create(lbl_exit, CC_CALLBACK_0(MainMenuScene::menuCloseCallback, this));
    auto menu = Menu::createWithArray({ btn_start, btn_settings, btn_exit });
    menu->alignItemsVertically();

    // add menu
    menu->setPosition(vorigin + Vec2(vsize.width / 2, vsize.height / 3));
    addChild(menu, 2);

    // add logo
    Sprite* logo = Sprite::create(SPRITE_NAME_LOGO);
    logo->setPosition(vorigin + Vec2(vsize.width / 2, vsize.height * 2 / 3));
    logo->setScale(vsize.width / logo->getContentSize().width);
    addChild(logo, 2);

    // add ball
    this->ball = new Ball(5);
    sprite_ball = this->ball->getNode();
    sprite_ball->setPosition(vorigin + Vec2(vsize.width / 2, vsize.height));
    sprite_ball->setRotation(-BALL_ANGLE / 2.0f);
    deltatime = 0;
    addChild(sprite_ball, 1);

    // clouds
    for (int i = 0; i < CLOUD_NUM; i++)
        spanCloud(true);

    // sounds
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(SOUND_HIT);
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(SOUND_METAL_HIT);
    if (UserDefault::getInstance()->getBoolForKey(CONFIG_KEY_MUSIC_ENABLED, true) &&
        !CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(MUSIC_BACKGROUND, true);

    wreckingmadness::SdkBoxHelper::PluginInit();
    return true;
}

void MainMenuScene::update(float dt) {
    deltatime += dt;
    if (sprite_ball != nullptr) {
        float l = this->ball->getLength();
        float theta = BALL_ANGLE * sin(sqrt(BALL_GRAVITY / l) * deltatime * BALL_SPEED);
        sprite_ball->setRotation(theta);
    }
}

void MainMenuScene::spanCloud(bool random) {
    auto cloud = Common::spanCloud();
    auto cloud_height = vorigin.y + Utilities::getRandom() * vsize.height;
    auto cloud_vel = CLOUD_SPEED_OFFSET + Utilities::getRandom() * CLOUD_SPEED;
    auto cloud_width = cloud->getBoundingBox().size.width;
    auto cloud_space = vsize.width + cloud_width * 2;
    auto cloud_x = vorigin.x + vsize.width + cloud_width;
    if (random)
        cloud_x = vorigin.x + Utilities::getRandom() * vsize.width;
    cloud->setPosition(Vec2(cloud_x, cloud_height));
    addChild(cloud, 0);
    auto new_cloud = CallFunc::create([this]() { spanCloud(false); });
    cloud->runAction(Sequence::create(MoveBy::create(cloud_space / cloud_vel, Vec2(-cloud_space, 0)),
        new_cloud, RemoveSelf::create(), nullptr));
}

void MainMenuScene::onEnterTransitionDidFinish() {
    scheduleUpdate();
}

void MainMenuScene::onExitTransitionDidFinish() {
    unscheduleUpdate();
}

void MainMenuScene::startGame() {
    SdkBoxHelper::CloseAd(AdType::GAMEOVER);
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_HIT);
    auto transition = TransitionFade::create(0.5, GameScene::createScene());
    Director::getInstance()->replaceScene(transition);
}

void MainMenuScene::menuCloseCallback() {
    CocosDenshion::SimpleAudioEngine::getInstance()->unloadEffect(SOUND_HIT);
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void MainMenuScene::menuSettingsCallback() {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_HIT);
    Common::enterSettingsScene();
}