#ifndef __MAINMENU_SCENE_H__
#define __MAINMENU_SCENE_H__

#include "cocos2d.h"
#include "GameScene.h"

using namespace cocos2d;

class MainMenuScene : public LayerColor
{
private:
    void startGame();
    void update(float dt);
    Vec2 vorigin;
    Size vsize;
    Node* sprite_ball;
public:
    Common* com;
    static Scene* createScene();
    void menuCloseCallback(Ref* pSender);
    void menuSettingsCallback();
    virtual bool init();

    void spanCloud(bool random);
    void onEnterTransitionDidFinish();
    void onExitTransitionDidFinish();

    // implement the "static create()" method manually
    CREATE_FUNC(MainMenuScene);
};

#endif // __MAINMENU_SCENE_H__
