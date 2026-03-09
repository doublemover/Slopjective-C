# M254 Replay And Bootstrap Proof Plus Runbook Closeout Expectations (E002)

Contract ID: `objc3c-runtime-replay-bootstrap-closeout/m254-e002-v1`

## Objective

Close out `M254` with one fail-closed lane-E gate that consumes the live
startup-registration/bootstrap evidence chain from `M254-E001`, publishes the
matching operator runbook, and proves that the runbook still exercises the real
integrated native path.

## Required implementation

1. Publish the canonical runbook at:
   - `docs/runbooks/m254_bootstrap_replay_operator_runbook.md`
2. Add this packet, a deterministic checker, and tooling tests:
   - `scripts/check_m254_e002_replay_and_bootstrap_proof_plus_runbook_closeout.py`
   - `tests/tooling/test_check_m254_e002_replay_and_bootstrap_proof_plus_runbook_closeout.py`
   - `scripts/run_m254_e002_lane_e_readiness.py`
3. Add `M254-E002` anchor text to:
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `tests/tooling/runtime/README.md`
   - `native/objc3c/src/driver/objc3_objc3_path.cpp`
   - `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
   - `native/objc3c/src/io/objc3_process.cpp`
4. The checker must fail closed unless:
   - `tmp/reports/m254/M254-E001/startup_registration_gate_summary.json`
     still reports a fully passing replay-stable bootstrap gate
   - `tmp/reports/m254/M254-D004/runtime_launch_integration_summary.json`
     still reports the authoritative compile/proof/smoke launch contract
   - the runbook remains synchronized with those same live command surfaces
5. Dynamic probes must execute the real operator smoke path from the runbook:
   - `scripts/check_objc3c_native_execution_smoke.ps1`
   - no mock-only substitute
   - no heuristic-only launch fallback
6. The runbook smoke probe must land at a stable path:
   - `tmp/artifacts/objc3c-native/execution-smoke/m254_e002_bootstrap_runbook_closeout/summary.json`
7. `package.json` must wire:
   - `check:objc3c:m254-e002-replay-and-bootstrap-proof-plus-runbook-closeout`
   - `test:tooling:m254-e002-replay-and-bootstrap-proof-plus-runbook-closeout`
   - `check:objc3c:m254-e002-lane-e-readiness`

## Non-goals

- No new runtime/bootstrap semantics.
- No new registration-table fields.
- No new operator launch heuristics.
- No milestone planning beyond `M254` closeout.

## Evidence

- `tmp/reports/m254/M254-E002/replay_bootstrap_runbook_closeout_summary.json`
