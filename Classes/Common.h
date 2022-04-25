#ifndef _COMMON_H_
#define _COMMON_H_

#include "cocos2d.h"
#include "network\HttpClient.h"

using namespace cocos2d;

class Common {
private:
    Node* ball = nullptr;
    float ball_length;
    float ball_radius;
public:
    // game common resources values
    Color4B background;
    std::string text_font;
    float text_size = 40.0f;
    float credits_size = 25.0f;
    Color3B playernamecolor;
    Color4B playernamecolor_dark;
    Common(unsigned int chain_length = 20);
    ~Common();

    // game sprites
    Sprite* spanCloud();
    Sprite* getSun();
    Node* getBall();
    float getBallLength();
    float getBallRadius();

    // score
    unsigned int getTopLocalScore();
    void setTopLocalScore(unsigned int score);
    Node* getEndGameMenu(int score, int top_score);
    void sendScore(unsigned int score); // returns the percentile
    bool getPlayerID();
    void onHttpRequestCompleted(cocos2d::network::HttpClient* sender, cocos2d::network::HttpResponse* response);

    // utilities
    void makeToast(std::string text, float duration, Layer* scene);
};

#endif // _COMMON_H_