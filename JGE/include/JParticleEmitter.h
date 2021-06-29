//-------------------------------------------------------------------------------------
//
// JGE++ is a hardware accelerated 2D game SDK for PSP/Windows.
//
// Licensed under the BSD license, see LICENSE in JGE root for details.
//
// Copyright (c) 2007 James Hui (a.k.a. Dr.Watson) <jhkhui@gmail.com>
//
//-------------------------------------------------------------------------------------

#ifndef __PARTICLE_EMITTER_H__
#define __PARTICLE_EMITTER_H__

#define INIT_PARTICLE_COUNT 32
#define MAX_PARTICLE_COUNT 256

#include <list>
#include <vector>

class JParticleEffect;
class JParticle;

//////////////////////////////////////////////////////////////////////////
/// Particle emitter. This is where the particles actually generated.
///
//////////////////////////////////////////////////////////////////////////
class JParticleEmitter {
public:
    //////////////////////////////////////////////////////////////////////////
    /// Constructor.
    ///
    /// @param parent - Particle effect that contains this emitter.
    ///
    //////////////////////////////////////////////////////////////////////////
    JParticleEmitter(JParticleEffect* parent);

    ~JParticleEmitter();

    //////////////////////////////////////////////////////////////////////////
    /// Set blending mode for rendering.
    ///
    /// @param srcBlend - Blending mode for source.
    /// @param destBlend - Blending mode for destination.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetBlending(int srcBlend, int destBlend);

    //////////////////////////////////////////////////////////////////////////
    /// Set image quad for particles.
    ///
    /// @param quad - Image quad.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetQuad(JQuad* quad);

    //////////////////////////////////////////////////////////////////////////
    /// Start emitting particles.
    ///
    //////////////////////////////////////////////////////////////////////////
    void Start();

    //////////////////////////////////////////////////////////////////////////
    /// Restart the emitter.
    ///
    //////////////////////////////////////////////////////////////////////////
    void ReStart();

    //////////////////////////////////////////////////////////////////////////
    /// Update the emitter.
    ///
    /// @param dt - Time elapsed since last update (in second).
    ///
    //////////////////////////////////////////////////////////////////////////
    void Update(float dt);

    //////////////////////////////////////////////////////////////////////////
    /// Render particles emitted by this emitter.
    ///
    //////////////////////////////////////////////////////////////////////////
    void Render();

    //////////////////////////////////////////////////////////////////////////
    /// Check if the emitter is done.
    ///
    /// @return True if the emitter is done.
    ///
    //////////////////////////////////////////////////////////////////////////
    bool Done() const;

    //////////////////////////////////////////////////////////////////////////
    /// Set active flag.
    ///
    /// @param flag - Active flag.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetActive(bool flag);

    //////////////////////////////////////////////////////////////////////////
    /// Move all particles to a distance.
    ///
    /// @param x - X distance to move.
    /// @param y - Y distance to move
    ///
    //////////////////////////////////////////////////////////////////////////
    void MoveAllParticles(float x, float y);

    //////////////////////////////////////////////////////////////////////////
    /// Emit certain amount of particles.
    ///
    /// @param count - Number of particles to emit.
    ///
    //////////////////////////////////////////////////////////////////////////
    void EmitParticles(int count);

    //////////////////////////////////////////////////////////////////////////
    /// Get idle particle to reuse.
    ///
    /// @return Idel particle to use.
    ///
    //////////////////////////////////////////////////////////////////////////
    JParticle* GetIdleParticle();

    //////////////////////////////////////////////////////////////////////////
    /// Put a particle in action.
    ///
    /// @param par - Particle to start playing.
    ///
    //////////////////////////////////////////////////////////////////////////
    void StartParticle(JParticle* par);

    //////////////////////////////////////////////////////////////////////////
    /// Set the maximum number of particles that this emitter can emit.
    ///
    /// @param count - Maximum number of particles.
    ///
    //////////////////////////////////////////////////////////////////////////
    void SetMaxParticleCount(int count);

    //////////////////////////////////////////////////////////////////////////
    /// \enum JParticleEmitterMode
    ///
    //////////////////////////////////////////////////////////////////////////
    enum JParticleEmitterMode {
        MODE_REPEAT,      ///< Emit particles and repeat when done.
        MODE_ONCE,        ///< Emit once.
        MODE_NTIMES,      ///< Emit N times.
        MODE_CONTINUOUS,  ///< Emit particles continuously.
        MODE_COUNT
    };

    //////////////////////////////////////////////////////////////////////////
    /// \enum JParticleEmitterType
    ///
    //////////////////////////////////////////////////////////////////////////
    enum JParticleEmitterType {
        TYPE_POINT,       ///< Emit from one point.
        TYPE_AREA,        ///< Emit from a rectangle area.
        TYPE_HORIZONTAL,  ///< Emit from a horizontal line.
        TYPE_VERTICAL,    ///< Emit from a vertical line.
        TYPE_CIRCLE,      ///< Emit from a circle.
        TYPE_COUNT
    };

protected:
    JParticleEffect* mParent;
    JQuad* mQuad;

public:
    int mType;
    int mId;

    JParticleData mQuantity;
    JParticleData mData[FIELD_COUNT];

    float mLifeBase;
    float mLifeMax;

    float mAngleBase;
    float mAngleMax;

    float mSpeedBase;
    float mSpeedMax;

    float mSizeBase;
    float mSizeMax;

    int mWidth;
    int mHeight;

    int mQuadIndex;

    int mEmitterMode;
    int mRepeatTimes;

    float mLife;

private:
    bool mActive;
    float mEmitTimer;

    int mRepeatCounter;
    int mActiveParticleCount;

    int mSrcBlending;
    int mDestBlending;
    int mMaxParticleCount;

    std::vector<JParticle*> mParticles;
};

#endif
