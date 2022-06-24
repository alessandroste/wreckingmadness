#include "AppDelegate.h"

#include <iomanip>
#include <sstream>

#include "../Integrations/SdkBoxHelper.h"
#include "GameScene.h"
#include "Common.h"
#include "Utilities.h"
#include "SoundService.h"
#ifdef SDKBOX_ENABLED
#include "PluginAdMob/PluginAdMob.h"
#endif

using namespace cocos2d;
using namespace wreckingmadness;

#define MIN_VELOCITY 300.0f
#define MAX_VELOCITY 450.0f
#define VEL_STEP 0.01f
#define SWIPE_FACTOR 14
#define CLOUD_SPEED 20
#define CLOUD_SPEED_OFFSET 10
#define CLOUD_NUM 20
#define BREAK_SPEED 3000
#define SCREEN_FILE "screenshot.png"
#define NODE_SPINNER_NAME "spinner"

Scene* GameScene::createScene() {
    auto scene = Scene::create();
    auto layer = GameScene::create();
    scene->addChild(layer);
    return scene;
}

bool GameScene::init() {
    if (!LayerColor::initWithColor(Common::BackgroundColor)) {
        return false;
    }

    // viewport measurements
    visibleSize = Director::getInstance()->getVisibleSize();
    visibleOrigin = Director::getInstance()->getVisibleOrigin();
    scale = Director::getInstance()->getContentScaleFactor();

    // currentScore init and currentScore label
    end = false;
    currentScore = 0;
    labelScore = Label::createWithTTF("0", TEXT_FONT, 40);
    labelScore->setPosition(visibleOrigin + Vec2(
        visibleSize.width / 2,
        visibleSize.height - labelScore->getContentSize().height));
    labelScore->setAnchorPoint(Vec2(0.5f, 0.5f));
    labelScore->enableOutline(Color4B(0, 0, 0, 255), 1);
    addChild(labelScore, 5);

    // set up building object
    skyscraper = new Building();

    // first floor
    generateFloor(true);

    // clouds
    for (int i = 0; i < CLOUD_NUM; i++) spanCloud(true);

    // ball
    ball = new Ball(20);
    auto ballNode = ball->getNode();
    ballNode->setPosition(visibleOrigin + Vec2(-visibleSize.width, 0));
    addChild(ballNode, 2);
    throwing = false;

    // sun
    auto sun = Common::getSun();
    sun->setPosition(visibleOrigin + Vec2(-sun->getContentSize().width, visibleSize.height * 2 / 3));
    sun->runAction(RepeatForever::create(Sequence::create(
        MoveBy::create(50, Vec2(visibleSize.width + sun->getContentSize().width * 2, 0)),
        MoveBy::create(0, Vec2(-visibleSize.width - sun->getContentSize().width * 2, 0)),
        nullptr)));
    addChild(sun, 0);

    // touch
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        initialTouchPos[0] = touch->getLocation().x;
        initialTouchPos[1] = touch->getLocation().y;
        currentTouchPos[0] = touch->getLocation().x;
        currentTouchPos[1] = touch->getLocation().y;
        isTouchDown = true;
        return true;
    };
    listener->onTouchMoved = [this](Touch* touch, Event* event) {
        currentTouchPos[0] = touch->getLocation().x;
        currentTouchPos[1] = touch->getLocation().y;
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event) { isTouchDown = false; };
    listener->onTouchCancelled = listener->onTouchEnded;
    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    isTouchDown = false;
    initialTouchPos[0] = 0;
    initialTouchPos[1] = 0;

    // player id
    if (Common::getPlayerID()) {
        auto playerId = UserDefault::getInstance()->getStringForKey(CONFIG_KEY_PLAYER_ID);
        CCLOG("[GameScene] Player ID exists, it is %s", playerId.c_str());
    }

    // difficulty
    currentSpeed = MIN_VELOCITY;
    speedSetPoint = currentSpeed;

    return true;
}

