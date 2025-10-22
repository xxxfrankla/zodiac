#include "jsi_bindings.hpp"

#include "AstroCoreHostObject.hpp"

#include <utility>

#if defined(__ANDROID__)
#include <jni.h>
#endif

namespace astro::jsi {

namespace jsi = facebook::jsi;

void install(jsi::Runtime& runtime) {
  auto installerObject = jsi::Object(runtime);

  auto installFn = jsi::Function::createFromHostFunction(
      runtime,
      jsi::PropNameID::forAscii(runtime, "install"),
      0,
      [](jsi::Runtime& rt, const jsi::Value&, const jsi::Value*, std::size_t) {
        auto binding = createAstroCoreBinding(rt);
        rt.global().setProperty(rt, "AstroCore", std::move(binding));
        return jsi::Value::undefined();
      });

  installerObject.setProperty(runtime, "install", std::move(installFn));
  runtime.global().setProperty(runtime, "AstroCore", std::move(installerObject));
}

#if defined(__ANDROID__)
extern "C" JNIEXPORT void JNICALL
Java_com_astrocore_AstroCorePackage_nativeInstall(JNIEnv*, jclass, jlong runtimePointer) {
  if (runtimePointer == 0) {
    return;
  }
  auto* runtime = reinterpret_cast<facebook::jsi::Runtime*>(runtimePointer);
  install(*runtime);
}
#endif

}  // namespace astro::jsi
