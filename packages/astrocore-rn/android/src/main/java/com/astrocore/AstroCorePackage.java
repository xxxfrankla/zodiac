package com.astrocore;

import android.util.Log;

import androidx.annotation.NonNull;

import com.facebook.react.ReactPackage;
import com.facebook.react.bridge.JavaScriptContextHolder;
import com.facebook.react.bridge.NativeModule;
import com.facebook.react.bridge.ReactApplicationContext;
import com.facebook.react.uimanager.ViewManager;

import java.util.Collections;
import java.util.List;

/**
 * ReactPackage that exposes the AstroCore JSI installer. Consumers should call
 * {@link AstroCorePackage#install(ReactApplicationContext)} from their application
 * once the React instance is created so the native bindings can register.
 */
public class AstroCorePackage implements ReactPackage {

  static {
    try {
      System.loadLibrary("astrocore");
    } catch (UnsatisfiedLinkError error) {
      Log.e("AstroCore", "Failed to load astrocore native library", error);
    }
  }

  @NonNull
  @Override
  public List<NativeModule> createNativeModules(@NonNull ReactApplicationContext reactContext) {
    return Collections.emptyList();
  }

  @NonNull
  @Override
  public List<ViewManager> createViewManagers(@NonNull ReactApplicationContext reactContext) {
    return Collections.emptyList();
  }

  public static void install(@NonNull ReactApplicationContext reactContext) {
    reactContext.runOnJSQueueThread(() -> {
      JavaScriptContextHolder contextHolder = reactContext.getJavaScriptContextHolder();
      long runtimePointer = contextHolder.get();
      if (runtimePointer == 0L) {
        Log.w("AstroCore", "JSI runtime not yet available. Delaying AstroCore install.");
        return;
      }
      nativeInstall(runtimePointer);
    });
  }

  private static native void nativeInstall(long runtimePointer);
}
