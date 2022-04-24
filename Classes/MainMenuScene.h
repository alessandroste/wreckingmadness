#ifndef __MAINMENU_SCENE_H__
#define __MAINMENU_SCENE_H__

#include "cocos2d.h"
#include "GameScene.h"

class MainMenuScene : public LayerColor
{
private:
    void startGame();
    void update(float dt);
    cocos2d::Vec2 vorigin;
    cocos2d::Size vsize;
    cocos2d::Node* sprite_ball;
    float deltatime;
public:
    Common* com;
    static Scene* createScene();
    void menuCloseCallback(Ref* pSender);
    void menuSettingsCallback();
    virtual bool init();

    void spanCloud(bool random);
    void onEnterTransitionDidFinish();
    void onExitTransitionDidFinish();

    CREATE_FUNC(MainMenuScene);
};

#endif // __MAINMENU_SCENE_H__
