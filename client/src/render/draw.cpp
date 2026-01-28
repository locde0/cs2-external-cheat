#include "draw.h"

namespace render {

	void DrawList::clear() {
		_rects.clear();
		_lines.clear();
	}

	void DrawList::addLine(const core::Vec2& a, const core::Vec2& b, const core::Color& c) {
		_lines.push_back({ a, b, c });
	}

	void DrawList::addBox(const core::Box& b, const core::Color& c) {
        if (b.thickness <= 1) {
            float x2 = b.x + b.w;
            float y2 = b.y + b.h;

            addLine({ b.x, b.y }, { x2, b.y }, c);
            addLine({ b.x, y2 }, { x2, y2 }, c);
            addLine({ b.x, b.y }, { b.x, y2 }, c);
            addLine({ x2, b.y }, { x2, y2 }, c);
            return;
        }

        float t = static_cast<float>(b.thickness);
        addRect({ b.x, b.y, b.w, t }, c);
        addRect({ b.x, b.y + b.h - t, b.w, t }, c);
        addRect({ b.x, b.y + t, t, b.h - 2 * t }, c);
        addRect({ b.x + b.w - t, b.y + t, t, b.h - 2 * t }, c);
	}

	void DrawList::addRect(const core::Rect& r, const core::Color& c) {
		_rects.push_back(RectCmd{ r, c });
	}

}