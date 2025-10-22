#pragma once

#include <memory>
#include <vector>

#include <jsi/jsi.h>

#include "astro/engine.hpp"

namespace astro::jsi {

class AstroCoreHostObject final : public facebook::jsi::HostObject {
 public:
  explicit AstroCoreHostObject(std::shared_ptr<AstroEngine> engine);

  std::vector<facebook::jsi::PropNameID> getPropertyNames(facebook::jsi::Runtime& runtime) override;
  facebook::jsi::Value get(facebook::jsi::Runtime& runtime, const facebook::jsi::PropNameID& name) override;

 private:
  std::shared_ptr<AstroEngine> engine_;
};

facebook::jsi::Object createAstroCoreBinding(facebook::jsi::Runtime& runtime);

}  // namespace astro::jsi
