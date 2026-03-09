# M263 Live Restart Hardening Edge Case And Compatibility Completion Expectations (D003)

Issue: `#7230`

Contract ID: `objc3c-runtime-live-restart-hardening/m263-d003-v1`

Scope: `M263` lane-D edge-case and compatibility completion that proves repeated reset/replay cycles remain idempotent, teardown-safe, and restart-compatible over the live runtime bootstrap path.

## Required outcomes

1. Replay without teardown must fail closed while preserving the current live runtime state.
2. `objc3_runtime_reset_for_testing` must clear live runtime state, zero image-local init cells, and preserve the retained bootstrap catalog for restart.
3. Repeated reset/replay cycles must remain deterministic and advance reset/replay generation evidence monotonically.
4. The emitted runtime registration manifest must publish explicit `M263-D003` proof fields for idempotence, teardown, and restart evidence.
5. Dynamic proof must compile restart fixtures, link an issue-local runtime probe against the emitted object, and prove first/second reset-replay cycles on the happy path.
6. Code/spec/package anchors must remain explicit and deterministic.
7. Evidence must land at `tmp/reports/m263/M263-D003/live_restart_hardening_summary.json`.

## Required artifacts

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `native/objc3c/src/io/objc3_process.cpp`
- `tests/tooling/runtime/m263_d003_live_restart_hardening_probe.cpp`
- `scripts/check_m263_d003_live_restart_hardening_edge_case_and_compatibility_completion.py`

## Non-goals

- no widening of the public runtime bootstrap ABI
- no multi-image bootstrap execution expansion
- no manifest-only proof in place of runtime-owned restart evidence
