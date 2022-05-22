#include "AppDelegate.h"

#include <iomanip>
#include <sstream>

#include "ui/UIEditBox/UIEditBox.h"
#include "ui/UIImageView.h"
#include "../Integrations/SdkBoxHelper.h"
#include "GameScene.h"
#include "Common.h"
#include "Utilities.h"
#include "SoundService.h"
#ifdef SDKBOX_ENABLED
#include "PluginAdMob/PluginAdMob.h"
#ifdef SDKBOX_FACEBOOK
#include "FacebookListener.h"
#endif
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
    generateFloor(true, 0);

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

#ifdef SDKBOX_FACEBOOK
    sdkbox::PluginFacebook::setListener(new FacebookListener());
#endif
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
                throwBall(-1, true, skyscraper->getUpperFloor()->getSprite()->getPositionY());
            isTouchDown = false;
        }
        else if (initialTouchPos[0] - currentTouchPos[0] < -SWIPE_FACTOR) {
            if (skyscraper->getUpperFloor()->getFloorType() == FloorType::METAL_LEFT ||
                skyscraper->getUpperFloor()->getFloorType() == FloorType::NORMAL ||
                skyscraper->getUpperFloor()->getFloorType() == FloorType::ROOF) {
                updateTop(Direction::RIGHT);
            }
            else
                throwBall(1, true, skyscraper->getUpperFloor()->getSprite()->getPositionY());
            isTouchDown = false;
        }
    }
}

void GameScene::update(float dt) {
    int num = skyscraper->getNumber();
    if (num != 0) {
        float upper_position = skyscraper->getUpperFloor()->getSprite()->getPositionY();
        float fl_height = skyscraper->getUpperFloor()->getSprite()->getBoundingBox().size.height;
        if (upper_position > visibleSize.height - fl_height / 2 + visibleOrigin.y) {
            // player loses game
            end = true;
            ball->getNode()->removeFromParent();
            endGame();
        }
        else if (!end) {
            if (currentSpeed < speedSetPoint)
                currentSpeed += VEL_STEP;
            for (int i = 0; i < num; i++) {
                float y = skyscraper->getNFloor(i)->getSprite()->getPositionY() + currentSpeed * dt;
                skyscraper->getNFloor(i)->getSprite()->setPositionY(y);
            }
            if (skyscraper->getLowerFloor()->getSprite()->getPositionY() > visibleOrigin.y - fl_height)
                generateFloor(false, 0);
        }
    }
    checkTouch(num);
}

void GameScene::screenCapturedCallback(bool succeed, const std::string& outputFile) {
    SdkBoxHelper::ShowAd(AdType::GAMEOVER);
    if (succeed) {
#if (SDKBOX_ENABLED && SDKBOX_FACEBOOK)
        outfile = outputFile;
        if (!sdkbox::PluginFacebook::isLoggedIn()) {
            sdkbox::PluginFacebook::login();
        }
        else {
            checkPostPerm();
        }
#else
        Utilities::makeToast("Screenshot done", ToastDuration::SHORT);
        Director::getInstance()->pause();
        PlatformAbstraction::getInstance()->shareImageFromFile(SCREEN_FILE);
        Director::getInstance()->resume();
#endif
    }
    else {
        Utilities::makeToast("Error encountered while doing screenshot", ToastDuration::SHORT);
    }
}

#if (SDKBOX_ENABLED && SDKBOX_FACEBOOK)
void GameScene::closeShare() {
    getChildByName("popup")->removeAllChildren();
    getChildByName("popup")->removeFromParent();
    endGameMenu->getChildByName("btns")->getChildByName<MenuItemImage*>("btn_restart")->setEnabled(true);
    endGameMenu->getChildByName("btns")->getChildByName<MenuItemImage*>("btn_exit")->setEnabled(true);
    endGameMenu->getChildByName("btns")->getChildByName<MenuItemImage*>("btn_share")->setEnabled(true);
    sdkbox::PluginAdMob::show("gameover");
}

