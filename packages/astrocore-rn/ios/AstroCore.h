#import <Foundation/Foundation.h>

@class RCTBridge;

NS_ASSUME_NONNULL_BEGIN

/**
 A small Objective-C++ shim that bootstraps the AstroCore JSI bindings on iOS.
 Call `installOnBridge:` during app start once the React bridge is ready.
 */
@interface AstroCoreInstaller : NSObject

+ (void)installOnBridge:(RCTBridge *)bridge;

@end

NS_ASSUME_NONNULL_END
