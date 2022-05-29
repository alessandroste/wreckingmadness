#include "AppDelegate.h"

#include <utility>
#include "Scenes/MainMenuScene.h"
#include "Integrations/SdkBoxHelper.h"
#include "Utilities.h"
#include "SoundService.h"

using namespace cocos2d;
using namespace wreckingmadness;

static auto designResolutionSize = cocos2d::Size(480, 640);
static auto smallResolutionSize = cocos2d::Size(480, 640);
static auto mediumResolutionSize = cocos2d::Size(960, 1280);
static auto largeResolutionSize = cocos2d::Size(1920, 2560);

AppDelegate::AppDelegate() {};

AppDelegate::~AppDelegate() {};

// if you want a different context,just modify the value of glContextAttrs
// it will takes effect on all platforms
void AppDelegate::initGLContextAttrs() {
    // set OpenGL context attributions,now can only set six attributions:
    // red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = { 8, 8, 8, 8, 24, 8 };
    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages,
// don't modify or remove this function
static int register_all_packages() {
    return 0; // flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    auto director = Director::getInstance();
    auto glView = director->getOpenGLView();
    if (!glView) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glView = GLViewImpl::createWithRect("WreckingMadness", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glView = GLViewImpl::create("WreckingMadness");
#endif
        director->setOpenGLView(glView);
    }

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval((float)1.0 / 60);

    // Set the design resolution
    glView->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    auto frameSize = glView->getFrameSize();
    // if the frame's height is larger than the height of large size.
    if (frameSize.height > mediumResolutionSize.height) {
        director->setContentScaleFactor(MIN(largeResolutionSize.height / designResolutionSize.height, largeResolutionSize.width / designResolutionSize.width));
    }
    // if the frame's height is larger than the height of small size.
    else if (frameSize.height > smallResolutionSize.height) {
        director->setContentScaleFactor(MIN(mediumResolutionSize.height / designResolutionSize.height, mediumResolutionSize.width / designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium size.
    else {
        director->setContentScaleFactor(MIN(smallResolutionSize.height / designResolutionSize.height, smallResolutionSize.width / designResolutionSize.width));
    }

    register_all_packages();
    setSearchPaths();

#ifdef COCOS2D_DEBUG
    director->setDisplayStats(true);
#endif

    SdkBoxHelper::Init();
    director->runWithScene(MainMenuScene::createScene());
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    SoundService::pauseAll();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    SoundService::resumeAll();
}

void AppDelegate::setSearchPaths() {
    auto director = Director::getInstance();
    CCLOG("[AppDelegate] Content scale factor %f", director->getContentScaleFactor());
    std::vector<std::string> searchOrder;
    if (director->getContentScaleFactor() >= 3.0)
        searchOrder.emplace_back("4x");
    if (director->getContentScaleFactor() >= 1.5)
        searchOrder.emplace_back("2x");
    searchOrder.emplace_back("1x");
    FileUtils::getInstance()->setSearchPaths(searchOrder);
}