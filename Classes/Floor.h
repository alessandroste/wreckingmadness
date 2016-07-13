#ifndef __FLOOR_H__
#define __FLOOR_H__
#include "cocos2d.h"

using namespace cocos2d;

enum floorStatus {
	broken, half, good
};

class Floor
{
private:
	Sprite* fl_sprite;
public:
	Floor(floorStatus status, std::string type, Sprite * sprite);
	~Floor();
	floorStatus fl_status;
	Sprite* getSprite();
	void setSprite(Sprite * s);
	std::string fl_type;
};

#endif