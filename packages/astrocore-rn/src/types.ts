export type StarIn = {
  raDeg: number;
  decDeg: number;
  mag: number;
  hip?: number;
};

export type EngineConfig = {
  fovDeg: number;
  width: number;
  height: number;
  applyRefraction?: boolean;
};

export type FrameMeta = {
  tUnixMs: number;
  lat: number;
  lon: number;
};

export type ObserverConfig = {
  latDeg: number;
  lonDeg: number;
  elevationM?: number;
};

export type PoseQuat = {
  w: number;
  x: number;
  y: number;
  z: number;
};
