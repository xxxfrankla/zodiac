#pragma once

#include <cmath>

#include "types.hpp"

namespace astro {

struct Vec3 {
  double x{0.0};
  double y{0.0};
  double z{0.0};
};

inline Vec3 operator+(const Vec3& a, const Vec3& b) {
  return {a.x + b.x, a.y + b.y, a.z + b.z};
}

inline Vec3 operator-(const Vec3& a, const Vec3& b) {
  return {a.x - b.x, a.y - b.y, a.z - b.z};
}

inline Vec3 operator*(const Vec3& v, double s) {
  return {v.x * s, v.y * s, v.z * s};
}

inline double dot(const Vec3& a, const Vec3& b) {
  return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 cross(const Vec3& a, const Vec3& b) {
  return {
      a.y * b.z - a.z * b.y,
      a.z * b.x - a.x * b.z,
      a.x * b.y - a.y * b.x};
}

inline double magnitude(const Vec3& v) {
  return std::sqrt(dot(v, v));
}

inline Vec3 normalize(const Vec3& v) {
  double len = magnitude(v);
  if (len == 0.0) {
    return {0.0, 0.0, 0.0};
  }
  return v * (1.0 / len);
}

class Quaternion {
 public:
  Quaternion() = default;
  Quaternion(double w, double x, double y, double z) : w_(w), x_(x), y_(y), z_(z) {}

  static Quaternion fromPose(const PoseQuat& pose) {
    return Quaternion(pose.w, pose.x, pose.y, pose.z).normalized();
  }

  Quaternion normalized() const {
    double len = std::sqrt(w_ * w_ + x_ * x_ + y_ * y_ + z_ * z_);
    if (len == 0.0) {
      return Quaternion();
    }
    return Quaternion(w_ / len, x_ / len, y_ / len, z_ / len);
  }

  Quaternion inverse() const {
    return Quaternion(w_, -x_, -y_, -z_).normalized();
  }

  Vec3 rotate(const Vec3& v) const {
    Quaternion p(0.0, v.x, v.y, v.z);
    Quaternion result = (*this) * p * this->inverse();
    return {result.x_, result.y_, result.z_};
  }

  Quaternion operator*(const Quaternion& other) const {
    return Quaternion(
        w_ * other.w_ - x_ * other.x_ - y_ * other.y_ - z_ * other.z_,
        w_ * other.x_ + x_ * other.w_ + y_ * other.z_ - z_ * other.y_,
        w_ * other.y_ - x_ * other.z_ + y_ * other.w_ + z_ * other.x_,
        w_ * other.z_ + x_ * other.y_ - y_ * other.x_ + z_ * other.w_);
  }

 private:
  double w_{1.0};
  double x_{0.0};
  double y_{0.0};
  double z_{0.0};
};

}  // namespace astro
