#include "androidPlatform.h"

using namespace cocos2d;
using namespace wreckingmadness;

void AndroidPlatform::shareImageFromFile(const std::string &fileName) {
    CCLOG("[AndroidPlatform] Sharing image from file %s", fileName.c_str());
    auto jniEnv = JniHelper::getEnv();
    auto jniActivity = JniHelper::getActivity();
    auto activityClass = jniEnv->GetObjectClass(jniActivity);
    CCLOG("[AndroidPlatform] Resolved Activity class");
    auto shareMethod = jniEnv->GetMethodID(activityClass, "shareIntent", "(Ljava/lang/String;)V");
    CCLOG("[AndroidPlatform] Resolved method id");
    auto parameter = jniEnv->NewStringUTF(fileName.c_str());
    jniEnv->CallVoidMethod(jniActivity, shareMethod, parameter);
    CCLOG("[AndroidPlatform] Shared image from file %s", fileName.c_str());
}