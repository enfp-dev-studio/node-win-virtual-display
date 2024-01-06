// Type definitions for node-mac-virtual-display
// Project: node-mac-virtual-display

export type DisplayInfo = {
  id: number;
  width: number;
  height: number;
};

// export function createVirtualDisplay(): DisplayInfo;
export function createVirtualDisplay(): void;
export function destroyVirtualDisplay(): boolean;
