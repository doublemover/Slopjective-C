# M245 Lowering/IR Portability Contracts Diagnostics Hardening Expectations (C007)

Contract ID: `objc3c-lowering-ir-portability-contracts-diagnostics-hardening/m245-c007-v1`
Status: Accepted
Dependencies: `M245-C006`
Scope: M245 lane-C lowering/IR portability contracts diagnostics hardening continuity with explicit `M245-C006` dependency governance.

## Objective

Fail closed unless lane-C lowering/IR portability contracts diagnostics hardening
anchors remain explicit, deterministic, and traceable across dependency
surfaces.
Code/spec anchors and milestone optimization improvements are mandatory scope inputs.

## Dependency Scope

- Issue `#6642` defines canonical lane-C diagnostics hardening scope.
- Dependency token: `M245-C006`.
- Upstream C006 assets remain mandatory prerequisites:
  - `docs/contracts/m245_lowering_ir_portability_contracts_edge_case_expansion_and_robustness_c006_expectations.md`
  - `spec/planning/compiler/m245/m245_c006_lowering_ir_portability_contracts_edge_case_expansion_and_robustness_packet.md`
  - `scripts/check_m245_c006_lowering_ir_portability_contracts_edge_case_expansion_and_robustness_contract.py`
  - `tests/tooling/test_check_m245_c006_lowering_ir_portability_contracts_edge_case_expansion_and_robustness_contract.py`
- C007 packet/checker/test assets remain mandatory:
  - `spec/planning/compiler/m245/m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_packet.md`
  - `scripts/check_m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_contract.py`
  - `tests/tooling/test_check_m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_contract.py`

## Shared Wiring Handoff

- Shared architecture/spec/package readiness anchors are tracked outside this
  lane-C packet and remain follow-up wiring owned by shared-file maintainers.
- This C007 contract pack enforces fail-closed snippet checks on owned lane-C
  packet artifacts and C006 dependency continuity.

## Milestone Optimization Improvements (Mandatory Scope Inputs)

- `package.json` includes `test:objc3c:lowering-replay-proof`.
- `package.json` includes `test:objc3c:execution-replay-proof`.

## Validation

- `python scripts/check_m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_contract.py`
- `python scripts/check_m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_contract.py --emit-json`
- `python -m pytest tests/tooling/test_check_m245_c007_lowering_ir_portability_contracts_diagnostics_hardening_contract.py -q`

## Evidence Path

- `tmp/reports/m245/M245-C007/lowering_ir_portability_contracts_diagnostics_hardening_summary.json`
