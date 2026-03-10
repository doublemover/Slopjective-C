# M259 C001 End-to-End Replay and Inspection Expectations

Contract ID: `objc3c-runnable-replay-and-inspection-evidence-freeze/m259-c001-v1`

Issue: `#7212`

## Objective

Freeze the current replay-proof and binary-inspection evidence boundary for the
runnable native Objective-C 3 slice.

## Required implementation

1. Add the issue-local contract/checker/test/readiness assets:
   - `docs/contracts/m259_c001_end_to_end_replay_and_inspection_expectations.md`
   - `spec/planning/compiler/m259/m259_c001_end_to_end_replay_and_inspection_packet.md`
   - `scripts/check_m259_c001_end_to_end_replay_and_inspection_contract_and_architecture_freeze.py`
   - `tests/tooling/test_check_m259_c001_end_to_end_replay_and_inspection_contract_and_architecture_freeze.py`
   - `scripts/run_m259_c001_lane_c_readiness.py`
2. Publish explicit docs/spec/package/script anchors in:
   - `docs/objc3c-native.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `scripts/check_objc3c_native_execution_smoke.ps1`
   - `scripts/check_objc3c_execution_replay_proof.ps1`
   - `package.json`
3. Freeze the canonical runnable replay/inspection evidence stack:
   - scalar/core execution smoke remains the broad runnable replay corpus
   - `M259-A002` remains the canonical integrated runnable-slice positive sample
   - one canonical object-inspection probe over the A002 sample proves the
     runnable slice still emits inspectable IR/object artifacts on the live path
4. Keep explicit non-goals and fail-closed rules:
   - no new runnable corpus expansion beyond the current smoke/replay plus A002
     integrated sample
   - no archive/static-link, multi-module, or multi-image inspection matrix yet
   - no new claims for blocks, ARC, async/await, actors, or other advanced
     unsupported surfaces
5. The contract must explicitly hand off to `M259-C002`.

## Canonical models

- Freeze model:
  `runnable-slice-replay-proof-and-single-sample-object-inspection-boundary`
- Evidence model:
  `execution-smoke-plus-replay-proof-plus-a002-object-section-anchor`
- Failure model:
  `fail-closed-on-runnable-replay-or-object-inspection-boundary-drift`

## Evidence

- `tmp/reports/m259/M259-C001/end_to_end_replay_and_inspection_summary.json`