void GameScene::onEnterTransitionDidFinish() {
    scheduleUpdate();
}

void GameScene::onExitTransitionDidStart() {
    unscheduleUpdate();
}

void GameScene::endGame() {
    labelScore->removeFromParent();
    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
    for (int i = 0; i < skyscraper->getNumber(); i++) {
        skyscraper->removeFloor();
    }

    SdkBoxHelper::ShowAd(AdType::GAMEOVER);

    auto localTopScore = Common::getTopLocalScore();
    auto currentTopScore = currentScore > localTopScore ? currentScore : localTopScore;
    endGameMenu = buildEndGameMenu(currentScore, currentTopScore);
    addChild(endGameMenu, 6);

    auto failureCallback = [this]() {
        if (endGameMenu != nullptr && endGameMenu->getChildByName(NODE_SPINNER_NAME) != nullptr) {
            endGameMenu->getChildByName(NODE_SPINNER_NAME)->setVisible(false);
            auto finalScoreLabel = Label::createWithTTF("Well done!", TEXT_FONT, TEXT_SIZE_DEFAULT, Size::ZERO, TextHAlignment::CENTER);
            finalScoreLabel->setTextColor(Common::ScorePercentageTextColor);
            finalScoreLabel->setPosition(visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height / 1.4f));
            endGameMenu->addChild(finalScoreLabel, 2);
        }
    };

    Common::processScore(currentScore, std::bind(&GameScene::percentileReceivedCallback, this, std::placeholders::_1), failureCallback);
}

void GameScene::restartGame() {
    SdkBoxHelper::CloseAd(AdType::GAMEOVER);
    auto transition = TransitionFade::create(0.5, GameScene::createScene());
    Director::getInstance()->replaceScene(transition);
}

void GameScene::checkTouch(int num) {
    if (num > 0 && isTouchDown) {
        if (initialTouchPos[0] - currentTouchPos[0] > SWIPE_FACTOR) {
            if (skyscraper->getUpperFloor()->getFloorType() == FloorType::METAL_RIGHT ||
                skyscraper->getUpperFloor()->getFloorType() == FloorType::NORMAL ||
                skyscraper->getUpperFloor()->getFloorType() == FloorType::ROOF) {
                updateTop(Direction::LEFT);
            }
            else
                throwBall(Direction::LEFT, true, skyscraper->getUpperFloor()->getSprite()->getPositionY());
            isTouchDown = false;
        }
        else if (initialTouchPos[0] - currentTouchPos[0] < -SWIPE_FACTOR) {
            if (skyscraper->getUpperFloor()->getFloorType() == FloorType::METAL_LEFT ||
                skyscraper->getUpperFloor()->getFloorType() == FloorType::NORMAL ||
                skyscraper->getUpperFloor()->getFloorType() == FloorType::ROOF) {
                updateTop(Direction::RIGHT);
            }
            else
                throwBall(Direction::RIGHT, true, skyscraper->getUpperFloor()->getSprite()->getPositionY());
            isTouchDown = false;
        }
    }
}

void GameScene::update(float dt) {
    int num = skyscraper->getNumber();
    if (num != 0) {
        float upperFloorPositionY = skyscraper->getUpperFloor()->getSprite()->getPositionY();
        float floorHeight = skyscraper->getUpperFloor()->getSprite()->getBoundingBox().size.height;
        if (upperFloorPositionY > visibleSize.height - floorHeight / 2 + visibleOrigin.y) {
            // player loses game
            end = true;
            ball->getNode()->removeFromParent();
            endGame();
        }
        else if (!end) {
            if (currentSpeed < speedSetPoint)
                currentSpeed += VEL_STEP;
            for (int i = 0; i < num; i++) {
                auto y = skyscraper->getNFloor(i)->getSprite()->getPositionY() + currentSpeed * dt;
                skyscraper->getNFloor(i)->getSprite()->setPositionY(y);
            }
            if (skyscraper->getLowerFloor()->getSprite()->getPositionY() > visibleOrigin.y - floorHeight)
                generateFloor(false);
        }
    }

    checkTouch(num);
}

