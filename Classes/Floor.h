#ifndef __FLOOR_H__
#define __FLOOR_H__

#include "cocos2d.h"

namespace wreckingmadness {
	enum FloorType {
		NORMAL,
		ROOF,
		METAL_LEFT,
		METAL_RIGHT
	};
	
	enum FloorStatus {
		BROKEN,
		HALF,
		GOOD
	};

	class Floor {
	private:
		cocos2d::Sprite* floorSprite;
		FloorStatus floorStatus;
		FloorType floorType;
		static std::vector<FloorType> availableFloorTypes;
		static std::string getSpriteNameFromType(FloorType floorType);
	public:
		Floor(FloorStatus status, FloorType type);
		~Floor();
		cocos2d::Sprite* getSprite();
		const FloorType& getFloorType() const { return floorType; }
		const FloorStatus& getFloorStatus() const { return floorStatus; }
		static FloorType getRandomFloorType();
	};
}

#endif