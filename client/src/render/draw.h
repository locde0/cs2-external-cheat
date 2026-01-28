#pragma once
#include <vector>
#include "../core/types.h"

namespace render {

	struct LineCmd {
		core::Vec2 a, b;
		core::Color color;
	};

	struct RectCmd {
		core::Rect rect;
		core::Color color;
	};

	class DrawList {
	public:
		void clear();

		void addLine(const core::Vec2&, const core::Vec2&, const core::Color&);
		void addBox(const core::Box&, const core::Color&);
		void addRect(const core::Rect&, const core::Color&);

		const std::vector<RectCmd>& rects() const { return _rects; }
		const std::vector<LineCmd>& lines() const { return _lines; }
		bool empty() const { return _rects.empty(); }

	private:
		std::vector<RectCmd> _rects;
		std::vector<LineCmd> _lines;
	};

}