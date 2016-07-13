#include "MainMenuScene.h"
#include "GameScene.h"
#include "Common.h"
#include "SimpleAudioEngine.h"
#ifdef SDKBOX_ENABLED
	#include "pluginadmob/PluginAdMob.h"
#endif

USING_NS_CC;

#define BALL_ANGLE 15
#define BALL_SPEED 10
#define BALL_GRAVITY 9.81
#define CLOUD_NUM 20
#define CLOUD_SPEED 20
#define CLOUD_SPEED_OFFSET 10

float deltatime;
Node * menu_ball;
Common * comm;

#ifdef SDKBOX_ENABLED
	class ADListener : public sdkbox::AdMobListener {
	private:
	    virtual void adViewDidReceiveAd(const std::string &name) {
	    	if (name == "gameover") sdkbox::PluginAdMob::show("gameover");
	    	CCLOG("AD RECEIVED");
	    }
	    virtual void adViewDidFailToReceiveAdWithError(const std::string &name, const std::string &msg) {}
	    virtual void adViewWillPresentScreen(const std::string &name) {}
	    virtual void adViewDidDismissScreen(const std::string &name) {}
	    virtual void adViewWillDismissScreen(const std::string &name) {
	        if (name == "gameover") sdkbox::PluginAdMob::cache("gameover");
	    	CCLOG("AD DISMISSED");
	    }
	    virtual void adViewWillLeaveApplication(const std::string &name) {}
	};
#endif

Scene* TheMenu::createScene()
{
    auto scene = Scene::create();
    auto layer = TheMenu::create();
    scene->addChild(layer);
    return scene;
}

bool TheMenu::init() {
	comm = new Common(5);
    if ( !LayerColor::initWithColor(comm->background) ){
        return false;
    }
	this->origin = Director::getInstance()->getVisibleOrigin();
	this->size = Director::getInstance()->getVisibleSize();

	// menu entries
	Label * button_start_label = Label::createWithTTF("START", comm->text_font, comm->text_size);
	button_start_label->setHorizontalAlignment(TextHAlignment::CENTER);
	button_start_label->enableOutline(Color4B(0, 0, 0, 255), 2);
	MenuItemLabel * button_start = MenuItemLabel::create(button_start_label, CC_CALLBACK_0(TheMenu::startGame, this));
	Label * button_exit_label = Label::createWithTTF("EXIT", comm->text_font, comm->text_size);
	button_exit_label->setHorizontalAlignment(TextHAlignment::CENTER);
	button_exit_label->enableOutline(Color4B(0, 0, 0, 255), 2);
	MenuItemLabel * button_exit = MenuItemLabel::create(button_exit_label, CC_CALLBACK_1(TheMenu::menuCloseCallback, this));
	Vector<MenuItem *> items;
	items.pushBack(button_start);
	items.pushBack(button_exit);
	Menu * menu = Menu::createWithArray(items);
	menu->alignItemsVertically();

	// add menu
	menu->setPosition(Vec2(this->origin.x+this->size.width/2,this->origin.y+this->size.height/3));
	this->addChild(menu, 2);

	// add logo
	Sprite * logo = Sprite::create("logo.png");
	logo->setPosition(Vec2(this->origin.x + this->size.width / 2, this->origin.y + this->size.height *2 / 3));
	logo->setScale(size.width/logo->getContentSize().width);
	this->addChild(logo, 2);

	// add ball
	menu_ball = comm->getBall();
	menu_ball->setPosition(Vec2(this->origin.x+this->size.width/2, this->origin.y+this->size.height));
	menu_ball->setRotation(-BALL_ANGLE / 2);
	deltatime = 0;
	this->addChild(menu_ball,1);

	// clouds
	for (int i = 0; i < CLOUD_NUM; i++) this->spanCloud(true);

	// sounds
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect("hit.wav");
	CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic("wreckingsound.wav",1);

	#ifdef SDKBOX_ENABLED
		sdkbox::PluginAdMob::init();
		sdkbox::PluginAdMob::cache("gameover");
		CCLOG("ADMOB INITIALIZED MENU");
		sdkbox::PluginAdMob::setListener(new ADListener());
	#endif

    return true;
}

void TheMenu::update(float dt) {
	deltatime += dt;
	if (menu_ball != nullptr) {
		float l = comm->getBallLength();
		float theta = BALL_ANGLE * sin(sqrt(BALL_GRAVITY / l)*deltatime*BALL_SPEED);
		menu_ball->setRotation(theta);
	}
}

void TheMenu::spanCloud(bool random) {
	Sprite * cloud = comm->spanCloud();
	float cloud_height = this->origin.y + ((float)rand() / (float)(RAND_MAX / this->size.height));
	float cloud_vel = CLOUD_SPEED_OFFSET + (float)rand() / (float)(RAND_MAX / CLOUD_SPEED);
	float cloud_width = cloud->getBoundingBox().size.width;
	float cloud_space = this->size.width + cloud_width * 2;
	float cloud_x = this->origin.x + this->size.width + cloud_width;
	if (random) cloud_x = this->origin.x + ((float)rand() / (float)(RAND_MAX / this->size.width));
	cloud->setPosition(Vec2(cloud_x, cloud_height));
	this->addChild(cloud, 0);
	auto new_cloud = CallFunc::create([this]() {
		this->spanCloud(false);
	});
	cloud->runAction(Sequence::create(MoveBy::create(cloud_space / cloud_vel, Vec2(-cloud_space, 0)),
		new_cloud, RemoveSelf::create(), nullptr));
}

void TheMenu::onEnterTransitionDidFinish() {
	this->scheduleUpdate();
}

void TheMenu::onExitTransitionDidFinish(){
	this->unscheduleUpdate();
}

void TheMenu::startGame(){
	#ifdef SDKBOX_ENABLED
		sdkbox::PluginAdMob::hide("gameover");
		sdkbox::PluginAdMob::cache("gameover");
	#endif
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("hit.wav");
	Director::getInstance()->replaceScene(TransitionFade::create(0.5, WreckingGame::createScene()));
}

void TheMenu::menuCloseCallback(Ref* pSender)
{
	CocosDenshion::SimpleAudioEngine::getInstance()->unloadEffect("hit.wav");
    Director::getInstance()->end();
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        exit(0);
    #endif
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)  
    #endif
}