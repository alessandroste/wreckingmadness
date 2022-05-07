#include "cocos2d.h"
#include "FirebaseHelper.h"
#ifdef FIREBASE
#include "firebase/admob.h"
#endif
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

#ifdef FIREBASE
firebase::admob::AdParent wreckingmadness::FirebaseHelper::getAdParent() {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    // Returns the iOS RootViewController's main view (i.e. the EAGLView).
    return (id)Director::getInstance()->getOpenGLView()->getEAGLView();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Returns the Android Activity.
    return cocos2d::JniHelper::getActivity();
#else
    // A void* for any other environments.
    return 0;
#endif
}
#endif

wreckingmadness::FirebaseHelper::FirebaseHelper() {
#ifdef FIREBASE
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    _pFirebaseApp = std::unique_ptr<firebase::App>(firebase::App::Create(firebase::AppOptions(), cocos2d::JniHelper::getEnv(), cocos2d::JniHelper::getActivity()));
#else
    _pFirebaseApp = std::unique_ptr<firebase::App>(firebase::App::Create(firebase::AppOptions()));
#endif
    firebase::admob::Initialize(*_pFirebaseApp, "ca-app-pub-5982841410553202~6695136411");
    CCLOG("Initialized AdMob via Firebase");
#endif
}

wreckingmadness::FirebaseHelper::~FirebaseHelper() {
}