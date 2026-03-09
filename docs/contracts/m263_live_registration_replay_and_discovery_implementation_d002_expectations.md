# M263 Live Registration Replay And Discovery Implementation Expectations (D002)

Issue: `#7229`

Contract ID: `objc3c-runtime-live-registration-discovery-replay/m263-d002-v1`

Scope: `M263` lane-D core runtime work that proves emitted metadata images register through the native runtime, publish discovery-root state through runtime-owned snapshots, and replay deterministically from the retained bootstrap catalog after reset.

## Required outcomes

1. Startup registration must leave one live registered image plus one retained bootstrap record per emitted translation unit.
2. `objc3_runtime_copy_image_walk_state_for_testing` must publish live discovery-root state after startup registration and after replay.
3. `objc3_runtime_reset_for_testing` must clear live registration state while preserving the retained bootstrap catalog and reset evidence.
4. `objc3_runtime_replay_registered_images_for_testing` must repopulate live registration state from the retained bootstrap catalog deterministically.
5. The emitted runtime registration manifest must publish explicit `M263-D002` proof fields for live registration, discovery tracking, and replay tracking.
6. Dynamic proof must compile native `.objc3` fixtures, link an issue-local runtime probe against the emitted object, and prove startup, reset, and replay on the happy path.
7. Code/spec/package anchors must remain explicit and deterministic.
8. Evidence must land at `tmp/reports/m263/M263-D002/live_registration_replay_and_discovery_summary.json`.

## Required artifacts

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `native/objc3c/src/io/objc3_process.cpp`
- `tests/tooling/runtime/m263_d002_live_registration_replay_tracking_probe.cpp`
- `scripts/check_m263_d002_live_registration_replay_and_discovery_implementation.py`

## Non-goals

- no new public runtime entrypoints beyond the already-exported reset/replay/image-walk surface
- no alternate replay mechanism outside the retained bootstrap catalog
- no manifest-only proof in place of live runtime evidence