void GameScene::checkPostPerm() {
    bool found = false;
    for (auto& permission : sdkbox::PluginFacebook::getPermissionList()) {
        if (permission.data() == sdkbox::FB_PERM_PUBLISH_POST) {
            found = true;
            CCLOG("Found permission to publish");
            break;
        }
    }
    if (!found) {
        CCLOG("Not found permission to publish");
        sdkbox::PluginFacebook::requestPublishPermissions({ sdkbox::FB_PERM_PUBLISH_POST });
    }
    else {
        shareDialog();
    }
}

void GameScene::shareDialog() {
    Layer* popup = Layer::create();
    ui::ImageView* scr = ui::ImageView::create();
    scr->loadTexture(outfile);
    scr->setScale(0.4);
    scr->setPosition(visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height * 2.2 / 3));
    popup->addChild(scr, 2);
    popup->setName("popup");
    DrawNode* shadow = DrawNode::create();
    shadow->drawSolidRect(Vec2(visibleOrigin.x, visibleOrigin.y), visibleOrigin + Vec2(visibleSize.width, visibleSize.height), Color4F::BLACK);
    popup->addChild(shadow);
    MenuItemImage* button_go = MenuItemImage::create("mb_share_n.png", "mb_share_p.png",
        CC_CALLBACK_0(GameScene::shareScreen, this, outfile, ""));
    button_go->setName("btn_go");
    MenuItemImage* button_ret = MenuItemImage::create("mb_ret_n.png", "mb_ret_p.png",
        CC_CALLBACK_0(GameScene::closeShare, this));
    button_ret->setName("btn_ret");
    Menu* menu_go = Menu::createWithArray({ button_ret, button_go });
    menu_go->alignItemsHorizontally();
    menu_go->setPosition(visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height / 4 - com->text_size));
    menu_go->setName("menu_go");
    popup->addChild(menu_go, 2);
    addChild(popup, 10);
    endGameMenu->getChildByName("btns")->getChildByName<MenuItemImage*>("btn_restart")->setEnabled(false);
    endGameMenu->getChildByName("btns")->getChildByName<MenuItemImage*>("btn_exit")->setEnabled(false);
    endGameMenu->getChildByName("btns")->getChildByName<MenuItemImage*>("btn_share")->setEnabled(false);
    sdkbox::PluginAdMob::hide("gameover");
}
#endif

void GameScene::shareScore() {
    SdkBoxHelper::CloseAd(AdType::GAMEOVER);
    utils::captureScreen(CC_CALLBACK_2(GameScene::screenCapturedCallback, this), SCREEN_FILE);
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

void GameScene::throwBall(int direction = 1, bool stopped = false, float height = 0) {
    if (!throwing) {
        throwing = true;
        auto ballNode = this->ball->getNode();
        auto ballRadius = this->ball->getRadius();
        auto ballLength = this->ball->getLength();
        auto xPos = (direction < 0) ?
            visibleOrigin.x + visibleSize.width + ballRadius * scale :
            visibleOrigin.x - ballRadius * scale;
        auto yPos = height + ballLength;
        ballNode->setPosition(Vec2(xPos, yPos));
        auto space1 = visibleSize.width / 2 - floorWidth / 2 + ballRadius * scale;
        auto space = visibleSize.width + floorWidth;
        if (!stopped) {
            ballNode->runAction(Sequence::create(
                MoveBy::create(space1 / BREAK_SPEED, Vec2(direction * space1, 0)),
                CallFunc::create(std::bind(&GameScene::playCrashSound, false)),
                CallFunc::create(CC_CALLBACK_0(GameScene::removeTop, this, direction)),
                DelayTime::create(0.02f),
                MoveBy::create(space / BREAK_SPEED, Vec2(direction * space, 0)),
                CallFunc::create(std::bind([this] { throwing = false; })),
                nullptr));
        }
        else {
            ballNode->runAction(Sequence::create(
                MoveBy::create(0.05f, Vec2(direction * space1, 0)),
                CallFunc::create(std::bind(&GameScene::playCrashSound, true)),
                MoveBy::create(0.2f, Vec2(-direction * space1, 0)),
                CallFunc::create(std::bind([this] { throwing = false; })),
                nullptr));
        }
    }
}

void GameScene::playCrashSound(bool metal = false) {
    if (!metal)
        SoundService::playEffect(Effect::HIT);
    else
        SoundService::playEffect(Effect::METAL_HIT);
}

void GameScene::percentileReceivedCallback(float percentage) {
    CCLOG("[GameScene] Percentage received by GAME %f", percentage);
    if (endGameMenu != nullptr && endGameMenu->getChildByName(NODE_SPINNER_NAME) != nullptr) {
        std::ostringstream stream;
        stream << "Better than" << std::endl << std::fixed << std::setprecision(2) << percentage << "%" << std::endl << "of players";
        endGameMenu->getChildByName(NODE_SPINNER_NAME)->removeFromParent();
        auto finalScoreLabel = Label::createWithTTF(stream.str(), TEXT_FONT, TEXT_SIZE_DEFAULT, Size::ZERO, TextHAlignment::CENTER);
        finalScoreLabel->setTextColor(Common::ScorePercentageTextColor);
        finalScoreLabel->setPosition(visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height / 1.4f));
        endGameMenu->addChild(finalScoreLabel, 2);
    }
}

