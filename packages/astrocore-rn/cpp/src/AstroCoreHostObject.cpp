#include "AstroCoreHostObject.hpp"

#include <chrono>
#include <cstdint>
#include <span>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "RingBuffer.hpp"
#include "astro/time.hpp"

namespace astro::jsi {

namespace jsi = facebook::jsi;

namespace {

EngineConfig readEngineConfig(jsi::Runtime& rt, const jsi::Object& object) {
  EngineConfig config{};

  if (object.hasProperty(rt, "fovDeg")) {
    config.fovDeg = object.getProperty(rt, "fovDeg").asNumber();
  }
  if (object.hasProperty(rt, "width")) {
    config.screen.width = static_cast<int>(object.getProperty(rt, "width").asNumber());
  }
  if (object.hasProperty(rt, "height")) {
    config.screen.height = static_cast<int>(object.getProperty(rt, "height").asNumber());
  }
  if (object.hasProperty(rt, "applyRefraction")) {
    config.applyRefraction = object.getProperty(rt, "applyRefraction").getBool();
  }

  return config;
}

Observer readObserver(jsi::Runtime& rt, const jsi::Object& object) {
  Observer observer{};
  observer.latDeg = object.getProperty(rt, "latDeg").asNumber();
  observer.lonDeg = object.getProperty(rt, "lonDeg").asNumber();
  if (object.hasProperty(rt, "elevationM")) {
    observer.elevationM = object.getProperty(rt, "elevationM").asNumber();
  }
  return observer;
}

PoseQuat readPose(jsi::Runtime& rt, const jsi::Object& object) {
  PoseQuat pose{};
  pose.w = object.getProperty(rt, "w").asNumber();
  pose.x = object.getProperty(rt, "x").asNumber();
  pose.y = object.getProperty(rt, "y").asNumber();
  pose.z = object.getProperty(rt, "z").asNumber();
  return pose;
}

std::vector<StarIn> readStarVector(jsi::Runtime& rt, const jsi::Value& value) {
  if (!value.isObject()) {
    throw jsi::JSError(rt, "AstroCore.setStars expects a Float32Array or StarIn[].");
  }

  jsi::Object object = value.getObject(rt);

  if (object.isArray(rt)) {
    const std::size_t length = static_cast<std::size_t>(object.getProperty(rt, "length").asNumber());
    std::vector<StarIn> stars;
    stars.reserve(length);
    for (std::size_t i = 0; i < length; ++i) {
      jsi::Value item = object.getPropertyAtIndex(rt, static_cast<uint32_t>(i));
      if (!item.isObject()) {
        continue;
      }
      jsi::Object starObj = item.getObject(rt);
      StarIn star{};
      star.raDeg = starObj.getProperty(rt, "raDeg").asNumber();
      star.decDeg = starObj.getProperty(rt, "decDeg").asNumber();
      star.mag = starObj.getProperty(rt, "mag").asNumber();
      if (starObj.hasProperty(rt, "hip")) {
        star.hip = static_cast<int>(starObj.getProperty(rt, "hip").asNumber());
      }
      stars.push_back(star);
    }
    return stars;
  }

  if (object.hasProperty(rt, "buffer") && object.hasProperty(rt, "BYTES_PER_ELEMENT")) {
    const auto bytesPerElement = object.getProperty(rt, "BYTES_PER_ELEMENT").asNumber();
    if (bytesPerElement != 4) {
      throw jsi::JSError(rt, "AstroCore.setStars expects a Float32Array.");
    }
    const std::size_t length = static_cast<std::size_t>(object.getProperty(rt, "length").asNumber());
    const std::size_t stride = 4;
    if (length % stride != 0) {
      throw jsi::JSError(rt, "Star buffer length must be a multiple of 4.");
    }
    jsi::Object bufferObj = object.getProperty(rt, "buffer").getObject(rt);
    jsi::ArrayBuffer arrayBuffer = bufferObj.getArrayBuffer(rt);
    auto* data = reinterpret_cast<float*>(arrayBuffer.data(rt));
    const std::size_t elementCount = length / stride;
    std::vector<StarIn> stars;
    stars.reserve(elementCount);

    for (std::size_t i = 0; i < elementCount; ++i) {
      const std::size_t base = i * stride;
      StarIn star{};
      star.raDeg = data[base];
      star.decDeg = data[base + 1];
      star.mag = data[base + 2];
      star.hip = static_cast<int>(data[base + 3]);
      stars.push_back(star);
    }
    return stars;
  }

  throw jsi::JSError(rt, "Unsupported star payload supplied to AstroCore.setStars.");
}

class FrameBufferHostObject final : public jsi::HostObject {
 public:
  FrameBufferHostObject(std::shared_ptr<AstroEngine> engine,
                        const float* data,
                        std::size_t byteLength)
      : engine_(std::move(engine)),
        data_(reinterpret_cast<const std::uint8_t*>(data)),
        length_(byteLength) {}

  std::size_t size(jsi::Runtime&) override {
    return length_;
  }

  uint8_t* data(jsi::Runtime&) override {
    return const_cast<std::uint8_t*>(data_);
  }

