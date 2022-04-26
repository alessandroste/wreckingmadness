#ifndef __SETTINGS_SCENE_H__
#define __SETTINGS_SCENE_H__

#include "cocos2d.h"
#include "MainMenuScene.h"
#include "Common.h"

namespace wreckingmadness {
    class SettingsScene : public cocos2d::LayerColor {
    private:
        cocos2d::Size visibleSize;
        cocos2d::Vec2 visibleOrigin;
        void toggleMusic();
        cocos2d::MenuItemImage* btnMusic;
    public:
        CREATE_FUNC(SettingsScene);
        static cocos2d::Scene* createScene();
        virtual bool init();
        static void logoutCallback();
    };
}

#endif