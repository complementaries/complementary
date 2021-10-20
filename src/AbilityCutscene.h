#ifndef ABILITY_OVERLAY_H
#define ABILITY_OVERLAY_H

namespace AbilityCutscene {
    bool init();
    void show();
    void tick();
    void render(float lag);
    bool isActive();
}

#endif
