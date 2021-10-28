#include "Savegame.h"

#include <cstring>
#include <filesystem>
#include <fstream>

static Savegame::Data data;

bool Savegame::init() {
    data = {};
    return false;
}

void Savegame::load() {
    if (std::filesystem::exists("save.bin")) {
        std::ifstream stream;
        stream.open("save.bin", std::ios::binary);

        char magic[5];
        stream.read(magic, 4);
        magic[4] = 0;

        // File magic must be CSAV
        if (strcmp(magic, "CSAV") != 0) {
            fprintf(stderr, "Corrupted save file: magic does not match\n");
            return;
        }
        stream.read((char*)&data, sizeof(Data));
    }
}

void Savegame::save() {
    std::ofstream stream;
    stream.open("save.bin", std::ios::binary);
    if (!stream.bad()) {

        stream.write("CSAV", 4);
        stream.write((char*)&data, sizeof(Data));
    }
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
    data.unlockedAbilities = amount;
    save();
}
