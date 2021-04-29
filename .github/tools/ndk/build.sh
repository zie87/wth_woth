#!/bin/bash


ndk-build -C projects/mtg/Android -B -j $(nproc) NDK_TOOLCHAIN_VERSION=4.9
android list targets
android update project -t 1 -p projects/mtg/Android
ant debug -f projects/mtg/Android/build.xml
