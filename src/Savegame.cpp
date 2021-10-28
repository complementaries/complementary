#include "Savegame.h"

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
            fprintf(stderr, "Corrupted save file: magic does not match\n");
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
        stream.write((char*)&data, sizeof(Data));
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
    save();
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
    save();
}
