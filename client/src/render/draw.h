#pragma once
#include <vector>
#include "../core/types.h"

namespace render {

	struct RectCmd {
		core::Rect rect;
		core::Color color;
	};

	class DrawList {
	public:
		void clear();
		void add(const core::Rect&, const core::Color&);
		const std::vector<RectCmd>& rects() const { return _rects; }
		bool empty() const { return _rects.empty(); }

	private:
		std::vector<RectCmd> _rects;
	};

}