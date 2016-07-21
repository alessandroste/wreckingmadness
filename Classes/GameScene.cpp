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

#define NUM_FLOORS 30
#define VELOCITY 320
#define MAX_VELOCITY 450
#define VEL_STEP 0.01
#define SWIPE_FACTOR 14
#define CLOUD_SPEED 20
#define CLOUD_SPEED_OFFSET 10
#define CLOUD_NUM 20
#define BREAK_SPEED 3000
#define SCREEN_FILE "screenshot.png"

Label * title;
Vec2 origin;
Size size;
Node * ball;
bool throwing;
float floor_width = 0;
bool end;
float scale;
WreckingGame * ptr;
Node * gmenu;
float vel, vel_set;

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
	gcomm = new Common(30);

    this->types["sx"]="floor_dx1.png";
    this->types["dx"]="floor_sx1.png";
	this->types["both"] = "floor1.png";

    if ( !LayerColor::initWithColor(gcomm->background) ){
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
    title = Label::createWithTTF("0", gcomm->text_font, 40);
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
	ball = gcomm->getBall();
	ball->setPosition(origin + Vec2(-size.width, 0));
	this->addChild(ball, 2);
	throwing = false;
	// sun

	Sprite * sun = gcomm->getSun();
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
	if (gcomm->getPlayerID()) {
		CCLOG("Player ID exists, it is %d", UserDefault::getInstance()->getIntegerForKey("playerid"));
	}
	
	// difficulty
	vel = VELOCITY;
	vel_set = vel;

	#ifdef SDKBOX_ENABLED
		sdkbox::PluginAdMob::init();
		sdkbox::PluginAdMob::cache("gameover");
		sdkbox::PluginFacebook::init();
		sdkbox::PluginFacebook::setListener(new FBL());
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
	if (score > gcomm->getTopLocalScore()) gcomm->setTopLocalScore(score);

	// menu
	gmenu = gcomm->getEndGameMenu(score, gcomm->getTopLocalScore());
	gmenu->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_exit")->setCallback(CC_CALLBACK_0(WreckingGame::closeCallback, this));
	gmenu->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_share")->setCallback(CC_CALLBACK_0(WreckingGame::shareScore, this));
	gmenu->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_restart")->setCallback(CC_CALLBACK_0(WreckingGame::restartGame, this));
	this->addChild(gmenu,6);

	gcomm->sendScore(score);

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
		//else if (initialTouchPos[1] - currentTouchPos[1] > size.width * SWIPE_FACTOR){
		//	//CCLOG("SWIPED DOWN");
		//	isTouchDown = false;
		//}
		//else if (initialTouchPos[1] - currentTouchPos[1] < -size.width * SWIPE_FACTOR){
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
			if (vel < vel_set) vel += VEL_STEP;
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
#ifdef SDKBOX_ENABLED
		if (!sdkbox::PluginFacebook::isLoggedIn()) {
			sdkbox::PluginFacebook::login();
		} 

		bool found = false;
		for (auto& permission : sdkbox::PluginFacebook::getPermissionList()){
        	if (permission.data() == sdkbox::FB_PERM_PUBLISH_POST){
        		found = true;
        		CCLOG("Found permission to publish");
        		break;
        	}
    	}
    	if (!found){
    		CCLOG("Not found permission to publish");
    		sdkbox::PluginFacebook::requestPublishPermissions({sdkbox::FB_PERM_PUBLISH_POST});
		} else {
			Layer * popup = Layer::create();
			ui::ImageView * scr = ui::ImageView::create();
			scr->loadTexture(outputFile);
			scr->setScale(0.4);
			scr->setPosition(origin + Vec2(size.width / 2, size.height *2 / 3));
			popup->addChild(scr,2);
			popup->setName("popup");
			DrawNode * shadow = DrawNode::create();
			shadow->drawSolidRect(Vec2(origin.x, origin.y), origin + Vec2(size.width, size.height), Color4F::BLACK);
			popup->addChild(shadow);

			// DrawNode * rectNode = DrawNode::create();
			// Vec2 rectangle[4];
			// rectangle[0] = Vec2(-size.width, -20 * Director::getInstance()->getContentScaleFactor());
			// rectangle[1] = Vec2(size.width, -20 * Director::getInstance()->getContentScaleFactor());
			// rectangle[2] = Vec2(size.width, 20 * Director::getInstance()->getContentScaleFactor());
			// rectangle[3] = Vec2(-size.width, 20 * Director::getInstance()->getContentScaleFactor());
			// rectNode->drawPolygon(rectangle, 4, Color4F(172 / 255.0, 192 / 255.0, 193 / 255.0, 1), 1, Color4F(180 / 255.0, 198 / 255.0, 199 / 255.0, 1));
			// rectNode->setPosition(origin + Vec2(size.width / 2, size.height / 3));
			// popup->addChild(rectNode, 1);
			//ui::EditBox * edit = ui::EditBox::create(Size(size.width, 40 * Director::getInstance()->getContentScaleFactor()), "null");
			//edit->setName("edb_name");
			//edit->setPosition(Vec2(size.width / 2, size.height / 3));
			//edit->setMaxLength(18);
			//edit->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
			//popup->addChild(edit, 2);

			MenuItemImage * button_go = MenuItemImage::create("mb_share_n.png", "mb_share_p.png",
				CC_CALLBACK_0(WreckingGame::shareScreen, this, outputFile, ""));
			button_go->setName("btn_go");
			MenuItemImage * button_ret = MenuItemImage::create("mb_ret_n.png", "mb_ret_p.png",
				CC_CALLBACK_0(WreckingGame::closeShare, this));
			button_ret->setName("btn_ret");
			Menu * menu_go = Menu::createWithArray({button_ret, button_go});
			menu_go->alignItemsHorizontally();
			menu_go->setPosition(origin + Vec2(size.width / 2, size.height / 4 - gcomm->text_size));
			menu_go->setName("menu_go");
			popup->addChild(menu_go, 2);
			this->addChild(popup, 10);
			gmenu->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_restart")->setEnabled(false);
			gmenu->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_exit")->setEnabled(false);
			gmenu->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_share")->setEnabled(false);
			sdkbox::PluginAdMob::hide("gameover");
		}
		
#else
		gcomm->makeToast("Screenshot done", 2, this);
#endif
	} else {
		gcomm->makeToast("error encoutered while doing screenshot", 2, this);
	}
}

