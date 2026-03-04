# M245 Lowering/IR Portability Contracts Core Feature Expansion Expectations (C004)

Contract ID: `objc3c-lowering-ir-portability-contracts-core-feature-expansion/m245-c004-v1`
Status: Accepted
Dependencies: `M245-C003`
Scope: M245 lane-C lowering/IR portability contracts core feature expansion continuity with explicit `M245-C003` dependency governance.

## Objective

Fail closed unless lane-C lowering/IR portability contracts core-feature
expansion anchors remain explicit, deterministic, and traceable across
dependency surfaces. Code/spec anchors and milestone optimization improvements
are mandatory scope inputs.

## Dependency Scope

- Issue `#6639` defines canonical lane-C core-feature expansion scope.
- Dependency token: `M245-C003`.
- Upstream C003 assets remain mandatory prerequisites:
  - `docs/contracts/m245_lowering_ir_portability_contracts_core_feature_implementation_c003_expectations.md`
  - `spec/planning/compiler/m245/m245_c003_lowering_ir_portability_contracts_core_feature_implementation_packet.md`
  - `scripts/check_m245_c003_lowering_ir_portability_contracts_core_feature_implementation_contract.py`
  - `tests/tooling/test_check_m245_c003_lowering_ir_portability_contracts_core_feature_implementation_contract.py`
- C004 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m245/m245_c004_lowering_ir_portability_contracts_core_feature_expansion_packet.md`
  - `scripts/check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py`

## Architecture and Spec Anchors

- `native/objc3c/src/ARCHITECTURE.md` preserves explicit lane-C lowering/IR
  portability contracts core-feature expansion dependency-token anchors.
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md` preserves lane-C lowering/IR
  portability contracts core-feature expansion fail-closed governance wording.
- `spec/MODULE_METADATA_AND_ABI_TABLES.md` preserves deterministic lane-C
  lowering/IR portability core-feature expansion metadata wording.

## Build and Readiness Integration

- `package.json` includes
  `check:objc3c:m245-c004-lowering-ir-portability-contracts-core-feature-expansion-contract`.
- `package.json` includes
  `test:tooling:m245-c004-lowering-ir-portability-contracts-core-feature-expansion-contract`.
- `package.json` includes `check:objc3c:m245-c004-lane-c-readiness`.
- lane-C readiness chaining remains deterministic and fail-closed:
  - `check:objc3c:m245-c003-lane-c-readiness`
  - `check:objc3c:m245-c004-lane-c-readiness`

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py`
- `python scripts/check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py -q`
- `npm run check:objc3c:m245-c004-lane-c-readiness`

## Evidence Path

- `tmp/reports/m245/M245-C004/lowering_ir_portability_contracts_core_feature_expansion_summary.json`
