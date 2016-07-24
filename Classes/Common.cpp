#include "Common.h"
#include "network\HttpClient.h"
#include "MD5.h"
#include "GameScene.h"

#define CHAIN_DISP 0.17
#define SCORE_KEY "poefjowvoavsdpf"
#define ID_KEY "owqenpsdfopwndsa"
#define API_LOC "http://alessandroste.host-ed.me/wm_services/"
#define GID_LOC "GetPlayerID.php"
#define UPD_LOC "UpdateScore.php"
#define FILL 2.2

Sprite * ball_s;
std::vector<Sprite *> chain_s;
bool requesting;
bool sending;

// for Android NDK
template <typename T>
std::string to_string(T value) {
	std::ostringstream os;
	os << value;
	return os.str();
}

Node * Common::getBall(){
	return ball;
}

float Common::getBallLength(){
	return this->ball_length;
}

float Common::getBallRadius() {
	return this->ball_radius;
}

unsigned int Common::getTopLocalScore() {
	return UserDefault::getInstance()->getIntegerForKey("ts");
}

void Common::setTopLocalScore(unsigned int score) {
	UserDefault::getInstance()->setIntegerForKey("ts",score);
}

Node * Common::getEndGameMenu(int score, int top_score){
	float off = 15;
	Color4F color(172 / 255.0, 192 / 255.0, 193 / 255.0, 1);
	Color4F lightcolor(180 / 255.0, 198 / 255.0, 199 / 255.0, 1);
	Color4B darkcolor(116, 139, 139, 1);
	Color4F boltcolor(161 / 255.0, 177 / 255.0, 177 / 255.0, 1);
	Color4B boltcolorB(150, 166, 166, 255);
	Node * menu = new Node;
	Size screensize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	// drawing rectangle background
	DrawNode * rectNode = DrawNode::create();
	Vec2 rectangle[4];
	rectangle[0] = Vec2(-screensize.width/FILL, -screensize.height/FILL + off);
	rectangle[1] = Vec2(screensize.width/FILL, -screensize.height/FILL + off);
	rectangle[2] = Vec2(screensize.width/FILL, screensize.height/FILL);
	rectangle[3] = Vec2(-screensize.width/FILL, screensize.height/FILL);
	rectNode->drawPolygon(rectangle, 4, color, 1, lightcolor);
	rectNode->setPosition(origin + screensize/2);
	menu->addChild(rectNode);

	// drawing light stripes
	float stripewidth = 80;
	float stripedist = 25;
	DrawNode * stripes = DrawNode::create();
	stripes->drawTriangle(Vec2(0, 0), Vec2(-stripewidth, 0), Vec2(0, stripewidth), lightcolor);
	Vec2 stripe1[4] = {
		Vec2(-stripewidth - stripedist,0),
		Vec2(0, stripewidth + stripedist),
		Vec2(0, stripewidth*2 + stripedist),
		Vec2(-stripewidth*2 - stripedist, 0)
	};
	stripes->drawPolygon(stripe1, 4, lightcolor, 0, Color4F::BLACK);
	Vec2 stripe2[4] = {
		Vec2(-stripewidth*2 - stripedist*2,0),
		Vec2(0, stripewidth*2 + stripedist*2),
		Vec2(0, stripewidth * 2.8 + stripedist*2),
		Vec2(-stripewidth * 2.8 - stripedist*2, 0)
	};
	stripes->drawPolygon(stripe2, 4, lightcolor, 0, Color4F::BLACK);
	stripes->setPosition(origin + Vec2(screensize.width * (1 / FILL + 1.0 / 2),off + screensize.height * (-1 / FILL + 1.0 / 2)));
	menu->addChild(stripes);

	// drawing bolts
	float boltradius = 6;
	DrawNode * bolt1 = DrawNode::create();
	DrawNode * bolt2 = DrawNode::create();
	DrawNode * bolt3 = DrawNode::create();
	DrawNode * bolt4 = DrawNode::create();
	bolt1->drawSolidCircle(Vec2(0, 0), boltradius, 0, 150, boltcolor);
	bolt2->drawSolidCircle(Vec2(0, 0), boltradius, 0, 150, boltcolor);
	bolt3->drawSolidCircle(Vec2(0, 0), boltradius, 0, 150, boltcolor);
	bolt4->drawSolidCircle(Vec2(0, 0), boltradius, 0, 150, boltcolor);
	bolt1->setPosition(origin + Vec2(screensize.width * (1 / FILL + 1.0 / 2) - boltradius*FILL, screensize.height * (1 / FILL + 1.0 / 2) - boltradius*FILL));
	bolt2->setPosition(origin + Vec2(screensize.width * (-1 / FILL + 1.0 / 2) + boltradius*FILL, screensize.height * (1 / FILL + 1.0 / 2) - boltradius*FILL));
	bolt3->setPosition(origin + Vec2(screensize.width * (1 / FILL + 1.0 / 2) - boltradius*FILL, off + screensize.height * (-1 / FILL + 1.0 / 2) + boltradius*FILL));
	bolt4->setPosition(origin + Vec2(screensize.width * (-1 / FILL + 1.0 / 2) + boltradius*FILL, off + screensize.height * (-1 / FILL + 1.0 / 2) + boltradius*FILL));
	menu->addChild(bolt1);
	menu->addChild(bolt2);
	menu->addChild(bolt3);
	menu->addChild(bolt4);
	Label * engrave = Label::createWithTTF("wrecking madness", text_font, 12);
	engrave->setPosition(origin + Vec2(screensize.width/2,off + screensize.height * (-1 / FILL + 1.0 / 2) + boltradius*FILL));
	engrave->setTextColor(boltcolorB);
	menu->addChild(engrave);

	Sprite * spinner = Sprite::create("spinner.png");
	spinner->setName("spinner");
	spinner->setScale(0.6);
	spinner->setPosition(origin + Vec2(screensize.width / 2, screensize.height / 1.4));
	spinner->runAction(RepeatForever::create(RotateBy::create(0.1,2*M_PI)));
	menu->addChild(spinner);

	// all the labels
	float border = text_size / 2;
	Label * title_lose = Label::createWithTTF("you lost", text_font, text_size);
	title_lose->setPosition(origin + Vec2(screensize.width / 2, screensize.height/2 + screensize.height/FILL - title_lose->getContentSize().height));
	menu->addChild(title_lose,1);
	Label * label_score = Label::createWithTTF("score", text_font, text_size);
	label_score->setAnchorPoint(Vec2(0,0.5));
	label_score->setPosition(origin + Vec2(screensize.width / 2 - screensize.width/FILL + border,
		screensize.height / 2 + text_size));
	menu->addChild(label_score,1);
	Label * label_top = Label::createWithTTF("your\ntop score", text_font, text_size);
	label_top->setAnchorPoint(Vec2(0,0.5));
	label_top->setPosition(origin + Vec2(screensize.width / 2 - screensize.width/FILL + border,
		screensize.height / 2 - text_size*2));
	menu->addChild(label_top,1);
	Label * score_top = Label::createWithTTF(to_string(top_score), text_font, text_size);
	score_top->setAnchorPoint(Vec2(1, 0.5));
	score_top->setPosition(origin + Vec2(screensize.width / 2 + screensize.width / FILL - border,
		screensize.height / 2 - text_size*4));
	menu->addChild(score_top,1);
	Label * score_ = Label::createWithTTF(to_string(score), text_font, text_size);
	score_->setAnchorPoint(Vec2(1, 0.5));
	score_->setPosition(origin + Vec2(screensize.width / 2 + screensize.width / FILL - border,
		screensize.height / 2));
	menu->addChild(score_,1);

	// buttons
	MenuItemImage * mb_exit = MenuItemImage::create("mb_exit_n.png", "mb_exit_p.png");
	MenuItemImage * mb_share = MenuItemImage::create("mb_fb_n.png", "mb_fb_p.png");
	MenuItemImage * mb_restart = MenuItemImage::create("mb_restart_n.png", "mb_restart_p.png");
	mb_exit->setName("btn_exit");
	mb_share->setName("btn_share");
	mb_restart->setName("btn_restart");
	Vector<MenuItem *> mbuttons;
	mbuttons.pushBack(mb_exit);
	mbuttons.pushBack(mb_share);
	mbuttons.pushBack(mb_restart);
	Menu * bt_menu = Menu::createWithArray(mbuttons);
	bt_menu->setName("btns");
	bt_menu->alignItemsHorizontally();
	bt_menu->setAnchorPoint(Vec2(0, 0.5));
	bt_menu->setPosition(origin + Vec2(
		screensize.width / 2,
		screensize.height * (1.0/2 - 1/FILL) + border * 4));
	menu->addChild(bt_menu);
	return menu;
}

