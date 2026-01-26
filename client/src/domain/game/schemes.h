#pragma once
#include "../math/math.h"
#include <cstdint>

namespace game {

    struct Entity {
        uintptr_t ptr;
        math::Vec3 pos;
        math::Vec3 head;
        int health;
        int team;
        bool is_valid;
    };

    struct LocalPlayer {
        uintptr_t ptr;
        int team;
        math::ViewMatrix vm;
    };

}