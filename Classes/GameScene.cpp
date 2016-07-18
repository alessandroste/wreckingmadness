#include "GameScene.h"
#include "Common.h"
#include "SimpleAudioEngine.h"
#ifdef SDKBOX_ENABLED
	#include "pluginadmob/PluginAdMob.h"
#endif

USING_NS_CC;

#define NUM_FLOORS 30
#define VELOCITY 320
#define MAX_VELOCITY 450
#define SWIPE_FACTOR 14
#define CLOUD_SPEED 20
#define CLOUD_SPEED_OFFSET 10
#define CLOUD_NUM 20
#define BREAK_SPEED 3000

Label * title;
Vec2 origin;
Size size;
Node * ball;
bool throwing;
float floor_width = 0;
bool end;
Common * gamecomm;
float scale;
WreckingGame * ptr;
Node * gmenu;
float vel;

// for Android NDK
template <typename T>
std::string to_string(T value) {
	std::ostringstream os;
	os << value;
	return os.str();
}

Scene* WreckingGame::createScene(){
    auto scene = Scene::create();
    auto layer = WreckingGame::create();
    scene->addChild(layer);
    return scene;
}

bool WreckingGame::init(){
	srand(time(NULL));
	gamecomm = new Common(30);

    this->types["sx"]="floor_dx1.png";
    this->types["dx"]="floor_sx1.png";
	this->types["both"] = "floor1.png";

    if ( !LayerColor::initWithColor(gamecomm->background) ){
        return false;
    }
	
	ptr = this;

	// viewport measurements
    size = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
	float space = size.height * 5;
	scale = Director::getInstance()->getContentScaleFactor();

	// score
	this->score = 0;
    title = Label::createWithTTF("0", gamecomm->text_font, 40);
    title->setPosition(origin + Vec2(
		size.width/2,
        size.height - title->getContentSize().height));
	title->setAnchorPoint(Vec2(0.5f,0.5f));
	title->enableOutline(Color4B(0, 0, 0, 255), 1);
    this->addChild(title, 5);

	// set up building object
	this->myBuilding = new Building();

	// first floor
	this->generateFloor(true, 0);
	end = false;

	// clouds
	for (int i = 0; i < CLOUD_NUM; i++) this->spanCloud(true);

	// ball
	ball = gamecomm->getBall();
	ball->setPosition(origin + Vec2(-size.width, 0));
	this->addChild(ball, 2);
	throwing = false;
	// sun

	Sprite * sun = gamecomm->getSun();
	sun->setPosition(origin + Vec2(-sun->getContentSize().width, size.height*2/3));
	this->addChild(sun, 0);
	sun->runAction(RepeatForever::create(Sequence::create(
		MoveBy::create(50, Vec2(size.width + sun->getContentSize().width * 2, 0)),
		MoveBy::create(0,  Vec2(-size.width -sun->getContentSize().width * 2, 0)),
		nullptr)));

	// touch
	auto listener = EventListenerTouchOneByOne::create();
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
	if (gamecomm->getPlayerID()) {
		CCLOG("Player ID exists, it is %d", UserDefault::getInstance()->getIntegerForKey("playerid"));
	}
	
	// difficulty
	vel = VELOCITY;

	#ifdef SDKBOX_ENABLED
		sdkbox::PluginAdMob::init();
		sdkbox::PluginAdMob::cache("gameover");
	#endif
    return true;
}

void WreckingGame::onEnterTransitionDidFinish() {
	this->scheduleUpdate();
}

void WreckingGame::onExitTransitionDidStart() {
	this->unscheduleUpdate();
}

