#pragma once
#ifndef _COMMON_H_
#define _COMMON_H_

#include "cocos2d.h"

using namespace cocos2d;

class Common {
private:
	Node * ball = nullptr;
	float ball_length;
	float ball_radius;
public: 
	Color4B background;
	std::string text_font;
	int text_size;
	Node * getBall();
	float getBallLength();
	float getBallRadius();
	Common(unsigned int chain_length = 20);
	~Common();
	Sprite * spanCloud();
	Sprite * getSun();
};


#endif // !_COMMON_H_