void Common::sendScore(unsigned int score) {
	if (!sending && !requesting) {
		network::HttpRequest * httpc = new network::HttpRequest();
		std::string sscore = to_string(score);
		std::string sid = to_string(UserDefault::getInstance()->getIntegerForKey("playerid"));
		std::string tobehashed = sid + sscore + SCORE_KEY;
		std::string url = API_LOC;
		url.append(UPD_LOC).append("?hash=").append(md5(tobehashed));
		url.append("&id=").append(sid);
		url.append("&score=").append(sscore);
		CCLOG("Updating new score to %s", url.c_str());
		httpc->setUrl(url);
		httpc->setRequestType(network::HttpRequest::Type::GET);
		httpc->setResponseCallback(CC_CALLBACK_2(Common::onHttpRequestCompleted, this));
		cocos2d::network::HttpClient::getInstance()->send(httpc);
		sending = true;
		httpc->release();
	}
}

bool Common::getPlayerID(){
	if (UserDefault::getInstance()->getIntegerForKey("playerid", -1) != -1) return true;
	else if (!requesting) {	
		network::HttpRequest * httpc = new network::HttpRequest();
		std::string url = API_LOC;
		url.append(GID_LOC);
		url.append("?hash=");
		url.append(md5(ID_KEY));
		CCLOG("Requesting a new ID from %s", url.c_str());
		httpc->setUrl(url);
		httpc->setRequestType(network::HttpRequest::Type::GET);
		httpc->setResponseCallback(CC_CALLBACK_2(Common::onHttpRequestCompleted, this));
		cocos2d::network::HttpClient::getInstance()->send(httpc);
		requesting = true;
		httpc->release();
		return false;
	} else return false;
}

