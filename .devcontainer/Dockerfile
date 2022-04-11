FROM mcr.microsoft.com/vscode/devcontainers/base:alpine

RUN apk -U update && apk -U add \
    python2 \
    git \ 
    gcc \
    g++ \
    make \
    cmake \
    fontconfig-dev \
    gtk+3.0-dev \
    glew-dev \
    curl-dev \
    sqlite-dev

# Android
ENV ANDROID_HOME      /opt/android-sdk-linux
ENV ANDROID_SDK_HOME  ${ANDROID_HOME}
ENV ANDROID_SDK_ROOT  ${ANDROID_HOME}
ENV ANDROID_SDK       ${ANDROID_HOME}

ENV PATH "${PATH}:${ANDROID_HOME}/cmdline-tools/latest/bin"
ENV PATH "${PATH}:${ANDROID_HOME}/cmdline-tools/tools/bin"
ENV PATH "${PATH}:${ANDROID_HOME}/tools/bin"
ENV PATH "${PATH}:${ANDROID_HOME}/build-tools/32.0.0"
ENV PATH "${PATH}:${ANDROID_HOME}/platform-tools"
ENV PATH "${PATH}:${ANDROID_HOME}/emulator"
ENV PATH "${PATH}:${ANDROID_HOME}/bin"

RUN apk -U update && apk -U add \
    wget \
    unzip \
    openjdk11 \
    cmake

RUN \
    mkdir -p ${ANDROID_SDK_ROOT}/cmdline-tools/latest && \
    mkdir -p /tmp/android-tools/ && \
    curl -k "https://dl.google.com/android/repository/commandlinetools-linux-7583922_latest.zip" -o /tmp/android-tools/commandlinetools-linux.zip && \
    unzip -q /tmp/android-tools/commandlinetools-linux.zip -d /tmp/android-tools/ && \
    mv /tmp/android-tools/cmdline-tools/* ${ANDROID_SDK_ROOT}/cmdline-tools/latest/ && \
    rm -rf /tmp/android-tools

RUN \
    yes | sdkmanager --licenses && \
    sdkmanager \
    "platform-tools" \
    "ndk-bundle" \
    "build-tools;31.0.0" \
    "platforms;android-31"