void WreckingGame::closeCallback(){
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
	title->removeFromParent();
	Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
	for (int i = 0; i < this->myBuilding->getNumber(); i++) {
		// better leave the building where it is
		// this->myBuilding->getUpperFloor()->getSprite()->removeFromParent();
		this->myBuilding->removeFloor();
	}

	// update top game score
	if (score > gamecomm->getTopLocalScore()) gamecomm->setTopLocalScore(score);

	// menu
	gmenu = gamecomm->getEndGameMenu(score, gamecomm->getTopLocalScore());
	gmenu->getChildByName("bts")->getChildByName<MenuItemImage *>("bexit")->setCallback(CC_CALLBACK_0(WreckingGame::closeCallback, this));
	gmenu->getChildByName("bts")->getChildByName<MenuItemImage *>("bshare")->setCallback(CC_CALLBACK_0(WreckingGame::shareScore, this));
	gmenu->getChildByName("bts")->getChildByName<MenuItemImage *>("brestart")->setCallback(CC_CALLBACK_0(WreckingGame::restartGame, this));
	this->addChild(gmenu,6);

	gamecomm->sendScore(score);

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
			//CCLOG("SWIPED LEFT");
			if (this->myBuilding->getUpperFloor()->fl_type == "sx" || this->myBuilding->getUpperFloor()->fl_type == "both") {
				this->updateTop("sx");
			}
			else this->throwBall(-1, true, this->myBuilding->getUpperFloor()->getSprite()->getPositionY());
			isTouchDown = false;
		}
		else if (initialTouchPos[0] - currentTouchPos[0] < -SWIPE_FACTOR)
		{
			//CCLOG("SWIPED RIGHT");
			if (this->myBuilding->getUpperFloor()->fl_type == "dx" || this->myBuilding->getUpperFloor()->fl_type == "both") {
				this->updateTop("dx");
			}
			else this->throwBall(1, true, this->myBuilding->getUpperFloor()->getSprite()->getPositionY());
			isTouchDown = false;
		}
		//else if (initialTouchPos[1] - currentTouchPos[1] > size.width * SWIPE_FACTOR)
		//{
		//	//CCLOG("SWIPED DOWN");
		//	isTouchDown = false;
		//}
		//else if (initialTouchPos[1] - currentTouchPos[1] < -size.width * SWIPE_FACTOR)
		//{
		//	//CCLOG("SWIPED UP");
		//	isTouchDown = false;
		//}
	}
}

void WreckingGame::update(float dt) {
	int num = this->myBuilding->getNumber();
	if (num != 0) {
		float upper_position = this->myBuilding->getUpperFloor()->getSprite()->getPositionY();
		float fl_height = this->myBuilding->getUpperFloor()->getSprite()->getBoundingBox().size.height;
		if (upper_position > size.height - fl_height/2 + origin.y) {
			// player loses game
			end = true;
			this->endGame();
			ball->removeFromParent();
		} else if (!end) {
			for (int i = 0; i < num; i++) {
				float old_y = this->myBuilding->getNFloor(i)->getSprite()->getPositionY();
				float y = old_y + vel*dt;
				this->myBuilding->getNFloor(i)->getSprite()->setPositionY(y);
			}
		}
		if (num < NUM_FLOORS && !end) this->generateFloor(false, 0);
	}
	this->checkTouch(num);
}

void WreckingGame::menuCloseCallback(Ref* pSender) {
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        exit(0);
    #endif
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        
    #endif
}

void WreckingGame::afterCaptured(bool succeed, const std::string & outputFile){
#ifdef SDKBOX_ENABLED
	sdkbox::PluginAdMob::show("gameover");
#endif	
	if (succeed) {
#ifdef COCOS2D_DEBUG
		CCLOG(" screen shot %s", outputFile.c_str());
#endif
		Node * t = gamecomm->makeToast("Succeded", 2);
		addChild(t,10);
		t->getActionManager()->resumeTarget(t);
	} else {
		Node * t = gamecomm->makeToast("error encoutered", 2);
		addChild(t, 10);
		t->getActionManager()->resumeTarget(t);
	}
}

