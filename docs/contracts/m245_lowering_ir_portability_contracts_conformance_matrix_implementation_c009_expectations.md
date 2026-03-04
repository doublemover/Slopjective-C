# M245 Lowering/IR Portability Contracts Conformance Matrix Implementation Expectations (C009)

Contract ID: `objc3c-lowering-ir-portability-contracts-conformance-matrix-implementation/m245-c009-v1`
Status: Accepted
Dependencies: `M245-C008`
Scope: M245 lane-C lowering/IR portability contracts conformance matrix implementation continuity with explicit `M245-C008` dependency governance.

## Objective

Fail closed unless lane-C lowering/IR portability contracts conformance matrix
implementation anchors remain explicit, deterministic, and traceable across
dependency surfaces.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6644` defines canonical lane-C conformance matrix implementation scope.
- Dependency token: `M245-C008`.
- Upstream C008 assets remain mandatory prerequisites:
  - `docs/contracts/m245_lowering_ir_portability_contracts_recovery_and_determinism_hardening_c008_expectations.md`
  - `spec/planning/compiler/m245/m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_packet.md`
  - `scripts/check_m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_contract.py`
- C009 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m245/m245_c009_lowering_ir_portability_contracts_conformance_matrix_implementation_packet.md`
  - `scripts/check_m245_c009_lowering_ir_portability_contracts_conformance_matrix_implementation_contract.py`
  - `tests/tooling/test_check_m245_c009_lowering_ir_portability_contracts_conformance_matrix_implementation_contract.py`

## Shared Wiring Handoff

- Shared architecture/spec/package readiness anchors are tracked outside this
  lane-C packet and remain follow-up wiring owned by shared-file maintainers.
- This C009 contract pack enforces fail-closed snippet checks on owned lane-C
  packet artifacts and C008 dependency continuity.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m245_c009_lowering_ir_portability_contracts_conformance_matrix_implementation_contract.py`
- `python scripts/check_m245_c009_lowering_ir_portability_contracts_conformance_matrix_implementation_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c009_lowering_ir_portability_contracts_conformance_matrix_implementation_contract.py -q`

## Evidence Path

- `tmp/reports/m245/M245-C009/lowering_ir_portability_contracts_conformance_matrix_implementation_summary.json`
