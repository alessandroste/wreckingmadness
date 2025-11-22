FROM mcr.microsoft.com/vscode/devcontainers/base:debian

RUN DEBIAN_FRONTEND=noninteractive apt update -y && apt install -y \
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
    libsqlite3-dev \
    wget \
    unzip \
    openjdk-21-jdk-headless \
    cmake \
    clangd \
    ccache

# Android
ENV ANDROID_HOME      /opt/android-sdk-linux

ENV PATH "${PATH}:${ANDROID_HOME}/cmdline-tools/latest/bin"
ENV PATH "${PATH}:${ANDROID_HOME}/cmdline-tools/tools/bin"
ENV PATH "${PATH}:${ANDROID_HOME}/tools/bin"
ENV PATH "${PATH}:${ANDROID_HOME}/platform-tools"
ENV PATH "${PATH}:${ANDROID_HOME}/emulator"
ENV PATH "${PATH}:${ANDROID_HOME}/bin"

RUN \
    mkdir -p ${ANDROID_HOME}/cmdline-tools/latest && \
    mkdir -p /tmp/android-tools/ && \
    curl -k "https://dl.google.com/android/repository/commandlinetools-linux-7583922_latest.zip" -o /tmp/android-tools/commandlinetools-linux.zip && \
    unzip -q /tmp/android-tools/commandlinetools-linux.zip -d /tmp/android-tools/ && \
    mv /tmp/android-tools/cmdline-tools/* ${ANDROID_HOME}/cmdline-tools/latest/ && \
    rm -rf /tmp/android-tools

ARG ANDROID_BUILD_TOOLS_VERSION="36.1.0"
ARG ANDROID_CMAKE_VERSION="3.31.6"
ARG ANDROID_NDK_VERSION="29.0.14206865"

RUN \
    yes | sdkmanager --licenses && \
    sdkmanager \
    "platform-tools" \
    "build-tools;${ANDROID_BUILD_TOOLS_VERSION}" \
    "ndk;${ANDROID_NDK_VERSION}" \
    "platforms;android-36" \
    "cmake;${ANDROID_CMAKE_VERSION}"

RUN \
    chown vscode -R ${ANDROID_HOME}

USER vscode