void GameScene::screenCapturedCallback(bool succeed, const std::string& outputFile) {
    SdkBoxHelper::ShowAd(AdType::GAMEOVER);
    if (succeed) {
        Utilities::makeToast("Screenshot done", ToastDuration::SHORT);
        Director::getInstance()->pause();
        PlatformAbstraction::getInstance()->shareImageFromFile(SCREEN_FILE);
        Director::getInstance()->resume();
    }
    else {
        Utilities::makeToast("Error encountered while taking screenshot", ToastDuration::SHORT);
    }
}

void GameScene::shareScore() {
    SdkBoxHelper::CloseAd(AdType::GAMEOVER);

    // using utils - suffers from frame size issues
    // utils::captureScreen(std::bind(&GameScene::screenCapturedCallback, std::placeholders::_1, std::placeholders::_2), SCREEN_FILE);

    auto visibleSize = Director::getInstance()->getWinSize();
    auto renderTexture = RenderTexture::create(visibleSize.width, visibleSize.height, PixelFormat::RGB888);
    renderTexture->begin();
    Director::getInstance()->getRunningScene()->visit();
    renderTexture->end();
    renderTexture->saveToFile(SCREEN_FILE, Image::Format::PNG, false, [](RenderTexture* texture, const std::string& fileName) {
        GameScene::screenCapturedCallback(true, fileName);
        });
}

void GameScene::spanCloud(bool random) {
    auto cloud = Common::spanCloud();
    auto cloudHeight = visibleOrigin.y + Utilities::getRandom() * visibleSize.height;
    auto cloudSpeed = CLOUD_SPEED_OFFSET + Utilities::getRandom() * CLOUD_SPEED;
    auto cloudWidth = cloud->getBoundingBox().size.width;
    auto cloudSpace = visibleSize.width + cloudWidth * 2;
    auto cloudX = visibleOrigin.x + visibleSize.width + cloudWidth;
    if (random)
        cloudX = visibleOrigin.x + Utilities::getRandom() * visibleSize.width;
    cloud->setPosition(Vec2(cloudX, cloudHeight));
    addChild(cloud, 1);
    auto cloudSpawnFunction = CallFunc::create([this]() { spanCloud(false); });
    cloud->runAction(Sequence::create(
        MoveBy::create(cloudSpace / cloudSpeed, Vec2(-cloudSpace, 0)),
        cloudSpawnFunction,
        RemoveSelf::create(),
        nullptr));
}

void GameScene::throwBall(Direction direction, bool stopped = false, float height = 0) {
    if (!throwing) {
        throwing = true;
        auto ballNode = ball->getNode();
        auto ballRadius = ball->getRadius();
        auto ballLength = ball->getLength();
        auto xPos = (direction == Direction::LEFT) ?
            visibleOrigin.x + visibleSize.width + ballRadius * scale :
            visibleOrigin.x - ballRadius * scale;
        auto yPos = height + ballLength;
        ballNode->setPosition(Vec2(xPos, yPos));
        auto space1 = visibleSize.width / 2 - floorWidth / 2 + ballRadius * scale;
        auto space = visibleSize.width + floorWidth;
        if (!stopped) {
            ballNode->runAction(Sequence::create(
                MoveBy::create(space1 / BREAK_SPEED, Vec2(direction * space1, 0)),
                CallFunc::create(std::bind(&SoundService::playEffect, Effect::HIT)),
                CallFunc::create(std::bind(&GameScene::removeTop, this, direction)),
                DelayTime::create(0.02f),
                MoveBy::create(space / BREAK_SPEED, Vec2(direction * space, 0)),
                CallFunc::create(std::bind([this] { throwing = false; })),
                nullptr));
        }
        else {
            ballNode->runAction(Sequence::create(
                MoveBy::create(0.05f, Vec2(direction * space1, 0)),
                CallFunc::create(std::bind(&SoundService::playEffect, Effect::METAL_HIT)),
                MoveBy::create(0.2f, Vec2(-direction * space1, 0)),
                CallFunc::create(std::bind([this] { throwing = false; })),
                nullptr));
        }
    }
}

