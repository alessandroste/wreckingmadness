#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Building.h"
#include "Floor.h"
#include "Common.h"

using namespace cocos2d;

class GameScene : public LayerColor
{
private:
    void update(float dt);
    Building* skyscraper;
    Size vsize;
    Vec2 vorigin;
    Label* lbl_score;
    Node* ball;
    Node* menu_gameend;
    float vel;
    float vel_set;
    float scale;
    bool end;
    bool throwing;
    float floor_width;
    std::string outfile;

    // block types
    int getTypesNumber();
    std::string getRandomTypeName();
    floorStatus getRandomFloorStatus();
    std::map<std::string, std::string> types;

public:
    Common* com; // will be accessed by FBUtils for notifications
    static Scene* createScene();
    unsigned int score;

    // game events
    void endGame();
    void restartGame();
    void onEnterTransitionDidFinish();
    void onExitTransitionDidStart();
    void closeCallback();

    // touch events
    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchMoved(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);
    void onTouchCancelled(Touch* touch, Event* event);
    bool isTouchDown;
    void checkTouch(int num);
    float initialTouchPos[2];
    float currentTouchPos[2];

    // building modification
    void generateFloor(bool roof, float correction);
    void removeTop(int dir);
    bool updateTop(std::string dir);

    static GameScene* getGame();

    // miscellaneous
    float getTimeTick();
    virtual bool init();
    void menuCloseCallback(Ref* pSender);
    void afterCaptured(bool succeed, const std::string& outputFile);
    void shareScore();

    // game effects
    void spanCloud(bool random);
    void throwBall(int direction, bool stopped, float height);
    void finishThrow();
    void playCrashSound(bool metal);

    // score
    void percReceived(float perc);

#if (SDKBOX_ENABLED && SDKBOX_FACEBOOK)
    void shareScreen(std::string file, std::string title);
    void closeShare();
    void checkPostPerm();
    void shareDialog();
#endif

    CREATE_FUNC(GameScene);
};

#endif