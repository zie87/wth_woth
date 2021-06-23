#!/bin/bash
set -e

#WAGIC_APK_PATH='distribution/android/wagic/outputs/apk'

cd cmake/android
  if [ "$1" == "Release" ]
  then
    ./gradlew assembleRelease
    #mv ${WAGIC_APK_PATH}/release/wagic-release-unsigned.apk ../../wagic_woth_release.apk
  else
    ./gradlew assembleDebug
    #mv ${WAGIC_APK_PATH}/debug/wagic-debug.apk ../../wagic_woth_debug.apk
  fi
cd ../..
