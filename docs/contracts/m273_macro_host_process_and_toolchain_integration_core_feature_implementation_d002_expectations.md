# M273 Macro Host Process And Toolchain Integration Core Feature Implementation Expectations (D002)

Contract ID: `objc3c-part10-macro-host-process-cache-runtime-integration/m273-d002-v1`

Issue: `#7357`

Expected proof:
- `artifacts/bin/objc3c-native.exe` compiles a positive Part 10 provider fixture and launches `artifacts/bin/objc3c-frontend-c-api-runner.exe` as the host process for macro/property-behavior cache population.
- The provider emits `module.runtime-import-surface.json` with a dedicated `objc_part10_macro_host_process_and_cache_runtime_integration` member that records:
  - `contract_id = objc3c-part10-macro-host-process-cache-runtime-integration/m273-d002-v1`
  - `source_contract_id = objc3c-part10-expansion-host-runtime-boundary/m273-d001-v1`
  - `host_executable_relative_path = artifacts/bin/objc3c-frontend-c-api-runner.exe`
  - `cache_root_relative_path = tmp/artifacts/objc3c-native/cache/part10`
  - `runtime_import_artifact_ready = true`
  - `separate_compilation_ready = true`
  - `deterministic = true`
- The provider also emits `module.part10-macro-host-cache.json` proving a cold host launch on the first identical build and deterministic cache reuse on the second identical build.
- A consumer fixture compiled with `--objc3-import-runtime-surface <provider/module.runtime-import-surface.json>` preserves the same Part 10 host-process/cache contract in both:
  - `module.runtime-import-surface.json`
  - `module.cross-module-runtime-link-plan.json`
- The cross-module link plan preserves the expected D002 contract ids, host executable path, cache root path, imported module inventory, and imported module replay/capability facts.
- The issue-local checker is fail-closed. It does not report observed-gap placeholders; the implementation is required to be live.
- Evidence lands at `tmp/reports/m273/M273-D002/macro_host_process_cache_runtime_integration_summary.json`
