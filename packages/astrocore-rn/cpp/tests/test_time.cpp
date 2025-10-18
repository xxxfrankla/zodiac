#include "astro/time.hpp"

#include <cassert>
#include <cmath>

namespace {
constexpr double kPi = 3.14159265358979323846;
constexpr double kTwoPi = 6.28318530717958647692;
}

int main() {
  const double jdEpoch = astro::time::unixMillisToJulianDate(0);
  assert(std::fabs(jdEpoch - 2440587.5) < 1e-6);

  const double gmst = astro::time::julianDateToGMSTRad(2451545.0);
  const double gmstHours = gmst * (12.0 / kPi);
  assert(gmstHours > 0.0 && gmstHours < 24.0);

  const double lst = astro::time::localSiderealTimeRad(2451545.0, 0.0);
  assert(lst >= 0.0 && lst < kTwoPi);

  return 0;
}
