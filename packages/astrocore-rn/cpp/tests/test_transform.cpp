#include "astro/transform.hpp"

#include <cassert>
#include <cmath>

namespace {
constexpr double kDegToRad = 0.01745329251994329577;
}

int main() {
  // Polaris-like: RA equals LST, Dec equals latitude -> Alt ~ 90°
  const double lst = 40.0 * kDegToRad;
  const double ra = 40.0;
  const double dec = 50.0;
  const double lat = 50.0;
  const auto horizontal = astro::transform::equatorialToHorizontal(ra, dec, lst, lat);
  assert(std::fabs(horizontal.altRad - (90.0 * kDegToRad)) < 1e-3);

  // At meridian crossing, azimuth near north.
  const auto horizontal2 = astro::transform::equatorialToHorizontal(ra, dec, lst, lat);
  assert(horizontal2.azRad < 0.2 || horizontal2.azRad > 6.08);

  // Refraction should slightly raise low altitude objects.
  const double altNoRef = 5.0 * kDegToRad;
  const double altRef = astro::transform::applyRefraction(altNoRef);
  assert(altRef > altNoRef);

  return 0;
}
