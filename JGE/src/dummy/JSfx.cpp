//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#include "JSoundSystem.h"

//////////////////////////////////////////////////////////////////////////
JMusic::JMusic() {}

void JMusic::Update() {}

int JMusic::getPlayTime() { return 0; }

JMusic::~JMusic() {}

//////////////////////////////////////////////////////////////////////////
JSample::JSample() {}

JSample::~JSample() {}

unsigned long JSample::fileSize() { return 0; }

//////////////////////////////////////////////////////////////////////////
JSoundSystem* JSoundSystem::mInstance = nullptr;

JSoundSystem* JSoundSystem::GetInstance() {
    if (mInstance == nullptr) {
        mInstance = new JSoundSystem();
        mInstance->InitSoundSystem();
    }
    return mInstance;
}

void JSoundSystem::Destroy() {
    if (mInstance) {
        mInstance->DestroySoundSystem();
        delete mInstance;
        mInstance = nullptr;
    }
}

JSoundSystem::JSoundSystem() {
    mVolume       = 0;
    mSampleVolume = 0;
}

JSoundSystem::~JSoundSystem() {}

void JSoundSystem::InitSoundSystem() {}

void JSoundSystem::DestroySoundSystem() {}

JMusic* JSoundSystem::LoadMusic(const char* fileName) { return nullptr; }

void JSoundSystem::PlayMusic(JMusic* music, bool looping) {}

void JSoundSystem::StopMusic(JMusic* music) {}

void JSoundSystem::PauseMusic(JMusic* music) { StopMusic(music); }

void JSoundSystem::ResumeMusic(JMusic* music) { PlayMusic(music); }

void JSoundSystem::SetVolume(int volume) {
    SetMusicVolume(volume);
    SetSfxVolume(volume);
}

void JSoundSystem::SetMusicVolume(int volume) { mVolume = volume; }

void JSoundSystem::SetSfxVolume(int volume) {
    mSampleVolume = volume;
    SetMusicVolume(mVolume);
}

JSample* JSoundSystem::LoadSample(const char* fileName) { return nullptr; }

void JSoundSystem::PlaySample(JSample* sample) {}
