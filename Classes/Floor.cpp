#include "Floor.h"
#include "Common.h"
#include "Utilities.h"

using namespace cocos2d;
using namespace wreckingmadness;

std::vector<FloorType> Floor::availableFloorTypes = std::vector<FloorType>{
    FloorType::NORMAL,
    FloorType::METAL_RIGHT,
    FloorType::METAL_LEFT
};

Floor::Floor(FloorStatus status, FloorType type) {
    floorStatus = status;
    floorType = type;
    floorSprite = Sprite::create(getSpriteNameFromType(type));
}

Floor::~Floor() = default;

Sprite* Floor::getSprite() {
    return floorSprite;
}

std::string Floor::getSpriteNameFromType(FloorType floorType) {
    switch (floorType) {
    case FloorType::ROOF:
        return SPRITE_FLOOR_ROOF;
    case FloorType::NORMAL:
        return SPRITE_FLOOR_NORMAL;
    case FloorType::METAL_LEFT:
        return SPRITE_FLOOR_METAL_LEFT;
    case FloorType::METAL_RIGHT:
        return SPRITE_FLOOR_METAL_RIGHT;
    default:
        throw std::exception();
    }
}

FloorType Floor::getRandomFloorType() {
    auto availableTypesSize = availableFloorTypes.size();
    auto randomIndex = std::floor(Utilities::getRandom(0, availableTypesSize - 1));
    return availableFloorTypes.at(randomIndex);
}
