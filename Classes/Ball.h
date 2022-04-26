#ifndef WRECKINGMADNESS_BALL_H
#define WRECKINGMADNESS_BALL_H

#define CHAIN_DISP 0.17f

#include "cocos2d.h"

namespace wreckingmadness {
    class Ball {
    private:
        unsigned int chainLength;
        float ballLength = 0.0f;
        float ballRadius = 0.0f;
        cocos2d::Node* node;
    public:
        Ball(unsigned int chainLength);
        ~Ball();
        cocos2d::Node* getNode();
        float getLength() const;
        float getRadius() const;
    };
}

#endif //WRECKINGMADNESS_BALL_H
