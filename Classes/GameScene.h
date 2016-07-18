#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Building.h"
#include "Floor.h"

using namespace cocos2d;

class WreckingGame : public LayerColor
{
	private:
		void update(float dt);
		Building* myBuilding;
	public:
	    static Scene* createScene();
		int score;

		// game events
		void endGame();
		void restartGame();
		void onEnterTransitionDidFinish();
		void onExitTransitionDidStart();
		void closeCallback();

		// touch events
		bool onTouchBegan(Touch *touch, Event *event);
		void onTouchMoved(Touch *touch, Event *event);
		void onTouchEnded(Touch *touch, Event *event);
		void onTouchCancelled(Touch *touch, Event *event);
		bool isTouchDown;
		void checkTouch(int num);
		float initialTouchPos[2];
		float currentTouchPos[2];

		// building modification
		void generateFloor(bool roof, float correction);
		void removeTop(int dir);
		bool updateTop(std::string dir);

		// block types
	    int getTypesNumber();
	    std::string getRandomTypeName();
		floorStatus getRandomFloorStatus();
		std::map<std::string, std::string> types;
		static WreckingGame * getGame();

		// miscellaneous
		float getTimeTick();
	    virtual bool init();
	    void menuCloseCallback(Ref* pSender);
		void afterCaptured(bool succeed, const std::string &outputFile);
		void shareScore();

		// game effects
		void spanCloud(bool random);
		void throwBall(int direction, bool stopped, float height);
		void finishThrow();
		void playCrashSound(bool metal);

		// score
		void percReceived(float perc);

	    // implement the "static create()" method manually
	    CREATE_FUNC(WreckingGame);
};

#endif
