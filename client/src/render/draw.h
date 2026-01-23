#pragma once
#include <cstdint>
#include <vector>

namespace render {
	
	struct Vec2 {
		float x, y;
	};

	struct Color {
		float r, g, b, a;
	};

	struct Rect {
		float x, y, w, h;
	};

	struct RectCmd {
		Rect rect;
		Color color;
	};

	class DrawList {
	public:
		void clear();
		void add(Rect, Color);
		const std::vector<RectCmd>& rects() const { return _rects; }

	private:
		std::vector<RectCmd> _rects;
	};

}