#include "astro/engine.hpp"
#include "astro/time.hpp"

#include <array>
#include <cassert>

int main() {
  astro::AstroEngine engine;

  astro::EngineConfig config{};
  config.fovDeg = 60.0;
  config.screen.width = 1080;
  config.screen.height = 1920;
  config.applyRefraction = true;
  engine.setConfig(config);

  astro::Observer observer{};
  observer.latDeg = 37.7749;
  observer.lonDeg = -122.4194;
  engine.setObserver(observer);

  std::array<astro::StarIn, 3> stars{{
      {0.0, observer.latDeg, 1.0, 1},
      {40.0, observer.latDeg - 10.0, 2.0, 2},
      {120.0, observer.latDeg - 20.0, 4.0, 3},
  }};

  engine.setStars(stars);
  engine.updatePose({1.0, 0.0, 0.0, 0.0});

  const double jd = astro::time::unixMillisToJulianDate(1700000000000ULL);
  const std::size_t visible = engine.computeFrame(jd);

  assert(visible <= stars.size());
  const auto& buffer = engine.ringBuffer();
  assert(buffer.count() == visible);
  assert(buffer.stride() == 4);

  return 0;
}
