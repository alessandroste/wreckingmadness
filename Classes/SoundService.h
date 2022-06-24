#ifndef __SOUNDSERVICE_H__
#define __SOUNDSERVICE_H__

#include "AudioEngine.h"

#define SOUND_HIT "hit.wav"
#define SOUND_METAL_HIT "metal_hit.wav"
#define MUSIC_BACKGROUND "wreckingsound.wav"

// Storage
#define CONFIG_KEY_MUSIC_ENABLED "musicEnabled"

namespace wreckingmadness {
    enum Effect {
        HIT,
        METAL_HIT
    };

    class SoundService {
    private:
        static SoundService* instance;
        static SoundService* getInstance();
        int backgroundAudioId;
    public:
        static void preloadEffects();
        static void setBackgroundMusic(bool on);
        static bool isBackgroundMusicEnabled();
        static void playEffect(Effect effect);
        static void playBackgroundMusic();
        static void pauseAll();
        static void resumeAll();
    };
}

#endif