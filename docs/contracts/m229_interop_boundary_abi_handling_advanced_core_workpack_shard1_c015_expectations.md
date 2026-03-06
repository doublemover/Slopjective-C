# M229 Interop boundary ABI handling Advanced Core Workpack (Shard 1) Expectations (B001)

Contract ID: `objc3c-interop-boundary-abi-handling/m229-c015-v1`
Status: Accepted
Owner: Objective-C 3 native lane-C
Issue: `#5343`
Dependencies: `M229-C014`

## Objective

Execute advanced core workpack (shard 1) governance for lane-C interop boundary ABI handling, locking deterministic declaration-grammar boundaries before modular split/scaffolding work begins.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- `docs/contracts/m229_interop_boundary_abi_handling_advanced_core_workpack_shard1_c015_expectations.md`
- `spec/planning/compiler/m229/m229_c015_interop_boundary_abi_handling_advanced_core_workpack_shard1_packet.md`
- `scripts/check_m229_c015_interop_boundary_abi_handling_contract.py`
- `tests/tooling/test_check_m229_c015_interop_boundary_abi_handling_contract.py`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json` (`check:objc3c:m229-c015-lane-c-readiness`)

## Deterministic Invariants

1. Contract-freeze artifacts and packet anchors must remain synchronized and fail closed on drift.
2. Lane-B architecture/spec/package anchors must remain explicit and deterministic for `M229-C015`.
3. Readiness checks must preserve lane-C freeze validation and emit evidence under `tmp/reports`.

## Required Commands

- `check:objc3c:m229-c015-interop-boundary-abi-handling-advanced-core-workpack-shard1-contract`
- `test:tooling:m229-c015-interop-boundary-abi-handling-advanced-core-workpack-shard1-contract`
- `check:objc3c:m229-c015-lane-c-readiness`
- `python scripts/check_m229_c015_interop_boundary_abi_handling_contract.py`
- `python -m pytest tests/tooling/test_check_m229_c015_interop_boundary_abi_handling_contract.py -q`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Evidence Output

- `tmp/reports/m229/M229-C015/interop_boundary_abi_handling_advanced_core_workpack_shard1_summary.json`
















































