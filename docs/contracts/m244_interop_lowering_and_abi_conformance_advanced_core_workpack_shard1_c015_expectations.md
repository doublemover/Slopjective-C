# M244 Interop Lowering and ABI Conformance advanced core workpack (shard 1) Expectations (C015)

Contract ID: `objc3c-interop-lowering-and-abi-conformance-advanced-core-workpack-shard1/m244-c015-v1`
Status: Accepted
Dependencies: `m244-c014`
Scope: lane-C interop lowering/ABI advanced core workpack (shard 1) governance with deterministic dependency continuity and fail-closed readiness integration.

## Objective

Execute lane-C advanced core workpack (shard 1) governance for interop
lowering and ABI conformance on top of C014 docs/runbook synchronization assets.
Deterministic anchors, explicit dependency tokens, and fail-closed behavior are mandatory scope inputs.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6564` defines canonical lane-C advanced core workpack (shard 1) scope.
- `m244-c014` assets remain mandatory prerequisites:
  - `docs/contracts/m244_interop_lowering_and_abi_conformance_release_candidate_and_replay_dry_run_c014_expectations.md`
  - `spec/planning/compiler/m244/m244_c014_interop_lowering_and_abi_conformance_release_candidate_and_replay_dry_run_packet.md`
  - `scripts/check_m244_c014_interop_lowering_and_abi_conformance_release_candidate_and_replay_dry_run_contract.py`
  - `tests/tooling/test_check_m244_c014_interop_lowering_and_abi_conformance_release_candidate_and_replay_dry_run_contract.py`

## Deterministic Invariants

1. lane-C release-candidate/replay dry-run dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chain enforces `m244-c014` before `m244-c015`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m244-c015-interop-lowering-abi-conformance-advanced-core-workpack-shard1-contract`.
- `package.json` includes
  `test:tooling:m244-c015-interop-lowering-abi-conformance-advanced-core-workpack-shard1-contract`.
- `package.json` includes `check:objc3c:m244-c015-lane-c-readiness`.
- lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m244-c014-lane-c-readiness`
  - `check:objc3c:m244-c015-lane-c-readiness`

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m244_c015_interop_lowering_and_abi_conformance_advanced_core_workpack_shard1_contract.py`
- `python scripts/check_m244_c015_interop_lowering_and_abi_conformance_advanced_core_workpack_shard1_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c015_interop_lowering_and_abi_conformance_advanced_core_workpack_shard1_contract.py -q`
- `npm run check:objc3c:m244-c015-lane-c-readiness`

## Evidence Path

- `tmp/reports/m244/m244-c015/interop_lowering_and_abi_conformance_advanced_core_workpack_shard1_contract_summary.json`




