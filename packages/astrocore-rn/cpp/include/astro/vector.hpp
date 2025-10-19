#pragma once

#include "Quaternion.hpp"
#include "types.hpp"

namespace astro::vector {

Vec3 horizontalToENU(const Horizontal& horizontal);
Vec3 rotateToDevice(const Vec3& enu, const Quaternion& orientation);
bool projectToScreen(const Vec3& deviceVec, const EngineConfig& config, float& outX, float& outY);

}  // namespace astro::vector