void WreckingGame::shareScore(){
	std::string target = "WreckingMadnessScreenshot.png";
#ifdef SDKBOX_ENABLED
	sdkbox::PluginAdMob::hide("gameover");
#endif
	utils::captureScreen(CC_CALLBACK_2(WreckingGame::afterCaptured, this), target);
}

void WreckingGame::spanCloud(bool random) {
	Sprite * cloud = gamecomm->spanCloud();
	float cloud_height = origin.y + ((float) rand() / (float) (RAND_MAX/size.height));
	float cloud_vel = CLOUD_SPEED_OFFSET + (float)rand() / (float)(RAND_MAX / CLOUD_SPEED);
	float cloud_width = cloud->getBoundingBox().size.width;
	float cloud_space = size.width + cloud_width * 2;
	float cloud_x = origin.x + size.width + cloud_width;
	if (random) cloud_x = origin.x + ((float)rand() / (float) (RAND_MAX/size.width));
	cloud->setPosition(Vec2(cloud_x, cloud_height));
	this->addChild(cloud, 1);
	auto new_cloud = CallFunc::create([this]() {
		this->spanCloud(false);
	});
	cloud->runAction(Sequence::create(MoveBy::create(cloud_space/cloud_vel, Vec2(-cloud_space, 0)),
		new_cloud, RemoveSelf::create(), nullptr));
}

void WreckingGame::throwBall(int direction = 1, bool stopped = false, float height = 0) {
	if (!throwing) {
		throwing = true;
		float xpos;
		switch (direction) {
		case -1:
			xpos = origin.x + size.width + gamecomm->getBallRadius()*scale;
			break;
		case 1:
			xpos = origin.x - gamecomm->getBallRadius()*scale;
			break;
		}
		float ypos = height + gamecomm->getBallLength();
		ball->setPosition(Vec2(xpos, ypos));
		float space1 = size.width / 2 - floor_width/2 + gamecomm->getBallRadius()*scale;
		float space = size.width + floor_width;
		if (!stopped) {
			ball->runAction(Sequence::create(
				MoveBy::create(space1 / BREAK_SPEED, Vec2(direction*space1, 0)),
				CallFunc::create(CC_CALLBACK_0(WreckingGame::playCrashSound, this, false)),
				CallFunc::create(CC_CALLBACK_0(WreckingGame::removeTop, this, direction)),
				DelayTime::create(0.02f),
				MoveBy::create(space / BREAK_SPEED, Vec2(direction * space, 0)),
				CallFunc::create(CC_CALLBACK_0(WreckingGame::finishThrow, this)),
				nullptr));
		} else {
			ball->runAction(Sequence::create(
				MoveBy::create(0.05f, Vec2(direction*space1, 0)),
				CallFunc::create(CC_CALLBACK_0(WreckingGame::playCrashSound, this, true)),
				MoveBy::create(0.2f, Vec2(-direction*space1, 0)),
				CallFunc::create(CC_CALLBACK_0(WreckingGame::finishThrow, this)),
				nullptr));
		}
	}
}

void WreckingGame::finishThrow(){
	throwing = false;
}

void WreckingGame::playCrashSound(bool metal = false){
	if (!metal) 
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("hit.wav");
	else 
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("metal_hit.wav");
}

void WreckingGame::percReceived(float perc){
#ifdef COCOS2D_DEBUG
	CCLOG("Percentage received by GAME %f", perc);
#endif
	if (gmenu != nullptr && gmenu->getChildByName("spinner") != nullptr) {
		char str[100];
		snprintf(str, sizeof(str), "Better than\n%.2f %%\nof players", perc);
		gmenu->getChildByName("spinner")->removeFromParent();
		Label * lblperc = Label::createWithTTF(str, gamecomm->text_font, gamecomm->text_size);
		lblperc->setHorizontalAlignment(TextHAlignment::CENTER);
		lblperc->setTextColor(Color4B(255,221,88,255));
		lblperc->setPosition(origin + Vec2(size.width / 2, size.height / 1.4));
		gmenu->addChild(lblperc, 2);
	}
}
 
