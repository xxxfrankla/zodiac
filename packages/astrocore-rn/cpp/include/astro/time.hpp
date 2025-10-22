#pragma once

#include <cstdint>

namespace astro::time {

double unixMillisToJulianDate(std::int64_t unixMs);
double julianDateToGMSTRad(double jd);
double localSiderealTimeRad(double jd, double lonDeg);

}  // namespace astro::time
