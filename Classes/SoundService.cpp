#include "SoundService.h"

#include "cocos2d.h"
#ifdef USE_AUDIOENGINE
#include "AudioEngine.h"
#else
#include "editor-support/cocostudio/SimpleAudioEngine.h"
#endif

using namespace cocos2d;
using namespace wreckingmadness;

void SoundService::preloadEffects() {
    CCLOG("[SoundService] Preloading effects");
#ifdef USE_AUDIOENGINE
    AudioEngine::preload(SOUND_HIT);
    AudioEngine::preload(SOUND_METAL_HIT);
#else
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(SOUND_HIT);
    CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(SOUND_METAL_HIT);
#endif
}

void SoundService::setBackgroundMusic(bool on) {
    CCLOG("[SoundService] Storing background music preference %d", on);
    UserDefault::getInstance()->setBoolForKey(CONFIG_KEY_MUSIC_ENABLED, on);
    SoundService::playBackgroundMusic();
}

bool SoundService::isBackgroundMusicEnabled() {
    return UserDefault::getInstance()->getBoolForKey(CONFIG_KEY_MUSIC_ENABLED, true);
}

void SoundService::playEffect(Effect effect) {
    switch (effect) {
    case Effect::HIT:
#ifdef USE_AUDIOENGINE
        AudioEngine::play2d(SOUND_HIT);
#else
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_HIT);
#endif
        break;
    case Effect::METAL_HIT:
#ifdef USE_AUDIOENGINE
        AudioEngine::play2d(SOUND_METAL_HIT);
#else
        CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(SOUND_METAL_HIT);
#endif
    default:
        break;
    }
}

void SoundService::playBackgroundMusic() {
    auto isMusicEnabled = UserDefault::getInstance()->getBoolForKey(CONFIG_KEY_MUSIC_ENABLED, true);
#ifdef USE_AUDIOENGINE

#else
    auto isMusicPlaying = CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
#endif
    if (isMusicEnabled && !isMusicPlaying)
        CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(MUSIC_BACKGROUND, true);
    else if (!isMusicEnabled && isMusicPlaying)
        CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic(MUSIC_BACKGROUND);
}

void SoundService::pauseAll() {
    CCLOG("[SoundService] Pausing all audio");
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

void SoundService::resumeAll() {
    CCLOG("[SoundService] Resuming all audio");
    CocosDenshion::SimpleAudioEngine::getInstance()->resumeAllEffects();
    if (SoundService::isBackgroundMusicEnabled())
        CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}