#if (SDKBOX_ENABLED && SDKBOX_FACEBOOK)
void GameScene::shareScreen(std::string file, std::string title) {
    sdkbox::FBShareInfo info;
    info.type = sdkbox::FB_PHOTO;
    info.title = title;
    info.image = file;
    sdkbox::PluginFacebook::share(info);
    getChildByName("popup")->getChildByName("menu_go")->getChildByName<MenuItemImage*>("btn_go")->setEnabled(false);
    getChildByName("popup")->getChildByName("menu_go")->getChildByName<MenuItemImage*>("btn_ret")->setEnabled(false);
    Sprite* spinner = Sprite::create("spinner.png");
    spinner->setName("spinner");
    spinner->setScale(0.6);
    spinner->setPosition(visibleOrigin + Vec2(visibleSize.width / 2, visibleSize.height / 1.4));
    spinner->runAction(RepeatForever::create(RotateBy::create(0.1, 2 * M_PI)));
    getChildByName("popup")->addChild(spinner, 15);
}
#endif

void GameScene::generateFloor(bool roof, float correction) {
    auto y = roof ? visibleOrigin.y : skyscraper->getLowerFloor()->getSprite()->getPositionY();
    auto floor = new Floor(FloorStatus::BROKEN, roof ? FloorType::ROOF : Floor::getRandomFloorType());
    floorWidth = floor->getSprite()->getBoundingBox().size.width;
    auto sprite_height = floor->getSprite()->getContentSize().height;
    floor->getSprite()->setPosition((Vec2(visibleOrigin.x + visibleSize.width / 2, y - sprite_height + correction)));
    addChild(floor->getSprite(), 3);
    skyscraper->addFloor(floor);
}

