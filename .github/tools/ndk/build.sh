#!/bin/bash

set -e

if [ $1 == "debug" ]; then
 ndk-build -C projects/mtg/Android -B -j $(nproc) NDK_DEBUG=1
else
 ndk-build -C projects/mtg/Android -B -j $(nproc) NDK_DEBUG=0
fi

android list targets
android update project -t 1 -p projects/mtg/Android
ant $1 -f projects/mtg/Android/build.xml
