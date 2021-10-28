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
        // GLIDE,
        DASH,
        TITLE,
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
    void setVolume(int soundId, int volume);
    void setDistanceToPlayer(int soundId, float distance, float xDistance, int threshold);
    bool loadSounds();
    void setMusicVolume(int volumePercent);
    void stopSound(int soundId);
    bool soundPlaying(int soundId);
    void quit();
    void mute();
}