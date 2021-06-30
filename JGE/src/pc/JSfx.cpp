//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#ifdef WITH_FMOD
#include "fmod.h"
#else
#define FSOUND_FREE 0
#endif

#include "JSoundSystem.h"
#include "JFileSystem.h"

//////////////////////////////////////////////////////////////////////////
JMusic::JMusic() {}

void JMusic::Update() {}

int JMusic::getPlayTime() {
#ifdef WITH_FMOD
    return static_cast<int>(FSOUND_GetCurrentPosition(JSoundSystem::GetInstance()->mChannel) /
                            44.1);  // todo more generic, here it's only 44kHz
#else
    return 0;
#endif
}

JMusic::~JMusic() {
#if defined WITH_FMOD
    JSoundSystem::GetInstance()->StopMusic(this);
    if (mTrack) FSOUND_Sample_Free(mTrack);
#endif
}

//////////////////////////////////////////////////////////////////////////
JSample::JSample() {}

JSample::~JSample() {
#if (defined WITH_FMOD)
    if (mSample) FSOUND_Sample_Free(mSample);
#endif
}

unsigned long JSample::fileSize() {
#ifdef WITH_FMOD
    return FSOUND_Sample_GetLength(mSample);
#else
    return 0;
#endif
}

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
#ifdef WITH_FMOD
    mChannel = FSOUND_FREE;
#endif
}

JSoundSystem::~JSoundSystem() {}

void JSoundSystem::InitSoundSystem() {
#ifdef WITH_FMOD
    FSOUND_Init(44100, 32, 0);
#endif
}

void JSoundSystem::DestroySoundSystem() {
#ifdef WITH_FMOD
    FSOUND_Close();
#endif
}

JMusic* JSoundSystem::LoadMusic(const char* fileName) {
#if (defined WITH_FMOD)
    JMusic* music = new JMusic();
    if (music) {
        JFileSystem* fileSystem = JFileSystem::GetInstance();
        if (fileSystem->OpenFile(fileName)) {
            int size     = fileSystem->GetFileSize();
            char* buffer = new char[size];
            fileSystem->ReadFile(buffer, size);
            music->mTrack = FSOUND_Sample_Load(FSOUND_UNMANAGED, buffer, FSOUND_LOADMEMORY, 0, size);

            delete[] buffer;
            fileSystem->CloseFile();
        }
    }
    return music;
#else
    return nullptr;
#endif
}

void JSoundSystem::PlayMusic(JMusic* music, bool looping) {
#if (defined WITH_FMOD)
    if (music && music->mTrack) {
        mChannel = FSOUND_PlaySound(mChannel, music->mTrack);
        SetMusicVolume(mVolume);

        if (looping)
            FSOUND_SetLoopMode(mChannel, FSOUND_LOOP_NORMAL);
        else
            FSOUND_SetLoopMode(mChannel, FSOUND_LOOP_OFF);
    }
#endif
}

void JSoundSystem::StopMusic(JMusic* music) {
#if (defined WITH_FMOD)
    FSOUND_StopSound(mChannel);
#endif
}

void JSoundSystem::PauseMusic(JMusic* music) { StopMusic(music); }

void JSoundSystem::ResumeMusic(JMusic* music) { PlayMusic(music); }

void JSoundSystem::SetVolume(int volume) {
    SetMusicVolume(volume);
    SetSfxVolume(volume);
}

void JSoundSystem::SetMusicVolume(int volume) {
#ifdef WITH_FMOD
    if (mChannel != FSOUND_FREE) FSOUND_SetVolumeAbsolute(mChannel, static_cast<int>(volume * 2.55));
#endif
    mVolume = volume;
}

void JSoundSystem::SetSfxVolume(int volume) {
    // this sets the volume to all channels then reverts back the volume for music..
    // that's a bit dirty but it works
#ifdef WITH_FMOD
    FSOUND_SetVolumeAbsolute(FSOUND_ALL, static_cast<int>(volume * 2.55));
#endif
    mSampleVolume = volume;
    SetMusicVolume(mVolume);
}

JSample* JSoundSystem::LoadSample(const char* fileName) {
#if (defined WITH_FMOD)
    JSample* sample = new JSample();
    if (sample) {
        JFileSystem* fileSystem = JFileSystem::GetInstance();
        if (fileSystem->OpenFile(fileName)) {
            int size     = fileSystem->GetFileSize();
            char* buffer = new char[size];
            fileSystem->ReadFile(buffer, size);
            sample->mSample = FSOUND_Sample_Load(FSOUND_UNMANAGED, buffer, FSOUND_LOADMEMORY, 0, size);

            delete[] buffer;
            fileSystem->CloseFile();
        } else
            sample->mSample = NULL;
    }
    return sample;
#else
    return nullptr;
#endif
}

void JSoundSystem::PlaySample(JSample* sample) {
#if (defined WITH_FMOD)
    if (sample && sample->mSample) {
        int channel = FSOUND_PlaySound(FSOUND_FREE, sample->mSample);
        FSOUND_SetVolumeAbsolute(channel, static_cast<int>(mSampleVolume * 2.55));
    }
#endif
}
