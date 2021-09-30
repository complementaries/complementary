#pragma once

#include <SDL.h>
#include <SDL_mixer.h>

namespace Sound {
    enum Sound {
        LIGHT_BG,
        DARK_BG,
        WORLD_SWITCH,
        JUMP,
        WIND,
        COLLECT,
        DEATH,
        /* GLIDE,*/ DASH,
        MAX
    };
}

namespace SoundManager {

    struct SoundObject {
        Mix_Chunk* sound;
        int channel = -1;
        int defaultVolume = MIX_MAX_VOLUME;
        bool playing = false;
    };

    bool init();

    bool playMusic();

    bool playSoundEffect(int soundId);

    bool playContinuousSound(int soundId);

    void switchMusic();

    bool play(int soundId, int channel, int volume = -1, int loops = 0);

    int findFreeChannel();

    int getIdFromChannel(int channel);

    void setVolume(int soundId, int volume);

    void setDistanceToPlayer(int soundId, float distance, float xDistance, int threshold);

    bool loadSounds();

    void stopSound(int soundId);

    void channelDone(int channel);

    bool soundPlaying(int soundId);

    void quit();

    void mute();
}