#include "AppDelegate.h"
#include "MainMenuScene.h"
#include "Utils.h"
#include "SimpleAudioEngine.h"
#ifdef SDKBOX_ENABLED
    #include "pluginadmob/PluginAdMob.h"
    #include "sdkbox/Sdkbox.h"
#endif

USING_NS_CC;

static cocos2d::Size designResolutionSize = cocos2d::Size(480, 640);
static cocos2d::Size smallResolutionSize = cocos2d::Size(480, 640);
static cocos2d::Size mediumResolutionSize = cocos2d::Size(960, 1280);
static cocos2d::Size largeResolutionSize = cocos2d::Size(1920, 2560);

AppDelegate::AppDelegate() {}

AppDelegate::~AppDelegate() {}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs() {
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages() {
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("WreckingMadness", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("WreckingMadness");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
#ifdef COCOS2D_DEBUG
    director->setDisplayStats(true);
#endif

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval((float)1.0 / 60);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::NO_BORDER);
    Size frameSize = glview->getFrameSize();
    // if the frame's height is larger than the height of medium size.
    if (frameSize.height > mediumResolutionSize.height) {        
        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is larger than the height of small size.
    else if (frameSize.height > smallResolutionSize.height) {        
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium size.
    else {        
        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    }

    register_all_packages();

	// cocos2d-x uses a folder based approach
	//CCLOG("Content scale factor %f", director->getContentScaleFactor());
	std::vector<std::string> searchOrder;
	if (director->getContentScaleFactor() >= 3.0) searchOrder.push_back("4x");
	if (director->getContentScaleFactor() >= 1.5) searchOrder.push_back("2x");
	searchOrder.push_back("1x");
	FileUtils::getInstance()->setSearchPaths(searchOrder);

    #ifdef SDKBOX_ENABLED
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        sdkbox::init("a2c6b56453d702eaaf90a7eb6060ff03", "4f6c1a0dd3580a65");
        #elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        sdkbox::init("e4a5357d4990f05c776ac7c6007d59dc", "1dd0f011c6419710", "googleplay");
        #endif
    #endif

    // run
    director->runWithScene(TheMenu::createScene());
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

	CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
	CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();
    // if you use SimpleAudioEngine, it must be pause
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
	CocosDenshion::SimpleAudioEngine::getInstance()->resumeAllEffects();
	CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
