#include "draw.h"

namespace render {

	void DrawList::clear() {
		_rects.clear();
	}

	void DrawList::add(const core::Rect& r, const core::Color& c) {
		_rects.push_back(RectCmd{ r, c });
	}

}