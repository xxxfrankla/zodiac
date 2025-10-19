#include "astro/transform.hpp"

#include <algorithm>
#include <cmath>

namespace astro::transform {

namespace {
constexpr double kDegToRad = 0.01745329251994329577;
constexpr double kRadToDeg = 57.2957795130823208768;
constexpr double kTwoPi = 6.28318530717958647692;

double wrapAzimuth(double az) {
  az = std::fmod(az, kTwoPi);
  if (az < 0.0) {
    az += kTwoPi;
  }
  return az;
}
}  // namespace

Horizontal equatorialToHorizontal(double raDeg, double decDeg, double lstRad, double latDeg) {
  const double raRad = raDeg * kDegToRad;
  const double decRad = decDeg * kDegToRad;
  const double latRad = latDeg * kDegToRad;

  const double hourAngle = lstRad - raRad;

  const double sinAlt = std::sin(decRad) * std::sin(latRad) +
                        std::cos(decRad) * std::cos(latRad) * std::cos(hourAngle);
  const double alt = std::asin(std::clamp(sinAlt, -1.0, 1.0));

  const double y = -std::sin(hourAngle) * std::cos(decRad);
  const double x = std::sin(decRad) * std::cos(latRad) -
                   std::cos(decRad) * std::sin(latRad) * std::cos(hourAngle);
  const double az = wrapAzimuth(std::atan2(y, x));

  return {alt, az};
}

double applyRefraction(double altRad) {
  const double altDeg = altRad * kRadToDeg;
  if (altDeg < -1.0) {
    return altRad;
  }

  const double refrDeg = 1.0 / std::tan((altDeg + 7.31 / (altDeg + 4.4)) * kDegToRad) / 60.0;
  return altRad + refrDeg * kDegToRad;
}

}  // namespace astro::transform
