# M263-D002 Live Registration, Replay, And Discovery Implementation Packet

Packet: `M263-D002`

Milestone: `M263`

Lane: `D`

Issue: `#7229`

Dependencies: `M263-D001`, `M263-C003`

## Summary

Prove the already-live runtime path that registers emitted metadata images at startup, publishes discovery-root state through runtime-owned snapshots, survives reset, and replays deterministically from the retained bootstrap catalog.

Contract ID: `objc3c-runtime-live-registration-discovery-replay/m263-d002-v1`

## Acceptance criteria

- publish an explicit `M263-D002` contract in docs, specs, runtime anchors, and the emitted runtime registration manifest
- prove startup registration leaves one live registered image and one retained bootstrap image per translation unit
- prove reset clears live registration state without dropping the retained bootstrap catalog
- prove replay restores live registration state and republishes discovery-root evidence through the canonical runtime snapshots
- land deterministic evidence at `tmp/reports/m263/M263-D002/live_registration_replay_and_discovery_summary.json`

## Inputs

- `tests/tooling/fixtures/native/m254_c002_runtime_bootstrap_metadata_library.objc3`
- `tests/tooling/fixtures/native/m254_c003_runtime_bootstrap_category_library.objc3`
- `tests/tooling/runtime/m263_d002_live_registration_replay_tracking_probe.cpp`

## Outputs

- emitted runtime registration manifests with `M263-D002` proof fields
- runtime probe evidence for startup registration, reset, and replay
- deterministic issue-local summary JSON under `tmp/reports/m263/M263-D002/`

## Next issue

`M263-D003`
