# M234 Accessor and Ivar Lowering Contracts Core Feature Expansion Expectations (C004)

Contract ID: `objc3c-accessor-and-ivar-lowering-contracts-core-feature-expansion/m234-c004-v1`
Status: Accepted
Dependencies: `M234-C003`
Scope: M234 lane-C accessor/ivar lowering contracts core feature expansion continuity with explicit `M234-C003` dependency governance.

## Objective

Fail closed unless lane-C accessor/ivar lowering contracts core-feature
expansion anchors remain explicit, deterministic, and traceable across
dependency surfaces. Code/spec anchors and milestone optimization improvements
are mandatory scope inputs.

## Dependency Scope

- Issue `#5722` defines canonical lane-C core-feature expansion scope.
- Dependency token: `M234-C003`.
- Upstream C003 assets remain mandatory prerequisites:
  - `docs/contracts/m234_accessor_and_ivar_lowering_contracts_core_feature_implementation_c003_expectations.md`
  - `spec/planning/compiler/m234/m234_c003_accessor_and_ivar_lowering_contracts_core_feature_implementation_packet.md`
  - `scripts/check_m234_c003_accessor_and_ivar_lowering_contracts_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m234_c003_accessor_and_ivar_lowering_contracts_core_feature_implementation_contract.py`
- C004 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m234/m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_packet.md`
  - `scripts/check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-C accessor/ivar
  lowering contracts core-feature expansion dependency-token anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C accessor/ivar
  lowering contracts core-feature expansion fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C
  accessor/ivar lowering core-feature expansion metadata wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m234-c004-accessor-and-ivar-lowering-contracts-core-feature-expansion-contract`.
- `package.json` includes
  `test:tooling:m234-c004-accessor-and-ivar-lowering-contracts-core-feature-expansion-contract`.
- `package.json` includes `check:objc3c:m234-c004-lane-c-readiness`.
- lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m234-c003-lane-c-readiness`
  - `check:objc3c:m234-c004-lane-c-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `compile:objc3c`.
- `package.json` includes `proof:objc3c`.
- `package.json` includes `test:objc3c:execution-replay-proof`.
- `package.json` includes `test:objc3c:perf-budget`.

## Validation

- `python scripts/check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py`
- `python scripts/check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m234_c004_accessor_and_ivar_lowering_contracts_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m234-c004-lane-c-readiness`

## Evidence Path

- `tmp/reports/m234/M234-C004/accessor_and_ivar_lowering_contracts_core_feature_expansion_summary.json`
