#pragma once

#include <memory>
#include <span>
#include <vector>

#include "ProjectConfig.hpp"
#include "types.hpp"

namespace astro {

class RingBuffer;

class AstroEngine {
 public:
  AstroEngine();
  ~AstroEngine();

  void setConfig(const EngineConfig& config);
  void setObserver(const Observer& observer);
  void setStars(std::span<const StarIn> stars);
  void updatePose(const PoseQuat& pose);

  std::size_t computeFrame(double jd);

  const RingBuffer& ringBuffer() const noexcept {
    return *ringBuffer_;
  }
  RingBuffer& ringBuffer() noexcept {
    return *ringBuffer_;
  }

 private:
  EngineConfig config_;
  Observer observer_;
  PoseQuat pose_;
  std::vector<StarIn> catalog_;
  std::vector<StarOut> output_;
  std::unique_ptr<RingBuffer> ringBuffer_;
  bool configReady_{false};
};

}  // namespace astro