#ifdef SDKBOX_ENABLED
void WreckingGame::closeShare(){
	getChildByName("popup")->removeAllChildren();
	getChildByName("popup")->removeFromParent();
	gmenu->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_restart")->setEnabled(true);
	gmenu->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_exit")->setEnabled(true);
	gmenu->getChildByName("btns")->getChildByName<MenuItemImage *>("btn_share")->setEnabled(true);
	sdkbox::PluginAdMob::show("gameover");
}
#endif

void WreckingGame::shareScore(){
#ifdef SDKBOX_ENABLED
	sdkbox::PluginAdMob::hide("gameover");
#endif
	utils::captureScreen(CC_CALLBACK_2(WreckingGame::afterCaptured, this), SCREEN_FILE);
}

void WreckingGame::spanCloud(bool random) {
	Sprite * cloud = gcomm->spanCloud();
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
			xpos = origin.x + size.width + gcomm->getBallRadius()*scale;
			break;
		case 1:
			xpos = origin.x - gcomm->getBallRadius()*scale;
			break;
		}
		float ypos = height + gcomm->getBallLength();
		ball->setPosition(Vec2(xpos, ypos));
		float space1 = size.width / 2 - floor_width/2 + gcomm->getBallRadius()*scale;
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
		Label * lblperc = Label::createWithTTF(str, gcomm->text_font, gcomm->text_size);
		lblperc->setHorizontalAlignment(TextHAlignment::CENTER);
		lblperc->setTextColor(Color4B(255,221,88,255));
		lblperc->setPosition(origin + Vec2(size.width / 2, size.height / 1.4));
		gmenu->addChild(lblperc, 2);
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
	spinner->setPosition(origin + Vec2(size.width / 2, size.height / 1.4));
	spinner->runAction(RepeatForever::create(RotateBy::create(0.1,2*M_PI)));
	getChildByName("popup")->addChild(spinner,15);
}

#endif // SDKBOX_ENABLED
 
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
		if (vel_set<MAX_VELOCITY) vel_set += 1.0f / 4;

#ifdef COCOS2D_DEBUG
		CCLOG("Velocity setpoint: %f", vel);
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
