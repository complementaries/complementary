#include "SoundManager.h"

#include "Arguments.h"
#include "Utils.h"

#include <iostream>

const static int maxMusicVolume = MIX_MAX_VOLUME / 4;
const static int soundEffectsGroup = 1;
const static int maxChannels = 16;
static int curMusicChannel = SoundManager::lightSoundID;
static bool muted = false;
static int musicVolume = maxMusicVolume;

SoundManager::SoundObject soundArray[Sound::MAX];

static int getIdFromChannel(int channel) {
    for (int i = 0; i < Sound::MAX; i++) {
        if (soundArray[i].channel == channel) {
            return i;
        }
    }
    // should not happen
    return -1;
}

int SoundManager::getMusicChannel() {
    return curMusicChannel;
}

static void channelDone(int channel) {
    // remove all effects from channel
    if (!Mix_UnregisterAllEffects(channel)) {
        Utils::print("Mix_UnregisterAllEffects: %s\n", Mix_GetError());
    }
    int ID = getIdFromChannel(channel);
    soundArray[ID].playing = false;
    soundArray[ID].channel = -1;
}

bool SoundManager::init() {
    if (Arguments::muted) {
        return false;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        Utils::printError("SDL Mixer failed to initialise: %s\n", SDL_GetError());
        Arguments::muted = true;
        return true;
    }

    Mix_AllocateChannels(maxChannels);
    Mix_GroupChannels(darkSoundID + 1, maxChannels - 1, soundEffectsGroup);
    Mix_ChannelFinished(channelDone);
    return false;
}

static bool play(int soundId, int channel, int volume, int loops) {
    if (soundArray[soundId].sound == nullptr) {
        return true;
    }
    soundArray[soundId].channel = Mix_PlayChannel(channel, soundArray[soundId].sound, loops);
    soundArray[soundId].playing = true;
    if (soundArray[soundId].channel == -1) return true;
    int newVolume = volume > -1 ? volume : soundArray[soundId].defaultVolume;
    Mix_Volume(soundArray[soundId].channel, muted ? 0 : newVolume);
    return false;
}

static bool play(int soundId, int channel, int volume) {
    return play(soundId, channel, volume, 0);
}

static bool play(int soundId, int channel) {
    return play(soundId, channel, -1);
}

bool SoundManager::playMusic() {
    if (Arguments::muted) {
        return false;
    }
    if (play(Sound::LIGHT_BG, curMusicChannel, musicVolume, -1)) return true;
    if (play(Sound::DARK_BG, 1 - curMusicChannel, 0, -1)) return true;
    return false;
}

static int findFreeChannel() {
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
    if (Arguments::muted) {
        return false;
    }
    soundArray[soundId].channel = findFreeChannel();
    return play(soundId, soundArray[soundId].channel);
}

bool SoundManager::playContinuousSound(int soundId) {
    if (Arguments::muted) {
        return false;
    }
    soundArray[soundId].channel = findFreeChannel();
    return play(soundId, soundArray[soundId].channel, -1, -1);
}

void SoundManager::switchMusic() {
    if (Arguments::muted) {
        return;
    }
    setVolume(curMusicChannel, 0);
    curMusicChannel = 1 - curMusicChannel;
    if (!muted) {
        setVolume(curMusicChannel, musicVolume);
    }
}

static void loadSound(Sound::Sound s, const char* path, int volume) {
    soundArray[s].sound = Mix_LoadWAV(path);
    soundArray[s].defaultVolume = volume;
    if (soundArray[s].sound == nullptr) {
        Utils::printError("could not load sound '%s'\n", path);
    }
}

bool SoundManager::loadSounds() {
    if (Arguments::muted) {
        return false;
    }
    loadSound(Sound::LIGHT_BG, "assets/sounds/light.ogg", musicVolume);
    loadSound(Sound::DARK_BG, "assets/sounds/dark.ogg", musicVolume);
    loadSound(Sound::WORLD_SWITCH, "assets/sounds/switch.ogg", MIX_MAX_VOLUME / 4);
    loadSound(Sound::JUMP, "assets/sounds/jump.ogg", MIX_MAX_VOLUME / 2);
    loadSound(Sound::DASH, "assets/sounds/dash.ogg", MIX_MAX_VOLUME);
    loadSound(Sound::WIND, "assets/sounds/wind.ogg", MIX_MAX_VOLUME);
    loadSound(Sound::COLLECT, "assets/sounds/collect.ogg", MIX_MAX_VOLUME / 2);
    loadSound(Sound::DEATH, "assets/sounds/death.ogg", MIX_MAX_VOLUME / 2);
    loadSound(Sound::TITLE, "assets/sounds/title.ogg", MIX_MAX_VOLUME / 2);
    loadSound(Sound::DOOR, "assets/sounds/door.ogg", MIX_MAX_VOLUME / 4);
    loadSound(Sound::TELEPORT, "assets/sounds/teleport.ogg", MIX_MAX_VOLUME / 4);
    loadSound(Sound::EXPLODE, "assets/sounds/explode.ogg", MIX_MAX_VOLUME / 3);
    loadSound(Sound::NEW_ABILITY, "assets/sounds/ability.ogg", MIX_MAX_VOLUME / 2);
    return false;
}

void SoundManager::setVolume(int soundId, int volume) {
    if (Arguments::muted) {
        return;
    }
    Mix_Volume(soundArray[soundId].channel, volume);
}

void SoundManager::setMusicVolume(int volumePercent) {
    musicVolume = maxMusicVolume * (float)volumePercent / 100.0f;
    if (!muted) {
        setVolume(curMusicChannel, musicVolume);
    }
}

void SoundManager::stopSound(int soundId) {
    if (Arguments::muted) {
        return;
    }
    Mix_HaltChannel(soundArray[soundId].channel);
}

void SoundManager::setDistanceToPlayer(int soundId, float distance, float xDistance,
                                       int threshold) {
    if (Arguments::muted) {
        return;
    }
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
    // -255 should be 0 and 0 should be 127
    xDist = (xDist + 255) / 2;

    if (!Mix_SetDistance(soundArray[soundId].channel, abs(dist))) {
        Utils::print("Mix_SetDistance: %s\n", Mix_GetError());
    }

    if (!Mix_SetPanning(soundArray[soundId].channel, xDist, 255 - xDist)) {
        Utils::print("Mix_SetPanning: %s\n", Mix_GetError());
    }
}

bool SoundManager::soundPlaying(int soundId) {
    if (Arguments::muted) {
        return false;
    }
    return soundArray[soundId].playing;
}

void SoundManager::quit() {
    // clean up our resources
    for (SoundObject object : soundArray) {
        if (object.sound != nullptr) {
            Mix_FreeChunk(object.sound);
        }
    }

    // quit SDL_mixer
    Mix_CloseAudio();
}

void SoundManager::mute() {
    if (Arguments::muted) {
        return;
    }
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