void Common::onHttpRequestCompleted(cocos2d::network::HttpClient * sender, cocos2d::network::HttpResponse * response){
	if (requesting) {
		std::vector<char> * buffer = response->getResponseData();
		char * concatenated = (char *)malloc(buffer->size() + 1);
		std::string s2(buffer->begin(), buffer->end());
		std::strcpy(concatenated, s2.c_str());
		CCLOG("Response for ID request %s", concatenated);
		int id = atoi(concatenated);
		CCLOG("Casted ID is %d", id);
		if (id != 0) {
			UserDefault::getInstance()->setIntegerForKey("playerid", id);
		}
		requesting = false;
	}
	if (sending) {
		std::vector<char> * buffer = response->getResponseData();
		char * concatenated = (char *)malloc(buffer->size() + 1);
		std::string s2(buffer->begin(), buffer->end());
		std::strcpy(concatenated, s2.c_str());
		CCLOG("Response for score update %s", concatenated);
		float perc = atof(concatenated);
		WreckingGame * g = WreckingGame::getGame();
		if (g == nullptr) {
			CCLOG("Can't connect to game scene");
		} else {
			g->percReceived(perc);
		}
		sending = false;
	}
}

Common::Common(unsigned int chain_length){
	// common settings
	this->background = Color4B(170, 210, 230, 255);
	this->text_font = "fonts/Bungee-Regular.ttf";
	this->text_size = 40;
	credits_size = 25;
	this->playernamecolor = Color3B(205, 220, 220);
	this->playernamecolor_dark = Color4B(161, 177, 177, 255);
	requesting = false;
	sending = false;

	// common ball
	this->ball_length = 0;
	ball = Node::create();
	ball_s = Sprite::create("ball.png");
	this->ball_length += ball->getContentSize().height;
	this->ball_radius = ball_s->getContentSize().width / 2;
	for (int i = 0; i < chain_length; i++) {
		Sprite * chain = Sprite::create("chain.png");
		chain->setPosition(Vec2(0,-i*chain->getContentSize().height*(1-CHAIN_DISP)));
		chain_s.push_back(chain);
		this->ball_length += chain->getContentSize().height*(1-CHAIN_DISP);
		ball->addChild(chain);
		ball_s->setPosition(Vec2(0,-ball_s->getContentSize().height/2 -i*chain->getContentSize().height*(1 - CHAIN_DISP)));
	}
	ball->addChild(ball_s);
}

Common::~Common(){
}

Sprite * Common::spanCloud(){
	// 9 is number of sprite versions
	int num = rand() % 9 + 1;
	std::string s = "clouds/cloud" + to_string(num) + ".png";
	Sprite * cloud = Sprite::create(s);
	return cloud;
}

Sprite * Common::getSun(){
	Animation * ani = Animation::create();
	ani->addSpriteFrameWithFile("sun_1.png");
	ani->addSpriteFrameWithFile("sun_2.png");
	ani->setDelayPerUnit(1.0f/2);
	Sprite * sun = Sprite::create("sun_1.png");
	sun->runAction(RepeatForever::create(Animate::create(ani)));
	return sun;
}

void Common::makeToast(std::string text, float duration, Layer * scene) {
	if (scene != nullptr) {
		float min_height = 20;
		int text_size = 25;
		Color4F back_col(0.3, 0.3, 0.3, 1);
		Node * toast = new Node;
		DrawNode * back = DrawNode::create();
		Label * lbl = Label::createWithTTF(text, text_font, text_size,
			Size(Director::getInstance()->getVisibleSize().width / FILL, 0), TextHAlignment::CENTER);
		float height;
		(lbl->getContentSize().height > min_height) ? height = lbl->getContentSize().height : height = min_height;
		height += 20;
		back->drawSolidRect(
			Vec2(0, 0),
			Vec2(
				Director::getInstance()->getVisibleSize().width,
				height
				), back_col);
		back->setPosition(Vec2(0, 0));
		toast->addChild(back);
		lbl->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2, height / 2));
		toast->addChild(lbl);
		Action * act = Sequence::create(
			MoveTo::create(1, Director::getInstance()->getVisibleOrigin() + Vec2(
				0, Director::getInstance()->getVisibleSize().height - height)),
			DelayTime::create(duration),
			MoveBy::create(1, Vec2(0, height)),
			RemoveSelf::create(),
			nullptr);
		toast->setPosition(Director::getInstance()->getVisibleOrigin() + Vec2(0, Director::getInstance()->getVisibleSize().height));
		scene->addChild(toast, 99);
		toast->runAction(act);
	}
}
