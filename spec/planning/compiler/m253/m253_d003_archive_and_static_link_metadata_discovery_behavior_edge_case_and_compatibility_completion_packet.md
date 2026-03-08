# M253-D003 Packet

## Scope

`M253-D003` completes the lane-D metadata discovery path that `M253-D002` intentionally left open.

This issue closes three concrete gaps:

1. cross-translation-unit public-anchor collisions,
2. multi-archive response/discovery fan-in,
3. duplicate `objc3c_entry` collisions from metadata-only library objects.

## Required implementation surface

- Extend the native IR metadata handoff so the linker-anchor seed is translation-unit-stable.
- Extend the object-level discovery JSON with translation-unit identity fields.
- Ensure metadata-only library objects without `main` do not export a colliding public `objc3c_entry`.
- Publish one canonical merged discovery/response artifact pair for downstream static-link orchestration.
- Keep the path fail closed when discovery inputs collide or are malformed.

## Code anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.h`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/io/objc3_process.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `scripts/merge_objc3_runtime_metadata_linker_artifacts.py`

## Acceptance boundaries

- Object-level artifacts remain the D002 contract surface.
- D003 adds merged static-link discovery on top of D002; it does not replace D002.
- D003 does not add runtime registration/startup bootstrap.
- D003 does not add archive construction to the native compiler CLI.

## Positive proof

- Compile identical module/metadata surface from two distinct source paths and prove distinct anchor/discovery symbols.
- Compile two distinct metadata-only libraries, archive them, merge their discovery artifacts, and prove:
  - plain link has no retained metadata sections,
  - merged-response link retains metadata sections,
  - merged retained metadata exceeds the single-archive retained baseline.

## Negative proof

- Merge utility rejects malformed or colliding discovery inputs without producing a false-success merged artifact.

## Evidence

- `tmp/reports/m253/M253-D003/archive_and_static_link_metadata_discovery_behavior_summary.json`
