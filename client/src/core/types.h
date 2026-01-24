#pragma once
#include <cstdint>

namespace core {

    struct Vec2 {
        float x, y;
    };

    struct Rect {
        float x, y, w, h;
    };

    struct Color {
        float r, g, b, a;

        static const Color red() { return { 1.f, 0.f, 0.f, 0.8f }; }
        static const Color green() { return { 0.f, 1.f, 0.f, 0.8f }; }
        static const Color transparent() { return { 0.f, 0.f, 0.f, 0.f }; }
    };

    struct Extent {
        int w, h;
    };

}