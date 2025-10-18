#pragma once

#include <algorithm>
#include <cstddef>
#include <span>
#include <vector>

namespace astro {

class RingBuffer {
 public:
  void configure(std::size_t stride, std::size_t capacity);

  float* writePtr();
  void commit(std::size_t count);

  const float* readPtr() const;
  std::span<const float> readSpan() const;
  std::size_t stride() const;
  std::size_t count() const;
  std::size_t byteLength() const;

 private:
  void ensureSize(std::vector<float>& buffer, std::size_t stride, std::size_t capacity);

  std::vector<float> front_;
  std::vector<float> back_;
  std::size_t stride_{0};
  std::size_t capacity_{0};
  std::size_t count_{0};
};

}  // namespace astro

namespace astro {

inline void RingBuffer::configure(std::size_t stride, std::size_t capacity) {
  stride_ = stride;
  capacity_ = capacity;
  ensureSize(front_, stride, capacity);
  ensureSize(back_, stride, capacity);
  count_ = 0;
}

inline void RingBuffer::ensureSize(std::vector<float>& buffer, std::size_t stride, std::size_t capacity) {
  buffer.resize(stride * capacity);
}

inline float* RingBuffer::writePtr() {
  return back_.data();
}

inline void RingBuffer::commit(std::size_t count) {
  if (count > capacity_) {
    count = capacity_;
  }
  count_ = count;
  std::swap(front_, back_);
}

inline const float* RingBuffer::readPtr() const {
  return front_.data();
}

inline std::span<const float> RingBuffer::readSpan() const {
  return std::span<const float>(front_.data(), count_ * stride_);
}

inline std::size_t RingBuffer::stride() const {
  return stride_;
}

inline std::size_t RingBuffer::count() const {
  return count_;
}

inline std::size_t RingBuffer::byteLength() const {
  return front_.size() * sizeof(float);
}

}  // namespace astro
