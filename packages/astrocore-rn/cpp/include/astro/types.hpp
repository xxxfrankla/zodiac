#pragma once

#include <cstdint>

namespace astro {

struct Observer {
  double latDeg{0.0};
  double lonDeg{0.0};
  double elevationM{0.0};
};

struct Horizontal {
  double altRad{0.0};
  double azRad{0.0};
};

struct PoseQuat {
  double w{1.0};
  double x{0.0};
  double y{0.0};
  double z{0.0};
};

struct ScreenSize {
  int width{0};
  int height{0};
};

struct StarIn {
  double raDeg{0.0};
  double decDeg{0.0};
  double mag{0.0};
  int hip{0};
};

struct StarOut {
  float x{0.0f};
  float y{0.0f};
  float mag{0.0f};
  int hip{0};
  bool visible{false};
};

struct EngineConfig {
  double fovDeg{60.0};
  ScreenSize screen{};
  bool applyRefraction{true};
};

}  // namespace astro
