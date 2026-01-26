#pragma once
#include "../math/math.h"
#include <string>
#include <cstdint>

namespace game {

    struct Entity {
        uintptr_t ptr;
        int health;
        int team;
        math::Vec3 feet;
		math::Vec3 mins;
		math::Vec3 maxs;
    };

    struct LocalPlayer {
        uintptr_t ptr;
        int team;
        math::ViewMatrix vm;
        uintptr_t spect_ptr;
    };

}