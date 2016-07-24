#include "GameScene.h"
#include "Common.h"
#include "SimpleAudioEngine.h"
#ifdef SDKBOX_ENABLED
#include "pluginadmob/PluginAdMob.h"
#include "pluginfacebook/PluginFacebook.h"
#include "FBUtils.h"
#include "ui\UIEditBox\UIEditBox.h"
#include "ui\UIImageView.h"
#endif

USING_NS_CC;

#define VELOCITY 320
#define MAX_VELOCITY 450
#define VEL_STEP 0.01f
#define SWIPE_FACTOR 14
#define CLOUD_SPEED 20
#define CLOUD_SPEED_OFFSET 10
#define CLOUD_NUM 20
#define BREAK_SPEED 3000
#define SCREEN_FILE "screenshot.png"

WreckingGame * ptr;

// for Android NDK
template <typename T>
std::string to_string(T value) {
	std::ostringstream os;
	os << value;
	return os.str();
}

Scene* WreckingGame::createScene() {
	Scene * scene = Scene::create();
	WreckingGame * layer = WreckingGame::create();
	scene->addChild(layer);
	return scene;
}

bool WreckingGame::init() {
	// utilities
	ptr = this;
	srand(time(NULL));
	com = new Common(30);

	// layer init
	if (!LayerColor::initWithColor(com->background)) {
		return false;
	}
	
	// viewport measurements
	vsize = Director::getInstance()->getVisibleSize();
	vorigin = Director::getInstance()->getVisibleOrigin();
	scale = Director::getInstance()->getContentScaleFactor();

	// names of textures
	types["sx"] = "floor_dx1.png";
	types["dx"] = "floor_sx1.png";
	types["both"] = "floor1.png";

	// score init and score label
	end = false;
	score = 0;
	lbl_score = Label::createWithTTF("0", com->text_font, 40);
	lbl_score->setPosition(vorigin + Vec2(
		vsize.width / 2,
		vsize.height - lbl_score->getContentSize().height));
	lbl_score->setAnchorPoint(Vec2(0.5f, 0.5f));
	lbl_score->enableOutline(Color4B(0, 0, 0, 255), 1);
	addChild(lbl_score, 5);

	// set up building object
	skyscraper = new Building();

	// first floor
	generateFloor(true, 0);
	
	// clouds
	for (int i = 0; i < CLOUD_NUM; i++) spanCloud(true);

	// ball
	ball = com->getBall();
	ball->setPosition(vorigin + Vec2(-vsize.width, 0));
	addChild(ball, 2);
	throwing = false;
	
	// sun
	Sprite * sun = com->getSun();
	sun->setPosition(vorigin + Vec2(-sun->getContentSize().width, vsize.height * 2 / 3));
	sun->runAction(RepeatForever::create(Sequence::create(
		MoveBy::create(50, Vec2(vsize.width + sun->getContentSize().width * 2, 0)),
		MoveBy::create(0, Vec2(-vsize.width - sun->getContentSize().width * 2, 0)),
		nullptr)));
	addChild(sun, 0);

	// touch
	EventListenerTouchOneByOne * listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);
	listener->onTouchBegan = CC_CALLBACK_2(WreckingGame::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(WreckingGame::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(WreckingGame::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(WreckingGame::onTouchCancelled, this);
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
	isTouchDown = false;
	initialTouchPos[0] = 0;
	initialTouchPos[1] = 0;

	// player id
	if (com->getPlayerID()) {
		// while checking condition a new ID is requested if not set
		CCLOG("Player ID exists, it is %d", UserDefault::getInstance()->getIntegerForKey("playerid"));
	}

	// difficulty
	vel = VELOCITY;
	vel_set = vel;

#ifdef SDKBOX_ENABLED
	sdkbox::PluginAdMob::init();
	sdkbox::PluginAdMob::cache("gameover");
	sdkbox::PluginFacebook::setListener(new FBL());
#endif
	return true;
}

void WreckingGame::onEnterTransitionDidFinish() {
	scheduleUpdate();
}

void WreckingGame::onExitTransitionDidStart() {
	unscheduleUpdate();
}

void WreckingGame::closeCallback() {
	CocosDenshion::SimpleAudioEngine::getInstance()->unloadEffect("hit.wav");
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)  
#endif
}

bool WreckingGame::onTouchBegan(Touch *touch, Event *event)
{
	initialTouchPos[0] = touch->getLocation().x;
	initialTouchPos[1] = touch->getLocation().y;
	currentTouchPos[0] = touch->getLocation().x;
	currentTouchPos[1] = touch->getLocation().y;
	isTouchDown = true;
	return true;
}

void WreckingGame::onTouchMoved(Touch *touch, Event *event)
{
	currentTouchPos[0] = touch->getLocation().x;
	currentTouchPos[1] = touch->getLocation().y;
}

void WreckingGame::onTouchEnded(Touch *touch, Event *event)
{
	isTouchDown = false;
}

void WreckingGame::onTouchCancelled(Touch *touch, Event *event)
{
	onTouchEnded(touch, event);
}

void WreckingGame::endGame() {
	lbl_score->removeFromParent();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	for (int i = 0; i < skyscraper->getNumber(); i++) {
		skyscraper->removeFloor();
	}

	// update top game score
	if (score > com->getTopLocalScore()) com->setTopLocalScore(score);

	// menu
	menu_gameend = com->getEndGameMenu(score, com->getTopLocalScore());
	menu_gameend->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_exit")->setCallback(CC_CALLBACK_0(WreckingGame::closeCallback, this));
	menu_gameend->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_share")->setCallback(CC_CALLBACK_0(WreckingGame::shareScore, this));
	menu_gameend->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_restart")->setCallback(CC_CALLBACK_0(WreckingGame::restartGame, this));
	addChild(menu_gameend, 6);

	com->sendScore(score);

#ifdef SDKBOX_ENABLED
	CCLOG("END GAME CHECK AD");
	if (sdkbox::PluginAdMob::isAvailable("gameover"))
		CCLOG("END GAME AD AVAILABLE");
	sdkbox::PluginAdMob::show("gameover");
#endif
}

void WreckingGame::restartGame() {
#ifdef SDKBOX_ENABLED
	sdkbox::PluginAdMob::cache("gameover");
	sdkbox::PluginAdMob::hide("gameover");
#endif
	Scene* newScene = WreckingGame::createScene();
	Director::getInstance()->replaceScene(TransitionFade::create(0.5, newScene));
}

void WreckingGame::checkTouch(int num) {
	if (num > 0 && isTouchDown) {
		if (initialTouchPos[0] - currentTouchPos[0] > SWIPE_FACTOR)
		{
			if (skyscraper->getUpperFloor()->fl_type == "sx" || skyscraper->getUpperFloor()->fl_type == "both") {
				updateTop("sx");
			}
			else throwBall(-1, true, skyscraper->getUpperFloor()->getSprite()->getPositionY());
			isTouchDown = false;
		}
		else if (initialTouchPos[0] - currentTouchPos[0] < -SWIPE_FACTOR)
		{
			if (skyscraper->getUpperFloor()->fl_type == "dx" || skyscraper->getUpperFloor()->fl_type == "both") {
				updateTop("dx");
			}
			else throwBall(1, true, skyscraper->getUpperFloor()->getSprite()->getPositionY());
			isTouchDown = false;
		}
	}
}

void WreckingGame::update(float dt) {
	int num = skyscraper->getNumber();
	if (num != 0) {
		float upper_position = skyscraper->getUpperFloor()->getSprite()->getPositionY();
		float fl_height = skyscraper->getUpperFloor()->getSprite()->getBoundingBox().size.height;
		if (upper_position > vsize.height - fl_height / 2 + vorigin.y) {
			// player loses game
			end = true;
			ball->removeFromParent();
			endGame();
		}
		else if (!end) {
			if (vel < vel_set) vel += VEL_STEP;
			for (int i = 0; i < num; i++) {
				float y = skyscraper->getNFloor(i)->getSprite()->getPositionY() + vel*dt;
				skyscraper->getNFloor(i)->getSprite()->setPositionY(y);
			}
			if (skyscraper->getLowerFloor()->getSprite()->getPositionY()> vorigin.y - fl_height)
				generateFloor(false, 0);
		}
	}
	checkTouch(num);
}

void WreckingGame::menuCloseCallback(Ref* pSender) {
	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#endif
}

void WreckingGame::afterCaptured(bool succeed, const std::string & outputFile) {
#ifdef SDKBOX_ENABLED
	sdkbox::PluginAdMob::show("gameover");
#endif	
	if (succeed) {
#ifdef SDKBOX_ENABLED
		if (!sdkbox::PluginFacebook::isLoggedIn()) {
			sdkbox::PluginFacebook::login();
		}

		bool found = false;
		for (auto& permission : sdkbox::PluginFacebook::getPermissionList()) {
			if (permission.data() == sdkbox::FB_PERM_PUBLISH_POST) {
				found = true;
				CCLOG("Found permission to publish");
				break;
			}
		}
		if (!found) {
			CCLOG("Not found permission to publish");
			sdkbox::PluginFacebook::requestPublishPermissions({ sdkbox::FB_PERM_PUBLISH_POST });
		}
		else {
			Layer * popup = Layer::create();
			ui::ImageView * scr = ui::ImageView::create();
			scr->loadTexture(outputFile);
			scr->setScale(0.4);
			scr->setPosition(vorigin + Vec2(vsize.width / 2, vsize.height * 2.2 / 3));
			popup->addChild(scr, 2);
			popup->setName("popup");
			DrawNode * shadow = DrawNode::create();
			shadow->drawSolidRect(Vec2(vorigin.x, vorigin.y), vorigin + Vec2(vsize.width, vsize.height), Color4F::BLACK);
			popup->addChild(shadow);

			MenuItemImage * button_go = MenuItemImage::create("mb_share_n.png", "mb_share_p.png",
				CC_CALLBACK_0(WreckingGame::shareScreen, this, outputFile, ""));
			button_go->setName("btn_go");
			MenuItemImage * button_ret = MenuItemImage::create("mb_ret_n.png", "mb_ret_p.png",
				CC_CALLBACK_0(WreckingGame::closeShare, this));
			button_ret->setName("btn_ret");
			Menu * menu_go = Menu::createWithArray({ button_ret, button_go });
			menu_go->alignItemsHorizontally();
			menu_go->setPosition(vorigin + Vec2(vsize.width / 2, vsize.height / 4 - com->text_size));
			menu_go->setName("menu_go");
			popup->addChild(menu_go, 2);
			addChild(popup, 10);
			menu_gameend->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_restart")->setEnabled(false);
			menu_gameend->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_exit")->setEnabled(false);
			menu_gameend->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_share")->setEnabled(false);
			sdkbox::PluginAdMob::hide("gameover");
		}

#else
		com->makeToast("Screenshot done", 2, this);
#endif
	}
	else {
		com->makeToast("error encoutered while doing screenshot", 2, this);
	}
}

#ifdef SDKBOX_ENABLED
void WreckingGame::closeShare() {
	getChildByName("popup")->removeAllChildren();
	getChildByName("popup")->removeFromParent();
	menu_gameend->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_restart")->setEnabled(true);
	menu_gameend->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_exit")->setEnabled(true);
	menu_gameend->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_share")->setEnabled(true);
	sdkbox::PluginAdMob::show("gameover");
}
#endif

void WreckingGame::shareScore() {
#ifdef SDKBOX_ENABLED
	sdkbox::PluginAdMob::hide("gameover");
#endif
	utils::captureScreen(CC_CALLBACK_2(WreckingGame::afterCaptured, this), SCREEN_FILE);
}

void WreckingGame::spanCloud(bool random) {
	Sprite * cloud = com->spanCloud();
	float cloud_height = vorigin.y + ((float)rand() / (float)(RAND_MAX / vsize.height));
	float cloud_vel = CLOUD_SPEED_OFFSET + (float)rand() / (float)(RAND_MAX / CLOUD_SPEED);
	float cloud_width = cloud->getBoundingBox().size.width;
	float cloud_space = vsize.width + cloud_width * 2;
	float cloud_x = vorigin.x + vsize.width + cloud_width;
	if (random) cloud_x = vorigin.x + ((float)rand() / (float)(RAND_MAX / vsize.width));
	cloud->setPosition(Vec2(cloud_x, cloud_height));
	addChild(cloud, 1);
	CallFunc * new_cloud = CallFunc::create([this]() {
		spanCloud(false);
	});
	cloud->runAction(Sequence::create(
		MoveBy::create(cloud_space / cloud_vel, Vec2(-cloud_space, 0)),
		new_cloud,
		RemoveSelf::create(),
		nullptr));
}

void WreckingGame::throwBall(int direction = 1, bool stopped = false, float height = 0) {
	if (!throwing) {
		throwing = true;
		float xpos;
		switch (direction) {
		case -1:
			xpos = vorigin.x + vsize.width + com->getBallRadius()*scale;
			break;
		case 1:
			xpos = vorigin.x - com->getBallRadius()*scale;
			break;
		}
		float ypos = height + com->getBallLength();
		ball->setPosition(Vec2(xpos, ypos));
		float space1 = vsize.width / 2 - floor_width / 2 + com->getBallRadius()*scale;
		float space = vsize.width + floor_width;
		if (!stopped) {
			ball->runAction(Sequence::create(
				MoveBy::create(space1 / BREAK_SPEED, Vec2(direction*space1, 0)),
				CallFunc::create(CC_CALLBACK_0(WreckingGame::playCrashSound, this, false)),
				CallFunc::create(CC_CALLBACK_0(WreckingGame::removeTop, this, direction)),
				DelayTime::create(0.02f),
				MoveBy::create(space / BREAK_SPEED, Vec2(direction * space, 0)),
				CallFunc::create(CC_CALLBACK_0(WreckingGame::finishThrow, this)),
				nullptr));
		}
		else {
			ball->runAction(Sequence::create(
				MoveBy::create(0.05f, Vec2(direction*space1, 0)),
				CallFunc::create(CC_CALLBACK_0(WreckingGame::playCrashSound, this, true)),
				MoveBy::create(0.2f, Vec2(-direction*space1, 0)),
				CallFunc::create(CC_CALLBACK_0(WreckingGame::finishThrow, this)),
				nullptr));
		}
	}
}

void WreckingGame::finishThrow() {
	throwing = false;
}

void WreckingGame::playCrashSound(bool metal = false) {
	if (!metal)
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("hit.wav");
	else
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("metal_hit.wav");
}

void WreckingGame::percReceived(float perc) {
#ifdef COCOS2D_DEBUG
	CCLOG("Percentage received by GAME %f", perc);
#endif
	if (menu_gameend != nullptr && menu_gameend->getChildByName("spinner") != nullptr) {
		char str[100];
		snprintf(str, sizeof(str), "Better than\n%.2f %%\nof players", perc);
		menu_gameend->getChildByName("spinner")->removeFromParent();
		Label * lbl_perc = Label::createWithTTF(str, com->text_font, com->text_size);
		lbl_perc->setHorizontalAlignment(TextHAlignment::CENTER);
		lbl_perc->setTextColor(Color4B(255, 221, 88, 255));
		lbl_perc->setPosition(vorigin + Vec2(vsize.width / 2, vsize.height / 1.4));
		menu_gameend->addChild(lbl_perc, 2);
	}
}

#ifdef SDKBOX_ENABLED
void WreckingGame::shareScreen(std::string file, std::string title) {
	sdkbox::FBShareInfo info;
	info.type = sdkbox::FB_PHOTO;
	info.title = title;
	info.image = file;
	sdkbox::PluginFacebook::share(info);
	getChildByName("popup")->getChildByName("menu_go")->getChildByName<MenuItemImage *>("btn_go")->setEnabled(false);
	getChildByName("popup")->getChildByName("menu_go")->getChildByName<MenuItemImage *>("btn_ret")->setEnabled(false);
	Sprite * spinner = Sprite::create("spinner.png");
	spinner->setName("spinner");
	spinner->setScale(0.6);
	spinner->setPosition(vorigin + Vec2(vsize.width / 2, vsize.height / 1.4));
	spinner->runAction(RepeatForever::create(RotateBy::create(0.1, 2 * M_PI)));
	getChildByName("popup")->addChild(spinner, 15);
}

#endif // SDKBOX_ENABLED

float WreckingGame::getTimeTick() {
	timeval time;
	gettimeofday(&time, NULL);
	unsigned long millisecs = (time.tv_sec * 1000) + (time.tv_usec / 1000);
	return (float)millisecs;
}

void WreckingGame::generateFloor(bool roof, float correction) {
	Sprite * sprite;
	float y = vorigin.y;
	std::string t;
	if (roof) {
		sprite = Sprite::create("roof1.png");
		t = "both";
		floor_width = sprite->getBoundingBox().size.width;

	}
	else {
		t = getRandomTypeName();
		sprite = Sprite::create(this->types[t]);
		y = skyscraper->getLowerFloor()->getSprite()->getPositionY();
	}
	Floor * cur_floor = new Floor(broken, t, sprite);
	float sprite_height = cur_floor->getSprite()->getContentSize().height;
	cur_floor->getSprite()->setPosition((Vec2(vorigin.x + vsize.width / 2, y - sprite_height + correction)));
	addChild(cur_floor->getSprite(), 3);
	skyscraper->addFloor(cur_floor);
}

void WreckingGame::removeTop(int dir) {
	if (!end) {
		// this space has to be the same also for floor
		float space = vsize.width + floor_width;
		skyscraper->getUpperFloor()->getSprite()->runAction(Sequence::create(
			MoveBy::create(space / BREAK_SPEED, Vec2(dir * space, 0)),
			RemoveSelf::create(),
			nullptr));

		// remove floor from data structure and update score
		skyscraper->removeFloor();
		score++;
		lbl_score->setString(to_string(score));

		// change difficulty with score
		if (vel_set < MAX_VELOCITY) vel_set += 1.0f / 4;

#ifdef COCOS2D_DEBUG
		CCLOG("Velocity setpoint: %f", vel);
#endif
	}
}

bool WreckingGame::updateTop(std::string dir)
{
	int sign = 0;
	if (dir == "dx") sign = 1;
	if (dir == "sx") sign = -1;
	switch (skyscraper->getUpperFloor()->fl_status) {
	case broken: {
		float y = skyscraper->getUpperFloor()->getSprite()->getPositionY() +
			skyscraper->getUpperFloor()->getSprite()->getContentSize().height / 2;
		throwBall(sign, false, y);
		return true;
	}
	case half: {
		skyscraper->getUpperFloor()->fl_status = broken;
		skyscraper->getUpperFloor()->getSprite()->runAction(MoveBy::create((float)0.01, Vec2(30 * sign, 0)));
		return false;
	}
	case good: {
		skyscraper->getUpperFloor()->fl_status = half;
		skyscraper->getUpperFloor()->getSprite()->runAction(MoveBy::create((float)0.01, Vec2(30 * sign, 0)));
		return false;
	}
	default: return false;
	}
}

int WreckingGame::getTypesNumber() {
	return types.size();
}

std::string WreckingGame::getRandomTypeName() {
	std::vector<std::string> vt_types;
	std::map<std::string, std::string>::iterator it;
	for (it = types.begin(); it != types.end(); it++) {
		vt_types.push_back(it->first);
	}
	int num = vt_types.size();
	return vt_types[rand() % num];
}

floorStatus WreckingGame::getRandomFloorStatus() {
	switch (rand() % 3) {
	case 0: return broken;
	case 1: return half;
	case 2: return good;
	default: return broken;
	}
}

WreckingGame * WreckingGame::getGame() {
	return ptr;
}
