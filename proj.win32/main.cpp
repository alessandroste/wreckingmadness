#include "main.h"
#include "AppDelegate.h"
#include "windowsPlatform.h"
#include "cocos2d.h"

using namespace cocos2d;
using namespace wreckingmadness;

int WINAPI _tWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPTSTR    lpCmdLine,
    int       nCmdShow) {

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // create the application instance
    AppDelegate app;
    WindowsPlatform platform;
    return Application::getInstance()->run();
}