#include "SoundManager.h"

#include <iostream>

const static int musicVolume = MIX_MAX_VOLUME / 2;
const static int lightSoundID = 0;
const static int darkSoundID = 1;
const static int soundEffectsGroup = 1;
const static int maxChannels = 16;
static int curMusicChannel = lightSoundID;
static bool muted = false;

SoundManager::SoundObject soundArray[Sound::MAX];

bool SoundManager::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        fprintf(stderr, "SDL failed to initialise: %s\n", SDL_GetError());
        return true;
    }

    Mix_AllocateChannels(maxChannels);
    Mix_GroupChannels(darkSoundID + 1, maxChannels - 1, soundEffectsGroup);
    Mix_ChannelFinished(channelDone);
    return false;
}

bool SoundManager::playMusic() {
    if (play(Sound::LIGHT_BG, curMusicChannel, musicVolume, -1)) return true;
    if (play(Sound::DARK_BG, 1 - curMusicChannel, 0, -1)) return true;
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
    soundArray[soundId].channel = findFreeChannel();
    return play(soundId, soundArray[soundId].channel);
}

bool SoundManager::playContinuousSound(int soundId) {
    soundArray[soundId].channel = findFreeChannel();
    return play(soundId, soundArray[soundId].channel, -1, -1);
}

void SoundManager::switchMusic() {
    setVolume(curMusicChannel, 0);
    curMusicChannel = 1 - curMusicChannel;
    if (!muted) {
        setVolume(curMusicChannel, musicVolume);
    }
}

bool SoundManager::loadSounds() {
    soundArray[Sound::LIGHT_BG].sound = Mix_LoadWAV("assets/sounds/light.ogg");
    soundArray[Sound::LIGHT_BG].defaultVolume = musicVolume;

    soundArray[Sound::DARK_BG].sound = Mix_LoadWAV("assets/sounds/dark.ogg");
    soundArray[Sound::DARK_BG].defaultVolume = musicVolume;

    soundArray[Sound::WORLD_SWITCH].sound = Mix_LoadWAV("assets/sounds/switch.ogg");
    soundArray[Sound::WORLD_SWITCH].defaultVolume = MIX_MAX_VOLUME;

    soundArray[Sound::JUMP].sound = Mix_LoadWAV("assets/sounds/jump2.ogg");
    soundArray[Sound::JUMP].defaultVolume = MIX_MAX_VOLUME / 2;

    soundArray[Sound::DASH].sound = Mix_LoadWAV("assets/sounds/dash.ogg");
    soundArray[Sound::DASH].defaultVolume = MIX_MAX_VOLUME / 2;

    soundArray[Sound::WIND].sound = Mix_LoadWAV("assets/sounds/wind.ogg");
    soundArray[Sound::WIND].defaultVolume = MIX_MAX_VOLUME / 2;

    for (SoundObject object : soundArray) {
        if (object.sound == NULL) return true;
    }
    return false;
}

// do not directly use this to play a sound, rather use playSoundEffect
bool SoundManager::play(int soundId, int channel, int volume, int loops) {
    soundArray[soundId].channel = Mix_PlayChannel(channel, soundArray[soundId].sound, loops);
    soundArray[soundId].playing = true;
    if (soundArray[soundId].channel == -1) return true;
    int newVolume = volume > -1 ? volume : soundArray[soundId].defaultVolume;
    Mix_Volume(soundArray[soundId].channel, muted ? 0 : newVolume);
    return false;
}

void SoundManager::setVolume(int soundId, int volume) {
    Mix_Volume(soundArray[soundId].channel, volume);
}

void SoundManager::stopSound(int soundId) {
    Mix_HaltChannel(soundArray[soundId].channel);
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

    if (!Mix_SetDistance(soundArray[soundId].channel, abs(dist))) {
        printf("Mix_SetDistance: %s\n", Mix_GetError());
    }

    if (!Mix_SetPanning(soundArray[soundId].channel, xDist, 255 - xDist)) {
        printf("Mix_SetPanning: %s\n", Mix_GetError());
    }
}

bool SoundManager::soundPlaying(int soundId) {
    return soundArray[soundId].playing;
}

void SoundManager::channelDone(int channel) {
    // remove all effects from channel
    if (!Mix_UnregisterAllEffects(channel)) {
        printf("Mix_UnregisterAllEffects: %s\n", Mix_GetError());
    }
    int ID = getIdFromChannel(channel);
    soundArray[ID].playing = false;
    soundArray[ID].channel = -1;
}

int SoundManager::getIdFromChannel(int channel) {
    for (int i = 0; i < Sound::MAX; i++) {
        if (soundArray[i].channel == channel) {
            return i;
        }
    }
    // should not happen
    return -1;
}

void SoundManager::quit() {
    // clean up our resources
    for (SoundObject object : soundArray) {
        Mix_FreeChunk(object.sound);
    }

    // quit SDL_mixer
    Mix_CloseAudio();
}

void SoundManager::mute() {
    if (muted) {
        setVolume(curMusicChannel, musicVolume);
        setVolume(1 - curMusicChannel, 0);

        for (int i = darkSoundID + 1; i < Sound::MAX; i++) {
            if (soundArray[i].playing) {
                setVolume(i, soundArray[i].defaultVolume);
            }
        }
    } else {
        for (int i = 0; i < 16; i++) {
            Mix_Volume(i, 0);
        }
    }
    muted = !muted;
}