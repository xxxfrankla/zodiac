#pragma once

#include "types.hpp"

namespace astro::transform {

Horizontal equatorialToHorizontal(double raDeg, double decDeg, double lstRad, double latDeg);
double applyRefraction(double altRad);

}  // namespace astro::transform
