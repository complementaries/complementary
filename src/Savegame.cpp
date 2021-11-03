#include "Savegame.h"

#include "Utils.h"
#include <cassert>
#include <cstring>
#include <filesystem>
#include <fstream>

static Savegame::Data data;
const char* SAVE_FILE_NAME = "save.bin";
const char* SAVE_FILE_TEMP_NAME = "save.bin.tmp";

bool Savegame::init() {
    reset();
    return false;
}

void Savegame::load() {
    if (std::filesystem::exists(SAVE_FILE_NAME)) {
        std::ifstream stream;
        stream.open(SAVE_FILE_NAME, std::ios::binary);

        char magic[5];
        stream.read(magic, 4);
        magic[4] = 0;

        // File magic must be CSAV
        if (strcmp(magic, "CSAV") != 0) {
            Utils::printError("Corrupted save file: magic does not match\n");
            return;
        }
        stream.read((char*)&data, sizeof(Data));
        stream.close();
    }
}

void Savegame::save() {
    std::ofstream stream;
    stream.open(SAVE_FILE_TEMP_NAME, std::ios::binary);
    if (!stream.bad()) {
        stream.write("CSAV", 4);
        stream.write(reinterpret_cast<char*>(&data), sizeof(Data));
        stream.close();
    }

    std::filesystem::rename(SAVE_FILE_TEMP_NAME, SAVE_FILE_NAME);
}

void Savegame::reset() {
    data = {};
}

void Savegame::unlockAbilities(Ability primary, Ability secondary) {
    data.unlockedAbilities |= 1 << static_cast<int>(primary);
    data.unlockedAbilities |= 1 << static_cast<int>(secondary);
}

bool Savegame::abilitiesUnlocked(Ability primary, Ability secondary) {
    return ((data.unlockedAbilities & (1 << static_cast<int>(primary))) != 0) &&
           ((data.unlockedAbilities & (1 << static_cast<int>(secondary))) != 0);
}

int Savegame::getCompletedLevels() {
    return data.completedLevels;
}

void Savegame::setCompletedLevels(int amount) {
    data.completedLevels = amount;
}

uint32_t Savegame::getCompletionTime(size_t levelIndex) {
    return data.completionTime[levelIndex];
}

void Savegame::setCompletionTime(size_t levelIndex, uint32_t ticks) {
    assert(levelIndex < MAX_LEVEL_COUNT);
    data.completionTime[levelIndex] = ticks;
}
