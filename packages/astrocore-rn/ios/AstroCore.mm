#import "AstroCore.h"

#import <React/RCTBridge+Private.h>
#import <React/RCTCxxBridge.h>
#import <React/RCTLog.h>
#import <React/RCTUtils.h>

#import "../cpp/src/jsi_bindings.hpp"

@implementation AstroCoreInstaller

+ (void)installOnBridge:(RCTBridge *)bridge {
  if (bridge == nil) {
    RCTLogError(@"[AstroCore] installOnBridge called with nil bridge.");
    return;
  }

  if (![bridge isKindOfClass:[RCTCxxBridge class]]) {
    RCTLogWarn(@"[AstroCore] installOnBridge expects an RCTCxxBridge instance.");
    return;
  }

  RCTCxxBridge *cxxBridge = (RCTCxxBridge *)bridge;
  facebook::jsi::Runtime *runtime = cxxBridge.runtime;
  if (runtime == nil) {
    RCTLogWarn(@"[AstroCore] JSI runtime not yet available. Delay installation.");
    return;
  }

  astro::jsi::install(*runtime);
}

@end
