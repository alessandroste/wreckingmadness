#include "MainMenuScene.h"
#include "GameScene.h"
#include "Common.h"
#include "SettingsScene.h"
#include "editor-support/cocostudio/SimpleAudioEngine.h"
#ifdef SDKBOX_ENABLED
#include "pluginadmob/PluginAdMob.h"
#include "pluginfacebook/PluginFacebook.h"
#endif

USING_NS_CC;

#define BALL_ANGLE 15
#define BALL_SPEED 10
#define BALL_GRAVITY 9.81
#define CLOUD_NUM 20
#define CLOUD_SPEED 20
#define CLOUD_SPEED_OFFSET 10

float deltatime;

#ifdef SDKBOX_ENABLED
class ADListener : public sdkbox::AdMobListener
{
private:
    virtual void adViewDidReceiveAd(const std::string& name) {
        if (name == "gameover")
            sdkbox::PluginAdMob::show("gameover");
        CCLOG("AD RECEIVED");
    }
    virtual void adViewDidFailToReceiveAdWithError(const std::string& name, const std::string& msg) {}
    virtual void adViewWillPresentScreen(const std::string& name) {}
    virtual void adViewDidDismissScreen(const std::string& name) {}
    virtual void adViewWillDismissScreen(const std::string& name) {
        if (name == "gameover")
            sdkbox::PluginAdMob::cache("gameover");
        CCLOG("AD DISMISSED");
    }
    virtual void adViewWillLeaveApplication(const std::string& name) {}
};
#endif

Scene* TheMenu::createScene() {
    Scene* scene = Scene::create();
    TheMenu* layer = TheMenu::create();
    scene->addChild(layer);
    return scene;
}

bool TheMenu::init() {
    com = new Common(5);
    if (!LayerColor::initWithColor(com->background)) {
        return false;
    }
    vorigin = Director::getInstance()->getVisibleOrigin();
    vsize = Director::getInstance()->getVisibleSize();

    // menu entries
    Label* lbl_start = Label::createWithTTF("START", com->text_font, com->text_size * 1.5);
    lbl_start->enableOutline(Color4B(0, 0, 0, 255), 2);
    MenuItemLabel* btn_start = MenuItemLabel::create(lbl_start, CC_CALLBACK_0(TheMenu::startGame, this));
    Label* lbl_settings = Label::createWithTTF("SETTINGS", com->text_font, com->text_size);
    lbl_settings->enableOutline(Color4B(0, 0, 0, 255), 2);
    MenuItemLabel* btn_settings = MenuItemLabel::create(lbl_settings, CC_CALLBACK_0(TheMenu::menuSettingsCallback, this));
    Label* lbl_exit = Label::createWithTTF("EXIT", com->text_font, com->text_size);
    lbl_exit->enableOutline(Color4B(0, 0, 0, 255), 2);
    MenuItemLabel* btn_exit = MenuItemLabel::create(lbl_exit, CC_CALLBACK_1(TheMenu::menuCloseCallback, this));
    Menu* menu = Menu::createWithArray({ btn_start, btn_settings, btn_exit });
    menu->alignItemsVertically();

    // add menu
    menu->setPosition(vorigin + Vec2(vsize.width / 2, vsize.height / 3));
    addChild(menu, 2);

    // add logo
    Sprite* logo = Sprite::create("logo.png");
    logo->setPosition(vorigin + Vec2(vsize.width / 2, vsize.height * 2 / 3));
    logo->setScale(vsize.width / logo->getContentSize().width);
    addChild(logo, 2);

    // add ball
    sprite_ball = com->getBall();
    sprite_ball->setPosition(vorigin + Vec2(vsize.width / 2, vsize.height));
    sprite_ball->setRotation(-BALL_ANGLE / 2);
    deltatime = 0;
    addChild(sprite_ball, 1);

    // clouds
    for (int i = 0; i < CLOUD_NUM; i++)
        this->spanCloud(true);

    // sounds
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("hit.wav");
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("metal_hit.wav");
    if (UserDefault::getInstance()->getBoolForKey("music", 1) && !CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("wreckingsound.wav", true);

#ifdef SDKBOX_ENABLED
    sdkbox::PluginAdMob::init();
    sdkbox::PluginAdMob::cache("gameover");
    CCLOG("ADMOB INITIALIZED MENU");
    sdkbox::PluginAdMob::setListener(new ADListener());
    sdkbox::PluginFacebook::init();
#endif

    return true;
}

void TheMenu::update(float dt) {
    deltatime += dt;
    if (sprite_ball != nullptr) {
        float l = com->getBallLength();
        float theta = BALL_ANGLE * sin(sqrt(BALL_GRAVITY / l) * deltatime * BALL_SPEED);
        sprite_ball->setRotation(theta);
    }
}

void TheMenu::spanCloud(bool random) {
    Sprite* cloud = com->spanCloud();
    float cloud_height = this->vorigin.y + ((float)rand() / (float)(RAND_MAX / this->vsize.height));
    float cloud_vel = CLOUD_SPEED_OFFSET + (float)rand() / (float)(RAND_MAX / CLOUD_SPEED);
    float cloud_width = cloud->getBoundingBox().size.width;
    float cloud_space = this->vsize.width + cloud_width * 2;
    float cloud_x = this->vorigin.x + this->vsize.width + cloud_width;
    if (random)
        cloud_x = this->vorigin.x + ((float)rand() / (float)(RAND_MAX / this->vsize.width));
    cloud->setPosition(Vec2(cloud_x, cloud_height));
    this->addChild(cloud, 0);
    auto new_cloud = CallFunc::create([this]()
        { this->spanCloud(false); });
    cloud->runAction(Sequence::create(MoveBy::create(cloud_space / cloud_vel, Vec2(-cloud_space, 0)),
        new_cloud, RemoveSelf::create(), nullptr));
}

void TheMenu::onEnterTransitionDidFinish() {
    this->scheduleUpdate();
}

void TheMenu::onExitTransitionDidFinish() {
    this->unscheduleUpdate();
}

void TheMenu::startGame() {
#ifdef SDKBOX_ENABLED
    // sdkbox::PluginAdMob::cache("gameover");
    sdkbox::PluginAdMob::hide("gameover");
#endif
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("hit.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5, WreckingGame::createScene()));
}

void TheMenu::menuCloseCallback(Ref* pSender) {
    CocosDenshion::SimpleAudioEngine::getInstance()->unloadEffect("hit.wav");
    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#endif
}

void TheMenu::menuSettingsCallback() {
    CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("hit.wav");
    Director::getInstance()->replaceScene(TransitionFade::create(0.5, SettingsScene::createScene()));
}
