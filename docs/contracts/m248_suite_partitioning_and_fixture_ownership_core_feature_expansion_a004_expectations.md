# M248 Suite Partitioning and Fixture Ownership Core Feature Expansion Expectations (A004)

Contract ID: `objc3c-suite-partitioning-and-fixture-ownership-core-feature-expansion/m248-a004-v1`
Status: Accepted
Dependencies: `M248-A003`
Scope: M248 lane-A core feature expansion continuity for suite partitioning and fixture ownership dependency wiring.

## Objective

Fail closed unless lane-A core feature expansion dependency anchors remain explicit, deterministic, and traceable across code/spec anchors and milestone optimization improvements as mandatory scope inputs.

## Dependency Scope

- Issue `#6791` defines canonical lane-A core feature expansion scope.
- `M248-A003` assets remain mandatory prerequisites:
  - `docs/contracts/m248_suite_partitioning_and_fixture_ownership_core_feature_implementation_a003_expectations.md`
  - `scripts/check_m248_a003_suite_partitioning_and_fixture_ownership_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m248_a003_suite_partitioning_and_fixture_ownership_core_feature_implementation_contract.py`
- Packet/checker/test assets for A004 remain mandatory:
  - `spec/planning/compiler/m248/m248_a004_suite_partitioning_and_fixture_ownership_core_feature_expansion_packet.md`
  - `scripts/check_m248_a004_suite_partitioning_and_fixture_ownership_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m248_a004_suite_partitioning_and_fixture_ownership_core_feature_expansion_contract.py`

## Deterministic Invariants

1. Lane-A core feature expansion dependency references remain explicit and fail closed when dependency tokens drift.
2. Readiness command chain enforces `M248-A003` before `M248-A004` evidence checks run.
3. Shared architecture/spec anchors remain explicit in:
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
4. Evidence output remains deterministic and reproducible under `tmp/reports/`.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m248-a004-suite-partitioning-fixture-ownership-core-feature-expansion-contract`.
- `package.json` includes
  `test:tooling:m248-a004-suite-partitioning-fixture-ownership-core-feature-expansion-contract`.
- `package.json` includes `check:objc3c:m248-a004-lane-a-readiness`.
- lane-A readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m248-a003-lane-a-readiness`
  - `check:objc3c:m248-a004-lane-a-readiness`

## Milestone Optimization Inputs

- `package.json` includes `test:objc3c:parser-replay-proof`.
- `package.json` includes `test:objc3c:parser-ast-extraction`.

## Validation

- `python scripts/check_m248_a004_suite_partitioning_and_fixture_ownership_core_feature_expansion_contract.py`
- `python -m pytest tests/tooling/test_check_m248_a004_suite_partitioning_and_fixture_ownership_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m248-a004-lane-a-readiness`

## Evidence Path

- `tmp/reports/m248/M248-A004/suite_partitioning_and_fixture_ownership_core_feature_expansion_contract_summary.json`
