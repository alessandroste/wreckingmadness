#include "SettingsScene.h"

#include "../Integrations/SdkBoxHelper.h"
#include "Common.h"
#include "MainMenuScene.h"
#include "Utilities.h"
#include "SoundService.h"
#ifdef SDKBOX_ENABLED
#include "PluginAdMob/PluginAdMob.h"
#ifdef SDKBOX_FACEBOOK
#include "pluginfacebook/PluginFacebook.h"
#endif
#endif

using namespace cocos2d;
using namespace wreckingmadness;

Scene* SettingsScene::createScene() {
    auto scene = Scene::create();
    auto layer = SettingsScene::create();
    scene->addChild(layer);
    return scene;
}

bool SettingsScene::init() {
    if (!LayerColor::initWithColor(Common::BackgroundColor)) {
        return false;
    }
    visibleSize = Director::getInstance()->getVisibleSize();
    visibleOrigin = Director::getInstance()->getVisibleOrigin();

#if (SDKBOX_ENABLED && SDKBOX_FACEBOOK)
    sdkbox::PluginFacebook::init();
    if (sdkbox::PluginFacebook::isLoggedIn()) {
        Label* lbl_logout = Label::createWithTTF("facebook\nlogout", com->text_font, com->text_size / 1.5);
        lbl_logout->enableOutline(Color4B(0, 0, 0, 255), 2);
        lbl_logout->setAlignment(TextHAlignment::CENTER);
        MenuItemLabel* btn_logout = MenuItemLabel::create(lbl_logout, CC_CALLBACK_0(SettingsScene::logoutCallback, this));
        Menu* menu = Menu::createWithItem(btn_logout);
        menu->setPosition(visibleOrigin + Vec2(visibleSize.width / 2, com->text_size * 4));
        addChild(menu);
    }
#endif
    auto credits = "wrecking madness is a simple game written " \
        "in C++ thanks to the awesome cocos2dx library.\n" \
        "Many free resources have been used, credits to " \
        "kenney.nl for sprites (some retouched).";

    auto lbl_credits = Label::createWithTTF(credits, TEXT_FONT, TEXT_SIZE_CREDITS,
        Size(visibleSize.width / 1.1f, 0));
    lbl_credits->setAnchorPoint(Vec2(0.5, 1));
    lbl_credits->enableOutline(Color4B(0, 0, 0, 255), 2);
    lbl_credits->setPosition(visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height - TEXT_SIZE_CREDITS));
    addChild(lbl_credits);

    auto btnReturn = MenuItemImage::create("mb_ret_n.png", "mb_ret_p.png",
        std::bind(&Common::enterMainMenuScene));
    btnMusic = MenuItemImage::create();
    if (SoundService::isBackgroundMusicEnabled()) {
        btnMusic->initWithNormalImage("mb_musicon_n.png", "mb_musicon_p.png", "mb_musicon_n.png", CC_CALLBACK_0(SettingsScene::toggleMusic, this));
    }
    else {
        btnMusic->initWithNormalImage("mb_musicoff_n.png", "mb_musicoff_p.png", "mb_musicoff_n.png", CC_CALLBACK_0(SettingsScene::toggleMusic, this));
    }

    auto menu_return = Menu::createWithArray({ btnReturn, btnMusic });
    menu_return->setPosition(visibleOrigin + Vec2(visibleSize.width / 2, TEXT_SIZE_START));
    menu_return->alignItemsHorizontally();
    addChild(menu_return);
    return true;
}

void SettingsScene::logoutCallback() {
    if (SdkBoxHelper::FacebookLogout()) {
        Utilities::makeToast("Logged out", ToastDuration::SHORT);
    }
    else {
        Utilities::makeToast("Already logged out", ToastDuration::SHORT);
    }
}

void SettingsScene::toggleMusic() {
    if (!SoundService::isBackgroundMusicEnabled()) {
        SoundService::setBackgroundMusic(true);
        btnMusic->initWithNormalImage("mb_musicon_n.png", "mb_musicon_p.png", "mb_musicon_n.png", CC_CALLBACK_0(SettingsScene::toggleMusic, this));
    }
    else {
        SoundService::setBackgroundMusic(false);
        btnMusic->initWithNormalImage("mb_musicoff_n.png", "mb_musicoff_p.png", "mb_musicoff_n.png", CC_CALLBACK_0(SettingsScene::toggleMusic, this));
    }
}
