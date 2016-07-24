#ifndef __SETTINGS_SCENE_H__
#define __SETTINGS_SCENE_H__

#include "cocos2d.h"
#include "MainMenuScene.h"
#include "Common.h"

using namespace cocos2d;

class SettingsScene : public LayerColor
{
private:
	Common * com;
	Size vsize;
	Vec2 vorigin;
	void update(float dt);
	void toggleMusic();
	MenuItemImage * btn_music;
public:
	static Scene* createScene();
	~SettingsScene();
	void returnCallback();
	virtual bool init();
#ifdef SDKBOX_ENABLED
	void logoutCallback();
#endif
	void onEnterTransitionDidFinish();
	void onExitTransitionDidFinish();
	// implement the "static create()" method manually
	CREATE_FUNC(SettingsScene);
};

#endif