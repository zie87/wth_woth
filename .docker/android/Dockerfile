# based on: https://github.com/RossMeikleham/PlutoBoy/blob/master/build/Android/Dockerfile
FROM --platform=linux/amd64 debian:stretch

USER root

ARG DEBIAN_FRONTEND=noninteractive

ENV LANG C.UTF-8
ENV LC_ALL C.UTF-8

ENV ANDROID_HOME /opt/android-sdk-linux

RUN dpkg --add-architecture i386 && \
    apt-get update && \
    apt-get install -y --no-install-recommends apt-utils && \
    apt-get install -y --no-install-recommends expect build-essential openssh-client curl make && \
    apt-get install -y --no-install-recommends file unzip wget gnupg && \
    apt-get install -y --no-install-recommends libncurses5:i386 libstdc++6:i386 zlib1g:i386 && \
    apt-get install -y --no-install-recommends openjdk-8-jdk ant maven && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Install Android SDK installer
RUN curl -o android-sdk.zip "https://dl.google.com/android/repository/tools_r25.2.5-linux.zip" \
  && unzip -C android-sdk.zip -d "${ANDROID_HOME}" \
  && rm *.zip

COPY .docker/android/scripts/ /opt/sdk-tools

ENV PATH ${PATH}:${ANDROID_HOME}/tools/bin:${ANDROID_HOME}/tools:${ANDROID_HOME}/platform-tools:/opt/sdk-tools

RUN sdkmanager --list \
  && /opt/sdk-tools/android-accept-licenses.sh "sdkmanager platform-tools" \
  && /opt/sdk-tools/android-accept-licenses.sh "sdkmanager build-tools;26.0.3" \
  && /opt/sdk-tools/android-accept-licenses.sh "sdkmanager sources;android-26" \
  && /opt/sdk-tools/android-accept-licenses.sh "sdkmanager platforms;android-26" \
  && sdkmanager --list

RUN curl -o ndk.zip https://dl.google.com/android/repository/android-ndk-r21e-linux-x86_64.zip
RUN unzip -q ndk.zip
ENV PATH /android-ndk-r21e:$PATH
