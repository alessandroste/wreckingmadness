#ifndef __MAINMENU_SCENE_H__
#define __MAINMENU_SCENE_H__

#include "cocos2d.h"
#include "GameScene.h"

using namespace cocos2d;

class TheMenu : public LayerColor
{
	private:
		void startGame();
		void update(float dt);
		Vec2 origin;
		Size size;
	public:
	    static Scene* createScene();
	    void menuCloseCallback(Ref* pSender);
		virtual bool init();

		void spanCloud(bool random);
		void onEnterTransitionDidFinish();
		void onExitTransitionDidFinish();

	    // implement the "static create()" method manually
	    CREATE_FUNC(TheMenu);
};

#endif // __MAINMENU_SCENE_H__
