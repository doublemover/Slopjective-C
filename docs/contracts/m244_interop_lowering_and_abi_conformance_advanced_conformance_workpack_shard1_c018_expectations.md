# M244 Interop Lowering and ABI Conformance advanced conformance workpack (shard 1) Expectations (C018)

Contract ID: `objc3c-interop-lowering-and-abi-conformance-advanced-conformance-workpack-shard1/m244-c018-v1`
Status: Accepted
Dependencies: `m244-c017`
Scope: lane-C interop lowering/ABI advanced conformance workpack (shard 1) governance with deterministic dependency continuity and fail-closed readiness integration.

## Objective

Execute lane-C advanced conformance workpack (shard 1) governance for interop
lowering and ABI conformance on top of C017 docs/runbook synchronization assets.
Deterministic anchors, explicit dependency tokens, and fail-closed behavior are mandatory scope inputs.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6567` defines canonical lane-C advanced conformance workpack (shard 1) scope.
- `m244-c017` assets remain mandatory prerequisites:
  - `docs/contracts/m244_interop_lowering_and_abi_conformance_advanced_diagnostics_workpack_shard1_c017_expectations.md`
  - `spec/planning/compiler/m244/m244_c017_interop_lowering_and_abi_conformance_advanced_diagnostics_workpack_shard1_packet.md`
  - `scripts/check_m244_c017_interop_lowering_and_abi_conformance_advanced_diagnostics_workpack_shard1_contract.py`
  - `tests/tooling/test_check_m244_c017_interop_lowering_and_abi_conformance_advanced_diagnostics_workpack_shard1_contract.py`

## Deterministic Invariants

1. lane-C release-candidate/replay dry-run dependency references remain explicit and
   fail closed when dependency tokens drift.
2. Readiness command chain enforces `m244-c017` before `m244-c018`
   evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m244-c018-interop-lowering-abi-conformance-advanced-conformance-workpack-shard1-contract`.
- `package.json` includes
  `test:tooling:m244-c018-interop-lowering-abi-conformance-advanced-conformance-workpack-shard1-contract`.
- `package.json` includes `check:objc3c:m244-c018-lane-c-readiness`.
- lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m244-c017-lane-c-readiness`
  - `check:objc3c:m244-c018-lane-c-readiness`

## Milestone Optimization Inputs

- `compile:objc3c`
- `proof:objc3c`
- `test:objc3c:execution-replay-proof`
- `test:objc3c:perf-budget`

## Validation

- `python scripts/check_m244_c018_interop_lowering_and_abi_conformance_advanced_conformance_workpack_shard1_contract.py`
- `python scripts/check_m244_c018_interop_lowering_and_abi_conformance_advanced_conformance_workpack_shard1_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m244_c018_interop_lowering_and_abi_conformance_advanced_conformance_workpack_shard1_contract.py -q`
- `npm run check:objc3c:m244-c018-lane-c-readiness`

## Evidence Path

- `tmp/reports/m244/m244-c018/interop_lowering_and_abi_conformance_advanced_conformance_workpack_shard1_contract_summary.json`







