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
# SDKBOX
- `$env:PYTHONIOENCODING = "UTF-8"`
- `C:\Tools\Pyportable-2.7.10rc1\python2.exe .\cocos2d\tools\cocos2d-console\plugins\plugin_package\sdkbox.pyc update`
# Firebase
- https://firebase.google.com/download/cpp in firebase_cpp_sdk

# To-do
- https://github.com/firebase/firebase-cpp-sdk

```
$ adb shell cat /data/local/tmp/lldb-server | run-as com.ales.wreckingmadness sh -c 'cat > /data/data/com.ales.wreckingmadness/lldb/bin/lldb-server && chmod 700 /data/data/com.ales.wreckingmadness/lldb/bin/lldb-server'
$ adb shell cat /data/local/tmp/start_lldb_server.sh | run-as com.ales.wreckingmadness sh -c 'cat > /data/data/com.ales.wreckingmadness/lldb/bin/start_lldb_server.sh && chmod 700 /data/data/com.ales.wreckingmadness/lldb/bin/start_lldb_server.sh'
Starting LLDB server: /data/data/com.ales.wreckingmadness/lldb/bin/start_lldb_server.sh /data/data/com.ales.wreckingmadness/lldb unix-abstract /com.ales.wreckingmadness-0 platform-1650823480768.sock "lldb process:gdb-remote packets"
Debugger attached to process 9656
```