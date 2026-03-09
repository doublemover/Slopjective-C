# M255 Replace Shim-Based Execution Smoke With Live Dispatch Evidence Cross-Lane Integration Sync Expectations (E002)

Contract ID: `objc3c-runtime-live-dispatch-smoke-replay-closeout/m255-e002-v1`

## Objective

Replace shim-era execution-smoke and replay-proof assumptions with one fail-closed lane-E closeout gate that proves supported message-send execution through the live runtime dispatch path.

## Required implementation

1. Update the integrated smoke and replay harnesses:
   - `scripts/check_objc3c_native_execution_smoke.ps1`
   - `scripts/check_objc3c_execution_replay_proof.ps1`
2. Smoke/replay summaries must publish live-dispatch terminology and evidence:
   - `requires_live_runtime_dispatch`
   - `runtime_dispatch_symbol`
   - `runtime_library`
   - `compatibility_runtime_shim`
   - `live_runtime_dispatch_default_symbol`
3. In-tree execution fixtures and fixture documentation must align to the live path:
   - `tests/tooling/fixtures/native/execution/positive/README.md`
   - `tests/tooling/fixtures/native/execution/negative/README.md`
   - message-send fixture sidecars must use `execution.requires_live_runtime_dispatch`
   - canonical symbol for live dispatch is `objc3_runtime_dispatch_i32`
4. Link-stage unresolved-symbol negatives must assert the canonical live symbol:
   - `link.unresolved_symbol:objc3_runtime_dispatch_i32`
5. Add this packet, checker, tooling test, and readiness runner:
   - `scripts/check_m255_e002_replace_shim_based_execution_smoke_with_live_dispatch_evidence_cross_lane_integration_sync.py`
   - `tests/tooling/test_check_m255_e002_replace_shim_based_execution_smoke_with_live_dispatch_evidence_cross_lane_integration_sync.py`
   - `scripts/run_m255_e002_lane_e_readiness.py`
6. Add explicit `M255-E002` anchor text to:
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/parse/objc3_parser.cpp`
   - `tests/tooling/runtime/objc3_msgsend_i32_shim.c`
7. The checker must fail closed unless:
   - `tmp/reports/m255/M255-E001/live_dispatch_gate_summary.json` still reports a fully passing gate
   - execution smoke passes through the live runtime path
   - execution replay proof produces byte-identical canonical summaries across two runs
   - the compatibility shim remains explicitly non-authoritative evidence only
8. Publish the canonical issue summary at:
   - `tmp/reports/m255/M255-E002/live_dispatch_smoke_replay_closeout_summary.json`

## Non-goals

- No new dispatch taxonomy or legality semantics.
- No new runtime lookup/cache behavior.
- No reintroduction of shim-only proof paths.

## Validation

- `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_native_execution_smoke.ps1`
- `pwsh -NoProfile -ExecutionPolicy Bypass -File scripts/check_objc3c_execution_replay_proof.ps1`
- `python scripts/check_m255_e002_replace_shim_based_execution_smoke_with_live_dispatch_evidence_cross_lane_integration_sync.py`
- `python -m pytest tests/tooling/test_check_m255_e002_replace_shim_based_execution_smoke_with_live_dispatch_evidence_cross_lane_integration_sync.py -q`
- `npm run check:objc3c:m255-e002-lane-e-readiness`

## Evidence

- `tmp/artifacts/objc3c-native/execution-smoke/m255_e002_live_dispatch_smoke/summary.json`
- `tmp/artifacts/objc3c-native/execution-replay-proof/<proof_run_id>/summary.json`
- `tmp/reports/m255/M255-E002/live_dispatch_smoke_replay_closeout_summary.json`
