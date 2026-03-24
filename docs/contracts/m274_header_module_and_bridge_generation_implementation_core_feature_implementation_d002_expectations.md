Contract ID: `objc3c-part11-header-module-and-bridge-generation/m274-d002-v1`
Issue: `#7371`

Current expectation:
- the compiler emits deterministic Part 11 sidecar artifacts for supported foreign-callable surfaces:
  - `module.part11-bridge.h`
  - `module.part11-bridge.modulemap`
  - `module.part11-bridge.json`
- the emitted IR carries:
  - `; part11_header_module_and_bridge_generation = ...`
  - `!objc3.objc_part11_header_module_and_bridge_generation = !{!112}`
- the private runtime proof surface includes `objc3_runtime_copy_part11_bridge_generation_snapshot_for_testing`
- provider and consumer `module.runtime-import-surface.json` artifacts publish `objc_part11_header_module_and_bridge_generation`
- mixed-module link plans preserve imported Part 11 header/module/bridge contract ids, replay keys, readiness bits, artifact-relative paths, and imported-module inventory
- the generated header text is operator-visible evidence and includes `#pragma once` plus the supported callable declarations
- the emitted IR carries:
  - deterministic replay keys for the local Part 11 bridge-generation packet
- mixed-module link plans fail closed on:
  - contract drift
  - source-contract drift
  - preservation-contract drift
  - missing readiness bits
  - mismatched artifact-relative paths
  - duplicate replay keys
- the issue-local checker is fail-closed
