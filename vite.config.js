import { nodeResolve } from "@rollup/plugin-node-resolve";
import { defineConfig } from "vite";

export default defineConfig({
  build: {
    outDir: "./dist",
  },
  plugins: [nodeResolve()],
});
