#pragma once
#include "../../core/types.h"

namespace math {
	
	struct Vec3 {
		float x, y, z;

		Vec3 operator+(const Vec3& o) const { return { x + o.x, y + o.y, z + o.z }; }
		Vec3 operator-(const Vec3& o) const { return { x - o.x, y - o.y, z - o.z }; }
	};

	struct ViewMatrix {
		float m[4][4];

		const float* operator[](int index) const { return m[index]; }
	};

	bool w2s(const Vec3& wp, Vec3& sp, const ViewMatrix& vm, const core::Extent& ss);

}