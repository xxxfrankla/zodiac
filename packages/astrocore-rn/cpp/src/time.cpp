#include "astro/time.hpp"

#include <cmath>

namespace astro::time {

namespace {
constexpr double kTwoPi = 6.28318530717958647692;
constexpr double kDegToRad = 0.01745329251994329577;
}  // namespace

double unixMillisToJulianDate(std::int64_t unixMs) {
  return static_cast<double>(unixMs) / 86400000.0 + 2440587.5;
}

double julianDateToGMSTRad(double jd) {
  const double daysSinceJ2000 = jd - 2451545.0;
  double gmstHours = 18.697374558 + 24.06570982441908 * daysSinceJ2000;
  gmstHours = std::fmod(gmstHours, 24.0);
  if (gmstHours < 0.0) {
    gmstHours += 24.0;
  }
  return gmstHours * (kTwoPi / 24.0);
}

double localSiderealTimeRad(double jd, double lonDeg) {
  const double gmst = julianDateToGMSTRad(jd);
  double lst = gmst + lonDeg * kDegToRad;
  lst = std::fmod(lst, kTwoPi);
  if (lst < 0.0) {
    lst += kTwoPi;
  }
  return lst;
}

}  // namespace astro::time
