#include "math.h"

namespace math {

	bool w2s(const Vec3& wp, Vec3& sp, const ViewMatrix& vm, const core::Extent& ss) {
        float w = vm[3][0] * wp.x + vm[3][1] * wp.y + vm[3][2] * wp.z + vm[3][3];

        if (w < 0.01f) return false;

        float inv_w = 1.f / w;
        float x = (vm[0][0] * wp.x + vm[0][1] * wp.y + vm[0][2] * wp.z + vm[0][3]) * inv_w;
        float y = (vm[1][0] * wp.x + vm[1][1] * wp.y + vm[1][2] * wp.z + vm[1][3]) * inv_w;

        sp.x = (ss.w * 0.5f) * (x + 1.0f);
        sp.y = (ss.h * 0.5f) * (1.0f - y);
        sp.z = w;
        return true;
    }

}