import { defineConfig } from "vite";
import { nodeResolve } from "@rollup/plugin-node-resolve";
import fixReactVirtualized from "esbuild-plugin-react-virtualized";

export default defineConfig({
  build: {
    outDir: "./dist",
  },
  plugins: [nodeResolve()],
  optimizeDeps: {
    esbuildOptions: {
      plugins: [fixReactVirtualized],
    },
  },
});
