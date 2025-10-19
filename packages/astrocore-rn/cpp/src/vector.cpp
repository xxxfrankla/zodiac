#include "astro/vector.hpp"

#include <cmath>

namespace astro::vector {

namespace {
constexpr double kDegToRad = 0.01745329251994329577;
}  // namespace

Vec3 horizontalToENU(const Horizontal& horizontal) {
  const double cosAlt = std::cos(horizontal.altRad);
  return {
      cosAlt * std::sin(horizontal.azRad),   // east
      cosAlt * std::cos(horizontal.azRad),   // north
      std::sin(horizontal.altRad)};          // up
}

Vec3 rotateToDevice(const Vec3& enu, const Quaternion& orientation) {
  return orientation.rotate(enu);
}

bool projectToScreen(const Vec3& deviceVec, const EngineConfig& config, float& outX, float& outY) {
  if (deviceVec.z <= 0.0) {
    return false;
  }

  const double halfWidth = static_cast<double>(config.screen.width) * 0.5;
  const double halfHeight = static_cast<double>(config.screen.height) * 0.5;
  const double fovRad = config.fovDeg * kDegToRad;
  const double focalLength = halfWidth / std::tan(fovRad * 0.5);

  const double ndcX = (deviceVec.x / deviceVec.z) * focalLength;
  const double ndcY = (deviceVec.y / deviceVec.z) * focalLength;

  outX = static_cast<float>(halfWidth + ndcX);
  outY = static_cast<float>(halfHeight - ndcY);

  return outX >= 0.0f && outX <= config.screen.width && outY >= 0.0f && outY <= config.screen.height;
}

}  // namespace astro::vector
