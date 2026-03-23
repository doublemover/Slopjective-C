# M267 Live Catch, Bridge, And Runtime Integration Core Feature Implementation Expectations (D002)

Contract ID: `objc3c-part6-live-error-runtime-integration/m267-d002-v1`

`M267-D002` must prove the runnable Part 6 lowering from `M267-C002`, the
artifact/replay surface from `M267-C003`, and the private helper ABI from
`M267-D001` already form one live executable path.

Required proof:

- emitted IR carries:
  - `; part6_live_error_runtime_integration = ...`
  - `!objc3.objc_part6_live_error_runtime_integration = !{!90}`
- generated native object code calls the private helper cluster for:
  - thrown-error store/load
  - status-code bridge normalization
  - `catch (NSError* error)` dispatch
- the emitted runtime-registration manifest still publishes the runtime support
  library archive path used by the linked probe
- the linked probe:
  - compiles against `artifacts/lib/objc3_runtime.lib`
  - executes the generated `runCase()` entrypoint
  - observes deterministic helper traffic through
    `objc3_runtime_copy_error_bridge_state_for_testing`

Required evidence:

- `tests/tooling/runtime/m267_d002_live_error_runtime_integration_probe.cpp`
- `tmp/reports/m267/M267-D002/live_error_runtime_integration_summary.json`
