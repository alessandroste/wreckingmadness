#ifndef __MAINMENU_SCENE_H__
#define __MAINMENU_SCENE_H__

#include "cocos2d.h"
#include "Ball.h"
#include "Common.h"

namespace wreckingmadness {
    class MainMenuScene : public cocos2d::LayerColor {
    private:
        void startGame();
        void update(float dt);
        cocos2d::Vec2 vorigin;
        cocos2d::Size vsize;
        cocos2d::Node *sprite_ball;
        float deltatime;
        Ball* ball;
    public:
        CREATE_FUNC(MainMenuScene);
        static cocos2d::Scene* createScene();
        virtual bool init();
        void menuCloseCallback();
        void menuSettingsCallback();
        void spanCloud(bool random);
        void onEnterTransitionDidFinish();
        void onExitTransitionDidFinish();
    };
}

#endif // __MAINMENU_SCENE_H__
