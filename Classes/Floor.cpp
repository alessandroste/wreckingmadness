#include "Floor.h"

Floor::Floor(floorStatus status, std::string type, Sprite * sprite){
	this->fl_status = status;
	this->fl_type = type;
	this->fl_sprite = sprite;
}

Floor::~Floor(){}

Sprite * Floor::getSprite(){
	return fl_sprite;
}

void Floor::setSprite(Sprite *s){
	fl_sprite = s;
}
