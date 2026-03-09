# M255-E002 Replace Shim-Based Execution Smoke With Live Dispatch Evidence Cross-Lane Integration Sync Packet

Packet: `M255-E002`

## Goal

Close `M255` by replacing shim-era execution-smoke and replay-proof assumptions with one integrated live-dispatch evidence path.

## Dependencies

- `M255-A002`
- `M255-B003`
- `M255-C004`
- `M255-D004`
- `M255-E001`

## Required invariants

- `M255-E001` remains fully passing and explicitly hands off to `M255-E002`.
- Execution smoke proves supported message sends through `objc3_runtime_dispatch_i32`.
- Execution replay proof compares canonicalized smoke summaries that retain live-dispatch evidence fields.
- `objc3_msgsend_i32` remains compatibility/test evidence only and is not accepted as authoritative live-execution proof.
- Message-send fixture sidecars use `execution.requires_live_runtime_dispatch`.
- Runtime-dispatch unresolved-symbol negatives assert `link.unresolved_symbol:objc3_runtime_dispatch_i32`.

## Required artifacts

- `docs/contracts/m255_replace_shim_based_execution_smoke_with_live_dispatch_evidence_cross_lane_integration_sync_e002_expectations.md`
- `scripts/check_m255_e002_replace_shim_based_execution_smoke_with_live_dispatch_evidence_cross_lane_integration_sync.py`
- `tests/tooling/test_check_m255_e002_replace_shim_based_execution_smoke_with_live_dispatch_evidence_cross_lane_integration_sync.py`
- `scripts/run_m255_e002_lane_e_readiness.py`
- `tmp/reports/m255/M255-E002/live_dispatch_smoke_replay_closeout_summary.json`

## Dynamic probe surface

- `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_execution_smoke.ps1`
- `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_execution_replay_proof.ps1`

## Closeout note

`M255-E002` is the milestone closeout issue for the live dispatch tranche. The next implementation work starts in `M256` on executable classes/protocols/categories.