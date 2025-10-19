#include "astro/engine.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>

#include "RingBuffer.hpp"
#include "astro/Quaternion.hpp"
#include "astro/time.hpp"
#include "astro/transform.hpp"
#include "astro/vector.hpp"

namespace astro {

namespace {
constexpr std::size_t kStride = ASTRO_RINGBUFFER_STRIDE;
}

AstroEngine::AstroEngine()
    : ringBuffer_(std::make_unique<RingBuffer>()) {
  ringBuffer_->configure(kStride, ASTRO_MAX_STARS);
}

AstroEngine::~AstroEngine() = default;

void AstroEngine::setConfig(const EngineConfig& config) {
  config_ = config;
  if (config_.fovDeg <= 0.0) {
    config_.fovDeg = ASTRO_DEFAULT_FOV_DEG;
  }
  configReady_ = config_.screen.width > 0 && config_.screen.height > 0;
}

void AstroEngine::setObserver(const Observer& observer) {
  observer_ = observer;
}

void AstroEngine::setStars(std::span<const StarIn> stars) {
  catalog_.assign(stars.begin(), stars.end());
  output_.resize(catalog_.size());
}

void AstroEngine::updatePose(const PoseQuat& pose) {
  pose_ = pose;
}

std::size_t AstroEngine::computeFrame(double jd) {
  if (!configReady_ || catalog_.empty()) {
    ringBuffer_->commit(0);
    return 0;
  }

  const double lst = time::localSiderealTimeRad(jd, observer_.lonDeg);
  const Quaternion orientation = Quaternion::fromPose(pose_);

  float* writePtr = ringBuffer_->writePtr();
  std::size_t visibleCount = 0;

  for (std::size_t i = 0; i < catalog_.size(); ++i) {
    const StarIn& star = catalog_[i];
    StarOut& out = output_[i];

    Horizontal horizontal = transform::equatorialToHorizontal(
        star.raDeg, star.decDeg, lst, observer_.latDeg);

    if (config_.applyRefraction) {
      horizontal.altRad = transform::applyRefraction(horizontal.altRad);
    }

    out.visible = horizontal.altRad > 0.0;
    out.hip = star.hip;
    out.mag = static_cast<float>(star.mag);
    out.x = 0.0f;
    out.y = 0.0f;

    if (!out.visible) {
      continue;
    }

    const Vec3 enu = vector::horizontalToENU(horizontal);
    const Vec3 deviceVec = vector::rotateToDevice(enu, orientation);

    float screenX = 0.0f;
    float screenY = 0.0f;
    out.visible = vector::projectToScreen(deviceVec, config_, screenX, screenY);
    if (!out.visible) {
      continue;
    }

    out.x = screenX;
    out.y = screenY;

    const std::size_t base = visibleCount * kStride;
    writePtr[base] = screenX;
    writePtr[base + 1] = screenY;
    writePtr[base + 2] = out.mag;
    writePtr[base + 3] = static_cast<float>(out.hip);
    visibleCount += 1;
  }

  ringBuffer_->commit(visibleCount);
  return visibleCount;
}

}  // namespace astro
