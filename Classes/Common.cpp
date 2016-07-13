#include "Common.h"

#define CHAIN_DISP 0.17

Sprite * ball_s;
std::vector<Sprite *> chain_s;

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

float Common::getBallRadius()
{
	return this->ball_radius;
}

Common::Common(unsigned int chain_length){
	// common settings
	this->background = Color4B(170, 210, 230, 255);
	this->text_font = "fonts/Bungee-Regular.ttf";
	this->text_size = 45;

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
