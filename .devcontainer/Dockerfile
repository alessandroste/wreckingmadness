FROM mcr.microsoft.com/vscode/devcontainers/base:debian

RUN apt update -y && apt install -y \
    python3 \
    git \ 
    gcc \
    g++ \
    make \
    cmake \
    libfontconfig1-dev \
    libgtk-3-dev \
    libglew-dev \
    libcurl4-gnutls-dev \
    libsqlite3-dev

# Android
ENV ANDROID_HOME      /opt/android-sdk-linux

ENV PATH "${PATH}:${ANDROID_HOME}/cmdline-tools/latest/bin"
ENV PATH "${PATH}:${ANDROID_HOME}/cmdline-tools/tools/bin"
ENV PATH "${PATH}:${ANDROID_HOME}/tools/bin"
ENV PATH "${PATH}:${ANDROID_HOME}/platform-tools"
ENV PATH "${PATH}:${ANDROID_HOME}/emulator"
ENV PATH "${PATH}:${ANDROID_HOME}/bin"

RUN apt install -y \
    wget \
    unzip \
    openjdk-17-jdk-headless \
    cmake

RUN \
    mkdir -p ${ANDROID_HOME}/cmdline-tools/latest && \
    mkdir -p /tmp/android-tools/ && \
    curl -k "https://dl.google.com/android/repository/commandlinetools-linux-7583922_latest.zip" -o /tmp/android-tools/commandlinetools-linux.zip && \
    unzip -q /tmp/android-tools/commandlinetools-linux.zip -d /tmp/android-tools/ && \
    mv /tmp/android-tools/cmdline-tools/* ${ANDROID_HOME}/cmdline-tools/latest/ && \
    rm -rf /tmp/android-tools

RUN \
    yes | sdkmanager --licenses && \
    sdkmanager \
    "platform-tools" \
    "build-tools;34.0.0" \
    "ndk;27.0.12077973" \
    "platforms;android-36"

RUN \
    chown vscode -R ${ANDROID_HOME}