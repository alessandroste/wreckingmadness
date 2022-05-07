#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Ball.h"
#include "Building.h"
#include "Floor.h"
#include "Common.h"

namespace wreckingmadness {
    enum Direction {
        LEFT,
        RIGHT
    };

    class GameScene : public cocos2d::LayerColor {
    private:
        void update(float dt);
        Ball* ball;
        Building* skyscraper;
        cocos2d::Size visibleSize;
        cocos2d::Vec2 visibleOrigin;
        cocos2d::Label* labelScore;
        cocos2d::Node* endGameMenu;
        float currentSpeed;
        float speedSetPoint;
        float scale;
        bool end;
        bool throwing;
        float floorWidth;
        std::string outfile;
        unsigned int currentScore;
        cocos2d::Node* buildEndGameMenu(unsigned int score, int top_score);
        bool isTouchDown;
        float initialTouchPos[2];
        float currentTouchPos[2];
    public:
        CREATE_FUNC(GameScene);
        static cocos2d::Scene* createScene();
        virtual bool init();

        // game events
        void endGame();
        static void restartGame();
        void onEnterTransitionDidFinish();
        void onExitTransitionDidStart();

        // touch events
        void checkTouch(int num);

        // building modification
        void generateFloor(bool roof, float correction);
        void removeTop(int dir);
        bool updateTop(Direction direction);

        // miscellaneous
        void screenCapturedCallback(bool succeed, const std::string& outputFile);
        void shareScore();

        // game effects
        void spanCloud(bool random);
        void throwBall(int direction, bool stopped, float height);
        static void playCrashSound(bool metal);
        void percentileReceivedCallback(float perc);

#if (SDKBOX_ENABLED && SDKBOX_FACEBOOK)
        void shareScreen(std::string file, std::string title);
        void closeShare();
        void checkPostPerm();
        void shareDialog();
#endif
    };
}

#endif