void GameScene::percentileReceivedCallback(float percentage) {
    CCLOG("[GameScene] Percentage received %f", percentage);
    if (endGameMenu != nullptr && endGameMenu->getChildByName(NODE_SPINNER_NAME) != nullptr) {
        endGameMenu->getChildByName(NODE_SPINNER_NAME)->removeFromParent();
        auto message = Utilities::formatString("Better than\n %.2f%% \nof players", percentage);
        auto labelSize = Size(endGameMenu->getBoundingBox().size.width, 0);
        auto finalScoreLabel = Label::createWithTTF(message, TEXT_FONT, TEXT_SIZE_SMALL, labelSize, TextHAlignment::CENTER);
        finalScoreLabel->setMaxLineWidth(endGameMenu->getBoundingBox().size.width);
        finalScoreLabel->setTextColor(Common::ScorePercentageTextColor);
        finalScoreLabel->setPosition(visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height / 1.4f));
        endGameMenu->addChild(finalScoreLabel, 2);
    }
}

void GameScene::generateFloor(bool roof) {
    auto y = roof ? visibleOrigin.y : skyscraper->getLowerFloor()->getSprite()->getPositionY();
    auto floor = new Floor(FloorStatus::BROKEN, roof ? FloorType::ROOF : Floor::getRandomFloorType());
    floorWidth = floor->getSprite()->getBoundingBox().size.width;
    auto spriteHeight = floor->getSprite()->getBoundingBox().size.height;
    floor->getSprite()->setPosition(Vec2(visibleOrigin.x + visibleSize.width / 2, y - spriteHeight));
    addChild(floor->getSprite(), 3);
    skyscraper->addFloor(floor);
}

void GameScene::removeTop(int dir) {
    if (!end) {
        // this space has to be the same also for floor
        auto space = visibleSize.width + floorWidth;
        skyscraper->getUpperFloor()->getSprite()->runAction(
            Sequence::create(
                MoveBy::create(space / BREAK_SPEED, Vec2(dir * space, 0)),
                RemoveSelf::create(),
                nullptr));

        // remove floor from data structure and update currentScore
        skyscraper->removeFloor();
        currentScore++;
        labelScore->setString(Utilities::to_string(currentScore));

        // change difficulty with currentScore
        if (speedSetPoint < MAX_VELOCITY)
            speedSetPoint += 0.25f;

        CCLOG("[GameScene] Velocity set point: %f", currentSpeed);
    }
}

bool GameScene::updateTop(Direction direction) {
    int sign = direction == Direction::RIGHT ? 1 : -1;
    switch (skyscraper->getUpperFloor()->getFloorStatus()) {
    case BROKEN:
        {
            auto y = skyscraper->getUpperFloor()->getSprite()->getPositionY() +
                skyscraper->getUpperFloor()->getSprite()->getContentSize().height / 2;
            throwBall(direction, false, y);
            return true;
        }
    default:
        return false;
    }
}

