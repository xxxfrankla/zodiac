import type { EngineConfig, FrameMeta, ObserverConfig, PoseQuat, StarIn } from './types';

type NativeAstroCore = {
  install: () => void;
  startEngine: (config: EngineConfig) => boolean;
  stopEngine: () => void;
  setStars: (stars: Float32Array | StarIn[]) => boolean;
  setObserver: (observer: ObserverConfig) => void;
  setConfig: (config: EngineConfig) => void;
  updatePose: (pose: PoseQuat) => void;
  computeFrame: (tUnixMs: FrameMeta['tUnixMs']) => number;
  getFrameBuffer: () => Float32Array;
};

const ASTRO_GLOBAL_KEY = 'AstroCore';

let cachedHost: NativeAstroCore | null = null;
let installed = false;

function resolveHost(): NativeAstroCore {
  if (cachedHost) {
    return cachedHost;
  }

  const host = (globalThis as Record<string, unknown>)[ASTRO_GLOBAL_KEY] as NativeAstroCore | undefined;
  if (!host) {
    throw new Error('AstroCore native bindings are not available. Ensure the native module is initialized.');
  }

  cachedHost = host;
  return host;
}

function ensureInstalled(): NativeAstroCore {
  const host = resolveHost();
  if (!installed) {
    throw new Error('AstroCore.install() must be called before using the engine.');
  }

  return host;
}

export function install(): void {
  const host = resolveHost();
  if (installed) {
    return;
  }

  if (typeof host.install !== 'function') {
    throw new Error('AstroCore.install() is not exposed by the native bindings.');
  }

  host.install();
  cachedHost = (globalThis as Record<string, unknown>)[ASTRO_GLOBAL_KEY] as NativeAstroCore | null;
  installed = true;
}

export function startEngine(config: EngineConfig): boolean {
  return ensureInstalled().startEngine(config);
}

export function stopEngine(): void {
  ensureInstalled().stopEngine();
}

export function setStars(stars: Float32Array | StarIn[]): boolean {
  const host = ensureInstalled();
  if (stars instanceof Float32Array) {
    return host.setStars(stars);
  }

  const packed = new Float32Array(stars.length * 4);
  for (let i = 0; i < stars.length; i += 1) {
    const star = stars[i];
    const base = i * 4;
    packed[base] = star.raDeg;
    packed[base + 1] = star.decDeg;
    packed[base + 2] = star.mag;
    packed[base + 3] = star.hip ?? 0;
  }

  return host.setStars(packed);
}

export function setObserver(observer: ObserverConfig): void {
  ensureInstalled().setObserver(observer);
}

export function setConfig(config: EngineConfig): void {
  ensureInstalled().setConfig(config);
}

export function updatePose(pose: PoseQuat): void {
  ensureInstalled().updatePose(pose);
}

export function computeFrame(tUnixMs: FrameMeta['tUnixMs']): number {
  return ensureInstalled().computeFrame(tUnixMs);
}

export function getFrameBuffer(): Float32Array {
  return ensureInstalled().getFrameBuffer();
}