 private:
  std::shared_ptr<AstroEngine> engine_;
  const std::uint8_t* data_;
  std::size_t length_;
};

}  // namespace

AstroCoreHostObject::AstroCoreHostObject(std::shared_ptr<AstroEngine> engine)
    : engine_(std::move(engine)) {}

std::vector<jsi::PropNameID> AstroCoreHostObject::getPropertyNames(jsi::Runtime& runtime) {
  std::vector<std::string> names = {
      "startEngine",
      "stopEngine",
      "setStars",
      "setObserver",
      "setConfig",
      "updatePose",
      "computeFrame",
      "getFrameBuffer"};

  std::vector<jsi::PropNameID> props;
  props.reserve(names.size());
  for (const auto& name : names) {
    props.push_back(jsi::PropNameID::forAscii(runtime, name));
  }
  return props;
}

jsi::Value AstroCoreHostObject::get(jsi::Runtime& runtime, const jsi::PropNameID& name) {
  const auto propName = name.utf8(runtime);

  if (propName == "startEngine") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        1,
        [engine = engine_](jsi::Runtime& rt, const jsi::Value&, const jsi::Value* args, std::size_t count) {
          if (count < 1 || !args[0].isObject()) {
            throw jsi::JSError(rt, "AstroCore.startEngine expects a config object.");
          }
          const EngineConfig config = readEngineConfig(rt, args[0].getObject(rt));
          engine->setConfig(config);
          return jsi::Value(true);
        });
  }

  if (propName == "stopEngine") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        0,
        [engine = engine_](jsi::Runtime&, const jsi::Value&, const jsi::Value*, std::size_t) {
          engine->ringBuffer().commit(0);
          return jsi::Value::undefined();
        });
  }

  if (propName == "setStars") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        1,
        [engine = engine_](jsi::Runtime& rt, const jsi::Value&, const jsi::Value* args, std::size_t count) {
          if (count < 1) {
            throw jsi::JSError(rt, "AstroCore.setStars expects an argument.");
          }
          auto stars = readStarVector(rt, args[0]);
          engine->setStars(std::span<const StarIn>(stars.data(), stars.size()));
          return jsi::Value(true);
        });
  }

  if (propName == "setObserver") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        1,
        [engine = engine_](jsi::Runtime& rt, const jsi::Value&, const jsi::Value* args, std::size_t count) {
          if (count < 1 || !args[0].isObject()) {
            throw jsi::JSError(rt, "AstroCore.setObserver expects an object.");
          }
          engine->setObserver(readObserver(rt, args[0].getObject(rt)));
          return jsi::Value::undefined();
        });
  }

  if (propName == "setConfig") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        1,
        [engine = engine_](jsi::Runtime& rt, const jsi::Value&, const jsi::Value* args, std::size_t count) {
          if (count < 1 || !args[0].isObject()) {
            throw jsi::JSError(rt, "AstroCore.setConfig expects a config object.");
          }
          engine->setConfig(readEngineConfig(rt, args[0].getObject(rt)));
          return jsi::Value::undefined();
        });
  }

  if (propName == "updatePose") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        1,
        [engine = engine_](jsi::Runtime& rt, const jsi::Value&, const jsi::Value* args, std::size_t count) {
          if (count < 1 || !args[0].isObject()) {
            throw jsi::JSError(rt, "AstroCore.updatePose expects an object.");
          }
          engine->updatePose(readPose(rt, args[0].getObject(rt)));
          return jsi::Value::undefined();
        });
  }

  if (propName == "computeFrame") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        1,
        [engine = engine_](jsi::Runtime& rt, const jsi::Value&, const jsi::Value* args, std::size_t count) {
          if (count < 1 || !args[0].isNumber()) {
            throw jsi::JSError(rt, "AstroCore.computeFrame expects a timestamp in milliseconds.");
          }
          const double jd = time::unixMillisToJulianDate(static_cast<std::int64_t>(args[0].asNumber()));
          auto visible = engine->computeFrame(jd);
          return jsi::Value(static_cast<double>(visible));
        });
  }

  if (propName == "getFrameBuffer") {
    return jsi::Function::createFromHostFunction(
        runtime,
        name,
        0,
        [engine = engine_](jsi::Runtime& rt, const jsi::Value&, const jsi::Value*, std::size_t) {
          const RingBuffer& buffer = engine->ringBuffer();
          const std::size_t byteLen = buffer.count() * buffer.stride() * sizeof(float);
          const float* data = buffer.readPtr();
          auto hostObject = std::make_shared<FrameBufferHostObject>(engine, data, byteLen);
          auto arrayBuffer = jsi::ArrayBuffer::createFromHostObject(rt, hostObject);
          jsi::Function float32ArrayCtor =
              rt.global().getPropertyAsFunction(rt, "Float32Array");
          return float32ArrayCtor.callAsConstructor(rt, arrayBuffer);
        });
  }

  return jsi::Value::undefined();
}

jsi::Object createAstroCoreBinding(jsi::Runtime& runtime) {
  auto engine = std::make_shared<AstroEngine>();
  auto hostObject = std::make_shared<AstroCoreHostObject>(engine);
  return jsi::Object::createFromHostObject(runtime, hostObject);
}

}  // namespace astro::jsi
