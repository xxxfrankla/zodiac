import { useEffect, useMemo, useRef, useState } from 'react';
import type { PoseQuat } from '../types';
import { computeFrame, getFrameBuffer, updatePose } from '../SkyEngine';

type UseSkyEngineOptions = {
  poseProvider: () => PoseQuat | null;
  frameIntervalMs?: number;
  active?: boolean;
  timestampProvider?: () => number;
};

type UseSkyEngineResult = {
  frameBuffer: Float32Array | null;
  frameCount: number;
};

/**
 * Convenience hook that polls device pose and drives the AstroCore frame loop.
 * The hook avoids any allocations in the hot path by reusing the Float32Array
 * returned from `getFrameBuffer()`.
 */
export function useSkyEngine(options: UseSkyEngineOptions): UseSkyEngineResult {
  const {
    poseProvider,
    frameIntervalMs = 16,
    active = true,
    timestampProvider = Date.now
  } = options;

  const [frameCount, setFrameCount] = useState(0);
  const frameRef = useRef<Float32Array | null>(null);

  useEffect(() => {
    if (!active) {
      return undefined;
    }

    let cancelled = false;
    let rafHandle: number | null = null;

    const tick = () => {
      if (cancelled) {
        return;
      }

      const pose = poseProvider();
      if (pose) {
        updatePose(pose);
      }

      const count = computeFrame(timestampProvider());
      frameRef.current = getFrameBuffer();
      setFrameCount(count);

      if (frameIntervalMs <= 16) {
        rafHandle = requestAnimationFrame(tick);
      } else {
        setTimeout(tick, frameIntervalMs);
      }
    };

    if (frameIntervalMs <= 16) {
      rafHandle = requestAnimationFrame(tick);
    } else {
      const timeout = setTimeout(tick, frameIntervalMs);
      rafHandle = -1;
      return () => {
        cancelled = true;
        clearTimeout(timeout);
      };
    }

    return () => {
      cancelled = true;
      if (rafHandle !== null && rafHandle >= 0) {
        cancelAnimationFrame(rafHandle);
      }
    };
  }, [active, frameIntervalMs, poseProvider, timestampProvider]);

  return useMemo(
    () => ({
      frameBuffer: frameRef.current,
      frameCount
    }),
    [frameCount]
  );
}
