# M273-D002 Packet: Macro Host Process And Toolchain Integration - Core Feature Implementation

- Issue: `#7357`
- Packet: `M273-D002`
- Contract ID: `objc3c-part10-macro-host-process-cache-runtime-integration/m273-d002-v1`
- Dependencies: `M273-C003`, `M273-D001`
- Next issue: `M273-E001`
- Scope:
  - turn the frozen `M273-D001` Part 10 host/runtime boundary into a real native-driver host-process path driven by `artifacts/bin/objc3c-native.exe`
  - launch `artifacts/bin/objc3c-frontend-c-api-runner.exe` on cold cache misses and publish deterministic cache reuse under `tmp/artifacts/objc3c-native/cache/part10`
  - emit `module.part10-macro-host-cache.json` as the issue-local host/cache proof sidecar
  - publish one dedicated Part 10 host-process/cache packet inside `module.runtime-import-surface.json`
  - preserve that packet across provider/consumer compilation and `module.cross-module-runtime-link-plan.json`
  - fail closed on incompatible imported Part 10 host/cache metadata instead of carrying observed-gap placeholders
