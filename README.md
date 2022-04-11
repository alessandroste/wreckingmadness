# Wrecking Maddness
This is a simple game to learn how to use cocos2d-x library to create cross platform mobile games.

# How to compile
## Windows
- Install python2 portable https://github.com/sganis/pyportable
- `git submodule update --init`
- `."C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"  -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -Sc:/Development/wreckingmadness -Bc:/Development/wreckingmadness/build -G "Visual Studio 17 2022" -T host=x64 -A win32 -DPYTHON_COMMAND:FILEPATH=C:\Tools\Pyportable-2.7.10rc1\python.exe`
- `."C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe" --build .\build\`
## Android
- Install Android SDK
- Download ndk-bundle, ndk, build-tools
- run gradle