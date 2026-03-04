# M245 Lowering/IR Portability Contracts Recovery and Determinism Hardening Expectations (C008)

Contract ID: `objc3c-lowering-ir-portability-contracts-recovery-and-determinism-hardening/m245-c008-v1`
Status: Accepted
Dependencies: `M245-C007`
Scope: M245 lane-C lowering/IR portability contracts recovery and determinism hardening continuity with explicit `M245-C007` dependency governance.

## Objective

Fail closed unless lane-C lowering/IR portability contracts recovery and determinism hardening
anchors remain explicit, deterministic, and traceable across dependency
surfaces.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6643` defines canonical lane-C recovery and determinism hardening scope.
- Dependency token: `M245-C007`.
- Upstream C007 assets remain mandatory prerequisites:
  - `docs/contracts/m245_lowering_ir_portability_contracts_diagnostics_hardening_c007_expectations.md`
  - `spec/planning/compiler/m245/m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_packet.md`
  - `scripts/check_m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_contract.py`
- C008 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m245/m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_packet.md`
  - `scripts/check_m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_contract.py`
  - `tests/tooling/test_check_m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_contract.py`

## Shared Wiring Handoff

- Shared architecture/spec/package readiness anchors are tracked outside this
  lane-C packet and remain follow-up wiring owned by shared-file maintainers.
- This C008 contract pack enforces fail-closed snippet checks on owned lane-C
  packet artifacts and C007 dependency continuity.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_contract.py`
- `python scripts/check_m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c008_lowering_ir_portability_contracts_recovery_and_determinism_hardening_contract.py -q`

## Evidence Path

- `tmp/reports/m245/M245-C008/lowering_ir_portability_contracts_recovery_and_determinism_hardening_summary.json`

