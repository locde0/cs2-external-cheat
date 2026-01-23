#include "draw.h"

namespace render {
	void DrawList::clear() {
		_rects.clear();
	}

	void DrawList::add(Rect r, Color c) {
		_rects.push_back(RectCmd{ r, c });
	}
}