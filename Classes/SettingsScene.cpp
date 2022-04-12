#include "SettingsScene.h"
#include "Common.h"
#include "MainMenuScene.h"
#include "editor-support/cocostudio/SimpleAudioEngine.h"
#ifdef SDKBOX_ENABLED
#include "pluginfacebook/PluginFacebook.h"
#include "pluginadmob/PluginAdMob.h"
#endif

Scene* SettingsScene::createScene() {
    Scene* scene = Scene::create();
    SettingsScene* layer = SettingsScene::create();
    scene->addChild(layer);
    return scene;
}

SettingsScene::~SettingsScene() {
    com->~Common();
}

void SettingsScene::returnCallback() {
    Director::getInstance()->replaceScene(TransitionFade::create(0.5, MainMenuScene::createScene()));
}

bool SettingsScene::init() {
    com = new Common(0);
    if (!LayerColor::initWithColor(com->background)) {
        return false;
    }
    vsize = Director::getInstance()->getVisibleSize();
    vorigin = Director::getInstance()->getVisibleOrigin();

#ifdef SDKBOX_ENABLED
    sdkbox::PluginFacebook::init();
    if (sdkbox::PluginFacebook::isLoggedIn()) {
        Label* lbl_logout = Label::createWithTTF("facebook\nlogout", com->text_font, com->text_size / 1.5);
        lbl_logout->enableOutline(Color4B(0, 0, 0, 255), 2);
        lbl_logout->setAlignment(TextHAlignment::CENTER);
        MenuItemLabel* btn_logout = MenuItemLabel::create(lbl_logout, CC_CALLBACK_0(SettingsScene::logoutCallback, this));
        Menu* menu = Menu::createWithItem(btn_logout);
        menu->setPosition(vorigin + Vec2(vsize.width / 2, com->text_size * 4));
        addChild(menu);
    }
#endif

    std::string credits = "wrecking madness is a simple game written in C++ thanks to the awesome cocos2dx library.\n";
    credits.append("Many free resources have been used, credits to kenney.nl for sprites (some retouched).");

    Label* lbl_credits = Label::createWithTTF(credits, com->text_font, com->credits_size,
        Size(vsize.width / 1.1f, 0));
    lbl_credits->setAnchorPoint(Vec2(0.5, 1));
    lbl_credits->enableOutline(Color4B(0, 0, 0, 255), 2);
    lbl_credits->setPosition(vorigin + Vec2(vsize.width / 2, vsize.height - com->credits_size));
    addChild(lbl_credits);

    MenuItemImage* btn_return = MenuItemImage::create("mb_ret_n.png", "mb_ret_p.png", CC_CALLBACK_0(SettingsScene::returnCallback, this));
    btn_music = MenuItemImage::create();
    if (UserDefault::getInstance()->getBoolForKey("music", true)) {
        btn_music->initWithNormalImage("mb_musicoff_n.png", "mb_musicoff_p.png", "mb_musicoff_n.png", CC_CALLBACK_0(SettingsScene::toggleMusic, this));
    }
    else {
        btn_music->initWithNormalImage("mb_musicon_n.png", "mb_musicon_p.png", "mb_musicon_n.png", CC_CALLBACK_0(SettingsScene::toggleMusic, this));
    }
    Menu* menu_return = Menu::createWithArray({ btn_return, btn_music });
    menu_return->setPosition(vorigin + Vec2(vsize.width / 2, com->text_size * 1.5));
    menu_return->alignItemsHorizontally();
    addChild(menu_return);

    return true;
}

#ifdef SDKBOX_ENABLED
void SettingsScene::logoutCallback() {
    if (sdkbox::PluginFacebook::isLoggedIn()) {
        sdkbox::PluginFacebook::logout();
        com->makeToast("Logging out", 2, this);
    }
    else {
        com->makeToast("Already logged out", 2, this);
    }
}
#endif

void SettingsScene::onEnterTransitionDidFinish() {
    scheduleUpdate();
}

void SettingsScene::onExitTransitionDidFinish() {
    unscheduleUpdate();
}

void SettingsScene::update(float dt) {
#ifdef SDKBOX_ENABLED
    sdkbox::PluginAdMob::hide("gameover");
#endif
}

void SettingsScene::toggleMusic() {
    if (UserDefault::getInstance()->getBoolForKey("music", true)) {
        UserDefault::getInstance()->setBoolForKey("music", false);
        if (CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
            CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
        btn_music->initWithNormalImage("mb_musicon_n.png", "mb_musicon_p.png", "mb_musicon_n.png", CC_CALLBACK_0(SettingsScene::toggleMusic, this));
    }
    else {
        UserDefault::getInstance()->setBoolForKey("music", true);
        if (!CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying())
            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("wreckingsound.wav", true);
        btn_music->initWithNormalImage("mb_musicoff_n.png", "mb_musicoff_p.png", "mb_musicoff_n.png", CC_CALLBACK_0(SettingsScene::toggleMusic, this));
    }
}
