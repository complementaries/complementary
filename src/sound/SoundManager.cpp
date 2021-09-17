#include "SoundManager.h"

#include <iostream>

const static int musicVolume = MIX_MAX_VOLUME / 2;
const static int lightSoundID = 0;
const static int darkSoundID = 1;
const static int soundEffectsGroup = 1;
const static int maxChannels = 16;
static int curMusicChannel = lightSoundID;

SoundManager::SoundObject SoundArray[Sound::MAXIMUM];

bool SoundManager::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return true;
    }

    Mix_AllocateChannels(maxChannels);
    Mix_GroupChannels(darkSoundID + 1, maxChannels - 1, soundEffectsGroup);

    return false;
}

bool SoundManager::playMusic() {
    if (play(LIGHT_BG, curMusicChannel, musicVolume, -1)) return true;
    if (play(DARK_BG, 1 - curMusicChannel, 0, -1)) return true;
    return false;
}

int SoundManager::findFreeChannel() {
    // find the first available channel in group 1
    int channel;
    channel = Mix_GroupAvailable(soundEffectsGroup);
    if (channel == -1) {
        // no channel available
        // search for oldest channel in use
        channel = Mix_GroupOldest(soundEffectsGroup);
    }
    return channel;
}

bool SoundManager::playSoundEffect(int soundId) {
    SoundArray[soundId].channel = findFreeChannel();
    return play(soundId, SoundArray[soundId].channel);
}

bool SoundManager::playContinuousSound(int soundId) {
    SoundArray[soundId].channel = findFreeChannel();
    return play(soundId, SoundArray[soundId].channel, -1, -1);
}

void SoundManager::switchMusic() {
    setVolume(curMusicChannel, 0);
    curMusicChannel = 1 - curMusicChannel;
    setVolume(curMusicChannel, musicVolume);
}

bool SoundManager::loadSounds() {
    SoundArray[Sound::LIGHT_BG].sound = Mix_LoadWAV("assets/sounds/light.ogg");
    SoundArray[Sound::LIGHT_BG].defaultVolume = musicVolume;

    SoundArray[Sound::DARK_BG].sound = Mix_LoadWAV("assets/sounds/dark.ogg");
    SoundArray[Sound::DARK_BG].defaultVolume = musicVolume;

    SoundArray[Sound::WORLD_SWITCH].sound = Mix_LoadWAV("assets/sounds/switch.ogg");
    SoundArray[Sound::WORLD_SWITCH].defaultVolume = MIX_MAX_VOLUME;

    SoundArray[Sound::JUMP].sound = Mix_LoadWAV("assets/sounds/jump2.ogg");
    SoundArray[Sound::JUMP].defaultVolume = MIX_MAX_VOLUME;

    SoundArray[Sound::TEST].sound = Mix_LoadWAV("assets/sounds/air.ogg");
    SoundArray[Sound::TEST].defaultVolume = MIX_MAX_VOLUME / 2;

    for (SoundObject object : SoundArray) {
        if (object.sound == NULL) return true;
    }
    return false;
}

// do not directly use this to play a sound, rather use playSoundEffect
bool SoundManager::play(int soundId, int channel, int volume, int loops) {
    SoundArray[soundId].channel = Mix_PlayChannel(channel, SoundArray[soundId].sound, loops);
    if (SoundArray[soundId].channel == -1) return true;
    Mix_Volume(SoundArray[soundId].channel,
               volume > -1 ? volume : SoundArray[soundId].defaultVolume);
    return false;
}

void SoundManager::setVolume(int soundId, int volume) {
    Mix_Volume(SoundArray[soundId].channel, volume);
}

void SoundManager::setDistanceToPlayer(int soundId, float distance, float xDistance,
                                       int threshold) {
    // distance from 0 to 255
    int dist, xDist;
    if (distance > threshold) {
        dist = 255;
    } else {
        dist = distance * 255 / threshold;
    }

    // x distance from -255 to 255
    if (xDistance > threshold) {
        xDist = 255;
    } else if (xDistance < threshold * (-1)) {
        xDist = -255;
    } else {
        xDist = xDistance * 255 / threshold;
    }
    // -255 shouls be 0 and 0 should be 127
    xDist = (xDist + 255) / 2;

    if (!Mix_SetDistance(SoundArray[soundId].channel, abs(dist))) {
        printf("Mix_SetDistance: %s\n", Mix_GetError());
    }

    if (!Mix_SetPanning(SoundArray[soundId].channel, xDist, 255 - xDist)) {
        printf("Mix_SetPanning: %s\n", Mix_GetError());
    }
}

void SoundManager::quit() {
    // clean up our resources
    for (SoundObject object : SoundArray) {
        Mix_FreeChunk(object.sound);
    }

    // quit SDL_mixer
    Mix_CloseAudio();
}