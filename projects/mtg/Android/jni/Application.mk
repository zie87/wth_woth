APP_PROJECT_PATH := $(call my-dir)/..
APP_CPPFLAGS += -frtti -fexceptions
APP_ABI := armeabi-v7a
APP_STL := c++_shared
APP_MODULES := main SDL

#APP_OPTIM is 'release' by default
APP_OPTIM := release
