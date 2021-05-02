//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#ifndef _JSOUNDSYSTEM_H_
#define _JSOUNDSYSTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "JTypes.h"

#ifdef ANDROID
    #include <SLES/OpenSLES.h>
    #include "SLES/OpenSLES_Android.h"

#elif defined USE_PHONON
    #include <phonon/AudioOutput>
    #include <phonon/MediaObject>
#else
    #ifdef WIN32
        #include <windows.h>
        #define WITH_FMOD
    #elif defined(PSP)
        #include <pspgu.h>
        #include <pspkernel.h>
        #include <pspdisplay.h>
        #include <pspdebug.h>
        #include <pspctrl.h>
        #include <time.h>
        #include <string.h>
        #include <pspaudiolib.h>
        #include <psprtc.h>

        #include "JAudio.h"
        #include "JMP3.h"
    #endif
    #ifdef WITH_FMOD
        #include "../Dependencies/include/fmod.h"
    #endif
#endif

//------------------------------------------------------------------------------------------------
using namespace std;

#ifdef USE_PHONON
class JMusic : public QObject {
    Q_OBJECT
#else
class JMusic {
#endif
public:
    JMusic();
    ~JMusic();
    void Update();
    int getPlayTime();

#ifdef USE_PHONON
    Phonon::AudioOutput* mOutput;
    Phonon::MediaObject* mMediaObject;
public slots:
    void seekAtTheBegining();
#elif defined(PSP)
    JMP3* mTrack;
#elif defined(IOS)
std::string filename;
std::string key;
std::string ext;
#elif defined WITH_FMOD
FSOUND_SAMPLE* mTrack;  // MP3 needed to be of "sample" type for FMOD, FMUSIC_MODULE is for MODs
#elif defined ANDROID
SLObjectItf playerObject;
SLPlayItf playInterface;
SLSeekItf seekInterface;
SLVolumeItf musicVolumeInterface;
#else
void* mTrack;
#endif  // WITH_FMOD
};

//------------------------------------------------------------------------------------------------
class JSample {
public:
    JSample();
    ~JSample();

    unsigned long fileSize();

#if defined(PSP)
    WAVDATA* mSample;
#elif defined(IOS)
    std::string filename;
    std::string key;
    std::string ext;

    void* mSample;
#elif defined(WITH_FMOD)
    FSOUND_SAMPLE* mSample;
#elif defined(USE_PHONON)
    Phonon::AudioOutput* mOutput;
    Phonon::MediaObject* mMediaObject;
    void* mSample;
#elif defined ANDROID
    SLObjectItf playerObject;
    SLPlayItf playInterface;
    SLVolumeItf sampleVolumeInterface;
    void* mSample;
#else
    void* mSample;
#endif
};

//////////////////////////////////////////////////////////////////////////
/// Sound engine for playing sound effects (WAV) and background
/// music (MP3).
///
//////////////////////////////////////////////////////////////////////////
class JSoundSystem {
public:
    //////////////////////////////////////////////////////////////////////////
    /// Get the singleton instance
    ///
    //////////////////////////////////////////////////////////////////////////
    static JSoundSystem* GetInstance();

    static void Destroy();

    //////////////////////////////////////////////////////////////////////////
    /// Load music.
    ///
    /// @note MP3 is the only supported format for the moment.
    ///
    /// @param filename - Name of the music file.
    ///
    //////////////////////////////////////////////////////////////////////////
    JMusic* LoadMusic(const char* fileName);

    //////////////////////////////////////////////////////////////////////////
    /// Delete music from memory.
    ///
    /// @param music - Music to be deleted.
    ///
    //////////////////////////////////////////////////////////////////////////
    // void FreeMusic(JMusic *music);

    //////////////////////////////////////////////////////////////////////////
    /// Play music.
    ///
    /// @param music - Music to be played.
    /// @param looping - Play the music in a loop.
    ///
    //////////////////////////////////////////////////////////////////////////
    void PlayMusic(JMusic* music, bool looping = false);

    //////////////////////////////////////////////////////////////////////////
    /// Stop playing.
    ///
    /// @param music - Music to be stopped.
    ///
    //////////////////////////////////////////////////////////////////////////
    void StopMusic(JMusic* music);

    //////////////////////////////////////////////////////////////////////////
    /// Resume playing.
    ///
    /// @param music - Music to be resumed.
    ///
    //////////////////////////////////////////////////////////////////////////
    void ResumeMusic(JMusic* music);

    //////////////////////////////////////////////////////////////////////////
    /// Pause playing.
    ///
    /// @param music - Music to be paused.
    ///
    //////////////////////////////////////////////////////////////////////////
    void PauseMusic(JMusic* music);

    //////////////////////////////////////////////////////////////////////////
    /// Load sound effect.
    ///
    /// @note WAV sound effect only.
    ///
    /// @param fileName - Sound effect for loading.
    ///
    //////////////////////////////////////////////////////////////////////////
    JSample* LoadSample(const char* fileName);

    //////////////////////////////////////////////////////////////////////////
    /// Delete sound effect from memory.
    ///
    /// @param sample - Sound to be deleted.
    ///
    //////////////////////////////////////////////////////////////////////////
    // void FreeSample(JSample *sample);

    //////////////////////////////////////////////////////////////////////////
    /// Play sound effect.
    ///
    /// @param sample - Sound for playing.
    ///
    //////////////////////////////////////////////////////////////////////////
    void PlaySample(JSample* sample);

    //////////////////////////////////////////////////////////////////////////
    /// Set volume for audio playback.
    ///
    /// @param volume - New volume.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetVolume(int volume);

    void SetMusicVolume(int volume);

    void SetSfxVolume(int volume);

    int mChannel;

protected:
    JSoundSystem();
    ~JSoundSystem();

    void InitSoundSystem();
    void DestroySoundSystem();

private:
    JMusic* mCurrentMusic;
    JSample* mCurrentSample;

    int mVolume;
    int mMusicVolume;
    int mSampleVolume;

    static JSoundSystem* mInstance;
};

#endif