void GameScene::removeTop(int dir) {
    if (!end) {
        // this space has to be the same also for floor
        float space = visibleSize.width + floorWidth;
        skyscraper->getUpperFloor()->getSprite()->runAction(Sequence::create(
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
    int sign = 0;
    if (direction == Direction::RIGHT)
        sign = 1;
    if (direction == Direction::LEFT)
        sign = -1;
    switch (skyscraper->getUpperFloor()->getFloorStatus()) {
    case BROKEN:
        {
            auto y = skyscraper->getUpperFloor()->getSprite()->getPositionY() +
                skyscraper->getUpperFloor()->getSprite()->getContentSize().height / 2;
            throwBall(sign, false, y);
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
    auto stripewidth = 80.0f;
    auto stripedist = 25.0f;
    auto stripes = DrawNode::create();
    stripes->drawTriangle(Vec2(0, 0), Vec2(-stripewidth, 0), Vec2(0, stripewidth), metalColorLight);
    Vec2 stripe1[4] = {
        Vec2(-stripewidth - stripedist,0),
        Vec2(0, stripewidth + stripedist),
        Vec2(0, stripewidth * 2 + stripedist),
        Vec2(-stripewidth * 2 - stripedist, 0)
    };
    stripes->drawPolygon(stripe1, 4, metalColorLight, 0, Color4F::BLACK);
    Vec2 stripe2[4] = {
        Vec2(-stripewidth * 2 - stripedist * 2,0),
        Vec2(0, stripewidth * 2 + stripedist * 2),
        Vec2(0, stripewidth * 2.8f + stripedist * 2),
        Vec2(-stripewidth * 2.8f - stripedist * 2, 0)
    };
    stripes->drawPolygon(stripe2, 4, metalColorLight, 0, Color4F::BLACK);
    stripes->setPosition(origin +
        Vec2(screenSize.width * (1 / FILL + 1.0f / 2), off + screenSize.height * (-1 / FILL + 1.0f / 2)));
    menu->addChild(stripes);

    // drawing bolts
    auto boltradius = 6.0f;
    auto bolt1 = DrawNode::create();
    auto bolt2 = DrawNode::create();
    auto bolt3 = DrawNode::create();
    auto bolt4 = DrawNode::create();
    bolt1->drawSolidCircle(Vec2(0, 0), boltradius, 0, 150, boltColor);
    bolt2->drawSolidCircle(Vec2(0, 0), boltradius, 0, 150, boltColor);
    bolt3->drawSolidCircle(Vec2(0, 0), boltradius, 0, 150, boltColor);
    bolt4->drawSolidCircle(Vec2(0, 0), boltradius, 0, 150, boltColor);
    bolt1->setPosition(origin + Vec2(screenSize.width * (1 / FILL + 1.0f / 2) - boltradius * FILL, screenSize.height * (1 / FILL + 1.0f / 2) - boltradius * FILL));
    bolt2->setPosition(origin + Vec2(screenSize.width * (-1 / FILL + 1.0f / 2) + boltradius * FILL, screenSize.height * (1 / FILL + 1.0f / 2) - boltradius * FILL));
    bolt3->setPosition(origin + Vec2(screenSize.width * (1 / FILL + 1.0f / 2) - boltradius * FILL, off + screenSize.height * (-1 / FILL + 1.0f / 2) + boltradius * FILL));
    bolt4->setPosition(origin + Vec2(screenSize.width * (-1 / FILL + 1.0f / 2) + boltradius * FILL, off + screenSize.height * (-1 / FILL + 1.0f / 2) + boltradius * FILL));
    menu->addChild(bolt1);
    menu->addChild(bolt2);
    menu->addChild(bolt3);
    menu->addChild(bolt4);
    auto engrave = Label::createWithTTF("wrecking madness", TEXT_FONT, 12);
    engrave->setPosition(origin + Vec2(screenSize.width / 2, off + screenSize.height * (-1 / FILL + 1.0f / 2) + boltradius * FILL));
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
    btnShare->setCallback(CC_CALLBACK_0(GameScene::shareScore, this));
    btnRestart->setCallback(std::bind(&GameScene::restartGame));

    auto buttons = Vector<MenuItem*>{ btnRestart, btnShare };
#if (SDKBOX_ENABLED && SDKBOX_FACEBOOK)
    auto btnFacebook = MenuItemImage::create("mb_fb_n.png", "mb_fb_p.png");
    btnFacebook->setName(BUTTON_FACEBOOK);
    buttons.pushBack(btnFacebook);
#endif
    buttons.pushBack(btnExit);
    auto bt_menu = Menu::createWithArray(buttons);
    bt_menu->alignItemsHorizontally();
    bt_menu->setAnchorPoint(Vec2(0, 0.5));
    bt_menu->setPosition(origin + Vec2(
        screenSize.width / 2,
        screenSize.height * (1.0f / 2 - 1 / FILL) + border * 4));
    menu->addChild(bt_menu);

    return menu;
}