Node* GameScene::buildEndGameMenu(unsigned int score, int topScore) {
    auto off = 15.0f;
    auto metalColor = Color4F(Common::MetalColor);
    auto metalColorLight = Color4F(Common::MetalColorLight);
    auto boltColor = Color4F(Common::BoltColor);
    auto menu = Node::create();
    auto screenSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();

    // drawing rectangle background
    auto rectNode = DrawNode::create();
    Vec2 rectangle[4];
    rectangle[0] = Vec2(-screenSize.width / FILL, -screenSize.height / FILL + off);
    rectangle[1] = Vec2(screenSize.width / FILL, -screenSize.height / FILL + off);
    rectangle[2] = Vec2(screenSize.width / FILL, screenSize.height / FILL);
    rectangle[3] = Vec2(-screenSize.width / FILL, screenSize.height / FILL);
    rectNode->drawPolygon(rectangle, 4, metalColor, 1, metalColorLight);
    rectNode->setPosition(origin + screenSize / 2);
    menu->addChild(rectNode);

    // drawing light stripes
    auto stripeWidth = 60.0f;
    auto stripeDistance = 20.0f;
    auto stripes = DrawNode::create();
    stripes->drawTriangle(Vec2(0, 0), Vec2(-stripeWidth, 0), Vec2(0, stripeWidth), metalColorLight);
    Vec2 stripe1[4] = {
        Vec2(-stripeWidth - stripeDistance, 0),
        Vec2(0, stripeWidth + stripeDistance),
        Vec2(0, stripeWidth * 2 + stripeDistance),
        Vec2(-stripeWidth * 2 - stripeDistance, 0)
    };
    stripes->drawPolygon(stripe1, 4, metalColorLight, 0, Color4F::BLACK);
    Vec2 stripe2[4] = {
        Vec2(-stripeWidth * 2 - stripeDistance * 2, 0),
        Vec2(0, stripeWidth * 2 + stripeDistance * 2),
        Vec2(0, stripeWidth * 2.8f + stripeDistance * 2),
        Vec2(-stripeWidth * 2.8f - stripeDistance * 2, 0)
    };
    stripes->drawPolygon(stripe2, 4, metalColorLight, 0, Color4F::BLACK);
    stripes->setPosition(origin +
        Vec2(screenSize.width * (1 / FILL + 0.5f), off + screenSize.height * (-1 / FILL + 0.5f)));
    menu->addChild(stripes);

    // drawing bolts
    auto boltRadius = 6.0f;
    auto bolt1 = DrawNode::create();
    auto bolt2 = DrawNode::create();
    auto bolt3 = DrawNode::create();
    auto bolt4 = DrawNode::create();
    bolt1->drawSolidCircle(Vec2(0, 0), boltRadius, 0, 150, boltColor);
    bolt2->drawSolidCircle(Vec2(0, 0), boltRadius, 0, 150, boltColor);
    bolt3->drawSolidCircle(Vec2(0, 0), boltRadius, 0, 150, boltColor);
    bolt4->drawSolidCircle(Vec2(0, 0), boltRadius, 0, 150, boltColor);
    bolt1->setPosition(origin + Vec2(screenSize.width * (1 / FILL + 1.0f / 2) - boltRadius * FILL, screenSize.height * (1 / FILL + 1.0f / 2) - boltRadius * FILL));
    bolt2->setPosition(origin + Vec2(screenSize.width * (-1 / FILL + 1.0f / 2) + boltRadius * FILL, screenSize.height * (1 / FILL + 1.0f / 2) - boltRadius * FILL));
    bolt3->setPosition(origin + Vec2(screenSize.width * (1 / FILL + 1.0f / 2) - boltRadius * FILL, off + screenSize.height * (-1 / FILL + 1.0f / 2) + boltRadius * FILL));
    bolt4->setPosition(origin + Vec2(screenSize.width * (-1 / FILL + 1.0f / 2) + boltRadius * FILL, off + screenSize.height * (-1 / FILL + 1.0f / 2) + boltRadius * FILL));
    menu->addChild(bolt1);
    menu->addChild(bolt2);
    menu->addChild(bolt3);
    menu->addChild(bolt4);
    auto engrave = Label::createWithTTF("wrecking madness", TEXT_FONT, 12);
    engrave->setPosition(origin + Vec2(screenSize.width / 2, off + screenSize.height * (-1 / FILL + 1.0f / 2) + boltRadius * FILL));
    engrave->setTextColor(Common::BoltColorDark);
    menu->addChild(engrave);

    auto spinner = Sprite::create(SPRITE_SPINNER);
    spinner->setName(NODE_SPINNER_NAME);
    spinner->setScale(0.6f);
    spinner->setPosition(origin + Vec2(screenSize.width / 2, screenSize.height / 1.4f));
    spinner->runAction(RepeatForever::create(RotateBy::create(0.1f, 2.0f * M_PI)));
    menu->addChild(spinner);

    // all the labels
    auto border = TEXT_SIZE_DEFAULT / 2;
    auto titleGameOver = Label::createWithTTF(MESSAGE_GAME_OVER, TEXT_FONT, TEXT_SIZE_DEFAULT);
    titleGameOver->setPosition(origin + Vec2(screenSize.width / 2, screenSize.height / 2 + screenSize.height / FILL - titleGameOver->getContentSize().height));
    menu->addChild(titleGameOver, 1);
    auto labelScore = Label::createWithTTF(MESSAGE_YOUR_SCORE, TEXT_FONT, TEXT_SIZE_CREDITS);
    labelScore->setAnchorPoint(Vec2(0, 0.5));
    labelScore->setPosition(origin + Vec2(screenSize.width / 2 - screenSize.width / FILL + border,
        screenSize.height / 2 + TEXT_SIZE_CREDITS));
    menu->addChild(labelScore, 1);
    auto labelTopScore = Label::createWithTTF(MESSAGE_YOUR_TOP_SCORE, TEXT_FONT, TEXT_SIZE_CREDITS);
    labelTopScore->setAnchorPoint(Vec2(0, 0.5));
    labelTopScore->setPosition(origin + Vec2(screenSize.width / 2 - screenSize.width / FILL + border,
        screenSize.height / 2 - TEXT_SIZE_CREDITS * 2));
    menu->addChild(labelTopScore, 1);
    auto score_top = Label::createWithTTF(Utilities::to_string(topScore), TEXT_FONT, TEXT_SIZE_DEFAULT);
    score_top->setAnchorPoint(Vec2(1, 0.5));
    score_top->setPosition(origin + Vec2(screenSize.width / 2 + screenSize.width / FILL - border,
        screenSize.height / 2 - TEXT_SIZE_CREDITS * 4));
    menu->addChild(score_top, 1);
    auto score_ = Label::createWithTTF(Utilities::to_string(score), TEXT_FONT, TEXT_SIZE_DEFAULT);
    score_->setAnchorPoint(Vec2(1, 0.5));
    score_->setPosition(origin + Vec2(screenSize.width / 2 + screenSize.width / FILL - border,
        screenSize.height / 2));
    menu->addChild(score_, 1);

    // buttons
    auto btnExit = MenuItemImage::create(SPRITE_BUTTON_EXIT_NORMAL, SPRITE_BUTTON_EXIT_PRESSED);
    auto btnRestart = MenuItemImage::create(SPRITE_BUTTON_RESTART_NORMAL, SPRITE_BUTTON_RESTART_PRESSED);
    auto btnShare = MenuItemImage::create(SPRITE_BUTTON_SHARE_NORMAL, SPRITE_BUTTON_SHARE_PRESSED);
    btnExit->setCallback(std::bind(&Common::enterMainMenuScene));
    btnShare->setCallback(std::bind(&GameScene::shareScore));
    btnRestart->setCallback(std::bind(&GameScene::restartGame));

    auto buttons = Vector<MenuItem*>{ btnRestart, btnShare, btnExit };
    auto buttonsMenu = Menu::createWithArray(buttons);
    buttonsMenu->setAnchorPoint(Vec2(0, 0.5));
    buttonsMenu->setPosition(origin + Vec2(
        screenSize.width / 2,
        screenSize.height * (0.5f - 1 / FILL) + border * 4));
    menu->addChild(buttonsMenu);
    buttonsMenu->alignItemsHorizontally();

    return menu;
}