#ifndef __SETTINGS_SCENE_H__
#define __SETTINGS_SCENE_H__

#include "cocos2d.h"
#include "MainMenuScene.h"
#include "Common.h"

namespace wreckingmadness {
    class SettingsScene : public LayerColor
    {
    private:
        Common* com;
        Size vsize;
        Vec2 vorigin;
        void update(float dt);
        void toggleMusic();
        MenuItemImage* btn_music;
    public:
        static Scene* createScene();
        ~SettingsScene();
        void returnCallback();
        virtual bool init();
        void logoutCallback();
        void onEnterTransitionDidFinish();
        void onExitTransitionDidFinish();
        CREATE_FUNC(SettingsScene);
    };
}

#endif