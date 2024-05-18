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

RUN apt install -y \
    wget \
    unzip \
    openjdk-17-jdk-headless \
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
    "build-tools;34.0.0" \
    "build-tools;32.0.0" \
    "ndk;24.0.8215888" \
    "platforms;android-34"

RUN \
    chown vscode -R ${ANDROID_HOME}