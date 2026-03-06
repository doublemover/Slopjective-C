# M232 Runtime Selector Binding Integration Contract and Architecture Freeze Expectations (B017)

Contract ID: `objc3c-method-lookup-and-overload-semantics/m232-b017-v1`
Status: Accepted
Owner: Objective-C 3 native lane-B
Issue: `#5597`
Dependencies: none

## Objective

Execute advanced diagnostics workpack (shard 1) governance for lane-B method lookup and overload semantics, locking deterministic declaration-grammar boundaries before modular split/scaffolding work begins.

This stage treats code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Scope Anchors

- `docs/contracts/m232_method_lookup_and_overload_semantics_advanced_diagnostics_workpack_shard_1_b017_expectations.md`
- `spec/planning/compiler/m232/m232_b017_method_lookup_and_overload_semantics_advanced_diagnostics_workpack_shard_1_packet.md`
- `scripts/check_m232_b017_method_lookup_and_overload_semantics_contract.py`
- `tests/tooling/test_check_m232_b017_method_lookup_and_overload_semantics_contract.py`
- `native/objc3c/src/ARCHITECTURE.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `package.json` (`check:objc3c:m232-b017-lane-b-readiness`)

## Deterministic Invariants

1. Contract-freeze artifacts and packet anchors must remain synchronized and fail closed on drift.
2. Lane-A architecture/spec/package anchors must remain explicit and deterministic for `M232-B017`.
3. Readiness checks must preserve lane-B freeze validation and emit evidence under `tmp/reports`.

## Required Commands

- `check:objc3c:m232-b017-method-lookup-and-overload-semantics-contract`
- `test:tooling:m232-b017-method-lookup-and-overload-semantics-contract`
- `check:objc3c:m232-b017-lane-b-readiness`
- `python scripts/check_m232_b017_method_lookup_and_overload_semantics_contract.py`
- `python -m pytest tests/tooling/test_check_m232_b017_method_lookup_and_overload_semantics_contract.py -q`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `test:objc3c:parser-ast-extraction`
- `test:objc3c:parser-replay-proof`
- `test:objc3c:execution-smoke`

## Evidence Output

- `tmp/reports/m232/M232-B017/method_lookup_and_overload_semantics_contract_summary.json`
























