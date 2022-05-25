#ifndef __MAINMENU_SCENE_H__
#define __MAINMENU_SCENE_H__

#include "cocos2d.h"
#include "Ball.h"
#include "Common.h"

namespace wreckingmadness {
    class MainMenuScene : public cocos2d::LayerColor {
    private:
        static void startGame();
        void update(float dt);
        cocos2d::Vec2 vOrigin;
        cocos2d::Size viewSize;
        cocos2d::Node* ballSprite;
        float deltatime;
        Ball* ball;
    public:
        CREATE_FUNC(MainMenuScene);
        static cocos2d::Scene* createScene();
        virtual bool init();
        
        static void menuCloseCallback();
        static void menuSettingsCallback();
        void spanCloud(bool random);
        void onEnterTransitionDidFinish();
        void onExitTransitionDidFinish();
    };
}

#endif // __MAINMENU_SCENE_H__
