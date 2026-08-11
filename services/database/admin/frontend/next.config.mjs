import path from "path";
import { fileURLToPath } from "url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const monorepoRoot = path.resolve(__dirname, "../../../..");

const backend = process.env.BACKEND_URL || "http://localhost:5060";

/** @type {import('next').NextConfig} */
const nextConfig = {
  basePath: process.env.NEXT_BASE_PATH || "",
  output: "standalone",
  sassOptions: {
    silenceDeprecations: ["legacy-js-api"],
    includePaths: [
      path.join(monorepoRoot, "shared/scss"),
      path.join(monorepoRoot, "shared/scss/m3-scss"),
    ],
    loadPaths: [
      path.join(monorepoRoot, "shared/scss/m3-scss"),
      path.join(monorepoRoot, "shared/scss"),
    ],
  },
  transpilePackages: [
    "@shared/m3",
    "@shared/components",
    "@shared/hooks",
    "@shared/hooks-canvas",
    "@shared/icons",
    "@shared/redux-core",
    "@shared/redux-slices",
    "@shared/scss",
    "@shared/service-adapters",
  ],
  turbopack: { root: monorepoRoot },
  async rewrites() {
    return [
      {
        source: "/api/pgadmin/:path*",
        destination: `${backend}/api/:path*`,
      },
      {
        source: "/api/health",
        destination: `${backend}/health`,
      },
    ];
  },
};

export default nextConfig;