float WreckingGame::getTimeTick() {
    timeval time;
    gettimeofday(&time, NULL);
    unsigned long millisecs = (time.tv_sec * 1000) + (time.tv_usec/1000);
    return (float) millisecs;
}

void WreckingGame::generateFloor(bool roof, float correction){
	Sprite * sprite;
	float y = origin.y;
	std::string t;
	if (roof) {
		sprite = Sprite::create("roof1.png");
		t = "both";
		floor_width = sprite->getBoundingBox().size.width;

	}
	else {
		t = this->getRandomTypeName();
		sprite = Sprite::create(this->types[t]);
		y = this->myBuilding->getLowerFloor()->getSprite()->getPositionY();
	}
	Floor * myFloor = new Floor(broken, t, sprite);
	float sprite_height = myFloor->getSprite()->getContentSize().height;
	myFloor->getSprite()->setPosition((Vec2(origin.x + size.width / 2, y - sprite_height + correction)));
	this->addChild(myFloor->getSprite(), 3);
	this->myBuilding->addFloor(myFloor);
}

void WreckingGame::removeTop(int dir){
	if (!end) {
		// this space has to be the same also for floor
		float space = size.width + floor_width;
		this->myBuilding->getUpperFloor()->getSprite()->runAction(Sequence::create(
			MoveBy::create(space / BREAK_SPEED, Vec2(dir * space, 0)),
			RemoveSelf::create(),
			nullptr));

		// remove floor from data structure and update score
		this->myBuilding->removeFloor();
		this->score++;
		title->setString(to_string(this->score));
		// change difficulty with score
		if (vel<MAX_VELOCITY) vel += 1.0f / 4;

#ifdef COCOS2D_DEBUG
		CCLOG("Velocity: %f", vel);
#endif
		//ball->runAction(Sequence::create(
		//	DelayTime::create(0.01f),
		//	CallFunc::create(CC_CALLBACK_0(WreckingGame::playCrashSound, this, false)),
		//	MoveBy::create(space / BREAK_SPEED, Vec2(dir * space, 0)),
		//	CallFunc::create(CC_CALLBACK_0(WreckingGame::finishThrow, this)),
		//	nullptr));
	}
}

bool WreckingGame::updateTop(std::string dir)
{
	int sign = 0;
	if (dir == "dx") sign =  1;
	if (dir == "sx") sign = -1;
	switch (this->myBuilding->getUpperFloor()->fl_status){
		case broken: {
			float y = this->myBuilding->getUpperFloor()->getSprite()->getPositionY() +
				this->myBuilding->getUpperFloor()->getSprite()->getContentSize().height/2;
			this->throwBall(sign, false, y);
			return true;
		}
		case half: {
			this->myBuilding->getUpperFloor()->fl_status = broken;
			this->myBuilding->getUpperFloor()->getSprite()->runAction(MoveBy::create((float)0.01, Vec2(30*sign, 0)));
			return false;
		}
		case good: {
			this->myBuilding->getUpperFloor()->fl_status = half;
			this->myBuilding->getUpperFloor()->getSprite()->runAction(MoveBy::create((float)0.01, Vec2(30*sign, 0)));
			return false;
		}
		default: return false;
	}
}

int WreckingGame::getTypesNumber(){
    return this->types.size();
}

std::string WreckingGame::getRandomTypeName(){
    std::vector<std::string> vt_types;
    std::map<std::string,std::string>::iterator it;
    for (it = this->types.begin(); it != this->types.end(); it++){
        vt_types.push_back(it->first);
    }
    int num = vt_types.size();
    return vt_types[rand() % num];
}

floorStatus WreckingGame::getRandomFloorStatus(){
	switch (rand()%3){
	case 0: return broken;
	case 1: return half;
	case 2: return good;
	default: return broken;
	}
}

WreckingGame * WreckingGame::getGame(){
	return ptr;
}
