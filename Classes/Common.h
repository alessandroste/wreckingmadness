#ifndef _COMMON_H_
#define _COMMON_H_

// Graphics
#define SPRITE_BALL_NAME "ball.png"
#define SPRITE_CHAIN_NAME "chain.png"
#define SPRITE_NAME_LOGO "logo.png"
#define SPRITE_FLOOR_NORMAL "floor1.png"
#define SPRITE_FLOOR_METAL_LEFT "floor_sx1.png"
#define SPRITE_FLOOR_METAL_RIGHT "floor_dx1.png"
#define SPRITE_FLOOR_ROOF "roof1.png"
#define SPRITE_BUTTON_SHARE_NORMAL "mb_share_n.png"
#define SPRITE_BUTTON_SHARE_PRESSED "mb_share_p.png"
#define SPRITE_BUTTON_EXIT_NORMAL "mb_exit_n.png"
#define SPRITE_BUTTON_EXIT_PRESSED "mb_exit_p.png"
#define SPRITE_BUTTON_RESTART_NORMAL "mb_restart_n.png"
#define SPRITE_BUTTON_RESTART_PRESSED "mb_restart_p.png"
#define TEXT_FONT "fonts/Bungee-Regular.ttf"
#define TEXT_SIZE_DEFAULT 40.0f
#define TEXT_SIZE_START TEXT_SIZE_DEFAULT * 1.5f
#define TEXT_SIZE_CREDITS 25.0f
#define FILL 2.2f

// Audio
#define SOUND_HIT "hit.wav"
#define SOUND_METAL_HIT "metal_hit.wav"
#define MUSIC_BACKGROUND "wreckingsound.wav"

// Storage
#define CONFIG_KEY_PLAYER_ID "playerid"
#define CONFIG_KEY_TOP_SCORE "ts"
#define CONFIG_KEY_MUSIC_ENABLED "musicEnabled"

// Messages

#include "cocos2d.h"
#include "network\HttpClient.h"
#include "Scenes\MainMenuScene.h"

namespace wreckingmadness {
    class Common {
    private:
    public:
        static cocos2d::Color4B BackgroundColor;
        static cocos2d::Color4B TextDarkColor;
        static cocos2d::Color4B MetalColor;
        static cocos2d::Color4B MetalColorLight;
        static cocos2d::Color4B MetalColorDark;
        static cocos2d::Color4B BoltColor;
        static cocos2d::Color4B BoltColorDark;
        static cocos2d::Color4B ToastBackgroundColor;

        Common();
        ~Common();

        // game sprites
        static cocos2d::Sprite* spanCloud();
        static cocos2d::Sprite* getSun();

        // currentScore
        static unsigned int getTopLocalScore();
        static void setTopLocalScore(unsigned int score);
        static void sendScore(unsigned int score);
        static bool getPlayerID();
        void onHttpRequestCompleted(
            cocos2d::network::HttpClient* sender,
            cocos2d::network::HttpResponse* response);

        static void enterSettingsScene();
        static void enterMainMenuScene();
    };
}

#endif // _COMMON_H_