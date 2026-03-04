# M245 Lowering/IR Portability Contracts Edge-Case and Compatibility Completion Expectations (C005)

Contract ID: `objc3c-lowering-ir-portability-contracts-edge-case-and-compatibility-completion/m245-c005-v1`
Status: Accepted
Dependencies: `M245-C004`
Scope: M245 lane-C lowering/IR portability contracts edge-case and compatibility completion continuity with explicit `M245-C004` dependency governance.

## Objective

Fail closed unless lane-C lowering/IR portability contracts edge-case and
compatibility completion anchors remain explicit, deterministic, and traceable
across dependency surfaces.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6640` defines canonical lane-C edge-case and compatibility completion scope.
- Dependency token: `M245-C004`.
- Upstream C004 assets remain mandatory prerequisites:
  - `docs/contracts/m245_lowering_ir_portability_contracts_core_feature_expansion_c004_expectations.md`
  - `spec/planning/compiler/m245/m245_c004_lowering_ir_portability_contracts_core_feature_expansion_packet.md`
  - `scripts/check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py`
  - `tests/tooling/test_check_m245_c004_lowering_ir_portability_contracts_core_feature_expansion_contract.py`
- C005 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m245/m245_c005_lowering_ir_portability_contracts_edge_case_and_compatibility_completion_packet.md`
  - `scripts/check_m245_c005_lowering_ir_portability_contracts_edge_case_and_compatibility_completion_contract.py`
  - `tests/tooling/test_check_m245_c005_lowering_ir_portability_contracts_edge_case_and_compatibility_completion_contract.py`

## Shared Wiring Handoff

- Shared architecture/spec/package readiness anchors are tracked outside this
  lane-C packet and remain follow-up wiring owned by shared-file maintainers.
- This C005 contract pack enforces fail-closed snippet checks on owned lane-C
  packet artifacts and C004 dependency continuity.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m245_c005_lowering_ir_portability_contracts_edge_case_and_compatibility_completion_contract.py`
- `python scripts/check_m245_c005_lowering_ir_portability_contracts_edge_case_and_compatibility_completion_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c005_lowering_ir_portability_contracts_edge_case_and_compatibility_completion_contract.py -q`

## Evidence Path

- `tmp/reports/m245/M245-C005/lowering_ir_portability_contracts_edge_case_and_compatibility_completion_summary.json`

