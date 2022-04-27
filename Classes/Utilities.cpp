#include "Utilities.h"
#include "Common.h"

using namespace cocos2d;
using namespace wreckingmadness;

std::random_device Utilities::randomDevice;
std::mt19937 Utilities::randomEngine = std::mt19937(randomDevice());
std::uniform_real_distribution<float> Utilities::floatDistribution = std::uniform_real_distribution<float>(0, 1);
std::map<ToastDuration, float> Utilities::durationMapping = std::map<ToastDuration, float>{
    { ToastDuration::SHORT, 2.0f },
    { ToastDuration::MEDIUM, 3.0f },
    { ToastDuration::LONG, 4.0f }
};

void Utilities::makeToast(std::string const& text, ToastDuration duration) {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto visibleOrigin = Director::getInstance()->getVisibleOrigin();
    auto backgroundColor = Color4F(Common::ToastBackgroundColor);
    auto toast = Node::create();
    auto back = DrawNode::create();
    auto labelSize = Size(visibleSize.width * TOAST_HORIZONTAL_FILL, 0);
    auto label = Label::createWithTTF(text, TEXT_FONT, TEXT_SIZE_CREDITS,
                                      labelSize, TextHAlignment::CENTER);
    auto labelHeight = label->getContentSize().height;
    auto height = 20 + (labelHeight > TOAST_MIN_HEIGHT ? labelHeight : TOAST_MIN_HEIGHT);
    back->drawSolidRect(Vec2(0, 0), 
                        Vec2(visibleSize.width, height), backgroundColor);
    back->setPosition(Vec2(0, 0));
    toast->addChild(back);
    label->setPosition(Vec2(visibleSize.width / 2, height / 2));
    toast->addChild(label);
    auto act = Sequence::create(
        MoveTo::create(1,
                       visibleOrigin + Vec2(0, visibleSize.height - height)),
        DelayTime::create(durationMapping[duration]),
        MoveBy::create(1, Vec2(0, height)),
        RemoveSelf::create(),
        nullptr);
    toast->setPosition(visibleOrigin + Vec2(0, visibleSize.height));
    Director::getInstance()->getRunningScene()->addChild(toast, 999);
    toast->runAction(act);
}

float Utilities::getRandom() {
    return floatDistribution(randomEngine);
}

int Utilities::getRandom(int min, int max) {
    auto intDistribution = std::uniform_int_distribution<int>(min, max);
    return intDistribution(randomEngine);
}