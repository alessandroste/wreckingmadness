#include "Ball.h"
#include "Common.h"

using namespace cocos2d;
using namespace wreckingmadness;

Ball::Ball(unsigned int chainLength) {
    this->chainLength = chainLength;
    node = Node::create();
    auto ballSprite = Sprite::create(SPRITE_BALL_NAME);
    ballLength += node->getContentSize().height;
    ballRadius = ballSprite->getContentSize().width / 2;
    for (int i = 0; i < chainLength; i++) {
        auto chain = Sprite::create(SPRITE_CHAIN_NAME);
        chain->setPosition(Vec2(0, -i * chain->getContentSize().height * (1 - CHAIN_DISP)));
        ballLength += chain->getContentSize().height * (1 - CHAIN_DISP);
        node->addChild(chain);
        ballSprite->setPosition(Vec2(0, -ballSprite->getContentSize().height / 2 - i * chain->getContentSize().height * (1 - CHAIN_DISP)));
    }

    node->addChild(ballSprite);
}

Ball::~Ball() = default;

cocos2d::Node* Ball::getNode() {
    return node;
}

float Ball::getLength() const {
    return ballLength;
}

float Ball::getRadius() const {
    return ballRadius;
}