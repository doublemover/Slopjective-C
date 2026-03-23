# M267-D002 Live Catch, Bridge, And Runtime Integration Core Feature Implementation Packet

Packet: `M267-D002`
Issue: `#7278`
Milestone: `M267`
Lane: `D`
Wave: `W60`

## Objective

Prove the current runnable Part 6 lowering and the packaged native runtime
already execute one truthful status-bridge plus `catch (NSError* error)` path.

## Dependencies

- `M267-C002`
- `M267-C003`
- `M267-D001`

## Required implementation surface

1. Publish one canonical live integration contract:
   `objc3c-part6-live-error-runtime-integration/m267-d002-v1`
2. Emit explicit IR anchors:
   - `; part6_live_error_runtime_integration = ...`
   - `!objc3.objc_part6_live_error_runtime_integration = !{!90}`
3. Keep the runtime helper surface private to
   `objc3_runtime_bootstrap_internal.h`
4. Prove generated object code links against the packaged runtime library using
   the emitted runtime-registration manifest path
5. Prove execution observes deterministic helper traffic through
   `objc3_runtime_copy_error_bridge_state_for_testing`

## Dynamic proof shape

- compile
  `tests/tooling/fixtures/native/m267_d002_live_error_runtime_integration_positive.objc3`
- link the generated `module.obj` with
  `tests/tooling/runtime/m267_d002_live_error_runtime_integration_probe.cpp`
  and `artifacts/lib/objc3_runtime.lib`
- execute the probe and require:
  - `rc = 54`
  - one thrown-error store
  - one thrown-error load
  - one status-bridge helper call
  - one catch-match helper call
  - zero `NSError` bridge helper calls
  - last catch kind name `nserror`

## Non-goals

- no public error-runtime helper ABI
- no generalized foreign exception ABI
- no broader cross-module live executable Part 6 claim

## Evidence

- `tmp/reports/m267/M267-D002/live_error_runtime_integration_summary.json`

## Next issue

- `M267-D003`
